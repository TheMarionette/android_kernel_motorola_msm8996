#ifndef __NFS_NETNS_H__
#define __NFS_NETNS_H__

#include <net/net_namespace.h>
#include <net/netns/generic.h>

struct nfs_net {
	struct cache_detail *nfs_dns_resolve;
	struct rpc_pipe *bl_device_pipe;
	struct list_head nfs_client_list;
	struct list_head nfs_volume_list;
#ifdef CONFIG_NFS_V4
	struct idr cb_ident_idr; /* Protected by nfs_client_lock */
#endif
};

extern int nfs_net_id;

#endif
