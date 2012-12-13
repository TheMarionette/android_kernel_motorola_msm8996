#ifndef _ASM_IRQ_H
#define _ASM_IRQ_H

#include <linux/hardirq.h>
#include <linux/types.h>

enum interruption_class {
	EXTERNAL_INTERRUPT,
	IO_INTERRUPT,
	EXTINT_CLK,
	EXTINT_EXC,
	EXTINT_EMS,
	EXTINT_TMR,
	EXTINT_TLA,
	EXTINT_PFL,
	EXTINT_DSD,
	EXTINT_VRT,
	EXTINT_SCP,
	EXTINT_IUC,
	EXTINT_CMS,
	EXTINT_CMC,
	EXTINT_CMR,
	IOINT_CIO,
	IOINT_QAI,
	IOINT_DAS,
	IOINT_C15,
	IOINT_C70,
	IOINT_TAP,
	IOINT_VMR,
	IOINT_LCS,
	IOINT_CLW,
	IOINT_CTC,
	IOINT_APB,
	IOINT_ADM,
	IOINT_CSC,
	IOINT_PCI,
	IOINT_MSI,
	NMI_NMI,
	NR_IRQS,
};

struct ext_code {
	unsigned short subcode;
	unsigned short code;
};

typedef void (*ext_int_handler_t)(struct ext_code, unsigned int, unsigned long);

int register_external_interrupt(u16 code, ext_int_handler_t handler);
int unregister_external_interrupt(u16 code, ext_int_handler_t handler);
void service_subclass_irq_register(void);
void service_subclass_irq_unregister(void);
void measurement_alert_subclass_register(void);
void measurement_alert_subclass_unregister(void);

#ifdef CONFIG_LOCKDEP
#  define disable_irq_nosync_lockdep(irq)	disable_irq_nosync(irq)
#  define disable_irq_nosync_lockdep_irqsave(irq, flags) \
						disable_irq_nosync(irq)
#  define disable_irq_lockdep(irq)		disable_irq(irq)
#  define enable_irq_lockdep(irq)		enable_irq(irq)
#  define enable_irq_lockdep_irqrestore(irq, flags) \
						enable_irq(irq)
#endif

#endif /* _ASM_IRQ_H */
