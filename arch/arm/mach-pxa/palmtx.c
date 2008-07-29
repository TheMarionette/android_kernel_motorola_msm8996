/*
 * Hardware definitions for PalmTX
 *
 * Author:     Marek Vasut <marek.vasut@gmail.com>
 *
 * Based on work of:
 *		Alex Osborne <ato@meshy.org>
 *		Cristiano P. <cristianop@users.sourceforge.net>
 *		Jan Herman <2hp@seznam.cz>
 *		Michal Hrusecky
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (find more info at www.hackndev.com)
 *
 */

#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/pda_power.h>
#include <linux/pwm_backlight.h>
#include <linux/gpio.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <asm/arch/audio.h>
#include <asm/arch/palmtx.h>
#include <asm/arch/mmc.h>
#include <asm/arch/pxafb.h>
#include <asm/arch/pxa-regs.h>
#include <asm/arch/mfp-pxa27x.h>
#include <asm/arch/irda.h>
#include <asm/arch/pxa27x_keypad.h>
#include <asm/arch/udc.h>

#include "generic.h"
#include "devices.h"

/******************************************************************************
 * Pin configuration
 ******************************************************************************/
static unsigned long palmtx_pin_config[] __initdata = {
	/* MMC */
	GPIO32_MMC_CLK,
	GPIO92_MMC_DAT_0,
	GPIO109_MMC_DAT_1,
	GPIO110_MMC_DAT_2,
	GPIO111_MMC_DAT_3,
	GPIO112_MMC_CMD,

	/* AC97 */
	GPIO28_AC97_BITCLK,
	GPIO29_AC97_SDATA_IN_0,
	GPIO30_AC97_SDATA_OUT,
	GPIO31_AC97_SYNC,

	/* IrDA */
	GPIO46_FICP_RXD,
	GPIO47_FICP_TXD,

	/* PWM */
	GPIO16_PWM0_OUT,

	/* USB */
	GPIO13_GPIO,

	/* PCMCIA */
	GPIO48_nPOE,
	GPIO49_nPWE,
	GPIO50_nPIOR,
	GPIO51_nPIOW,
	GPIO85_nPCE_1,
	GPIO54_nPCE_2,
	GPIO79_PSKTSEL,
	GPIO55_nPREG,
	GPIO56_nPWAIT,
	GPIO57_nIOIS16,
};

/******************************************************************************
 * SD/MMC card controller
 ******************************************************************************/
static int palmtx_mci_init(struct device *dev, irq_handler_t palmtx_detect_int,
				void *data)
{
	int err = 0;

	/* Setup an interrupt for detecting card insert/remove events */
	err = request_irq(IRQ_GPIO_PALMTX_SD_DETECT_N, palmtx_detect_int,
			IRQF_DISABLED | IRQF_SAMPLE_RANDOM |
			IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
			"SD/MMC card detect", data);
	if (err) {
		printk(KERN_ERR "%s: cannot request SD/MMC card detect IRQ\n",
				__func__);
		return err;
	}

	err = gpio_request(GPIO_NR_PALMTX_SD_POWER, "SD_POWER");
	if (err)
		goto pwr_err;

	err = gpio_request(GPIO_NR_PALMTX_SD_READONLY, "SD_READONLY");
	if (err)
		goto ro_err;

	printk(KERN_DEBUG "%s: irq registered\n", __func__);

	return 0;

ro_err:
	gpio_free(GPIO_NR_PALMTX_SD_POWER);
pwr_err:
	free_irq(IRQ_GPIO_PALMTX_SD_DETECT_N, data);
	return err;
}

static void palmtx_mci_exit(struct device *dev, void *data)
{
	gpio_free(GPIO_NR_PALMTX_SD_READONLY);
	gpio_free(GPIO_NR_PALMTX_SD_POWER);
	free_irq(IRQ_GPIO_PALMTX_SD_DETECT_N, data);
}

static void palmtx_mci_power(struct device *dev, unsigned int vdd)
{
	struct pxamci_platform_data *p_d = dev->platform_data;
	gpio_set_value(GPIO_NR_PALMTX_SD_POWER, p_d->ocr_mask & (1 << vdd));
}

static int palmtx_mci_get_ro(struct device *dev)
{
	return gpio_get_value(GPIO_NR_PALMTX_SD_READONLY);
}

static struct pxamci_platform_data palmtx_mci_platform_data = {
	.ocr_mask	= MMC_VDD_32_33 | MMC_VDD_33_34,
	.setpower	= palmtx_mci_power,
	.get_ro		= palmtx_mci_get_ro,
	.init 		= palmtx_mci_init,
	.exit		= palmtx_mci_exit,
};

