/*
 * OMAP44xx and 54xx PRM common functions
 *
 * Copyright (C) 2009-2013 Texas Instruments, Inc.
 * Copyright (C) 2009-2010 Nokia Corporation
 *
 * Paul Walmsley (paul@pwsan.com)
 * Rajendra Nayak (rnayak@ti.com)
 * Benoit Cousson (b-cousson@ti.com)
 *
 * This file is automatically generated from the OMAP hardware databases.
 * We respectfully ask that any modifications to this file be coordinated
 * with the public linux-omap@vger.kernel.org mailing list and the
 * authors above to ensure that the autogeneration scripts are kept
 * up-to-date with the file contents.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __ARCH_ARM_MACH_OMAP2_PRM44XX_54XX_H
#define __ARCH_ARM_MACH_OMAP2_PRM44XX_54XX_H

/* Function prototypes */
#ifndef __ASSEMBLER__

extern u32 omap4_prm_read_inst_reg(s16 inst, u16 idx);
extern void omap4_prm_write_inst_reg(u32 val, s16 inst, u16 idx);
extern u32 omap4_prm_rmw_inst_reg_bits(u32 mask, u32 bits, s16 inst, s16 idx);

/* OMAP4/OMAP5-specific VP functions */
u32 omap4_prm_vp_check_txdone(u8 vp_id);
void omap4_prm_vp_clear_txdone(u8 vp_id);

/*
 * OMAP4/OMAP5 access functions for voltage controller (VC) and
 * voltage proccessor (VP) in the PRM.
 */
extern u32 omap4_prm_vcvp_read(u8 offset);
extern void omap4_prm_vcvp_write(u32 val, u8 offset);
extern u32 omap4_prm_vcvp_rmw(u32 mask, u32 bits, u8 offset);

extern void omap44xx_prm_reconfigure_io_chain(void);

/* PRM interrupt-related functions */
extern void omap44xx_prm_read_pending_irqs(unsigned long *events);
extern void omap44xx_prm_ocp_barrier(void);
extern void omap44xx_prm_save_and_clear_irqen(u32 *saved_mask);
extern void omap44xx_prm_restore_irqen(u32 *saved_mask);

extern int __init omap44xx_prm_init(void);
extern u32 omap44xx_prm_get_reset_sources(void);

#endif

#endif
