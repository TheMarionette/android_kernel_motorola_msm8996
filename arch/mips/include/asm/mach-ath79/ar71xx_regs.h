/*
 *  Atheros AR71XX/AR724X/AR913X SoC register definitions
 *
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef __ASM_MACH_AR71XX_REGS_H
#define __ASM_MACH_AR71XX_REGS_H

#include <linux/types.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/bitops.h>

#define AR71XX_APB_BASE		0x18000000
#define AR71XX_EHCI_BASE	0x1b000000
#define AR71XX_EHCI_SIZE	0x1000
#define AR71XX_OHCI_BASE	0x1c000000
#define AR71XX_OHCI_SIZE	0x1000
#define AR71XX_SPI_BASE		0x1f000000
#define AR71XX_SPI_SIZE		0x01000000

#define AR71XX_DDR_CTRL_BASE	(AR71XX_APB_BASE + 0x00000000)
#define AR71XX_DDR_CTRL_SIZE	0x100
#define AR71XX_UART_BASE	(AR71XX_APB_BASE + 0x00020000)
#define AR71XX_UART_SIZE	0x100
#define AR71XX_USB_CTRL_BASE	(AR71XX_APB_BASE + 0x00030000)
#define AR71XX_USB_CTRL_SIZE	0x100
#define AR71XX_GPIO_BASE        (AR71XX_APB_BASE + 0x00040000)
#define AR71XX_GPIO_SIZE        0x100
#define AR71XX_PLL_BASE		(AR71XX_APB_BASE + 0x00050000)
#define AR71XX_PLL_SIZE		0x100
#define AR71XX_RESET_BASE	(AR71XX_APB_BASE + 0x00060000)
#define AR71XX_RESET_SIZE	0x100

#define AR7240_USB_CTRL_BASE	(AR71XX_APB_BASE + 0x00030000)
#define AR7240_USB_CTRL_SIZE	0x100
#define AR7240_OHCI_BASE	0x1b000000
#define AR7240_OHCI_SIZE	0x1000

#define AR724X_EHCI_BASE	0x1b000000
#define AR724X_EHCI_SIZE	0x1000

#define AR913X_EHCI_BASE	0x1b000000
#define AR913X_EHCI_SIZE	0x1000
#define AR913X_WMAC_BASE	(AR71XX_APB_BASE + 0x000C0000)
#define AR913X_WMAC_SIZE	0x30000

/*
 * DDR_CTRL block
 */
#define AR71XX_DDR_REG_PCI_WIN0		0x7c
#define AR71XX_DDR_REG_PCI_WIN1		0x80
#define AR71XX_DDR_REG_PCI_WIN2		0x84
#define AR71XX_DDR_REG_PCI_WIN3		0x88
#define AR71XX_DDR_REG_PCI_WIN4		0x8c
#define AR71XX_DDR_REG_PCI_WIN5		0x90
#define AR71XX_DDR_REG_PCI_WIN6		0x94
#define AR71XX_DDR_REG_PCI_WIN7		0x98
#define AR71XX_DDR_REG_FLUSH_GE0	0x9c
#define AR71XX_DDR_REG_FLUSH_GE1	0xa0
#define AR71XX_DDR_REG_FLUSH_USB	0xa4
#define AR71XX_DDR_REG_FLUSH_PCI	0xa8

#define AR724X_DDR_REG_FLUSH_GE0	0x7c
#define AR724X_DDR_REG_FLUSH_GE1	0x80
#define AR724X_DDR_REG_FLUSH_USB	0x84
#define AR724X_DDR_REG_FLUSH_PCIE	0x88

#define AR913X_DDR_REG_FLUSH_GE0	0x7c
#define AR913X_DDR_REG_FLUSH_GE1	0x80
#define AR913X_DDR_REG_FLUSH_USB	0x84
#define AR913X_DDR_REG_FLUSH_WMAC	0x88

/*
 * PLL block
 */
#define AR71XX_PLL_REG_CPU_CONFIG	0x00
#define AR71XX_PLL_REG_SEC_CONFIG	0x04
#define AR71XX_PLL_REG_ETH0_INT_CLOCK	0x10
#define AR71XX_PLL_REG_ETH1_INT_CLOCK	0x14

#define AR71XX_PLL_DIV_SHIFT		3
#define AR71XX_PLL_DIV_MASK		0x1f
#define AR71XX_CPU_DIV_SHIFT		16
#define AR71XX_CPU_DIV_MASK		0x3
#define AR71XX_DDR_DIV_SHIFT		18
#define AR71XX_DDR_DIV_MASK		0x3
#define AR71XX_AHB_DIV_SHIFT		20
#define AR71XX_AHB_DIV_MASK		0x7

