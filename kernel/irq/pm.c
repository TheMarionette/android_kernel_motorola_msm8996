/*
 * linux/kernel/irq/pm.c
 *
 * Copyright (C) 2009 Rafael J. Wysocki <rjw@sisk.pl>, Novell Inc.
 *
 * This file contains power management functions related to interrupts.
 */

#include <linux/irq.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#include "internals.h"

/**
 * suspend_device_irqs - disable all currently enabled interrupt lines
 *
 * During system-wide suspend or hibernation device drivers need to be prevented
 * from receiving interrupts and this function is provided for this purpose.
 * It marks all interrupt lines in use, except for the timer ones, as disabled
 * and sets the IRQS_SUSPENDED flag for each of them.
 */
void suspend_device_irqs(void)
{
	struct irq_desc *desc;
	int irq;

	for_each_irq_desc(irq, desc) {
		unsigned long flags;

		raw_spin_lock_irqsave(&desc->lock, flags);
		__disable_irq(desc, irq, true);
		raw_spin_unlock_irqrestore(&desc->lock, flags);
	}

	for_each_irq_desc(irq, desc)
		if (desc->istate & IRQS_SUSPENDED)
			synchronize_irq(irq);
}
EXPORT_SYMBOL_GPL(suspend_device_irqs);

/**
 * resume_device_irqs - enable interrupt lines disabled by suspend_device_irqs()
 *
 * Enable all interrupt lines previously disabled by suspend_device_irqs() that
 * have the IRQS_SUSPENDED flag set.
 */
void resume_device_irqs(void)
{
	struct irq_desc *desc;
	int irq;

	for_each_irq_desc(irq, desc) {
		unsigned long flags;

		raw_spin_lock_irqsave(&desc->lock, flags);
		__enable_irq(desc, irq, true);
		raw_spin_unlock_irqrestore(&desc->lock, flags);
	}
}
EXPORT_SYMBOL_GPL(resume_device_irqs);

/**
 * check_wakeup_irqs - check if any wake-up interrupts are pending
 */
int check_wakeup_irqs(void)
{
	struct irq_desc *desc;
	int irq;

	for_each_irq_desc(irq, desc) {
		if (irqd_is_wakeup_set(&desc->irq_data)) {
			if (desc->istate & IRQS_PENDING)
				return -EBUSY;
			continue;
		}
		/*
		 * Check the non wakeup interrupts whether they need
		 * to be masked before finally going into suspend
		 * state. That's for hardware which has no wakeup
		 * source configuration facility. The chip
		 * implementation indicates that with
		 * IRQCHIP_MASK_ON_SUSPEND.
		 */
		if (desc->istate & IRQS_SUSPENDED &&
		    irq_desc_get_chip(desc)->flags & IRQCHIP_MASK_ON_SUSPEND)
			mask_irq(desc);
	}

	return 0;
}
