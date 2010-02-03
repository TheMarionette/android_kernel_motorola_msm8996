/*
 * arch/arm/mach-kirkwood/net2big_v2-setup.c
 *
 * LaCie 2Big Network v2 board setup
 *
 * Copyright (C) 2010 Simon Guinot <sguinot@lacie.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/physmap.h>
#include <linux/spi/flash.h>
#include <linux/spi/spi.h>
#include <linux/ata_platform.h>
#include <linux/mv643xx_eth.h>
#include <linux/i2c.h>
#include <linux/i2c/at24.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/leds.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <mach/kirkwood.h>
#include <plat/time.h>
#include "common.h"
#include "mpp.h"

/*****************************************************************************
 * 512KB SPI Flash on Boot Device (MACRONIX MX25L4005)
 ****************************************************************************/

static struct mtd_partition net2big_v2_flash_parts[] = {
	{
		.name = "u-boot",
		.size = MTDPART_SIZ_FULL,
		.offset = 0,
		.mask_flags = MTD_WRITEABLE, /* force read-only */
	},
};

static const struct flash_platform_data net2big_v2_flash = {
	.type		= "mx25l4005a",
	.name		= "spi_flash",
	.parts		= net2big_v2_flash_parts,
	.nr_parts	= ARRAY_SIZE(net2big_v2_flash_parts),
};

static struct spi_board_info __initdata net2big_v2_spi_slave_info[] = {
	{
		.modalias	= "m25p80",
		.platform_data	= &net2big_v2_flash,
		.irq		= -1,
		.max_speed_hz	= 20000000,
		.bus_num	= 0,
		.chip_select	= 0,
	},
};

/*****************************************************************************
 * Ethernet
 ****************************************************************************/

static struct mv643xx_eth_platform_data net2big_v2_ge00_data = {
	.phy_addr	= MV643XX_ETH_PHY_ADDR(8),
};

/*****************************************************************************
 * I2C devices
 ****************************************************************************/

static struct at24_platform_data at24c04 = {
	.byte_len	= SZ_4K / 8,
	.page_size	= 16,
};

/*
 * i2c addr | chip         | description
 * 0x50     | HT24LC04     | eeprom (512B)
 */

static struct i2c_board_info __initdata net2big_v2_i2c_info[] = {
	{
		I2C_BOARD_INFO("24c04", 0x50),
		.platform_data  = &at24c04,
	}
};

/*****************************************************************************
 * SATA
 ****************************************************************************/

static struct mv_sata_platform_data net2big_v2_sata_data = {
	.n_ports	= 2,
};

static int __initdata net2big_v2_gpio_hdd_power[] = { 16, 17 };

static void __init net2big_v2_sata_power_init(void)
{
	int i;
	int err;

	/* Power up all hard disks. */
	for (i = 0; i < ARRAY_SIZE(net2big_v2_gpio_hdd_power); i++) {
		err = gpio_request(net2big_v2_gpio_hdd_power[i], NULL);
		if (err == 0) {
			err = gpio_direction_output(
					net2big_v2_gpio_hdd_power[i], 1);
			/* Free the HDD power GPIOs. This allow user-space to
			 * configure them via the gpiolib sysfs interface. */
			gpio_free(net2big_v2_gpio_hdd_power[i]);
		}
		if (err)
			pr_err("net2big_v2: failed to power-up HDD%d\n", i + 1);
	}
}

/*****************************************************************************
 * GPIO keys
 ****************************************************************************/

#define NET2BIG_V2_GPIO_SWITCH_POWER_ON		13
#define NET2BIG_V2_GPIO_SWITCH_POWER_OFF	15
#define NET2BIG_V2_GPIO_FUNC_BUTTON		34

#define NET2BIG_V2_SWITCH_POWER_ON		0x1
#define NET2BIG_V2_SWITCH_POWER_OFF		0x2

static struct gpio_keys_button net2big_v2_buttons[] = {
	[0] = {
		.type           = EV_SW,
		.code           = NET2BIG_V2_SWITCH_POWER_ON,
		.gpio           = NET2BIG_V2_GPIO_SWITCH_POWER_ON,
		.desc           = "Back power switch (on|auto)",
		.active_low     = 1,
	},
	[1] = {
		.type           = EV_SW,
		.code           = NET2BIG_V2_SWITCH_POWER_OFF,
		.gpio           = NET2BIG_V2_GPIO_SWITCH_POWER_OFF,
		.desc           = "Back power switch (auto|off)",
		.active_low     = 1,
	},
	[2] = {
		.code		= KEY_OPTION,
		.gpio		= NET2BIG_V2_GPIO_FUNC_BUTTON,
		.desc		= "Function button",
		.active_low	= 1,
	},
};

static struct gpio_keys_platform_data net2big_v2_button_data = {
	.buttons	= net2big_v2_buttons,
	.nbuttons	= ARRAY_SIZE(net2big_v2_buttons),
};

static struct platform_device net2big_v2_gpio_buttons = {
	.name		= "gpio-keys",
	.id		= -1,
	.dev		= {
		.platform_data	= &net2big_v2_button_data,
	},
};

/*****************************************************************************
 * GPIO LEDs
 ****************************************************************************/

