/*
 * Copyright (c) 2005-2010 Brocade Communications Systems, Inc.
 * All rights reserved
 * www.brocade.com
 *
 * Linux driver for Brocade Fibre Channel Host Bus Adapter.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (GPL) Version 2 as
 * published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef __BFA_IOC_H__
#define __BFA_IOC_H__

#include "bfad_drv.h"
#include "bfa_cs.h"
#include "bfi.h"

#define BFA_DBG_FWTRC_ENTS	(BFI_IOC_TRC_ENTS)
#define BFA_DBG_FWTRC_LEN					\
	(BFA_DBG_FWTRC_ENTS * sizeof(struct bfa_trc_s) +	\
	(sizeof(struct bfa_trc_mod_s) -				\
	BFA_TRC_MAX * sizeof(struct bfa_trc_s)))
/*
 * BFA timer declarations
 */
typedef void (*bfa_timer_cbfn_t)(void *);

/*
 * BFA timer data structure
 */
struct bfa_timer_s {
	struct list_head	qe;
	bfa_timer_cbfn_t timercb;
	void		*arg;
	int		timeout;	/* in millisecs */
};

/*
 * Timer module structure
 */
struct bfa_timer_mod_s {
	struct list_head timer_q;
};

#define BFA_TIMER_FREQ 200 /* specified in millisecs */

void bfa_timer_beat(struct bfa_timer_mod_s *mod);
void bfa_timer_begin(struct bfa_timer_mod_s *mod, struct bfa_timer_s *timer,
			bfa_timer_cbfn_t timercb, void *arg,
			unsigned int timeout);
void bfa_timer_stop(struct bfa_timer_s *timer);

/*
 * Generic Scatter Gather Element used by driver
 */
struct bfa_sge_s {
	u32	sg_len;
	void		*sg_addr;
};

#define bfa_sge_word_swap(__sge) do {					     \
	((u32 *)(__sge))[0] = swab32(((u32 *)(__sge))[0]);      \
	((u32 *)(__sge))[1] = swab32(((u32 *)(__sge))[1]);      \
	((u32 *)(__sge))[2] = swab32(((u32 *)(__sge))[2]);      \
} while (0)

#define bfa_swap_words(_x)  (	\
	((_x) << 32) | ((_x) >> 32))

#ifdef __BIG_ENDIAN
#define bfa_sge_to_be(_x)
#define bfa_sge_to_le(_x)	bfa_sge_word_swap(_x)
#define bfa_sgaddr_le(_x)	bfa_swap_words(_x)
#else
#define	bfa_sge_to_be(_x)	bfa_sge_word_swap(_x)
#define bfa_sge_to_le(_x)
#define bfa_sgaddr_le(_x)	(_x)
#endif

/*
 * PCI device information required by IOC
 */
struct bfa_pcidev_s {
	int		pci_slot;
	u8		pci_func;
	u16		device_id;
	u16		ssid;
	void __iomem	*pci_bar_kva;
};

/*
 * Structure used to remember the DMA-able memory block's KVA and Physical
 * Address
 */
struct bfa_dma_s {
	void		*kva;	/* ! Kernel virtual address	*/
	u64	pa;	/* ! Physical address		*/
};

#define BFA_DMA_ALIGN_SZ	256
#define BFA_ROUNDUP(_l, _s)	(((_l) + ((_s) - 1)) & ~((_s) - 1))

/*
 * smem size for Crossbow and Catapult
 */
#define BFI_SMEM_CB_SIZE	0x200000U	/* ! 2MB for crossbow	*/
#define BFI_SMEM_CT_SIZE	0x280000U	/* ! 2.5MB for catapult	*/

#define bfa_dma_be_addr_set(dma_addr, pa)	\
		__bfa_dma_be_addr_set(&dma_addr, (u64)pa)
static inline void
__bfa_dma_be_addr_set(union bfi_addr_u *dma_addr, u64 pa)
{
	dma_addr->a32.addr_lo = cpu_to_be32(pa);
	dma_addr->a32.addr_hi = cpu_to_be32(pa >> 32);
}

#define bfa_alen_set(__alen, __len, __pa)	\
	__bfa_alen_set(__alen, __len, (u64)__pa)

