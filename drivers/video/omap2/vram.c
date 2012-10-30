/*
 * VRAM manager for OMAP
 *
 * Copyright (C) 2009 Nokia Corporation
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*#define DEBUG*/

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/memblock.h>
#include <linux/completion.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/module.h>

#include <asm/setup.h>

#include <plat/vram.h>

#ifdef DEBUG
#define DBG(format, ...) pr_debug("VRAM: " format, ## __VA_ARGS__)
#else
#define DBG(format, ...)
#endif

/* postponed regions are used to temporarily store region information at boot
 * time when we cannot yet allocate the region list */
#define MAX_POSTPONED_REGIONS 10

static bool vram_initialized;
static int postponed_cnt;
static struct {
	unsigned long paddr;
	size_t size;
} postponed_regions[MAX_POSTPONED_REGIONS];

struct vram_alloc {
	struct list_head list;
	unsigned long paddr;
	unsigned pages;
};

struct vram_region {
	struct list_head list;
	struct list_head alloc_list;
	unsigned long paddr;
	unsigned pages;
};

static DEFINE_MUTEX(region_mutex);
static LIST_HEAD(region_list);

static struct vram_region *omap_vram_create_region(unsigned long paddr,
		unsigned pages)
{
	struct vram_region *rm;

	rm = kzalloc(sizeof(*rm), GFP_KERNEL);

	if (rm) {
		INIT_LIST_HEAD(&rm->alloc_list);
		rm->paddr = paddr;
		rm->pages = pages;
	}

	return rm;
}

#if 0
static void omap_vram_free_region(struct vram_region *vr)
{
	list_del(&vr->list);
	kfree(vr);
}
#endif

static struct vram_alloc *omap_vram_create_allocation(struct vram_region *vr,
		unsigned long paddr, unsigned pages)
{
	struct vram_alloc *va;
	struct vram_alloc *new;

	new = kzalloc(sizeof(*va), GFP_KERNEL);

	if (!new)
		return NULL;

	new->paddr = paddr;
	new->pages = pages;

	list_for_each_entry(va, &vr->alloc_list, list) {
		if (va->paddr > new->paddr)
			break;
	}

	list_add_tail(&new->list, &va->list);

	return new;
}

static void omap_vram_free_allocation(struct vram_alloc *va)
{
	list_del(&va->list);
	kfree(va);
}

int omap_vram_add_region(unsigned long paddr, size_t size)
{
	struct vram_region *rm;
	unsigned pages;

	if (vram_initialized) {
		DBG("adding region paddr %08lx size %d\n",
				paddr, size);

		size &= PAGE_MASK;
		pages = size >> PAGE_SHIFT;

		rm = omap_vram_create_region(paddr, pages);
		if (rm == NULL)
			return -ENOMEM;

		list_add(&rm->list, &region_list);
	} else {
		if (postponed_cnt == MAX_POSTPONED_REGIONS)
			return -ENOMEM;

		postponed_regions[postponed_cnt].paddr = paddr;
		postponed_regions[postponed_cnt].size = size;

		++postponed_cnt;
	}
	return 0;
}

int omap_vram_free(unsigned long paddr, size_t size)
{
	struct vram_region *rm;
	struct vram_alloc *alloc;
	unsigned start, end;

	DBG("free mem paddr %08lx size %d\n", paddr, size);

	size = PAGE_ALIGN(size);

	mutex_lock(&region_mutex);

	list_for_each_entry(rm, &region_list, list) {
		list_for_each_entry(alloc, &rm->alloc_list, list) {
			start = alloc->paddr;
			end = alloc->paddr + (alloc->pages >> PAGE_SHIFT);

			if (start >= paddr && end < paddr + size)
				goto found;
		}
	}

	mutex_unlock(&region_mutex);
	return -EINVAL;

found:
	omap_vram_free_allocation(alloc);

	mutex_unlock(&region_mutex);
	return 0;
}
EXPORT_SYMBOL(omap_vram_free);