/******************************************************************************
 * GPIO keyboard
 ******************************************************************************/
static unsigned int palmtx_matrix_keys[] = {
	KEY(0, 0, KEY_POWER),
	KEY(0, 1, KEY_F1),
	KEY(0, 2, KEY_ENTER),

	KEY(1, 0, KEY_F2),
	KEY(1, 1, KEY_F3),
	KEY(1, 2, KEY_F4),

	KEY(2, 0, KEY_UP),
	KEY(2, 2, KEY_DOWN),

	KEY(3, 0, KEY_RIGHT),
	KEY(3, 2, KEY_LEFT),

};

static struct pxa27x_keypad_platform_data palmtx_keypad_platform_data = {
	.matrix_key_rows	= 4,
	.matrix_key_cols	= 3,
	.matrix_key_map		= palmtx_matrix_keys,
	.matrix_key_map_size	= ARRAY_SIZE(palmtx_matrix_keys),

	.debounce_interval	= 30,
};

/******************************************************************************
 * GPIO keys
 ******************************************************************************/
static struct gpio_keys_button palmtx_pxa_buttons[] = {
	{KEY_F8, GPIO_NR_PALMTX_HOTSYNC_BUTTON_N, 1, "HotSync Button" },
};

static struct gpio_keys_platform_data palmtx_pxa_keys_data = {
	.buttons	= palmtx_pxa_buttons,
	.nbuttons	= ARRAY_SIZE(palmtx_pxa_buttons),
};

static struct platform_device palmtx_pxa_keys = {
	.name	= "gpio-keys",
	.id	= -1,
	.dev	= {
		.platform_data = &palmtx_pxa_keys_data,
	},
};

/******************************************************************************
 * Backlight
 ******************************************************************************/
static int palmtx_backlight_init(struct device *dev)
{
	int ret;

	ret = gpio_request(GPIO_NR_PALMTX_BL_POWER, "BL POWER");
	if (ret)
		goto err;
	ret = gpio_request(GPIO_NR_PALMTX_LCD_POWER, "LCD POWER");
	if (ret)
		goto err2;

	return 0;
err2:
	gpio_free(GPIO_NR_PALMTX_BL_POWER);
err:
	return ret;
}

static int palmtx_backlight_notify(int brightness)
{
	gpio_set_value(GPIO_NR_PALMTX_BL_POWER, brightness);
	gpio_set_value(GPIO_NR_PALMTX_LCD_POWER, brightness);
	return brightness;
}

static void palmtx_backlight_exit(struct device *dev)
{
	gpio_free(GPIO_NR_PALMTX_BL_POWER);
	gpio_free(GPIO_NR_PALMTX_LCD_POWER);
}

static struct platform_pwm_backlight_data palmtx_backlight_data = {
	.pwm_id		= 0,
	.max_brightness	= PALMTX_MAX_INTENSITY,
	.dft_brightness	= PALMTX_MAX_INTENSITY,
	.pwm_period_ns	= PALMTX_PERIOD_NS,
	.init		= palmtx_backlight_init,
	.notify		= palmtx_backlight_notify,
	.exit		= palmtx_backlight_exit,
};

static struct platform_device palmtx_backlight = {
	.name	= "pwm-backlight",
	.dev	= {
		.parent		= &pxa27x_device_pwm0.dev,
		.platform_data	= &palmtx_backlight_data,
	},
};

/******************************************************************************
 * IrDA
 ******************************************************************************/
static void palmtx_irda_transceiver_mode(struct device *dev, int mode)
{
	gpio_set_value(GPIO_NR_PALMTX_IR_DISABLE, mode & IR_OFF);
	pxa2xx_transceiver_mode(dev, mode);
}

static struct pxaficp_platform_data palmtx_ficp_platform_data = {
	.transceiver_cap	= IR_SIRMODE | IR_FIRMODE | IR_OFF,
	.transceiver_mode	= palmtx_irda_transceiver_mode,
};

/******************************************************************************
 * UDC
 ******************************************************************************/
static void palmtx_udc_command(int cmd)
{
	gpio_set_value(GPIO_NR_PALMTX_USB_POWER, !cmd);
	udelay(50);
	gpio_set_value(GPIO_NR_PALMTX_USB_PULLUP, !cmd);
}

