#ifndef _LINUX_SWSUSP_H
#define _LINUX_SWSUSP_H

#if defined(CONFIG_X86) || defined(CONFIG_FRV) || defined(CONFIG_PPC32) || defined(CONFIG_PPC64)
#include <asm/suspend.h>
#endif
#include <linux/swap.h>
#include <linux/notifier.h>
#include <linux/init.h>
#include <linux/pm.h>
#include <linux/mm.h>

/* struct pbe is used for creating lists of pages that should be restored
 * atomically during the resume from disk, because the page frames they have
 * occupied before the suspend are in use.
 */
struct pbe {
	void *address;		/* address of the copy */
	void *orig_address;	/* original address of a page */
	struct pbe *next;
};

/* mm/page_alloc.c */
extern void drain_local_pages(void);
extern void mark_free_pages(struct zone *zone);

#if defined(CONFIG_PM) && defined(CONFIG_VT) && defined(CONFIG_VT_CONSOLE)
extern int pm_prepare_console(void);
extern void pm_restore_console(void);
#else
static inline int pm_prepare_console(void) { return 0; }
static inline void pm_restore_console(void) {}
#endif

/**
 * struct hibernation_ops - hibernation platform support
 *
 * The methods in this structure allow a platform to override the default
 * mechanism of shutting down the machine during a hibernation transition.
 *
 * All three methods must be assigned.
 *
 * @prepare: prepare system for hibernation
 * @enter: shut down system after state has been saved to disk
 * @finish: finish/clean up after state has been reloaded
 */
struct hibernation_ops {
	int (*prepare)(void);
	int (*enter)(void);
	void (*finish)(void);
};

#if defined(CONFIG_PM) && defined(CONFIG_SOFTWARE_SUSPEND)
/* kernel/power/snapshot.c */
extern void __register_nosave_region(unsigned long b, unsigned long e, int km);
static inline void register_nosave_region(unsigned long b, unsigned long e)
{
	__register_nosave_region(b, e, 0);
}
static inline void register_nosave_region_late(unsigned long b, unsigned long e)
{
	__register_nosave_region(b, e, 1);
}
extern int swsusp_page_is_forbidden(struct page *);
extern void swsusp_set_page_free(struct page *);
extern void swsusp_unset_page_free(struct page *);
extern unsigned long get_safe_page(gfp_t gfp_mask);

extern void hibernation_set_ops(struct hibernation_ops *ops);
extern int hibernate(void);
#else
static inline void register_nosave_region(unsigned long b, unsigned long e) {}
static inline void register_nosave_region_late(unsigned long b, unsigned long e) {}
static inline int swsusp_page_is_forbidden(struct page *p) { return 0; }
static inline void swsusp_set_page_free(struct page *p) {}
static inline void swsusp_unset_page_free(struct page *p) {}

static inline void hibernation_set_ops(struct hibernation_ops *ops) {}
static inline int hibernate(void) { return -ENOSYS; }
#endif /* defined(CONFIG_PM) && defined(CONFIG_SOFTWARE_SUSPEND) */

void save_processor_state(void);
void restore_processor_state(void);
struct saved_context;
void __save_processor_state(struct saved_context *ctxt);
void __restore_processor_state(struct saved_context *ctxt);

#endif /* _LINUX_SWSUSP_H */