static int _omap_vram_reserve(unsigned long paddr, unsigned pages)
{
	struct vram_region *rm;
	struct vram_alloc *alloc;
	size_t size;

	size = pages << PAGE_SHIFT;

	list_for_each_entry(rm, &region_list, list) {
		unsigned long start, end;

		DBG("checking region %lx %d\n", rm->paddr, rm->pages);

		start = rm->paddr;
		end = start + (rm->pages << PAGE_SHIFT) - 1;
		if (start > paddr || end < paddr + size - 1)
			continue;

		DBG("block ok, checking allocs\n");

		list_for_each_entry(alloc, &rm->alloc_list, list) {
			end = alloc->paddr - 1;

			if (start <= paddr && end >= paddr + size - 1)
				goto found;

			start = alloc->paddr + (alloc->pages << PAGE_SHIFT);
		}

		end = rm->paddr + (rm->pages << PAGE_SHIFT) - 1;

		if (!(start <= paddr && end >= paddr + size - 1))
			continue;
found:
		DBG("found area start %lx, end %lx\n", start, end);

		if (omap_vram_create_allocation(rm, paddr, pages) == NULL)
			return -ENOMEM;

		return 0;
	}

	return -ENOMEM;
}

int omap_vram_reserve(unsigned long paddr, size_t size)
{
	unsigned pages;
	int r;

	DBG("reserve mem paddr %08lx size %d\n", paddr, size);

	size = PAGE_ALIGN(size);
	pages = size >> PAGE_SHIFT;

	mutex_lock(&region_mutex);

	r = _omap_vram_reserve(paddr, pages);

	mutex_unlock(&region_mutex);

	return r;
}
EXPORT_SYMBOL(omap_vram_reserve);

static int _omap_vram_alloc(unsigned pages, unsigned long *paddr)
{
	struct vram_region *rm;
	struct vram_alloc *alloc;

	list_for_each_entry(rm, &region_list, list) {
		unsigned long start, end;

		DBG("checking region %lx %d\n", rm->paddr, rm->pages);

		start = rm->paddr;

		list_for_each_entry(alloc, &rm->alloc_list, list) {
			end = alloc->paddr;

			if (end - start >= pages << PAGE_SHIFT)
				goto found;

			start = alloc->paddr + (alloc->pages << PAGE_SHIFT);
		}

		end = rm->paddr + (rm->pages << PAGE_SHIFT);
found:
		if (end - start < pages << PAGE_SHIFT)
			continue;

		DBG("found %lx, end %lx\n", start, end);

		alloc = omap_vram_create_allocation(rm, start, pages);
		if (alloc == NULL)
			return -ENOMEM;

		*paddr = start;

		return 0;
	}

	return -ENOMEM;
}

int omap_vram_alloc(size_t size, unsigned long *paddr)
{
	unsigned pages;
	int r;

	BUG_ON(!size);

	DBG("alloc mem size %d\n", size);

	size = PAGE_ALIGN(size);
	pages = size >> PAGE_SHIFT;

	mutex_lock(&region_mutex);

	r = _omap_vram_alloc(pages, paddr);

	mutex_unlock(&region_mutex);

	return r;
}
EXPORT_SYMBOL(omap_vram_alloc);

void omap_vram_get_info(unsigned long *vram,
		unsigned long *free_vram,
		unsigned long *largest_free_block)
{
	struct vram_region *vr;
	struct vram_alloc *va;

	*vram = 0;
	*free_vram = 0;
	*largest_free_block = 0;

	mutex_lock(&region_mutex);

	list_for_each_entry(vr, &region_list, list) {
		unsigned free;
		unsigned long pa;

		pa = vr->paddr;
		*vram += vr->pages << PAGE_SHIFT;

		list_for_each_entry(va, &vr->alloc_list, list) {
			free = va->paddr - pa;
			*free_vram += free;
			if (free > *largest_free_block)
				*largest_free_block = free;
			pa = va->paddr + (va->pages << PAGE_SHIFT);
		}

		free = vr->paddr + (vr->pages << PAGE_SHIFT) - pa;
		*free_vram += free;
		if (free > *largest_free_block)
			*largest_free_block = free;
	}

	mutex_unlock(&region_mutex);
}
EXPORT_SYMBOL(omap_vram_get_info);