#define AR724X_PLL_REG_CPU_CONFIG	0x00
#define AR724X_PLL_REG_PCIE_CONFIG	0x18

#define AR724X_PLL_DIV_SHIFT		0
#define AR724X_PLL_DIV_MASK		0x3ff
#define AR724X_PLL_REF_DIV_SHIFT	10
#define AR724X_PLL_REF_DIV_MASK		0xf
#define AR724X_AHB_DIV_SHIFT		19
#define AR724X_AHB_DIV_MASK		0x1
#define AR724X_DDR_DIV_SHIFT		22
#define AR724X_DDR_DIV_MASK		0x3

#define AR913X_PLL_REG_CPU_CONFIG	0x00
#define AR913X_PLL_REG_ETH_CONFIG	0x04
#define AR913X_PLL_REG_ETH0_INT_CLOCK	0x14
#define AR913X_PLL_REG_ETH1_INT_CLOCK	0x18

#define AR913X_PLL_DIV_SHIFT		0
#define AR913X_PLL_DIV_MASK		0x3ff
#define AR913X_DDR_DIV_SHIFT		22
#define AR913X_DDR_DIV_MASK		0x3
#define AR913X_AHB_DIV_SHIFT		19
#define AR913X_AHB_DIV_MASK		0x1

/*
 * USB_CONFIG block
 */
#define AR71XX_USB_CTRL_REG_FLADJ	0x00
#define AR71XX_USB_CTRL_REG_CONFIG	0x04

/*
 * RESET block
 */
#define AR71XX_RESET_REG_TIMER			0x00
#define AR71XX_RESET_REG_TIMER_RELOAD		0x04
#define AR71XX_RESET_REG_WDOG_CTRL		0x08
#define AR71XX_RESET_REG_WDOG			0x0c
#define AR71XX_RESET_REG_MISC_INT_STATUS	0x10
#define AR71XX_RESET_REG_MISC_INT_ENABLE	0x14
#define AR71XX_RESET_REG_PCI_INT_STATUS		0x18
#define AR71XX_RESET_REG_PCI_INT_ENABLE		0x1c
#define AR71XX_RESET_REG_GLOBAL_INT_STATUS	0x20
#define AR71XX_RESET_REG_RESET_MODULE		0x24
#define AR71XX_RESET_REG_PERFC_CTRL		0x2c
#define AR71XX_RESET_REG_PERFC0			0x30
#define AR71XX_RESET_REG_PERFC1			0x34
#define AR71XX_RESET_REG_REV_ID			0x90

#define AR913X_RESET_REG_GLOBAL_INT_STATUS	0x18
#define AR913X_RESET_REG_RESET_MODULE		0x1c
#define AR913X_RESET_REG_PERF_CTRL		0x20
#define AR913X_RESET_REG_PERFC0			0x24
#define AR913X_RESET_REG_PERFC1			0x28

#define AR724X_RESET_REG_RESET_MODULE		0x1c

#define MISC_INT_ETHSW			BIT(12)
#define MISC_INT_TIMER4			BIT(10)
#define MISC_INT_TIMER3			BIT(9)
#define MISC_INT_TIMER2			BIT(8)
#define MISC_INT_DMA			BIT(7)
#define MISC_INT_OHCI			BIT(6)
#define MISC_INT_PERFC			BIT(5)
#define MISC_INT_WDOG			BIT(4)
#define MISC_INT_UART			BIT(3)
#define MISC_INT_GPIO			BIT(2)
#define MISC_INT_ERROR			BIT(1)
#define MISC_INT_TIMER			BIT(0)

#define AR71XX_RESET_EXTERNAL		BIT(28)
#define AR71XX_RESET_FULL_CHIP		BIT(24)
#define AR71XX_RESET_CPU_NMI		BIT(21)
#define AR71XX_RESET_CPU_COLD		BIT(20)
#define AR71XX_RESET_DMA		BIT(19)
#define AR71XX_RESET_SLIC		BIT(18)
#define AR71XX_RESET_STEREO		BIT(17)
#define AR71XX_RESET_DDR		BIT(16)
#define AR71XX_RESET_GE1_MAC		BIT(13)
#define AR71XX_RESET_GE1_PHY		BIT(12)
#define AR71XX_RESET_USBSUS_OVERRIDE	BIT(10)
#define AR71XX_RESET_GE0_MAC		BIT(9)
#define AR71XX_RESET_GE0_PHY		BIT(8)
#define AR71XX_RESET_USB_OHCI_DLL	BIT(6)
#define AR71XX_RESET_USB_HOST		BIT(5)
#define AR71XX_RESET_USB_PHY		BIT(4)
#define AR71XX_RESET_PCI_BUS		BIT(1)
#define AR71XX_RESET_PCI_CORE		BIT(0)