static inline void
__bfa_alen_set(struct bfi_alen_s *alen, u32 len, u64 pa)
{
	alen->al_len = cpu_to_be32(len);
	bfa_dma_be_addr_set(alen->al_addr, pa);
}

struct bfa_ioc_regs_s {
	void __iomem *hfn_mbox_cmd;
	void __iomem *hfn_mbox;
	void __iomem *lpu_mbox_cmd;
	void __iomem *lpu_mbox;
	void __iomem *lpu_read_stat;
	void __iomem *pss_ctl_reg;
	void __iomem *pss_err_status_reg;
	void __iomem *app_pll_fast_ctl_reg;
	void __iomem *app_pll_slow_ctl_reg;
	void __iomem *ioc_sem_reg;
	void __iomem *ioc_usage_sem_reg;
	void __iomem *ioc_init_sem_reg;
	void __iomem *ioc_usage_reg;
	void __iomem *host_page_num_fn;
	void __iomem *heartbeat;
	void __iomem *ioc_fwstate;
	void __iomem *alt_ioc_fwstate;
	void __iomem *ll_halt;
	void __iomem *alt_ll_halt;
	void __iomem *err_set;
	void __iomem *ioc_fail_sync;
	void __iomem *shirq_isr_next;
	void __iomem *shirq_msk_next;
	void __iomem *smem_page_start;
	u32	smem_pg0;
};

#define bfa_mem_read(_raddr, _off)	swab32(readl(((_raddr) + (_off))))
#define bfa_mem_write(_raddr, _off, _val)	\
			writel(swab32((_val)), ((_raddr) + (_off)))
/*
 * IOC Mailbox structures
 */
struct bfa_mbox_cmd_s {
	struct list_head	qe;
	u32	msg[BFI_IOC_MSGSZ];
};

/*
 * IOC mailbox module
 */
typedef void (*bfa_ioc_mbox_mcfunc_t)(void *cbarg, struct bfi_mbmsg_s *m);
struct bfa_ioc_mbox_mod_s {
	struct list_head		cmd_q;	/*  pending mbox queue	*/
	int			nmclass;	/*  number of handlers */
	struct {
		bfa_ioc_mbox_mcfunc_t	cbfn;	/*  message handlers	*/
		void			*cbarg;
	} mbhdlr[BFI_MC_MAX];
};

/*
 * IOC callback function interfaces
 */
typedef void (*bfa_ioc_enable_cbfn_t)(void *bfa, enum bfa_status status);
typedef void (*bfa_ioc_disable_cbfn_t)(void *bfa);
typedef void (*bfa_ioc_hbfail_cbfn_t)(void *bfa);
typedef void (*bfa_ioc_reset_cbfn_t)(void *bfa);
struct bfa_ioc_cbfn_s {
	bfa_ioc_enable_cbfn_t	enable_cbfn;
	bfa_ioc_disable_cbfn_t	disable_cbfn;
	bfa_ioc_hbfail_cbfn_t	hbfail_cbfn;
	bfa_ioc_reset_cbfn_t	reset_cbfn;
};

/*
 * IOC event notification mechanism.
 */
enum bfa_ioc_event_e {
	BFA_IOC_E_ENABLED	= 1,
	BFA_IOC_E_DISABLED	= 2,
	BFA_IOC_E_FAILED	= 3,
};

typedef void (*bfa_ioc_notify_cbfn_t)(void *, enum bfa_ioc_event_e);

struct bfa_ioc_notify_s {
	struct list_head		qe;
	bfa_ioc_notify_cbfn_t	cbfn;
	void			*cbarg;
};

/*
 * Initialize a IOC event notification structure
 */
#define bfa_ioc_notify_init(__notify, __cbfn, __cbarg) do {	\
	(__notify)->cbfn = (__cbfn);      \
	(__notify)->cbarg = (__cbarg);      \
} while (0)

struct bfa_iocpf_s {
	bfa_fsm_t		fsm;
	struct bfa_ioc_s	*ioc;
	bfa_boolean_t		fw_mismatch_notified;
	bfa_boolean_t		auto_recover;
	u32			poll_time;
};

