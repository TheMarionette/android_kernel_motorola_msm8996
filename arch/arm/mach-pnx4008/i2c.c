/*
 * I2C initialization for PNX4008.
 *
 * Author: Vitaly Wool <vitalywool@gmail.com>
 *
 * 2005-2006 (c) MontaVista Software, Inc. This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/i2c-pnx.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <mach/platform.h>
#include <mach/irqs.h>
#include <mach/i2c.h>

static struct i2c_pnx_algo_data pnx_algo_data0 = {
	.base = PNX4008_I2C1_BASE,
	.irq = I2C_1_INT,
};

static struct i2c_pnx_algo_data pnx_algo_data1 = {
	.base = PNX4008_I2C2_BASE,
	.irq = I2C_2_INT,
};

static struct i2c_pnx_algo_data pnx_algo_data2 = {
	.base = (PNX4008_USB_CONFIG_BASE + 0x300),
	.irq = USB_I2C_INT,
};

static struct i2c_adapter pnx_adapter0 = {
	.name = I2C_CHIP_NAME "0",
	.algo_data = &pnx_algo_data0,
};
static struct i2c_adapter pnx_adapter1 = {
	.name = I2C_CHIP_NAME "1",
	.algo_data = &pnx_algo_data1,
};

static struct i2c_adapter pnx_adapter2 = {
	.name = "USB-I2C",
	.algo_data = &pnx_algo_data2,
};

static struct i2c_pnx_data i2c0_data = {
	.adapter = &pnx_adapter0,
};

static struct i2c_pnx_data i2c1_data = {
	.adapter = &pnx_adapter1,
};

static struct i2c_pnx_data i2c2_data = {
	.adapter = &pnx_adapter2,
};

static struct platform_device i2c0_device = {
	.name = "pnx-i2c",
	.id = 0,
	.dev = {
		.platform_data = &i2c0_data,
	},
};

static struct platform_device i2c1_device = {
	.name = "pnx-i2c",
	.id = 1,
	.dev = {
		.platform_data = &i2c1_data,
	},
};

static struct platform_device i2c2_device = {
	.name = "pnx-i2c",
	.id = 2,
	.dev = {
		.platform_data = &i2c2_data,
	},
};

static struct platform_device *devices[] __initdata = {
	&i2c0_device,
	&i2c1_device,
	&i2c2_device,
};

void __init pnx4008_register_i2c_devices(void)
{
	platform_add_devices(devices, ARRAY_SIZE(devices));
}