#define AR7240_RESET_USB_HOST		BIT(5)
#define AR7240_RESET_OHCI_DLL		BIT(3)

#define AR724X_RESET_GE1_MDIO		BIT(23)
#define AR724X_RESET_GE0_MDIO		BIT(22)
#define AR724X_RESET_PCIE_PHY_SERIAL	BIT(10)
#define AR724X_RESET_PCIE_PHY		BIT(7)
#define AR724X_RESET_PCIE		BIT(6)
#define AR724X_RESET_USB_HOST		BIT(5)
#define AR724X_RESET_USB_PHY		BIT(4)
#define AR724X_RESET_USBSUS_OVERRIDE	BIT(3)

#define AR913X_RESET_AMBA2WMAC		BIT(22)
#define AR913X_RESET_USBSUS_OVERRIDE	BIT(10)
#define AR913X_RESET_USB_HOST		BIT(5)
#define AR913X_RESET_USB_PHY		BIT(4)

#define REV_ID_MAJOR_MASK		0xfff0
#define REV_ID_MAJOR_AR71XX		0x00a0
#define REV_ID_MAJOR_AR913X		0x00b0
#define REV_ID_MAJOR_AR7240		0x00c0
#define REV_ID_MAJOR_AR7241		0x0100
#define REV_ID_MAJOR_AR7242		0x1100
#define REV_ID_MAJOR_AR9330		0x0110
#define REV_ID_MAJOR_AR9331		0x1110

#define AR71XX_REV_ID_MINOR_MASK	0x3
#define AR71XX_REV_ID_MINOR_AR7130	0x0
#define AR71XX_REV_ID_MINOR_AR7141	0x1
#define AR71XX_REV_ID_MINOR_AR7161	0x2
#define AR71XX_REV_ID_REVISION_MASK	0x3
#define AR71XX_REV_ID_REVISION_SHIFT	2

#define AR913X_REV_ID_MINOR_MASK	0x3
#define AR913X_REV_ID_MINOR_AR9130	0x0
#define AR913X_REV_ID_MINOR_AR9132	0x1
#define AR913X_REV_ID_REVISION_MASK	0x3
#define AR913X_REV_ID_REVISION_SHIFT	2

#define AR933X_REV_ID_REVISION_MASK	0x3

#define AR724X_REV_ID_REVISION_MASK	0x3

/*
 * SPI block
 */
#define AR71XX_SPI_REG_FS	0x00	/* Function Select */
#define AR71XX_SPI_REG_CTRL	0x04	/* SPI Control */
#define AR71XX_SPI_REG_IOC	0x08	/* SPI I/O Control */
#define AR71XX_SPI_REG_RDS	0x0c	/* Read Data Shift */

#define AR71XX_SPI_FS_GPIO	BIT(0)	/* Enable GPIO mode */

#define AR71XX_SPI_CTRL_RD	BIT(6)	/* Remap Disable */
#define AR71XX_SPI_CTRL_DIV_MASK 0x3f

#define AR71XX_SPI_IOC_DO	BIT(0)	/* Data Out pin */
#define AR71XX_SPI_IOC_CLK	BIT(8)	/* CLK pin */
#define AR71XX_SPI_IOC_CS(n)	BIT(16 + (n))
#define AR71XX_SPI_IOC_CS0	AR71XX_SPI_IOC_CS(0)
#define AR71XX_SPI_IOC_CS1	AR71XX_SPI_IOC_CS(1)
#define AR71XX_SPI_IOC_CS2	AR71XX_SPI_IOC_CS(2)
#define AR71XX_SPI_IOC_CS_ALL	(AR71XX_SPI_IOC_CS0 | AR71XX_SPI_IOC_CS1 | \
				 AR71XX_SPI_IOC_CS2)

/*
 * GPIO block
 */
#define AR71XX_GPIO_REG_OE		0x00
#define AR71XX_GPIO_REG_IN		0x04
#define AR71XX_GPIO_REG_OUT		0x08
#define AR71XX_GPIO_REG_SET		0x0c
#define AR71XX_GPIO_REG_CLEAR		0x10
#define AR71XX_GPIO_REG_INT_MODE	0x14
#define AR71XX_GPIO_REG_INT_TYPE	0x18
#define AR71XX_GPIO_REG_INT_POLARITY	0x1c
#define AR71XX_GPIO_REG_INT_PENDING	0x20
#define AR71XX_GPIO_REG_INT_ENABLE	0x24
#define AR71XX_GPIO_REG_FUNC		0x28

#define AR71XX_GPIO_COUNT		16
#define AR724X_GPIO_COUNT		18
#define AR913X_GPIO_COUNT		22

#endif /* __ASM_MACH_AR71XX_REGS_H */