#if defined(CONFIG_DEBUG_FS)
static int vram_debug_show(struct seq_file *s, void *unused)
{
	struct vram_region *vr;
	struct vram_alloc *va;
	unsigned size;

	mutex_lock(&region_mutex);

	list_for_each_entry(vr, &region_list, list) {
		size = vr->pages << PAGE_SHIFT;
		seq_printf(s, "%08lx-%08lx (%d bytes)\n",
				vr->paddr, vr->paddr + size - 1,
				size);

		list_for_each_entry(va, &vr->alloc_list, list) {
			size = va->pages << PAGE_SHIFT;
			seq_printf(s, "    %08lx-%08lx (%d bytes)\n",
					va->paddr, va->paddr + size - 1,
					size);
		}
	}

	mutex_unlock(&region_mutex);

	return 0;
}

static int vram_debug_open(struct inode *inode, struct file *file)
{
	return single_open(file, vram_debug_show, inode->i_private);
}

static const struct file_operations vram_debug_fops = {
	.open           = vram_debug_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

static int __init omap_vram_create_debugfs(void)
{
	struct dentry *d;

	d = debugfs_create_file("vram", S_IRUGO, NULL,
			NULL, &vram_debug_fops);
	if (IS_ERR(d))
		return PTR_ERR(d);

	return 0;
}
#endif

static __init int omap_vram_init(void)
{
	int i;

	vram_initialized = 1;

	for (i = 0; i < postponed_cnt; i++)
		omap_vram_add_region(postponed_regions[i].paddr,
				postponed_regions[i].size);

#ifdef CONFIG_DEBUG_FS
	if (omap_vram_create_debugfs())
		pr_err("VRAM: Failed to create debugfs file\n");
#endif

	return 0;
}

arch_initcall(omap_vram_init);

/* boottime vram alloc stuff */

/* set from board file */
static u32 omap_vram_sdram_start __initdata;
static u32 omap_vram_sdram_size __initdata;

/* set from kernel cmdline */
static u32 omap_vram_def_sdram_size __initdata;
static u32 omap_vram_def_sdram_start __initdata;

static int __init omap_vram_early_vram(char *p)
{
	omap_vram_def_sdram_size = memparse(p, &p);
	if (*p == ',')
		omap_vram_def_sdram_start = simple_strtoul(p + 1, &p, 16);
	return 0;
}
early_param("vram", omap_vram_early_vram);

/*
 * Called from map_io. We need to call to this early enough so that we
 * can reserve the fixed SDRAM regions before VM could get hold of them.
 */
void __init omap_vram_reserve_sdram_memblock(void)
{
	u32 paddr;
	u32 size = 0;

	/* cmdline arg overrides the board file definition */
	if (omap_vram_def_sdram_size) {
		size = omap_vram_def_sdram_size;
		paddr = omap_vram_def_sdram_start;
	}

	if (!size) {
		size = omap_vram_sdram_size;
		paddr = omap_vram_sdram_start;
	}

#ifdef CONFIG_OMAP2_VRAM_SIZE
	if (!size) {
		size = CONFIG_OMAP2_VRAM_SIZE * 1024 * 1024;
		paddr = 0;
	}
#endif

	if (!size)
		return;

	size = ALIGN(size, SZ_2M);

	if (paddr) {
		if (paddr & ~PAGE_MASK) {
			pr_err("VRAM start address 0x%08x not page aligned\n",
					paddr);
			return;
		}

		if (!memblock_is_region_memory(paddr, size)) {
			pr_err("Illegal SDRAM region 0x%08x..0x%08x for VRAM\n",
					paddr, paddr + size - 1);
			return;
		}

		if (memblock_is_region_reserved(paddr, size)) {
			pr_err("FB: failed to reserve VRAM - busy\n");
			return;
		}

		if (memblock_reserve(paddr, size) < 0) {
			pr_err("FB: failed to reserve VRAM - no memory\n");
			return;
		}
	} else {
		paddr = memblock_alloc(size, SZ_2M);
	}

	memblock_free(paddr, size);
	memblock_remove(paddr, size);

	omap_vram_add_region(paddr, size);

	pr_info("Reserving %u bytes SDRAM for VRAM\n", size);
}

void __init omap_vram_set_sdram_vram(u32 size, u32 start)
{
	omap_vram_sdram_start = start;
	omap_vram_sdram_size = size;
}
