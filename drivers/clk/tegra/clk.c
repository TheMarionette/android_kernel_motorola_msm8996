/*
 * Copyright (c) 2012, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/of.h>
#include <linux/clk/tegra.h>

#include "clk.h"

#define CLK_OUT_ENB_L			0x010
#define CLK_OUT_ENB_H			0x014
#define CLK_OUT_ENB_U			0x018
#define CLK_OUT_ENB_V			0x360
#define CLK_OUT_ENB_W			0x364
#define CLK_OUT_ENB_X			0x280
#define CLK_OUT_ENB_SET_L		0x320
#define CLK_OUT_ENB_CLR_L		0x324
#define CLK_OUT_ENB_SET_H		0x328
#define CLK_OUT_ENB_CLR_H		0x32c
#define CLK_OUT_ENB_SET_U		0x330
#define CLK_OUT_ENB_CLR_U		0x334
#define CLK_OUT_ENB_SET_V		0x440
#define CLK_OUT_ENB_CLR_V		0x444
#define CLK_OUT_ENB_SET_W		0x448
#define CLK_OUT_ENB_CLR_W		0x44c
#define CLK_OUT_ENB_SET_X		0x284
#define CLK_OUT_ENB_CLR_X		0x288

#define RST_DEVICES_L			0x004
#define RST_DEVICES_H			0x008
#define RST_DEVICES_U			0x00C
#define RST_DFLL_DVCO			0x2F4
#define RST_DEVICES_V			0x358
#define RST_DEVICES_W			0x35C
#define RST_DEVICES_X			0x28C
#define RST_DEVICES_SET_L		0x300
#define RST_DEVICES_CLR_L		0x304
#define RST_DEVICES_SET_H		0x308
#define RST_DEVICES_CLR_H		0x30c
#define RST_DEVICES_SET_U		0x310
#define RST_DEVICES_CLR_U		0x314
#define RST_DEVICES_SET_V		0x430
#define RST_DEVICES_CLR_V		0x434
#define RST_DEVICES_SET_W		0x438
#define RST_DEVICES_CLR_W		0x43c

/* Global data of Tegra CPU CAR ops */
static struct tegra_cpu_car_ops dummy_car_ops;
struct tegra_cpu_car_ops *tegra_cpu_car_ops = &dummy_car_ops;

static int periph_banks;

static struct tegra_clk_periph_regs periph_regs[] = {
	[0] = {
		.enb_reg = CLK_OUT_ENB_L,
		.enb_set_reg = CLK_OUT_ENB_SET_L,
		.enb_clr_reg = CLK_OUT_ENB_CLR_L,
		.rst_reg = RST_DEVICES_L,
		.rst_set_reg = RST_DEVICES_SET_L,
		.rst_clr_reg = RST_DEVICES_CLR_L,
	},
	[1] = {
		.enb_reg = CLK_OUT_ENB_H,
		.enb_set_reg = CLK_OUT_ENB_SET_H,
		.enb_clr_reg = CLK_OUT_ENB_CLR_H,
		.rst_reg = RST_DEVICES_H,
		.rst_set_reg = RST_DEVICES_SET_H,
		.rst_clr_reg = RST_DEVICES_CLR_H,
	},
	[2] = {
		.enb_reg = CLK_OUT_ENB_U,
		.enb_set_reg = CLK_OUT_ENB_SET_U,
		.enb_clr_reg = CLK_OUT_ENB_CLR_U,
		.rst_reg = RST_DEVICES_U,
		.rst_set_reg = RST_DEVICES_SET_U,
		.rst_clr_reg = RST_DEVICES_CLR_U,
	},
	[3] = {
		.enb_reg = CLK_OUT_ENB_V,
		.enb_set_reg = CLK_OUT_ENB_SET_V,
		.enb_clr_reg = CLK_OUT_ENB_CLR_V,
		.rst_reg = RST_DEVICES_V,
		.rst_set_reg = RST_DEVICES_SET_V,
		.rst_clr_reg = RST_DEVICES_CLR_V,
	},
	[4] = {
		.enb_reg = CLK_OUT_ENB_W,
		.enb_set_reg = CLK_OUT_ENB_SET_W,
		.enb_clr_reg = CLK_OUT_ENB_CLR_W,
		.rst_reg = RST_DEVICES_W,
		.rst_set_reg = RST_DEVICES_SET_W,
		.rst_clr_reg = RST_DEVICES_CLR_W,
	},
};

struct tegra_clk_periph_regs *get_reg_bank(int clkid)
{
	int reg_bank = clkid / 32;

	if (reg_bank < periph_banks)
		return &periph_regs[reg_bank];
	else {
		WARN_ON(1);
		return NULL;
	}
}

int __init tegra_clk_set_periph_banks(int num)
{
	if (num > ARRAY_SIZE(periph_regs))
		return -EINVAL;

	periph_banks = num;

	return 0;
}

void __init tegra_init_dup_clks(struct tegra_clk_duplicate *dup_list,
				struct clk *clks[], int clk_max)
{
	struct clk *clk;

	for (; dup_list->clk_id < clk_max; dup_list++) {
		clk = clks[dup_list->clk_id];
		dup_list->lookup.clk = clk;
		clkdev_add(&dup_list->lookup);
	}
}

void __init tegra_init_from_table(struct tegra_clk_init_table *tbl,
				  struct clk *clks[], int clk_max)
{
	struct clk *clk;

	for (; tbl->clk_id < clk_max; tbl++) {
		clk = clks[tbl->clk_id];
		if (IS_ERR_OR_NULL(clk))
			return;

		if (tbl->parent_id < clk_max) {
			struct clk *parent = clks[tbl->parent_id];
			if (clk_set_parent(clk, parent)) {
				pr_err("%s: Failed to set parent %s of %s\n",
				       __func__, __clk_get_name(parent),
				       __clk_get_name(clk));
				WARN_ON(1);
			}
		}

		if (tbl->rate)
			if (clk_set_rate(clk, tbl->rate)) {
				pr_err("%s: Failed to set rate %lu of %s\n",
				       __func__, tbl->rate,
				       __clk_get_name(clk));
				WARN_ON(1);
			}

		if (tbl->state)
			if (clk_prepare_enable(clk)) {
				pr_err("%s: Failed to enable %s\n", __func__,
				       __clk_get_name(clk));
				WARN_ON(1);
			}
	}
}

tegra_clk_apply_init_table_func tegra_clk_apply_init_table;

void __init tegra_clocks_apply_init_table(void)
{
	if (!tegra_clk_apply_init_table)
		return;

	tegra_clk_apply_init_table();
}