static struct pxa2xx_udc_mach_info palmtx_udc_info __initdata = {
	.gpio_vbus		= GPIO_NR_PALMTX_USB_DETECT_N,
	.gpio_vbus_inverted	= 1,
	.udc_command		= palmtx_udc_command,
};

/******************************************************************************
 * Power supply
 ******************************************************************************/
static int power_supply_init(struct device *dev)
{
	int ret;

	ret = gpio_request(GPIO_NR_PALMTX_POWER_DETECT, "CABLE_STATE_AC");
	if (ret)
		goto err_cs_ac;

	ret = gpio_request(GPIO_NR_PALMTX_USB_DETECT_N, "CABLE_STATE_USB");
	if (ret)
		goto err_cs_usb;

	return 0;

err_cs_usb:
	gpio_free(GPIO_NR_PALMTX_POWER_DETECT);
err_cs_ac:
	return ret;
}

static int palmtx_is_ac_online(void)
{
	return gpio_get_value(GPIO_NR_PALMTX_POWER_DETECT);
}

static int palmtx_is_usb_online(void)
{
	return !gpio_get_value(GPIO_NR_PALMTX_USB_DETECT_N);
}

static void power_supply_exit(struct device *dev)
{
	gpio_free(GPIO_NR_PALMTX_USB_DETECT_N);
	gpio_free(GPIO_NR_PALMTX_POWER_DETECT);
}

static char *palmtx_supplicants[] = {
	"main-battery",
};

static struct pda_power_pdata power_supply_info = {
	.init            = power_supply_init,
	.is_ac_online    = palmtx_is_ac_online,
	.is_usb_online   = palmtx_is_usb_online,
	.exit            = power_supply_exit,
	.supplied_to     = palmtx_supplicants,
	.num_supplicants = ARRAY_SIZE(palmtx_supplicants),
};

static struct platform_device power_supply = {
	.name = "pda-power",
	.id   = -1,
	.dev  = {
		.platform_data = &power_supply_info,
	},
};

/******************************************************************************
 * Framebuffer
 ******************************************************************************/
static struct pxafb_mode_info palmtx_lcd_modes[] = {
{
	.pixclock	= 57692,
	.xres		= 320,
	.yres		= 480,
	.bpp		= 16,

	.left_margin	= 32,
	.right_margin	= 1,
	.upper_margin	= 7,
	.lower_margin	= 1,

	.hsync_len	= 4,
	.vsync_len	= 1,
},
};

static struct pxafb_mach_info palmtx_lcd_screen = {
	.modes		= palmtx_lcd_modes,
	.num_modes	= ARRAY_SIZE(palmtx_lcd_modes),
	.lcd_conn	= LCD_COLOR_TFT_16BPP | LCD_PCLK_EDGE_FALL,
};

/******************************************************************************
 * Machine init
 ******************************************************************************/
static struct platform_device *devices[] __initdata = {
#if defined(CONFIG_KEYBOARD_GPIO) || defined(CONFIG_KEYBOARD_GPIO_MODULE)
	&palmtx_pxa_keys,
#endif
	&palmtx_backlight,
	&power_supply,
};

static struct map_desc palmtx_io_desc[] __initdata = {
{
	.virtual	= PALMTX_PCMCIA_VIRT,
	.pfn		= __phys_to_pfn(PALMTX_PCMCIA_PHYS),
	.length		= PALMTX_PCMCIA_SIZE,
	.type		= MT_DEVICE
},
};

static void __init palmtx_map_io(void)
{
	pxa_map_io();
	iotable_init(palmtx_io_desc, ARRAY_SIZE(palmtx_io_desc));
}

static void __init palmtx_init(void)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(palmtx_pin_config));

	set_pxa_fb_info(&palmtx_lcd_screen);
	pxa_set_mci_info(&palmtx_mci_platform_data);
	pxa_set_udc_info(&palmtx_udc_info);
	pxa_set_ac97_info(NULL);
	pxa_set_ficp_info(&palmtx_ficp_platform_data);
	pxa_set_keypad_info(&palmtx_keypad_platform_data);

	platform_add_devices(devices, ARRAY_SIZE(devices));
}

MACHINE_START(PALMTX, "Palm T|X")
	.phys_io	= PALMTX_PHYS_IO_START,
	.io_pg_offst	= io_p2v(0x40000000),
	.boot_params	= 0xa0000100,
	.map_io		= palmtx_map_io,
	.init_irq	= pxa27x_init_irq,
	.timer		= &pxa_timer,
	.init_machine	= palmtx_init
MACHINE_END