struct bfa_ioc_s {
	bfa_fsm_t		fsm;
	struct bfa_s		*bfa;
	struct bfa_pcidev_s	pcidev;
	struct bfa_timer_mod_s	*timer_mod;
	struct bfa_timer_s	ioc_timer;
	struct bfa_timer_s	sem_timer;
	struct bfa_timer_s	hb_timer;
	u32		hb_count;
	struct list_head	notify_q;
	void			*dbg_fwsave;
	int			dbg_fwsave_len;
	bfa_boolean_t		dbg_fwsave_once;
	enum bfi_pcifn_class	clscode;
	struct bfa_ioc_regs_s	ioc_regs;
	struct bfa_trc_mod_s	*trcmod;
	struct bfa_ioc_drv_stats_s	stats;
	bfa_boolean_t		fcmode;
	bfa_boolean_t		pllinit;
	bfa_boolean_t		stats_busy;	/*  outstanding stats */
	u8			port_id;
	struct bfa_dma_s	attr_dma;
	struct bfi_ioc_attr_s	*attr;
	struct bfa_ioc_cbfn_s	*cbfn;
	struct bfa_ioc_mbox_mod_s mbox_mod;
	struct bfa_ioc_hwif_s	*ioc_hwif;
	struct bfa_iocpf_s	iocpf;
	enum bfi_asic_gen	asic_gen;
	enum bfi_asic_mode	asic_mode;
	enum bfi_port_mode	port0_mode;
	enum bfi_port_mode	port1_mode;
	enum bfa_mode_s		port_mode;
	u8			ad_cap_bm;	/* adapter cap bit mask */
	u8			port_mode_cfg;	/* config port mode */
};

struct bfa_ioc_hwif_s {
	bfa_status_t (*ioc_pll_init) (void __iomem *rb, enum bfi_asic_mode m);
	bfa_boolean_t	(*ioc_firmware_lock)	(struct bfa_ioc_s *ioc);
	void		(*ioc_firmware_unlock)	(struct bfa_ioc_s *ioc);
	void		(*ioc_reg_init)	(struct bfa_ioc_s *ioc);
	void		(*ioc_map_port)	(struct bfa_ioc_s *ioc);
	void		(*ioc_isr_mode_set)	(struct bfa_ioc_s *ioc,
					bfa_boolean_t msix);
	void		(*ioc_notify_fail)	(struct bfa_ioc_s *ioc);
	void		(*ioc_ownership_reset)	(struct bfa_ioc_s *ioc);
	bfa_boolean_t   (*ioc_sync_start)       (struct bfa_ioc_s *ioc);
	void		(*ioc_sync_join)	(struct bfa_ioc_s *ioc);
	void		(*ioc_sync_leave)	(struct bfa_ioc_s *ioc);
	void		(*ioc_sync_ack)		(struct bfa_ioc_s *ioc);
	bfa_boolean_t	(*ioc_sync_complete)	(struct bfa_ioc_s *ioc);
	bfa_boolean_t	(*ioc_lpu_read_stat)	(struct bfa_ioc_s *ioc);
};

/*
 * ASIC block configurtion related
 */

typedef void (*bfa_ablk_cbfn_t)(void *, enum bfa_status);

struct bfa_ablk_s {
	struct bfa_ioc_s	*ioc;
	struct bfa_ablk_cfg_s	*cfg;
	u16			*pcifn;
	struct bfa_dma_s	dma_addr;
	bfa_boolean_t		busy;
	struct bfa_mbox_cmd_s	mb;
	bfa_ablk_cbfn_t		cbfn;
	void			*cbarg;
	struct bfa_ioc_notify_s	ioc_notify;
};

#define bfa_ioc_pcifn(__ioc)		((__ioc)->pcidev.pci_func)
#define bfa_ioc_devid(__ioc)		((__ioc)->pcidev.device_id)
#define bfa_ioc_bar0(__ioc)		((__ioc)->pcidev.pci_bar_kva)
#define bfa_ioc_portid(__ioc)		((__ioc)->port_id)
#define bfa_ioc_asic_gen(__ioc)		((__ioc)->asic_gen)
#define bfa_ioc_is_cna(__ioc)	\
	((bfa_ioc_get_type(__ioc) == BFA_IOC_TYPE_FCoE) ||	\
	 (bfa_ioc_get_type(__ioc) == BFA_IOC_TYPE_LL))
#define bfa_ioc_fetch_stats(__ioc, __stats) \
		(((__stats)->drv_stats) = (__ioc)->stats)
