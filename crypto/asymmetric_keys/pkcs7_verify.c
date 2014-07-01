/* Verify the signature on a PKCS#7 message.
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

#define pr_fmt(fmt) "PKCS7: "fmt
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/asn1.h>
#include <crypto/hash.h>
#include "public_key.h"
#include "pkcs7_parser.h"

/*
 * Digest the relevant parts of the PKCS#7 data
 */
static int pkcs7_digest(struct pkcs7_message *pkcs7,
			struct pkcs7_signed_info *sinfo)
{
	struct crypto_shash *tfm;
	struct shash_desc *desc;
	size_t digest_size, desc_size;
	void *digest;
	int ret;

	kenter(",%u,%u", sinfo->index, sinfo->sig.pkey_hash_algo);

	if (sinfo->sig.pkey_hash_algo >= PKEY_HASH__LAST ||
	    !hash_algo_name[sinfo->sig.pkey_hash_algo])
		return -ENOPKG;

	/* Allocate the hashing algorithm we're going to need and find out how
	 * big the hash operational data will be.
	 */
	tfm = crypto_alloc_shash(hash_algo_name[sinfo->sig.pkey_hash_algo],
				 0, 0);
	if (IS_ERR(tfm))
		return (PTR_ERR(tfm) == -ENOENT) ? -ENOPKG : PTR_ERR(tfm);

	desc_size = crypto_shash_descsize(tfm) + sizeof(*desc);
	sinfo->sig.digest_size = digest_size = crypto_shash_digestsize(tfm);

	ret = -ENOMEM;
	digest = kzalloc(digest_size + desc_size, GFP_KERNEL);
	if (!digest)
		goto error_no_desc;

	desc = digest + digest_size;
	desc->tfm   = tfm;
	desc->flags = CRYPTO_TFM_REQ_MAY_SLEEP;

	/* Digest the message [RFC2315 9.3] */
	ret = crypto_shash_init(desc);
	if (ret < 0)
		goto error;
	ret = crypto_shash_finup(desc, pkcs7->data, pkcs7->data_len, digest);
	if (ret < 0)
		goto error;
	pr_devel("MsgDigest = [%*ph]\n", 8, digest);

	/* However, if there are authenticated attributes, there must be a
	 * message digest attribute amongst them which corresponds to the
	 * digest we just calculated.
	 */
	if (sinfo->msgdigest) {
		u8 tag;

		if (sinfo->msgdigest_len != sinfo->sig.digest_size) {
			pr_debug("Sig %u: Invalid digest size (%u)\n",
				 sinfo->index, sinfo->msgdigest_len);
			ret = -EBADMSG;
			goto error;
		}

		if (memcmp(digest, sinfo->msgdigest, sinfo->msgdigest_len) != 0) {
			pr_debug("Sig %u: Message digest doesn't match\n",
				 sinfo->index);
			ret = -EKEYREJECTED;
			goto error;
		}

		/* We then calculate anew, using the authenticated attributes
		 * as the contents of the digest instead.  Note that we need to
		 * convert the attributes from a CONT.0 into a SET before we
		 * hash it.
		 */
		memset(digest, 0, sinfo->sig.digest_size);

		ret = crypto_shash_init(desc);
		if (ret < 0)
			goto error;
		tag = ASN1_CONS_BIT | ASN1_SET;
		ret = crypto_shash_update(desc, &tag, 1);
		if (ret < 0)
			goto error;
		ret = crypto_shash_finup(desc, sinfo->authattrs,
					 sinfo->authattrs_len, digest);
		if (ret < 0)
			goto error;
		pr_devel("AADigest = [%*ph]\n", 8, digest);
	}

	sinfo->sig.digest = digest;
	digest = NULL;

error:
	kfree(digest);
error_no_desc:
	crypto_free_shash(tfm);
	kleave(" = %d", ret);
	return ret;
}