/*
 * The LEDs are controlled by a CPLD and can be configured through a GPIO
 * extension bus:
 *
 * - address register : bit [0-2] -> GPIO [47-49]
 * - data register    : bit [0-2] -> GPIO [44-46]
 * - enable register  : GPIO 29
 *
 * Address register selection:
 *
 * addr | register
 * ----------------------------
 *   0  | front LED
 *   1  | front LED brightness
 *   2  | HDD LED brightness
 *   3  | HDD1 LED
 *   4  | HDD2 LED
 *
 * Data register configuration:
 *
 * data | LED brightness
 * -------------------------------------------------
 *   0  | min (off)
 *   -  | -
 *   7  | max
 *
 * data | front LED mode
 * -------------------------------------------------
 *   0  | fix off
 *   1  | fix blue on
 *   2  | fix red on
 *   3  | blink blue on=1 sec and blue off=1 sec
 *   4  | blink red on=1 sec and red off=1 sec
 *   5  | blink blue on=2.5 sec and red on=0.5 sec
 *   6  | blink blue on=1 sec and red on=1 sec
 *   7  | blink blue on=0.5 sec and blue off=2.5 sec
 *
 * data | HDD LED mode
 * -------------------------------------------------
 *   0  | fix blue on
 *   1  | SATA activity blink
 *   2  | fix red on
 *   3  | blink blue on=1 sec and blue off=1 sec
 *   4  | blink red on=1 sec and red off=1 sec
 *   5  | blink blue on=2.5 sec and red on=0.5 sec
 *   6  | blink blue on=1 sec and red on=1 sec
 *   7  | blink blue on=0.5 sec and blue off=2.5 sec
 */

/*****************************************************************************
 * Timer
 ****************************************************************************/

static void net2big_v2_timer_init(void)
{
	kirkwood_tclk = 166666667;
	orion_time_init(IRQ_KIRKWOOD_BRIDGE, kirkwood_tclk);
}

struct sys_timer net2big_v2_timer = {
	.init = net2big_v2_timer_init,
};

/*****************************************************************************
 * General Setup
 ****************************************************************************/

static unsigned int net2big_v2_mpp_config[] __initdata = {
	MPP0_SPI_SCn,
	MPP1_SPI_MOSI,
	MPP2_SPI_SCK,
	MPP3_SPI_MISO,
	MPP6_SYSRST_OUTn,
	MPP7_GPO,		/* Request power-off */
	MPP8_TW_SDA,
	MPP9_TW_SCK,
	MPP10_UART0_TXD,
	MPP11_UART0_RXD,
	MPP13_GPIO,		/* Rear power switch (on|auto) */
	MPP14_GPIO,		/* USB fuse alarm */
	MPP15_GPIO,		/* Rear power switch (auto|off) */
	MPP16_GPIO,		/* SATA HDD1 power */
	MPP17_GPIO,		/* SATA HDD2 power */
	MPP20_SATA1_ACTn,
	MPP21_SATA0_ACTn,
	MPP24_GPIO,		/* USB mode select */
	MPP26_GPIO,		/* USB device vbus */
	MPP28_GPIO,		/* USB enable host vbus */
	MPP29_GPIO,		/* GPIO extension ALE */
	MPP34_GPIO,		/* Rear Push button */
	MPP35_GPIO,		/* Inhibit switch power-off */
	MPP36_GPIO,		/* SATA HDD1 presence */
	MPP37_GPIO,		/* SATA HDD2 presence */
	MPP40_GPIO,		/* eSATA presence */
	MPP44_GPIO,		/* GPIO extension (data 0) */
	MPP45_GPIO,		/* GPIO extension (data 1) */
	MPP46_GPIO,		/* GPIO extension (data 2) */
	MPP47_GPIO,		/* GPIO extension (addr 0) */
	MPP48_GPIO,		/* GPIO extension (addr 1) */
	MPP49_GPIO,		/* GPIO extension (addr 2) */
	0
};

#define NET2BIG_V2_GPIO_POWER_OFF		7

static void net2big_v2_power_off(void)
{
	gpio_set_value(NET2BIG_V2_GPIO_POWER_OFF, 1);
}

static void __init net2big_v2_init(void)
{
	/*
	 * Basic setup. Needs to be called early.
	 */
	kirkwood_init();
	kirkwood_mpp_conf(net2big_v2_mpp_config);

	net2big_v2_sata_power_init();

	kirkwood_ehci_init();
	kirkwood_ge00_init(&net2big_v2_ge00_data);
	kirkwood_sata_init(&net2big_v2_sata_data);
	kirkwood_uart0_init();
	spi_register_board_info(net2big_v2_spi_slave_info,
				ARRAY_SIZE(net2big_v2_spi_slave_info));
	kirkwood_spi_init();
	kirkwood_i2c_init();
	i2c_register_board_info(0, net2big_v2_i2c_info,
				ARRAY_SIZE(net2big_v2_i2c_info));

	platform_device_register(&net2big_v2_gpio_buttons);

	if (gpio_request(NET2BIG_V2_GPIO_POWER_OFF, "power-off") == 0 &&
	    gpio_direction_output(NET2BIG_V2_GPIO_POWER_OFF, 0) == 0)
		pm_power_off = net2big_v2_power_off;
	else
		pr_err("net2big_v2: failed to configure power-off GPIO\n");
}

MACHINE_START(NET2BIG_V2, "LaCie 2Big Network v2")
	.phys_io	= KIRKWOOD_REGS_PHYS_BASE,
	.io_pg_offst	= ((KIRKWOOD_REGS_VIRT_BASE) >> 18) & 0xfffc,
	.boot_params	= 0x00000100,
	.init_machine	= net2big_v2_init,
	.map_io		= kirkwood_map_io,
	.init_irq	= kirkwood_init_irq,
	.timer		= &net2big_v2_timer,
MACHINE_END