#define bfa_ioc_clr_stats(__ioc)	\
		memset(&(__ioc)->stats, 0, sizeof((__ioc)->stats))
#define bfa_ioc_maxfrsize(__ioc)	((__ioc)->attr->maxfrsize)
#define bfa_ioc_rx_bbcredit(__ioc)	((__ioc)->attr->rx_bbcredit)
#define bfa_ioc_speed_sup(__ioc)	\
	BFI_ADAPTER_GETP(SPEED, (__ioc)->attr->adapter_prop)
#define bfa_ioc_get_nports(__ioc)	\
	BFI_ADAPTER_GETP(NPORTS, (__ioc)->attr->adapter_prop)

#define bfa_ioc_stats(_ioc, _stats)	((_ioc)->stats._stats++)
#define BFA_IOC_FWIMG_MINSZ	(16 * 1024)
#define BFA_IOC_FW_SMEM_SIZE(__ioc)			\
	((bfa_ioc_asic_gen(__ioc) == BFI_ASIC_GEN_CB)	\
	 ? BFI_SMEM_CB_SIZE : BFI_SMEM_CT_SIZE)
#define BFA_IOC_FLASH_CHUNK_NO(off)		(off / BFI_FLASH_CHUNK_SZ_WORDS)
#define BFA_IOC_FLASH_OFFSET_IN_CHUNK(off)	(off % BFI_FLASH_CHUNK_SZ_WORDS)
#define BFA_IOC_FLASH_CHUNK_ADDR(chunkno)  (chunkno * BFI_FLASH_CHUNK_SZ_WORDS)

/*
 * IOC mailbox interface
 */
void bfa_ioc_mbox_queue(struct bfa_ioc_s *ioc, struct bfa_mbox_cmd_s *cmd);
void bfa_ioc_mbox_register(struct bfa_ioc_s *ioc,
		bfa_ioc_mbox_mcfunc_t *mcfuncs);
void bfa_ioc_mbox_isr(struct bfa_ioc_s *ioc);
void bfa_ioc_mbox_send(struct bfa_ioc_s *ioc, void *ioc_msg, int len);
bfa_boolean_t bfa_ioc_msgget(struct bfa_ioc_s *ioc, void *mbmsg);
void bfa_ioc_mbox_regisr(struct bfa_ioc_s *ioc, enum bfi_mclass mc,
		bfa_ioc_mbox_mcfunc_t cbfn, void *cbarg);

/*
 * IOC interfaces
 */

#define bfa_ioc_pll_init_asic(__ioc) \
	((__ioc)->ioc_hwif->ioc_pll_init((__ioc)->pcidev.pci_bar_kva, \
			   (__ioc)->asic_mode))

bfa_status_t bfa_ioc_pll_init(struct bfa_ioc_s *ioc);
bfa_status_t bfa_ioc_cb_pll_init(void __iomem *rb, enum bfi_asic_mode mode);
bfa_status_t bfa_ioc_ct_pll_init(void __iomem *rb, enum bfi_asic_mode mode);
bfa_status_t bfa_ioc_ct2_pll_init(void __iomem *rb, enum bfi_asic_mode mode);

#define bfa_ioc_isr_mode_set(__ioc, __msix) do {			\
	if ((__ioc)->ioc_hwif->ioc_isr_mode_set)			\
		((__ioc)->ioc_hwif->ioc_isr_mode_set(__ioc, __msix));	\
} while (0)
#define	bfa_ioc_ownership_reset(__ioc)				\
			((__ioc)->ioc_hwif->ioc_ownership_reset(__ioc))
#define bfa_ioc_get_fcmode(__ioc)	((__ioc)->fcmode)
#define bfa_ioc_lpu_read_stat(__ioc) do {			\
	if ((__ioc)->ioc_hwif->ioc_lpu_read_stat)		\
		((__ioc)->ioc_hwif->ioc_lpu_read_stat(__ioc));	\
} while (0)

void bfa_ioc_set_cb_hwif(struct bfa_ioc_s *ioc);
void bfa_ioc_set_ct_hwif(struct bfa_ioc_s *ioc);
void bfa_ioc_set_ct2_hwif(struct bfa_ioc_s *ioc);
void bfa_ioc_ct2_poweron(struct bfa_ioc_s *ioc);

