#ifndef __ASM_ARCH_LITTLETON_H
#define __ASM_ARCH_LITTLETON_H

#include <mach/gpio-pxa.h>

#define LITTLETON_ETH_PHYS	0x30000000

#define LITTLETON_GPIO_LCD_CS	(17)

#define EXT0_GPIO_BASE	(PXA_NR_BUILTIN_GPIO)
#define EXT0_GPIO(x)	(EXT0_GPIO_BASE + (x))

#define LITTLETON_NR_IRQS	(IRQ_BOARD_START + 8)

#endif /* __ASM_ARCH_LITTLETON_H */