/*
 * Find the key (X.509 certificate) to use to verify a PKCS#7 message.  PKCS#7
 * uses the issuer's name and the issuing certificate serial number for
 * matching purposes.  These must match the certificate issuer's name (not
 * subject's name) and the certificate serial number [RFC 2315 6.7].
 */
static int pkcs7_find_key(struct pkcs7_message *pkcs7,
			  struct pkcs7_signed_info *sinfo)
{
	struct x509_certificate *x509;
	unsigned certix = 1;

	kenter("%u,%u,%u",
	       sinfo->index, sinfo->raw_serial_size, sinfo->raw_issuer_size);

	for (x509 = pkcs7->certs; x509; x509 = x509->next, certix++) {
		/* I'm _assuming_ that the generator of the PKCS#7 message will
		 * encode the fields from the X.509 cert in the same way in the
		 * PKCS#7 message - but I can't be 100% sure of that.  It's
		 * possible this will need element-by-element comparison.
		 */
		if (x509->raw_serial_size != sinfo->raw_serial_size ||
		    memcmp(x509->raw_serial, sinfo->raw_serial,
			   sinfo->raw_serial_size) != 0)
			continue;
		pr_devel("Sig %u: Found cert serial match X.509[%u]\n",
			 sinfo->index, certix);

		if (x509->raw_issuer_size != sinfo->raw_issuer_size ||
		    memcmp(x509->raw_issuer, sinfo->raw_issuer,
			   sinfo->raw_issuer_size) != 0) {
			pr_warn("Sig %u: X.509 subject and PKCS#7 issuer don't match\n",
				sinfo->index);
			continue;
		}

		if (x509->pub->pkey_algo != sinfo->sig.pkey_algo) {
			pr_warn("Sig %u: X.509 algo and PKCS#7 sig algo don't match\n",
				sinfo->index);
			continue;
		}

		sinfo->signer = x509;
		return 0;
	}
	pr_warn("Sig %u: Issuing X.509 cert not found (#%*ph)\n",
		sinfo->index, sinfo->raw_serial_size, sinfo->raw_serial);
	return -ENOKEY;
}

/*
 * Verify one signed information block from a PKCS#7 message.
 */
static int pkcs7_verify_one(struct pkcs7_message *pkcs7,
			    struct pkcs7_signed_info *sinfo)
{
	int ret;

	kenter(",%u", sinfo->index);

	/* First of all, digest the data in the PKCS#7 message and the
	 * signed information block
	 */
	ret = pkcs7_digest(pkcs7, sinfo);
	if (ret < 0)
		return ret;

	/* Find the key for the signature */
	ret = pkcs7_find_key(pkcs7, sinfo);
	if (ret < 0)
		return ret;

	pr_devel("Using X.509[%u] for sig %u\n",
		 sinfo->signer->index, sinfo->index);

	/* Verify the PKCS#7 binary against the key */
	ret = public_key_verify_signature(sinfo->signer->pub, &sinfo->sig);
	if (ret < 0)
		return ret;

	pr_devel("Verified signature %u\n", sinfo->index);

	return 0;
}

/**
 * pkcs7_verify - Verify a PKCS#7 message
 * @pkcs7: The PKCS#7 message to be verified
 */
int pkcs7_verify(struct pkcs7_message *pkcs7)
{
	struct pkcs7_signed_info *sinfo;
	struct x509_certificate *x509;
	int ret, n;

	kenter("");

	for (n = 0, x509 = pkcs7->certs; x509; x509 = x509->next, n++) {
		ret = x509_get_sig_params(x509);
		if (ret < 0)
			return ret;
		pr_debug("X.509[%u] %s\n", n, x509->authority);
	}

	for (sinfo = pkcs7->signed_infos; sinfo; sinfo = sinfo->next) {
		ret = pkcs7_verify_one(pkcs7, sinfo);
		if (ret < 0) {
			kleave(" = %d", ret);
			return ret;
		}
	}

	kleave(" = 0");
	return 0;
}
EXPORT_SYMBOL_GPL(pkcs7_verify);