void bfa_ioc_attach(struct bfa_ioc_s *ioc, void *bfa,
		struct bfa_ioc_cbfn_s *cbfn, struct bfa_timer_mod_s *timer_mod);
void bfa_ioc_auto_recover(bfa_boolean_t auto_recover);
void bfa_ioc_detach(struct bfa_ioc_s *ioc);
void bfa_ioc_pci_init(struct bfa_ioc_s *ioc, struct bfa_pcidev_s *pcidev,
		enum bfi_pcifn_class clscode);
void bfa_ioc_mem_claim(struct bfa_ioc_s *ioc,  u8 *dm_kva, u64 dm_pa);
void bfa_ioc_enable(struct bfa_ioc_s *ioc);
void bfa_ioc_disable(struct bfa_ioc_s *ioc);
bfa_boolean_t bfa_ioc_intx_claim(struct bfa_ioc_s *ioc);

void bfa_ioc_boot(struct bfa_ioc_s *ioc, u32 boot_type,
		u32 boot_env);
void bfa_ioc_isr(struct bfa_ioc_s *ioc, struct bfi_mbmsg_s *msg);
void bfa_ioc_error_isr(struct bfa_ioc_s *ioc);
bfa_boolean_t bfa_ioc_is_operational(struct bfa_ioc_s *ioc);
bfa_boolean_t bfa_ioc_is_initialized(struct bfa_ioc_s *ioc);
bfa_boolean_t bfa_ioc_is_disabled(struct bfa_ioc_s *ioc);
bfa_boolean_t bfa_ioc_is_acq_addr(struct bfa_ioc_s *ioc);
bfa_boolean_t bfa_ioc_fw_mismatch(struct bfa_ioc_s *ioc);
bfa_boolean_t bfa_ioc_adapter_is_disabled(struct bfa_ioc_s *ioc);
void bfa_ioc_reset_fwstate(struct bfa_ioc_s *ioc);
enum bfa_ioc_type_e bfa_ioc_get_type(struct bfa_ioc_s *ioc);
void bfa_ioc_get_adapter_serial_num(struct bfa_ioc_s *ioc, char *serial_num);
void bfa_ioc_get_adapter_fw_ver(struct bfa_ioc_s *ioc, char *fw_ver);
void bfa_ioc_get_adapter_optrom_ver(struct bfa_ioc_s *ioc, char *optrom_ver);
void bfa_ioc_get_adapter_model(struct bfa_ioc_s *ioc, char *model);
void bfa_ioc_get_adapter_manufacturer(struct bfa_ioc_s *ioc,
		char *manufacturer);
void bfa_ioc_get_pci_chip_rev(struct bfa_ioc_s *ioc, char *chip_rev);
enum bfa_ioc_state bfa_ioc_get_state(struct bfa_ioc_s *ioc);

void bfa_ioc_get_attr(struct bfa_ioc_s *ioc, struct bfa_ioc_attr_s *ioc_attr);
void bfa_ioc_get_adapter_attr(struct bfa_ioc_s *ioc,
		struct bfa_adapter_attr_s *ad_attr);
void bfa_ioc_debug_memclaim(struct bfa_ioc_s *ioc, void *dbg_fwsave);
bfa_status_t bfa_ioc_debug_fwsave(struct bfa_ioc_s *ioc, void *trcdata,
		int *trclen);
bfa_status_t bfa_ioc_debug_fwtrc(struct bfa_ioc_s *ioc, void *trcdata,
				 int *trclen);
bfa_status_t bfa_ioc_debug_fwcore(struct bfa_ioc_s *ioc, void *buf,
	u32 *offset, int *buflen);
bfa_boolean_t bfa_ioc_sem_get(void __iomem *sem_reg);
void bfa_ioc_fwver_get(struct bfa_ioc_s *ioc,
			struct bfi_ioc_image_hdr_s *fwhdr);
bfa_boolean_t bfa_ioc_fwver_cmp(struct bfa_ioc_s *ioc,
			struct bfi_ioc_image_hdr_s *fwhdr);
bfa_status_t bfa_ioc_fw_stats_get(struct bfa_ioc_s *ioc, void *stats);
bfa_status_t bfa_ioc_fw_stats_clear(struct bfa_ioc_s *ioc);

/*
 * asic block configuration related APIs
 */
