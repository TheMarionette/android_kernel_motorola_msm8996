#ifndef __ASM_CPU_SH2A_CACHEFLUSH_H
#define __ASM_CPU_SH2A_CACHEFLUSH_H

/* 
 * Cache flushing:
 *
 *  - flush_cache_all() flushes entire cache
 *  - flush_cache_mm(mm) flushes the specified mm context's cache lines
 *  - flush_cache_dup mm(mm) handles cache flushing when forking
 *  - flush_cache_page(mm, vmaddr, pfn) flushes a single page
 *  - flush_cache_range(vma, start, end) flushes a range of pages
 *
 *  - flush_dcache_page(pg) flushes(wback&invalidates) a page for dcache
 *  - flush_icache_range(start, end) flushes(invalidates) a range for icache
 *  - flush_icache_page(vma, pg) flushes(invalidates) a page for icache
 *
 *  Caches are indexed (effectively) by physical address on SH-2, so
 *  we don't need them.
 */
#define flush_cache_all()			do { } while (0)
#define flush_cache_mm(mm)			do { } while (0)
#define flush_cache_dup_mm(mm)			do { } while (0)
#define flush_cache_range(vma, start, end)	do { } while (0)
#define flush_cache_page(vma, vmaddr, pfn)	do { } while (0)
#define flush_dcache_page(page)			do { } while (0)
void flush_icache_range(unsigned long start, unsigned long end);
#define flush_icache_page(vma,pg)		do { } while (0)
#define flush_cache_sigtramp(vaddr)		do { } while (0)

#endif /* __ASM_CPU_SH2A_CACHEFLUSH_H */
