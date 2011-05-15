/*
 * arch/arm/plat-orion/include/plat/common.h
 *
 * Marvell Orion SoC common setup code used by different mach-/common.c
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __PLAT_COMMON_H
#include <linux/mv643xx_eth.h>

struct dsa_platform_data;

void __init orion_uart0_init(unsigned int membase,
			     resource_size_t mapbase,
			     unsigned int irq,
			     unsigned int uartclk);

void __init orion_uart1_init(unsigned int membase,
			     resource_size_t mapbase,
			     unsigned int irq,
			     unsigned int uartclk);

void __init orion_uart2_init(unsigned int membase,
			     resource_size_t mapbase,
			     unsigned int irq,
			     unsigned int uartclk);

void __init orion_uart3_init(unsigned int membase,
			     resource_size_t mapbase,
			     unsigned int irq,
			     unsigned int uartclk);

void __init orion_rtc_init(unsigned long mapbase,
			   unsigned long irq);

void __init orion_ge00_init(struct mv643xx_eth_platform_data *eth_data,
			    struct mbus_dram_target_info *mbus_dram_info,
			    unsigned long mapbase,
			    unsigned long irq,
			    unsigned long irq_err,
			    int tclk);

void __init orion_ge01_init(struct mv643xx_eth_platform_data *eth_data,
			    struct mbus_dram_target_info *mbus_dram_info,
			    unsigned long mapbase,
			    unsigned long irq,
			    unsigned long irq_err,
			    int tclk);

void __init orion_ge10_init(struct mv643xx_eth_platform_data *eth_data,
			    struct mbus_dram_target_info *mbus_dram_info,
			    unsigned long mapbase,
			    unsigned long irq,
			    unsigned long irq_err,
			    int tclk);

void __init orion_ge11_init(struct mv643xx_eth_platform_data *eth_data,
			    struct mbus_dram_target_info *mbus_dram_info,
			    unsigned long mapbase,
			    unsigned long irq,
			    unsigned long irq_err,
			    int tclk);

void __init orion_ge00_switch_init(struct dsa_platform_data *d,
				   int irq);

#endif