u32	bfa_ablk_meminfo(void);
void bfa_ablk_memclaim(struct bfa_ablk_s *ablk, u8 *dma_kva, u64 dma_pa);
void bfa_ablk_attach(struct bfa_ablk_s *ablk, struct bfa_ioc_s *ioc);
bfa_status_t bfa_ablk_query(struct bfa_ablk_s *ablk,
		struct bfa_ablk_cfg_s *ablk_cfg,
		bfa_ablk_cbfn_t cbfn, void *cbarg);
bfa_status_t bfa_ablk_adapter_config(struct bfa_ablk_s *ablk,
		enum bfa_mode_s mode, int max_pf, int max_vf,
		bfa_ablk_cbfn_t cbfn, void *cbarg);
bfa_status_t bfa_ablk_port_config(struct bfa_ablk_s *ablk, int port,
		enum bfa_mode_s mode, int max_pf, int max_vf,
		bfa_ablk_cbfn_t cbfn, void *cbarg);
bfa_status_t bfa_ablk_pf_create(struct bfa_ablk_s *ablk, u16 *pcifn,
		u8 port, enum bfi_pcifn_class personality, int bw,
		bfa_ablk_cbfn_t cbfn, void *cbarg);
bfa_status_t bfa_ablk_pf_delete(struct bfa_ablk_s *ablk, int pcifn,
		bfa_ablk_cbfn_t cbfn, void *cbarg);
bfa_status_t bfa_ablk_pf_update(struct bfa_ablk_s *ablk, int pcifn, int bw,
		bfa_ablk_cbfn_t cbfn, void *cbarg);
bfa_status_t bfa_ablk_optrom_en(struct bfa_ablk_s *ablk,
		bfa_ablk_cbfn_t cbfn, void *cbarg);
bfa_status_t bfa_ablk_optrom_dis(struct bfa_ablk_s *ablk,
		bfa_ablk_cbfn_t cbfn, void *cbarg);

/*
 * bfa mfg wwn API functions
 */
mac_t bfa_ioc_get_mac(struct bfa_ioc_s *ioc);
mac_t bfa_ioc_get_mfg_mac(struct bfa_ioc_s *ioc);

/*
 * F/W Image Size & Chunk
 */
extern u32 bfi_image_cb_size;
extern u32 bfi_image_ct_size;
extern u32 bfi_image_ct2_size;
extern u32 *bfi_image_cb;
extern u32 *bfi_image_ct;
extern u32 *bfi_image_ct2;

static inline u32 *
bfi_image_cb_get_chunk(u32 off)
{
	return (u32 *)(bfi_image_cb + off);
}

static inline u32 *
bfi_image_ct_get_chunk(u32 off)
{
	return (u32 *)(bfi_image_ct + off);
}

static inline u32 *
bfi_image_ct2_get_chunk(u32 off)
{
	return (u32 *)(bfi_image_ct2 + off);
}

static inline u32*
bfa_cb_image_get_chunk(enum bfi_asic_gen asic_gen, u32 off)
{
	switch (asic_gen) {
	case BFI_ASIC_GEN_CB:
		return bfi_image_cb_get_chunk(off);
		break;
	case BFI_ASIC_GEN_CT:
		return bfi_image_ct_get_chunk(off);
		break;
	case BFI_ASIC_GEN_CT2:
		return bfi_image_ct2_get_chunk(off);
		break;
	default:
		return NULL;
	}
}

static inline u32
bfa_cb_image_get_size(enum bfi_asic_gen asic_gen)
{
	switch (asic_gen) {
	case BFI_ASIC_GEN_CB:
		return bfi_image_cb_size;
		break;
	case BFI_ASIC_GEN_CT:
		return bfi_image_ct_size;
		break;
	case BFI_ASIC_GEN_CT2:
		return bfi_image_ct2_size;
		break;
	default:
		return 0;
	}
}

/*
 * CNA TRCMOD declaration
 */
/*
 * !!! Only append to the enums defined here to avoid any versioning
 * !!! needed between trace utility and driver version
 */
enum {
	BFA_TRC_CNA_PORT	= 1,
	BFA_TRC_CNA_IOC		= 2,
	BFA_TRC_CNA_IOC_CB	= 3,
	BFA_TRC_CNA_IOC_CT	= 4,
};

#endif /* __BFA_IOC_H__ */
