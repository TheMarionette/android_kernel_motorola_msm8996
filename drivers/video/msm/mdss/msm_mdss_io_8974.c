/* Copyright (c) 2012-2015, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/clk/msm-clk.h>

#include "mdss_dsi.h"
#include "mdss_edp.h"

#define MDSS_DSI_DSIPHY_REGULATOR_CTRL_0	0x00
#define MDSS_DSI_DSIPHY_REGULATOR_CTRL_1	0x04
#define MDSS_DSI_DSIPHY_REGULATOR_CTRL_2	0x08
#define MDSS_DSI_DSIPHY_REGULATOR_CTRL_3	0x0c
#define MDSS_DSI_DSIPHY_REGULATOR_CTRL_4	0x10
#define MDSS_DSI_DSIPHY_REGULATOR_CAL_PWR_CFG	0x18
#define MDSS_DSI_DSIPHY_LDO_CNTRL		0x1dc
#define MDSS_DSI_DSIPHY_REGULATOR_TEST		0x294
#define MDSS_DSI_DSIPHY_STRENGTH_CTRL_0		0x184
#define MDSS_DSI_DSIPHY_STRENGTH_CTRL_1		0x188
#define MDSS_DSI_DSIPHY_STRENGTH_CTRL_2		0x18c
#define MDSS_DSI_DSIPHY_TIMING_CTRL_0		0x140
#define MDSS_DSI_DSIPHY_GLBL_TEST_CTRL		0x1d4
#define MDSS_DSI_DSIPHY_CTRL_0			0x170
#define MDSS_DSI_DSIPHY_CTRL_1			0x174

#define SW_RESET BIT(2)
#define SW_RESET_PLL BIT(0)
#define PWRDN_B BIT(7)

/* 8996 */
#define DATALANE_OFFSET_FROM_BASE_8996	0x100
#define DATALANE_SIZE_8996			0x80

#define DSIPHY_CMN_GLBL_TEST_CTRL		0x0018
#define DSIPHY_CMN_CTRL_0			0x001c
#define DSIPHY_CMN_CTRL_1			0x0020
#define DSIPHY_CMN_LDO_CNTRL			0x004c
#define DSIPHY_PLL_CLKBUFLR_EN			0x041c
#define DSIPHY_PLL_PLL_BANDGAP			0x0508

static void mdss_dsi_ctrl_phy_reset(struct mdss_dsi_ctrl_pdata *ctrl)
{
	/* start phy sw reset */
	MIPI_OUTP(ctrl->ctrl_base + 0x12c, 0x0001);
	udelay(1000);
	wmb();	/* make sure reset */
	/* end phy sw reset */
	MIPI_OUTP(ctrl->ctrl_base + 0x12c, 0x0000);
	udelay(100);
	wmb();	/* maek sure reset cleared */
}

void mdss_dsi_phy_sw_reset(struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct mdss_dsi_ctrl_pdata *sctrl = NULL;
	struct dsi_shared_data *sdata;

	if (ctrl == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	sdata = ctrl->shared_data;

	if (ctrl == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	if (IS_MDSS_MAJOR_MINOR_SAME(ctrl->shared_data->hw_rev,
		MDSS_DSI_HW_REV_104)) {
		if (mdss_dsi_is_ctrl_clk_master(ctrl))
			sctrl = mdss_dsi_get_ctrl_clk_slave();
		else
			return;
	}

	/* start phy sw reset */
	mdss_dsi_ctrl_phy_reset(ctrl);
	if (sctrl)
		mdss_dsi_ctrl_phy_reset(sctrl);

	if ((sdata->hw_rev == MDSS_DSI_HW_REV_103) &&
		!mdss_dsi_is_hw_config_dual(sdata) &&
		mdss_dsi_is_right_ctrl(ctrl)) {

		/*
		 * phy sw reset will wipe out the pll settings for PLL.
		 * Need to explicitly turn off PLL1 if unused to avoid
		 * current leakage issues.
		 */
		if ((mdss_dsi_is_hw_config_split(sdata) ||
			mdss_dsi_is_pll_src_pll0(sdata)) &&
			ctrl->vco_dummy_clk) {
			pr_debug("Turn off unused PLL1 registers\n");
			clk_set_rate(ctrl->vco_dummy_clk, 1);
		}
	}
}

static void mdss_dsi_phy_regulator_disable(struct mdss_dsi_ctrl_pdata *ctrl)
{
	if (!ctrl) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	if (IS_MDSS_MAJOR_MINOR_SAME(ctrl->shared_data->hw_rev,
		MDSS_DSI_HW_REV_104))
		return;

	MIPI_OUTP(ctrl->phy_regulator_io.base + 0x018, 0x000);
}

static void mdss_dsi_phy_shutdown(struct mdss_dsi_ctrl_pdata *ctrl)
{
	if (!ctrl) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	if (IS_MDSS_MAJOR_MINOR_SAME(ctrl->shared_data->hw_rev,
		MDSS_DSI_HW_REV_104)) {
		MIPI_OUTP(ctrl->phy_io.base + DSIPHY_PLL_CLKBUFLR_EN, 0);
		MIPI_OUTP(ctrl->phy_io.base + DSIPHY_CMN_GLBL_TEST_CTRL, 0);
		MIPI_OUTP(ctrl->phy_io.base + DSIPHY_CMN_CTRL_0, 0);
	} else {
		MIPI_OUTP(ctrl->phy_io.base + MDSS_DSI_DSIPHY_CTRL_0, 0x000);
	}
}

/**
 * mdss_dsi_lp_cd_rx() -- enable LP and CD at receiving
 * @ctrl: pointer to DSI controller structure
 *
 * LP: low power
 * CD: contention detection
 */
void mdss_dsi_lp_cd_rx(struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct mdss_dsi_phy_ctrl *pd;

	if (!ctrl) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	if (IS_MDSS_MAJOR_MINOR_SAME(ctrl->shared_data->hw_rev,
		MDSS_DSI_HW_REV_104))
		return;

	pd = &(((ctrl->panel_data).panel_info.mipi).dsi_phy_db);

	/* Strength ctrl 1, LP Rx + CD Rxcontention detection */
	MIPI_OUTP((ctrl->phy_io.base) + 0x0188, pd->strength[1]);
	wmb();
}

static void mdss_dsi_28nm_phy_regulator_enable(
		struct mdss_dsi_ctrl_pdata *ctrl_pdata)
{
	struct mdss_dsi_phy_ctrl *pd;
	pd = &(((ctrl_pdata->panel_data).panel_info.mipi).dsi_phy_db);

	if (pd->reg_ldo_mode) {
		/* Regulator ctrl 0 */
		MIPI_OUTP(ctrl_pdata->phy_regulator_io.base, 0x0);
		/* Regulator ctrl - CAL_PWR_CFG */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x18, pd->regulator[6]);
		/* Add H/w recommended delay */
		udelay(1000);
		/* Regulator ctrl - TEST */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x14, pd->regulator[5]);
		/* Regulator ctrl 3 */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0xc, pd->regulator[3]);
		/* Regulator ctrl 2 */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x8, pd->regulator[2]);
		/* Regulator ctrl 1 */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x4, pd->regulator[1]);
		/* Regulator ctrl 4 */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x10, pd->regulator[4]);
		/* LDO ctrl */
		if (MIPI_INP(ctrl_pdata->ctrl_base) == MDSS_DSI_HW_REV_103_1)
			MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x1dc, 0x05);
		else
			MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x1dc, 0x0d);
	} else {
		/* Regulator ctrl 0 */
		MIPI_OUTP(ctrl_pdata->phy_regulator_io.base,
					0x0);
		/* Regulator ctrl - CAL_PWR_CFG */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x18, pd->regulator[6]);
		/* Add H/w recommended delay */
		udelay(1000);
		/* Regulator ctrl 1 */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x4, pd->regulator[1]);
		/* Regulator ctrl 2 */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x8, pd->regulator[2]);
		/* Regulator ctrl 3 */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0xc, pd->regulator[3]);
		/* Regulator ctrl 4 */
		MIPI_OUTP((ctrl_pdata->phy_regulator_io.base)
				+ 0x10, pd->regulator[4]);
		/* LDO ctrl */
		MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x1dc, 0x00);
		/* Regulator ctrl 0 */
		MIPI_OUTP(ctrl_pdata->phy_regulator_io.base,
				pd->regulator[0]);
	}
}

static void mdss_dsi_28nm_phy_config(struct mdss_dsi_ctrl_pdata *ctrl_pdata)
{
	struct mdss_dsi_phy_ctrl *pd;
	int i, off, ln, offset;

	if (!ctrl_pdata) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	pd = &(((ctrl_pdata->panel_data).panel_info.mipi).dsi_phy_db);

	/* Strength ctrl 0 for 28nm PHY*/
	if ((ctrl_pdata->shared_data->hw_rev <= MDSS_DSI_HW_REV_103) &&
		(ctrl_pdata->shared_data->hw_rev != MDSS_DSI_HW_REV_103))
		MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x0184, pd->strength[0]);

	off = 0x0140;	/* phy timing ctrl 0 - 11 */
	for (i = 0; i < 12; i++) {
		MIPI_OUTP((ctrl_pdata->phy_io.base) + off, pd->timing[i]);
		wmb();
		off += 4;
	}

	/* MMSS_DSI_0_PHY_DSIPHY_CTRL_1 */
	MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x0174, 0x00);
	/* MMSS_DSI_0_PHY_DSIPHY_CTRL_0 */
	MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x0170, 0x5f);
	wmb();

	/* 4 lanes + clk lane configuration */
	/* lane config n * (0 - 4) & DataPath setup */
	for (ln = 0; ln < 5; ln++) {
		off = (ln * 0x40);
		for (i = 0; i < 9; i++) {
			offset = i + (ln * 9);
			MIPI_OUTP((ctrl_pdata->phy_io.base) + off,
							pd->lanecfg[offset]);
			wmb();
			off += 4;
		}
	}

	/* MMSS_DSI_0_PHY_DSIPHY_CTRL_0 */
	MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x0170, 0x5f);
	wmb();

	/* DSI_0_PHY_DSIPHY_GLBL_TEST_CTRL */
	if (((ctrl_pdata->panel_data).panel_info.pdest == DISPLAY_1) ||
		(ctrl_pdata->shared_data->hw_rev == MDSS_DSI_HW_REV_103_1))
		MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x01d4, 0x01);
	else
		MIPI_OUTP((ctrl_pdata->phy_io.base) + 0x01d4, 0x00);
	wmb();

	off = 0x01b4;	/* phy BIST ctrl 0 - 5 */
	for (i = 0; i < 6; i++) {
		MIPI_OUTP((ctrl_pdata->phy_io.base) + off, pd->bistctrl[i]);
		wmb();
		off += 4;
	}

}

static void mdss_dsi_20nm_phy_regulator_enable(struct mdss_dsi_ctrl_pdata
	*ctrl_pdata)
{
	struct mdss_dsi_phy_ctrl *pd;
	void __iomem *phy_io_base;

	pd = &(((ctrl_pdata->panel_data).panel_info.mipi).dsi_phy_db);
	phy_io_base = ctrl_pdata->phy_regulator_io.base;

	if (pd->regulator_len != 7) {
		pr_err("%s: wrong regulator settings\n", __func__);
		return;
	}

	if (pd->reg_ldo_mode) {
		MIPI_OUTP(ctrl_pdata->phy_io.base + MDSS_DSI_DSIPHY_LDO_CNTRL,
			0x1d);
	} else {
		MIPI_OUTP(phy_io_base + MDSS_DSI_DSIPHY_REGULATOR_CTRL_1,
			pd->regulator[1]);
		MIPI_OUTP(phy_io_base + MDSS_DSI_DSIPHY_REGULATOR_CTRL_2,
			pd->regulator[2]);
		MIPI_OUTP(phy_io_base + MDSS_DSI_DSIPHY_REGULATOR_CTRL_3,
			pd->regulator[3]);
		MIPI_OUTP(phy_io_base + MDSS_DSI_DSIPHY_REGULATOR_CTRL_4,
			pd->regulator[4]);
		MIPI_OUTP(phy_io_base + MDSS_DSI_DSIPHY_REGULATOR_CAL_PWR_CFG,
			pd->regulator[6]);
		MIPI_OUTP(ctrl_pdata->phy_io.base + MDSS_DSI_DSIPHY_LDO_CNTRL,
			0x00);
		MIPI_OUTP(phy_io_base + MDSS_DSI_DSIPHY_REGULATOR_CTRL_0,
			pd->regulator[0]);
	}
}

static void mdss_dsi_20nm_phy_config(struct mdss_dsi_ctrl_pdata *ctrl_pdata)
{
	struct mdss_dsi_phy_ctrl *pd;
	int i, off, ln, offset;

	pd = &(((ctrl_pdata->panel_data).panel_info.mipi).dsi_phy_db);

	if (pd->strength_len != 2) {
		pr_err("%s: wrong strength ctrl\n", __func__);
		return;
	}

	MIPI_OUTP((ctrl_pdata->phy_io.base) + MDSS_DSI_DSIPHY_STRENGTH_CTRL_0,
		pd->strength[0]);


	if (!mdss_dsi_is_hw_config_dual(ctrl_pdata->shared_data)) {
		if (mdss_dsi_is_hw_config_split(ctrl_pdata->shared_data) ||
			mdss_dsi_is_left_ctrl(ctrl_pdata) ||
			(mdss_dsi_is_right_ctrl(ctrl_pdata) &&
			mdss_dsi_is_pll_src_pll0(ctrl_pdata->shared_data)))
			MIPI_OUTP((ctrl_pdata->phy_io.base) +
				MDSS_DSI_DSIPHY_GLBL_TEST_CTRL, 0x00);
		else
			MIPI_OUTP((ctrl_pdata->phy_io.base) +
				MDSS_DSI_DSIPHY_GLBL_TEST_CTRL, 0x01);
	} else {
		if (mdss_dsi_is_left_ctrl(ctrl_pdata))
			MIPI_OUTP((ctrl_pdata->phy_io.base) +
				MDSS_DSI_DSIPHY_GLBL_TEST_CTRL, 0x00);
		else
			MIPI_OUTP((ctrl_pdata->phy_io.base) +
				MDSS_DSI_DSIPHY_GLBL_TEST_CTRL, 0x01);
	}

	if (pd->lanecfg_len != 45) {
		pr_err("%s: wrong lane cfg\n", __func__);
		return;
	}

	/* 4 lanes + clk lane configuration */
	/* lane config n * (0 - 4) & DataPath setup */
	for (ln = 0; ln < 5; ln++) {
		off = (ln * 0x40);
		for (i = 0; i < 9; i++) {
			offset = i + (ln * 9);
			MIPI_OUTP((ctrl_pdata->phy_io.base) + off,
				pd->lanecfg[offset]);
			wmb();
			off += 4;
		}
	}

	off = 0;	/* phy timing ctrl 0 - 11 */
	for (i = 0; i < 12; i++) {
		MIPI_OUTP((ctrl_pdata->phy_io.base) +
			MDSS_DSI_DSIPHY_TIMING_CTRL_0 + off, pd->timing[i]);
		wmb();
		off += 4;
	}

	MIPI_OUTP((ctrl_pdata->phy_io.base) + MDSS_DSI_DSIPHY_CTRL_1, 0);
	/* make sure everything is written before enable */
	wmb();
	MIPI_OUTP((ctrl_pdata->phy_io.base) + MDSS_DSI_DSIPHY_CTRL_0, 0x7f);
}

static void mdss_dsi_8996_pll_source_standalone(
				struct mdss_dsi_ctrl_pdata *ctrl)
{
	u32 data;

	/*
	 * pll right output enabled
	 * bit clk select from left
	 */
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_PLL_CLKBUFLR_EN, 0x01);
	data = MIPI_INP((ctrl->phy_io.base) + DSIPHY_CMN_GLBL_TEST_CTRL);
	data &= ~BIT(2);
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_CMN_GLBL_TEST_CTRL, data);
}

static void mdss_dsi_8996_pll_source_from_right(
				struct mdss_dsi_ctrl_pdata *ctrl)
{
	u32 data;

	/*
	 * pll left + right output disabled
	 * bit clk select from right
	 */
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_PLL_CLKBUFLR_EN, 0x00);
	data = MIPI_INP((ctrl->phy_io.base) + DSIPHY_CMN_GLBL_TEST_CTRL);
	data |= BIT(2);
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_CMN_GLBL_TEST_CTRL, data);

	/* enable bias current for pll1 during split display case */
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_PLL_PLL_BANDGAP, 0x3);
}

static void mdss_dsi_8996_pll_source_from_left(
				struct mdss_dsi_ctrl_pdata *ctrl)
{
	u32 data;

	/*
	 * pll left + right output enabled
	 * bit clk select from left
	 */
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_PLL_CLKBUFLR_EN, 0x03);
	data = MIPI_INP((ctrl->phy_io.base) + DSIPHY_CMN_GLBL_TEST_CTRL);
	data &= ~BIT(2);
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_CMN_GLBL_TEST_CTRL, data);
}

static void mdss_dsi_8996_phy_regulator_enable(
	struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct mdss_dsi_phy_ctrl *pd;
	int j, off, ln, cnt, ln_off;
	char *ip;
	void __iomem *base;

	pd = &(((ctrl->panel_data).panel_info.mipi).dsi_phy_db);
	/* 4 lanes + clk lane configuration */
	for (ln = 0; ln < 5; ln++) {
		/*
		 * data lane offset frome base: 0x100
		 * data lane size: 0x80
		 */
		base = ctrl->phy_io.base +
				DATALANE_OFFSET_FROM_BASE_8996;
		base += (ln * DATALANE_SIZE_8996); /* lane base */

		/* vreg ctrl, 1 * 5 */
		cnt = 1;
		ln_off = cnt * ln;
		ip = &pd->regulator[ln_off];
		off = 0x64;
		for (j = 0; j < cnt; j++, off += 4)
			MIPI_OUTP(base + off, *ip++);
	}

	wmb(); /* make sure registers committed */

}

static void mdss_dsi_8996_phy_config(struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct mdss_dsi_phy_ctrl *pd;
	int j, off, ln, cnt, ln_off;
	char *ip;
	void __iomem *base;

	pd = &(((ctrl->panel_data).panel_info.mipi).dsi_phy_db);

	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_CMN_LDO_CNTRL, 0x1c);

	/* clk_en */
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_CMN_GLBL_TEST_CTRL, 0x1);

	if (pd->lanecfg_len != 20) {
		pr_err("%s: wrong lane cfg\n", __func__);
		return;
	}

	if (pd->strength_len != 10) {
		pr_err("%s: wrong strength ctrl\n", __func__);
		return;
	}

	if (pd->regulator_len != 5) {
		pr_err("%s: wrong regulator setting\n", __func__);
		return;
	}

	/* 4 lanes + clk lane configuration */
	for (ln = 0; ln < 5; ln++) {
		/*
		 * data lane offset frome base: 0x100
		 * data lane size: 0x80
		 */
		base = ctrl->phy_io.base +
				DATALANE_OFFSET_FROM_BASE_8996;
		base += (ln * DATALANE_SIZE_8996); /* lane base */

		/* lane cfg, 4 * 5 */
		cnt = 4;
		ln_off = cnt * ln;
		ip = &pd->lanecfg[ln_off];
		off = 0x0;
		for (j = 0; j < cnt; j++) {
			MIPI_OUTP(base + off, *ip++);
			off += 4;
		}

		/* test str */
		MIPI_OUTP(base + 0x14, 0x0088);	/* fixed */

		/* phy timing, 8 * 5 */
		cnt = 8;
		ln_off = cnt * ln;
		ip = &pd->timing_8996[ln_off];
		off = 0x18;
		for (j = 0; j < cnt; j++, off += 4)
			MIPI_OUTP(base + off, *ip++);

		/* strength, 2 * 5 */
		cnt = 2;
		ln_off = cnt * ln;
		ip = &pd->strength[ln_off];
		off = 0x38;
		for (j = 0; j < cnt; j++, off += 4)
			MIPI_OUTP(base + off, *ip++);
	}

	wmb(); /* make sure registers committed */

	/* reset digital block */
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_CMN_CTRL_1, 0x80);
	udelay(100);
	MIPI_OUTP((ctrl->phy_io.base) + DSIPHY_CMN_CTRL_1, 0x00);

	if (mdss_dsi_is_hw_config_split(ctrl->shared_data)) {
		if (mdss_dsi_is_left_ctrl(ctrl))
			mdss_dsi_8996_pll_source_from_left(ctrl);
		else
			mdss_dsi_8996_pll_source_from_right(ctrl);
	} else {
		if (mdss_dsi_is_right_ctrl(ctrl) &&
			mdss_dsi_is_pll_src_pll0(ctrl->shared_data))
			mdss_dsi_8996_pll_source_from_left(ctrl);
		else
			mdss_dsi_8996_pll_source_standalone(ctrl);
	}

	wmb(); /* make sure registers committed */
}

static void mdss_dsi_phy_regulator_ctrl(struct mdss_dsi_ctrl_pdata *ctrl,
	bool enable)
{
	struct mdss_dsi_ctrl_pdata *other_ctrl;
	struct dsi_shared_data *sdata;

	if (!ctrl) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	sdata = ctrl->shared_data;

	mutex_lock(&sdata->phy_reg_lock);
	if (enable) {
		switch (ctrl->shared_data->hw_rev) {
		case MDSS_DSI_HW_REV_104:
		case MDSS_DSI_HW_REV_104_1:
			mdss_dsi_8996_phy_regulator_enable(ctrl);
			break;
		case MDSS_DSI_HW_REV_103:
			mdss_dsi_20nm_phy_regulator_enable(ctrl);
			break;
		default:
			mdss_dsi_28nm_phy_regulator_enable(ctrl);
			break;
		}
		ctrl->is_phyreg_enabled = 1;
	} else {
		/*
		 * In split-dsi/dual-dsi configuration, the dsi phy regulator
		 * should be turned off only when both the DSI devices are
		 * going to be turned off since it is shared.
		 */
		if (mdss_dsi_is_hw_config_split(ctrl->shared_data) ||
			mdss_dsi_is_hw_config_dual(ctrl->shared_data)) {
			other_ctrl = mdss_dsi_get_other_ctrl(ctrl);
			if (other_ctrl && !other_ctrl->is_phyreg_enabled)
				mdss_dsi_phy_regulator_disable(ctrl);
		} else {
			mdss_dsi_phy_regulator_disable(ctrl);
		}
		ctrl->is_phyreg_enabled = 0;
	}
	mutex_unlock(&sdata->phy_reg_lock);
}

static void mdss_dsi_phy_ctrl(struct mdss_dsi_ctrl_pdata *ctrl, bool enable)
{
	struct mdss_dsi_ctrl_pdata *other_ctrl;
	if (!ctrl) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	if (enable) {
		switch (ctrl->shared_data->hw_rev) {
		case MDSS_DSI_HW_REV_104:
		case MDSS_DSI_HW_REV_104_1:
			mdss_dsi_8996_phy_config(ctrl);
			break;
		case MDSS_DSI_HW_REV_103:
			mdss_dsi_20nm_phy_config(ctrl);
			break;
		default:
			mdss_dsi_28nm_phy_config(ctrl);
			break;
		}
	} else {
		/*
		 * In split-dsi configuration, the phy should be disabled for
		 * the first controller only when the second controller is
		 * disabled. This is true regardless of whether broadcast
		 * mode is enabled.
		 */
		if (mdss_dsi_is_hw_config_split(ctrl->shared_data)) {
			other_ctrl = mdss_dsi_get_other_ctrl(ctrl);
			if (mdss_dsi_is_right_ctrl(ctrl) && other_ctrl) {
				mdss_dsi_phy_shutdown(other_ctrl);
				mdss_dsi_phy_shutdown(ctrl);
			}
		} else {
			mdss_dsi_phy_shutdown(ctrl);
		}
	}
}

void mdss_dsi_phy_disable(struct mdss_dsi_ctrl_pdata *ctrl)
{
	if (ctrl == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return;
	}

	mdss_dsi_phy_ctrl(ctrl, false);
	mdss_dsi_phy_regulator_ctrl(ctrl, false);
	/*
	 * Wait for the registers writes to complete in order to
	 * ensure that the phy is completely disabled
	 */
	wmb();
}

void mdss_dsi_phy_init(struct mdss_dsi_ctrl_pdata *ctrl)
{
	mdss_dsi_phy_regulator_ctrl(ctrl, true);
	mdss_dsi_phy_ctrl(ctrl, true);
}

void mdss_dsi_core_clk_deinit(struct device *dev, struct dsi_shared_data *sdata)
{
	if (sdata->mmss_misc_ahb_clk)
		devm_clk_put(dev, sdata->mmss_misc_ahb_clk);
	if (sdata->ext_pixel1_clk)
		devm_clk_put(dev, sdata->ext_pixel1_clk);
	if (sdata->ext_byte1_clk)
		devm_clk_put(dev, sdata->ext_byte1_clk);
	if (sdata->ext_pixel0_clk)
		devm_clk_put(dev, sdata->ext_pixel0_clk);
	if (sdata->ext_byte0_clk)
		devm_clk_put(dev, sdata->ext_byte0_clk);
	if (sdata->axi_clk)
		devm_clk_put(dev, sdata->axi_clk);
	if (sdata->ahb_clk)
		devm_clk_put(dev, sdata->ahb_clk);
	if (sdata->mdp_core_clk)
		devm_clk_put(dev, sdata->mdp_core_clk);
}

int mdss_dsi_clk_refresh(struct mdss_panel_data *pdata)
{
	struct mdss_dsi_ctrl_pdata *ctrl_pdata = NULL;
	int rc = 0;

	ctrl_pdata = container_of(pdata, struct mdss_dsi_ctrl_pdata,
							panel_data);
	rc = mdss_dsi_clk_div_config(&pdata->panel_info,
			pdata->panel_info.mipi.frame_rate);
	if (rc) {
		pr_err("%s: unable to initialize the clk dividers\n",
								__func__);
		return rc;
	}
	ctrl_pdata->refresh_clk_rate = false;
	ctrl_pdata->pclk_rate = pdata->panel_info.mipi.dsi_pclk_rate;
	ctrl_pdata->byte_clk_rate = pdata->panel_info.clk_rate / 8;
	pr_debug("%s ctrl_pdata->byte_clk_rate=%d ctrl_pdata->pclk_rate=%d\n",
		__func__, ctrl_pdata->byte_clk_rate, ctrl_pdata->pclk_rate);

	rc = mdss_dsi_clk_set_link_rate(ctrl_pdata->dsi_clk_handle,
			MDSS_DSI_LINK_BYTE_CLK, ctrl_pdata->byte_clk_rate,
			MDSS_DSI_CLK_UPDATE_CLK_RATE_AT_ON);
	if (rc) {
		pr_err("%s: dsi_byte_clk - clk_set_rate failed\n",
				__func__);
		return rc;
	}

	rc = mdss_dsi_clk_set_link_rate(ctrl_pdata->dsi_clk_handle,
			MDSS_DSI_LINK_PIX_CLK, ctrl_pdata->pclk_rate,
			MDSS_DSI_CLK_UPDATE_CLK_RATE_AT_ON);
	if (rc) {
		pr_err("%s: dsi_pixel_clk - clk_set_rate failed\n",
				__func__);
		return rc;
	}
	return rc;
}

int mdss_dsi_core_clk_init(struct platform_device *pdev,
	struct dsi_shared_data *sdata)
{
	struct device *dev = NULL;
	int rc = 0;

	if (!pdev) {
		pr_err("%s: Invalid pdev\n", __func__);
		goto error;
	}

	dev = &pdev->dev;

	/* Mandatory Clocks */
	sdata->mdp_core_clk = devm_clk_get(dev, "mdp_core_clk");
	if (IS_ERR(sdata->mdp_core_clk)) {
		rc = PTR_ERR(sdata->mdp_core_clk);
		pr_err("%s: Unable to get mdp core clk. rc=%d\n",
			__func__, rc);
		goto error;
	}

	sdata->ahb_clk = devm_clk_get(dev, "iface_clk");
	if (IS_ERR(sdata->ahb_clk)) {
		rc = PTR_ERR(sdata->ahb_clk);
		pr_err("%s: Unable to get mdss ahb clk. rc=%d\n",
			__func__, rc);
		goto error;
	}

	sdata->axi_clk = devm_clk_get(dev, "bus_clk");
	if (IS_ERR(sdata->axi_clk)) {
		rc = PTR_ERR(sdata->axi_clk);
		pr_err("%s: Unable to get axi bus clk. rc=%d\n",
			__func__, rc);
		goto error;
	}

	/* Optional Clocks */
	sdata->ext_byte0_clk = devm_clk_get(dev, "ext_byte0_clk");
	if (IS_ERR(sdata->ext_byte0_clk)) {
		pr_debug("%s: unable to get byte0 clk rcg. rc=%d\n",
			__func__, rc);
		sdata->ext_byte0_clk = NULL;
	}

	sdata->ext_pixel0_clk = devm_clk_get(dev, "ext_pixel0_clk");
	if (IS_ERR(sdata->ext_pixel0_clk)) {
		pr_debug("%s: unable to get pixel0 clk rcg. rc=%d\n",
			__func__, rc);
		sdata->ext_pixel0_clk = NULL;
	}

	sdata->ext_byte1_clk = devm_clk_get(dev, "ext_byte1_clk");
	if (IS_ERR(sdata->ext_byte1_clk)) {
		pr_debug("%s: unable to get byte1 clk rcg. rc=%d\n",
			__func__, rc);
		sdata->ext_byte1_clk = NULL;
	}

	sdata->ext_pixel1_clk = devm_clk_get(dev, "ext_pixel1_clk");
	if (IS_ERR(sdata->ext_pixel1_clk)) {
		pr_debug("%s: unable to get pixel1 clk rcg. rc=%d\n",
			__func__, rc);
		sdata->ext_pixel1_clk = NULL;
	}

	sdata->mmss_misc_ahb_clk = devm_clk_get(dev, "core_mmss_clk");
	if (IS_ERR(sdata->mmss_misc_ahb_clk)) {
		sdata->mmss_misc_ahb_clk = NULL;
		pr_debug("%s: Unable to get mmss misc ahb clk\n",
			__func__);
	}

error:
	if (rc)
		mdss_dsi_core_clk_deinit(dev, sdata);
	return rc;
}

void mdss_dsi_link_clk_deinit(struct device *dev,
	struct mdss_dsi_ctrl_pdata *ctrl)
{
	if (ctrl->vco_dummy_clk)
		devm_clk_put(dev, ctrl->vco_dummy_clk);
	if (ctrl->pixel_clk_rcg)
		devm_clk_put(dev, ctrl->pixel_clk_rcg);
	if (ctrl->byte_clk_rcg)
		devm_clk_put(dev, ctrl->byte_clk_rcg);
	if (ctrl->byte_clk)
		devm_clk_put(dev, ctrl->byte_clk);
	if (ctrl->esc_clk)
		devm_clk_put(dev, ctrl->esc_clk);
	if (ctrl->pixel_clk)
		devm_clk_put(dev, ctrl->pixel_clk);
}

int mdss_dsi_link_clk_init(struct platform_device *pdev,
	struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct device *dev = NULL;
	int rc = 0;

	if (!pdev) {
		pr_err("%s: Invalid pdev\n", __func__);
		goto error;
	}

	dev = &pdev->dev;

	/* Mandatory Clocks */
	ctrl->byte_clk = devm_clk_get(dev, "byte_clk");
	if (IS_ERR(ctrl->byte_clk)) {
		rc = PTR_ERR(ctrl->byte_clk);
		pr_err("%s: can't find dsi_byte_clk. rc=%d\n",
			__func__, rc);
		ctrl->byte_clk = NULL;
		goto error;
	}

	ctrl->pixel_clk = devm_clk_get(dev, "pixel_clk");
	if (IS_ERR(ctrl->pixel_clk)) {
		rc = PTR_ERR(ctrl->pixel_clk);
		pr_err("%s: can't find dsi_pixel_clk. rc=%d\n",
			__func__, rc);
		ctrl->pixel_clk = NULL;
		goto error;
	}

	ctrl->esc_clk = devm_clk_get(dev, "core_clk");
	if (IS_ERR(ctrl->esc_clk)) {
		rc = PTR_ERR(ctrl->esc_clk);
		pr_err("%s: can't find dsi_esc_clk. rc=%d\n",
			__func__, rc);
		ctrl->esc_clk = NULL;
		goto error;
	}

	/* Optional Clocks */
	ctrl->byte_clk_rcg = devm_clk_get(dev, "byte_clk_rcg");
	if (IS_ERR(ctrl->byte_clk_rcg)) {
		pr_debug("%s: can't find byte clk rcg. rc=%d\n", __func__, rc);
		ctrl->byte_clk_rcg = NULL;
	}

	ctrl->pixel_clk_rcg = devm_clk_get(dev, "pixel_clk_rcg");
	if (IS_ERR(ctrl->pixel_clk_rcg)) {
		pr_debug("%s: can't find pixel clk rcg. rc=%d\n", __func__, rc);
		ctrl->pixel_clk_rcg = NULL;
	}

	ctrl->vco_dummy_clk = devm_clk_get(dev, "pll_vco_dummy_clk");
	if (IS_ERR(ctrl->vco_dummy_clk)) {
		pr_debug("%s: can't find vco dummy clk. rc=%d\n", __func__, rc);
		ctrl->vco_dummy_clk = NULL;
	}

error:
	if (rc)
		mdss_dsi_link_clk_deinit(dev, ctrl);
	return rc;
}

void mdss_dsi_shadow_clk_deinit(struct device *dev,
	struct mdss_dsi_ctrl_pdata *ctrl)
{
	if (ctrl->mux_byte_clk)
		devm_clk_put(dev, ctrl->mux_byte_clk);
	if (ctrl->mux_pixel_clk)
		devm_clk_put(dev, ctrl->mux_pixel_clk);
	if (ctrl->pll_byte_clk)
		devm_clk_put(dev, ctrl->pll_byte_clk);
	if (ctrl->pll_pixel_clk)
		devm_clk_put(dev, ctrl->pll_pixel_clk);
	if (ctrl->shadow_byte_clk)
		devm_clk_put(dev, ctrl->shadow_byte_clk);
	if (ctrl->shadow_pixel_clk)
		devm_clk_put(dev, ctrl->shadow_pixel_clk);
}

int mdss_dsi_shadow_clk_init(struct platform_device *pdev,
		struct mdss_dsi_ctrl_pdata *ctrl)
{
	struct device *dev = NULL;
	int rc = 0;

	if (!pdev) {
		pr_err("%s: Invalid pdev\n", __func__);
		return -EINVAL;
	}

	dev = &pdev->dev;
	ctrl->mux_byte_clk = devm_clk_get(dev, "pll_byte_clk_mux");
	if (IS_ERR(ctrl->mux_byte_clk)) {
		rc = PTR_ERR(ctrl->mux_byte_clk);
		pr_err("%s: can't find mux_byte_clk. rc=%d\n",
			__func__, rc);
		ctrl->mux_byte_clk = NULL;
		goto error;
	}

	ctrl->mux_pixel_clk = devm_clk_get(dev, "pll_pixel_clk_mux");
	if (IS_ERR(ctrl->mux_pixel_clk)) {
		rc = PTR_ERR(ctrl->mux_pixel_clk);
		pr_err("%s: can't find mdss_mux_pixel_clk. rc=%d\n",
			__func__, rc);
		ctrl->mux_pixel_clk = NULL;
		goto error;
	}

	ctrl->pll_byte_clk = devm_clk_get(dev, "pll_byte_clk_src");
	if (IS_ERR(ctrl->pll_byte_clk)) {
		rc = PTR_ERR(ctrl->pll_byte_clk);
		pr_err("%s: can't find pll_byte_clk. rc=%d\n",
			__func__, rc);
		ctrl->pll_byte_clk = NULL;
		goto error;
	}

	ctrl->pll_pixel_clk = devm_clk_get(dev, "pll_pixel_clk_src");
	if (IS_ERR(ctrl->pll_pixel_clk)) {
		rc = PTR_ERR(ctrl->pll_pixel_clk);
		pr_err("%s: can't find pll_pixel_clk. rc=%d\n",
			__func__, rc);
		ctrl->pll_pixel_clk = NULL;
		goto error;
	}

	ctrl->shadow_byte_clk = devm_clk_get(dev, "pll_shadow_byte_clk_src");
	if (IS_ERR(ctrl->shadow_byte_clk)) {
		rc = PTR_ERR(ctrl->shadow_byte_clk);
		pr_err("%s: can't find shadow_byte_clk. rc=%d\n",
			__func__, rc);
		ctrl->shadow_byte_clk = NULL;
		goto error;
	}

	ctrl->shadow_pixel_clk = devm_clk_get(dev, "pll_shadow_pixel_clk_src");
	if (IS_ERR(ctrl->shadow_pixel_clk)) {
		rc = PTR_ERR(ctrl->shadow_pixel_clk);
		pr_err("%s: can't find shadow_pixel_clk. rc=%d\n",
			__func__, rc);
		ctrl->shadow_pixel_clk = NULL;
		goto error;
	}

error:
	if (rc)
		mdss_dsi_shadow_clk_deinit(dev, ctrl);
	return rc;
}

int mdss_dsi_clk_div_config(struct mdss_panel_info *panel_info,
			    int frame_rate)
{
	struct mdss_panel_data *pdata  = container_of(panel_info,
			struct mdss_panel_data, panel_info);
	struct  mdss_dsi_ctrl_pdata *ctrl_pdata = container_of(pdata,
			struct mdss_dsi_ctrl_pdata, panel_data);
	u64 h_period, v_period, clk_rate;
	u32 dsi_pclk_rate;
	u8 lanes = 0, bpp;

	if (panel_info->mipi.data_lane3)
		lanes += 1;
	if (panel_info->mipi.data_lane2)
		lanes += 1;
	if (panel_info->mipi.data_lane1)
		lanes += 1;
	if (panel_info->mipi.data_lane0)
		lanes += 1;

	switch (panel_info->mipi.dst_format) {
	case DSI_CMD_DST_FORMAT_RGB888:
	case DSI_VIDEO_DST_FORMAT_RGB888:
	case DSI_VIDEO_DST_FORMAT_RGB666_LOOSE:
		bpp = 3;
		break;
	case DSI_CMD_DST_FORMAT_RGB565:
	case DSI_VIDEO_DST_FORMAT_RGB565:
		bpp = 2;
		break;
	default:
		bpp = 3;	/* Default format set to RGB888 */
		break;
	}

	h_period = mdss_panel_get_htotal(panel_info, true);
	v_period = mdss_panel_get_vtotal(panel_info);

	if (ctrl_pdata->refresh_clk_rate || (frame_rate !=
	     panel_info->mipi.frame_rate) ||
	    (!panel_info->clk_rate)) {
		if (lanes > 0) {
			panel_info->clk_rate = h_period * v_period * frame_rate
				* bpp * 8;
			do_div(panel_info->clk_rate, lanes);
		} else {
			pr_err("%s: forcing mdss_dsi lanes to 1\n", __func__);
			panel_info->clk_rate =
				h_period * v_period * frame_rate * bpp * 8;
		}
	}

	if (panel_info->clk_rate == 0)
		panel_info->clk_rate = 454000000;

	clk_rate = panel_info->clk_rate;
	do_div(clk_rate, 8 * bpp);
	dsi_pclk_rate = (u32) clk_rate * lanes;

	if ((dsi_pclk_rate < 3300000) || (dsi_pclk_rate > 250000000))
		dsi_pclk_rate = 35000000;
	panel_info->mipi.dsi_pclk_rate = dsi_pclk_rate;

	return 0;
}

/**
 * mdss_dsi_ulps_config() - Program DSI lanes to enter/exit ULPS mode
 * @ctrl: pointer to DSI controller structure
 * @enable: 1 to enter ULPS, 0 to exit ULPS
 *
 * This function executes the necessary programming sequence to enter/exit
 * DSI Ultra-Low Power State (ULPS). This function assumes that the link and
 * core clocks are already on.
 */
static int mdss_dsi_ulps_config(struct mdss_dsi_ctrl_pdata *ctrl,
	int enable)
{
	int ret = 0;
	struct mdss_panel_data *pdata = NULL;
	struct mdss_panel_info *pinfo;
	struct mipi_panel_info *mipi;
	u32 lane_status = 0;
	u32 active_lanes = 0;

	if (!ctrl) {
		pr_err("%s: invalid input\n", __func__);
		return -EINVAL;
	}

	pdata = &ctrl->panel_data;
	if (!pdata) {
		pr_err("%s: Invalid panel data\n", __func__);
		return -EINVAL;
	}
	pinfo = &pdata->panel_info;
	mipi = &pinfo->mipi;

	if (!mdss_dsi_ulps_feature_enabled(pdata) ||
			!pinfo->ulps_suspend_enabled) {
		pr_debug("%s: ULPS feature is not enabled\n", __func__);
		return 0;
	}

	/*
	 * No need to enter ULPS when transitioning from splash screen to
	 * boot animation since it is expected that the clocks would be turned
	 * right back on.
	 */
	if (pinfo->cont_splash_enabled) {
		pr_debug("%s: skip ULPS config with splash screen enabled\n",
			__func__);
		return 0;
	}

	/* clock lane will always be programmed for ulps */
	active_lanes = BIT(4);
	/*
	 * make a note of all active data lanes for which ulps entry/exit
	 * is needed
	 */
	if (mipi->data_lane0)
		active_lanes |= BIT(0);
	if (mipi->data_lane1)
		active_lanes |= BIT(1);
	if (mipi->data_lane2)
		active_lanes |= BIT(2);
	if (mipi->data_lane3)
		active_lanes |= BIT(3);

	pr_debug("%s: configuring ulps (%s) for ctrl%d, active lanes=0x%08x\n",
		__func__, (enable ? "on" : "off"), ctrl->ndx,
		active_lanes);

	if (enable && !ctrl->ulps) {
		/*
		 * ULPS Entry Request.
		 * Wait for a short duration to ensure that the lanes
		 * enter ULP state.
		 */
		MIPI_OUTP(ctrl->ctrl_base + 0x0AC, active_lanes);
		usleep_range(100, 100);

		/* Check to make sure that all active data lanes are in ULPS */
		lane_status = MIPI_INP(ctrl->ctrl_base + 0xA8);
		if (lane_status & (active_lanes << 8)) {
			pr_err("%s: ULPS entry req failed for ctrl%d. Lane status=0x%08x\n",
				__func__, ctrl->ndx, lane_status);
			ret = -EINVAL;
			goto error;
		}

		ctrl->ulps = true;
	} else if (!enable && ctrl->ulps) {
		/*
		 * Clear out any phy errors prior to exiting ULPS
		 * This fixes certain instances where phy does not exit
		 * ULPS cleanly.
		 */
		mdss_dsi_dln0_phy_err(ctrl);

		/*
		 * ULPS Exit Request
		 * Hardware requirement is to wait for at least 1ms
		 */
		MIPI_OUTP(ctrl->ctrl_base + 0x0AC, active_lanes << 8);
		usleep_range(1000, 1000);

		/*
		 * Sometimes when exiting ULPS, it is possible that some DSI
		 * lanes are not in the stop state which could lead to DSI
		 * commands not going through. To avoid this, force the lanes
		 * to be in stop state.
		 */
		MIPI_OUTP(ctrl->ctrl_base + 0x0AC, active_lanes << 16);

		MIPI_OUTP(ctrl->ctrl_base + 0x0AC, 0x0);

		/*
		 * Wait for a short duration before enabling
		 * data transmission
		 */
		usleep_range(100, 100);

		lane_status = MIPI_INP(ctrl->ctrl_base + 0xA8);
		ctrl->ulps = false;
	} else {
		pr_debug("%s: No change requested: %s -> %s\n", __func__,
			ctrl->ulps ? "enabled" : "disabled",
			enable ? "enabled" : "disabled");
	}

	pr_debug("%s: DSI lane status = 0x%08x. Ulps %s\n", __func__,
		lane_status, enable ? "enabled" : "disabled");

error:
	return ret;
}

/**
 * mdss_dsi_clamp_ctrl() - Program DSI clamps for supporting power collapse
 * @ctrl: pointer to DSI controller structure
 * @enable: 1 to enable clamps, 0 to disable clamps
 *
 * For idle-screen usecases with command mode panels, MDSS can be power
 * collapsed. However, DSI phy needs to remain on. To avoid any mismatch
 * between the DSI controller state, DSI phy needs to be clamped before
 * power collapsing. This function executes the required programming
 * sequence to configure these DSI clamps. This function should only be called
 * when the DSI link clocks are disabled.
 */
static int mdss_dsi_clamp_ctrl(struct mdss_dsi_ctrl_pdata *ctrl, int enable)
{
	struct mipi_panel_info *mipi = NULL;
	u32 clamp_reg, regval = 0;
	u32 clamp_reg_off, phyrst_reg_off;

	if (!ctrl) {
		pr_err("%s: invalid input\n", __func__);
		return -EINVAL;
	}

	if (!ctrl->mmss_misc_io.base) {
		pr_err("%s: mmss_misc_io not mapped\n", __func__);
		return -EINVAL;
	}

	clamp_reg_off = ctrl->shared_data->ulps_clamp_ctrl_off;
	phyrst_reg_off = ctrl->shared_data->ulps_phyrst_ctrl_off;
	mipi = &ctrl->panel_data.panel_info.mipi;

	/* clock lane will always be clamped */
	clamp_reg = BIT(9);
	if (ctrl->ulps)
		clamp_reg |= BIT(8);
	/* make a note of all active data lanes which need to be clamped */
	if (mipi->data_lane0) {
		clamp_reg |= BIT(7);
		if (ctrl->ulps)
			clamp_reg |= BIT(6);
	}
	if (mipi->data_lane1) {
		clamp_reg |= BIT(5);
		if (ctrl->ulps)
			clamp_reg |= BIT(4);
	}
	if (mipi->data_lane2) {
		clamp_reg |= BIT(3);
		if (ctrl->ulps)
			clamp_reg |= BIT(2);
	}
	if (mipi->data_lane3) {
		clamp_reg |= BIT(1);
		if (ctrl->ulps)
			clamp_reg |= BIT(0);
	}
	pr_debug("%s: called for ctrl%d, enable=%d, clamp_reg=0x%08x\n",
		__func__, ctrl->ndx, enable, clamp_reg);
	if (enable && !ctrl->mmss_clamp) {
		regval = MIPI_INP(ctrl->mmss_misc_io.base + clamp_reg_off);
		/* Enable MMSS DSI Clamps */
		if (ctrl->ndx == DSI_CTRL_0) {
			MIPI_OUTP(ctrl->mmss_misc_io.base + clamp_reg_off,
				regval | clamp_reg);
			MIPI_OUTP(ctrl->mmss_misc_io.base + clamp_reg_off,
				regval | (clamp_reg | BIT(15)));
		} else if (ctrl->ndx == DSI_CTRL_1) {
			MIPI_OUTP(ctrl->mmss_misc_io.base + clamp_reg_off,
				regval | (clamp_reg << 16));
			MIPI_OUTP(ctrl->mmss_misc_io.base + clamp_reg_off,
				regval | ((clamp_reg << 16) | BIT(31)));
		}
		/* update clamp ctrl before setting phy reset disable */
		wmb();

		/*
		 * This register write ensures that DSI PHY will not be
		 * reset when mdss ahb clock reset is asserted while coming
		 * out of power collapse
		 */
		if (IS_MDSS_MAJOR_MINOR_SAME(ctrl->shared_data->hw_rev,
			MDSS_DSI_HW_REV_104)) {

			regval = MIPI_INP(ctrl->mmss_misc_io.base +
				clamp_reg_off);
			MIPI_OUTP(ctrl->mmss_misc_io.base + clamp_reg_off,
				regval | BIT(30));
		} else {
			MIPI_OUTP(ctrl->mmss_misc_io.base + phyrst_reg_off,
				0x1);
		}
		/* make sure that clamp ctrl is updated before disable call */
		wmb();
		ctrl->mmss_clamp = true;
	} else if (!enable && ctrl->mmss_clamp) {
		if (IS_MDSS_MAJOR_MINOR_SAME(ctrl->shared_data->hw_rev,
			MDSS_DSI_HW_REV_104)) {

			regval = MIPI_INP(ctrl->mmss_misc_io.base +
				clamp_reg_off);
			MIPI_OUTP(ctrl->mmss_misc_io.base + clamp_reg_off,
				regval & ~BIT(30));
		} else {
			MIPI_OUTP(ctrl->mmss_misc_io.base + phyrst_reg_off,
				0x0);
		}
		/* update clamp ctrl before unsetting phy reset disable */
		wmb();

		regval = MIPI_INP(ctrl->mmss_misc_io.base + clamp_reg_off);
		/* Disable MMSS DSI Clamps */
		if (ctrl->ndx == DSI_CTRL_0)
			MIPI_OUTP(ctrl->mmss_misc_io.base + clamp_reg_off,
				regval & ~(clamp_reg | BIT(15)));
		else if (ctrl->ndx == DSI_CTRL_1)
			MIPI_OUTP(ctrl->mmss_misc_io.base + clamp_reg_off,
				regval & ~((clamp_reg << 16) | BIT(31)));
		/* make sure that clamp ctrl is updated before enable call */
		wmb();
		ctrl->mmss_clamp = false;
	} else {
		pr_debug("%s: No change requested: %s -> %s\n", __func__,
			ctrl->mmss_clamp ? "enabled" : "disabled",
			enable ? "enabled" : "disabled");
	}

	return 0;
}

DEFINE_MUTEX(dsi_clk_mutex);

int mdss_dsi_clk_ctrl(struct mdss_dsi_ctrl_pdata *ctrl, void *clk_handle,
	enum mdss_dsi_clk_type clk_type, enum mdss_dsi_clk_state clk_state)
{
	int rc = 0;
	struct mdss_dsi_ctrl_pdata *mctrl = NULL;
	int i;

	if (!ctrl) {
		pr_err("%s: Invalid arg\n", __func__);
		return -EINVAL;
	}

	mutex_lock(&dsi_clk_mutex);
	/*
	 * In sync_wait_broadcast mode, we need to enable clocks
	 * for the other controller as well when enabling clocks
	 * for the trigger controller.
	 *
	 * If sync wait_broadcase mode is not enabled, but if split display
	 * mode is enabled where both DSI controller's branch clocks are
	 * sourced out of a single PLL, then we need to ensure that the
	 * controller associated with that PLL also has it's clocks turned
	 * on. This is required to make sure that if that controller's PLL/PHY
	 * are clamped then they can be removed.
	 */
	if (mdss_dsi_sync_wait_trigger(ctrl)) {
		mctrl = mdss_dsi_get_other_ctrl(ctrl);
		if (!mctrl)
			pr_warn("%s: Unable to get other control\n", __func__);
	} else if (mdss_dsi_is_ctrl_clk_slave(ctrl)) {
		mctrl = mdss_dsi_get_ctrl_clk_master();
		if (!mctrl)
			pr_warn("%s: Unable to get clk master control\n",
				__func__);
	}

	/*
	 * When DSI is used in split mode, the link clock for master controller
	 * has to be turned on first before the link clock for slave can be
	 * turned on. In case the current controller is a slave, an ON vote is
	 * cast for master before changing the state of the slave clock. After
	 * the state change for slave, the ON votes will be removed depending on
	 * the new state.
	 */
	pr_debug("%s: DSI_%d: clk = %d, state = %d, caller = %pS, mctrl=%d\n",
		 __func__, ctrl->ndx, clk_type, clk_state,
		 __builtin_return_address(0), mctrl ? 1 : 0);
	if (mctrl && (clk_type & MDSS_DSI_LINK_CLK)) {
		rc = mdss_dsi_clk_req_state(mctrl->dsi_clk_handle,
					     MDSS_DSI_ALL_CLKS,
					     MDSS_DSI_CLK_ON);
		if (rc) {
			pr_err("%s: failed to turn on mctrl clocks, rc=%d\n",
				 __func__, rc);
			goto error;
		}
		ctrl->m_vote_cnt++;
	}

	rc = mdss_dsi_clk_req_state(clk_handle, clk_type, clk_state);
	if (rc) {
		pr_err("%s: failed set clk state, rc = %d\n", __func__, rc);
		goto error;
	}

	if (mctrl && (clk_type & MDSS_DSI_LINK_CLK) &&
			clk_state != MDSS_DSI_CLK_ON) {

		/*
		 * In case of split dsi, an ON vote is cast for all state change
		 * requests. If the current state is ON, then the vote would not
		 * be released.
		 *
		 * If the current state is ECG, there is one possible way to
		 * transition in to this state, which is ON -> ECG. In this case
		 * two votes will be removed because one was cast at ON and
		 * other when entering ECG.
		 *
		 * If the current state is OFF, it could have been due to two
		 * possible transitions in to OFF state.
		 *	1. ON -> OFF: In this case two votes were cast by the
		 *	   slave controller, one during ON (which is not
		 *	   removed) and one during OFF. So we need to remove two
		 *	   votes.
		 *	2. ECG -> OFF: In this case there is only one vote
		 *	   for ON, since the previous ECG state must have
		 *	   removed two votes to let clocks turn off.
		 *
		 * To satisfy the above requirement, m_vote_cnt keeps track of
		 * the number of ON votes for master requested by slave. For
		 * every OFF/ECG state request, Either 2 or m_vote_cnt number of
		 * votes are removed depending on which is lower.
		 */
		for (i = 0; (i < ctrl->m_vote_cnt && i < 2); i++) {
			rc = mdss_dsi_clk_req_state(mctrl->dsi_clk_handle,
						     MDSS_DSI_ALL_CLKS,
						     MDSS_DSI_CLK_OFF);
			if (rc) {
				pr_err("%s: failed to set mctrl clk state, rc = %d\n",
				       __func__, rc);
				goto error;
			}
		}
		ctrl->m_vote_cnt -= i;
		pr_debug("%s: ctrl=%d, m_vote_cnt=%d\n", __func__, ctrl->ndx,
			 ctrl->m_vote_cnt);
	}

error:
	mutex_unlock(&dsi_clk_mutex);
	return rc;
}

int mdss_dsi_pre_clkoff_cb(void *priv,
			   enum mdss_dsi_clk_type clk,
			   enum mdss_dsi_clk_state new_state)
{
	int rc = 0;
	struct mdss_dsi_ctrl_pdata *ctrl = priv;
	struct mdss_panel_data *pdata = NULL;

	pdata = &ctrl->panel_data;

	if ((clk & MDSS_DSI_LINK_CLK) && (new_state == MDSS_DSI_CLK_OFF)) {

		rc = mdss_dsi_ulps_config(ctrl, 1);
		if (rc) {
			pr_err("%s: failed enable ulps, rc = %d\n",
			       __func__, rc);
		}
	}

	if ((clk & MDSS_DSI_CORE_CLK) && (new_state == MDSS_DSI_CLK_OFF)) {

		/*
		 * Enable DSI clamps only if entering idle power collapse or
		 * when ULPS during suspend is enabled.
		 */
		if ((pdata->panel_info.blank_state != MDSS_PANEL_BLANK_BLANK) ||
			pdata->panel_info.ulps_suspend_enabled) {
			rc = mdss_dsi_clamp_ctrl(ctrl, 1);
			if (rc)
				pr_err("%s: Failed to enable dsi clamps. rc=%d\n",
					__func__, rc);
		}
	}

	return rc;
}

int mdss_dsi_post_clkon_cb(void *priv,
			   enum mdss_dsi_clk_type clk,
			   enum mdss_dsi_clk_state curr_state)
{
	int rc = 0;
	struct mdss_panel_data *pdata = NULL;
	struct mdss_dsi_ctrl_pdata *ctrl = priv;

	pdata = &ctrl->panel_data;

	if (clk & MDSS_DSI_CORE_CLK) {
		if (!pdata->panel_info.cont_splash_enabled)
			mdss_dsi_read_hw_revision(ctrl);

		/*
		 * Phy and controller setup is needed if coming out of idle
		 * power collapse with clamps enabled.
		 */
		if (ctrl->mmss_clamp) {
			mdss_dsi_phy_init(ctrl);
			mdss_dsi_ctrl_setup(ctrl);
		}

		if (ctrl->ulps) {
			/*
			 * ULPS Entry Request. This is needed if the lanes were
			 * in ULPS prior to power collapse, since after
			 * power collapse and reset, the DSI controller resets
			 * back to idle state and not ULPS. This ulps entry
			 * request will transition the state of the DSI
			 * controller to ULPS which will match the state of the
			 * DSI phy. This needs to be done prior to disabling
			 * the DSI clamps.
			 *
			 * Also, reset the ulps flag so that ulps_config
			 * function would reconfigure the controller state to
			 * ULPS.
			 */
			ctrl->ulps = false;
			rc = mdss_dsi_ulps_config(ctrl, 1);
			if (rc) {
				pr_err("%s: Failed to enter ULPS. rc=%d\n",
					__func__, rc);
				goto error;
			}
		}

		rc = mdss_dsi_clamp_ctrl(ctrl, 0);
		if (rc) {
			pr_err("%s: Failed to disable dsi clamps. rc=%d\n",
				__func__, rc);
			goto error;
		}
	}
	if (clk & MDSS_DSI_LINK_CLK) {
		rc = mdss_dsi_ulps_config(ctrl, 0);
		if (rc) {
			pr_err("%s: failed to disable ulps, rc= %d\n",
			       __func__, rc);
			goto error;
		}
	}
error:
	return rc;
}

int mdss_dsi_post_clkoff_cb(void *priv,
			    enum mdss_dsi_clk_type clk_type,
			    enum mdss_dsi_clk_state curr_state)
{
	int rc = 0;
	struct mdss_dsi_ctrl_pdata *ctrl = priv;
	struct mdss_panel_data *pdata = NULL;
	struct dsi_shared_data *sdata;
	int i;

	if (!ctrl) {
		pr_err("%s: Invalid arg\n", __func__);
		return -EINVAL;
	}

	if ((clk_type & MDSS_DSI_CORE_CLK) &&
	    (curr_state == MDSS_DSI_CLK_OFF)) {
		sdata = ctrl->shared_data;
		pdata = &ctrl->panel_data;

		for (i = DSI_MAX_PM - 1; i >= DSI_CORE_PM; i--) {
			if ((i != DSI_CORE_PM) &&
			    (pdata->panel_info.blank_state !=
			     MDSS_PANEL_BLANK_BLANK))
				continue;
			rc = msm_dss_enable_vreg(
				sdata->power_data[i].vreg_config,
				sdata->power_data[i].num_vreg, 0);
			if (rc) {
				pr_warn("%s: failed to disable vregs for %s\n",
					__func__,
					__mdss_dsi_pm_name(i));
				rc = 0;
			} else {
				ctrl->core_power = false;
			}
		}
	}
	return rc;
}

int mdss_dsi_pre_clkon_cb(void *priv,
			  enum mdss_dsi_clk_type clk_type,
			  enum mdss_dsi_clk_state new_state)
{
	int rc = 0;
	struct mdss_dsi_ctrl_pdata *ctrl = priv;
	struct mdss_panel_data *pdata = NULL;
	struct dsi_shared_data *sdata;
	int i;

	if (!ctrl) {
		pr_err("%s: invalid input\n", __func__);
		return -EINVAL;
	}

	if ((clk_type & MDSS_DSI_CORE_CLK) && (new_state == MDSS_DSI_CLK_ON) &&
	    (ctrl->core_power == false)) {
		sdata = ctrl->shared_data;
		pdata = &ctrl->panel_data;
		/*
		 *              Enable DSI core power
		 * 1.> PANEL_PM are controlled as part of
		 *     panel_power_ctrl. Needed not be handled here.
		 * 2.> CORE_PM are controlled by dsi clk manager.
		 * 2.> PHY_PM and CTRL_PM need to be enabled/disabled
		 *     only during unblank/blank. Their state should
		 *     not be changed during static screen.
		 */
		pr_debug("%s: Enable DSI core power\n", __func__);
		for (i = DSI_CORE_PM; i < DSI_MAX_PM; i++) {
			if ((i != DSI_CORE_PM) &&
			    (pdata->panel_info.blank_state !=
				MDSS_PANEL_BLANK_BLANK) &&
				!pdata->panel_info.cont_splash_enabled)
				continue;
			rc = msm_dss_enable_vreg(
				sdata->power_data[i].vreg_config,
				sdata->power_data[i].num_vreg, 1);
			if (rc) {
				pr_err("%s: failed to enable vregs for %s\n",
					__func__,
					__mdss_dsi_pm_name(i));
			} else {
				ctrl->core_power = true;
			}

		}
	}

	return rc;
}

void mdss_edp_clk_deinit(struct mdss_edp_drv_pdata *edp_drv)
{
	if (edp_drv->aux_clk)
		clk_put(edp_drv->aux_clk);
	if (edp_drv->pixel_clk)
		clk_put(edp_drv->pixel_clk);
	if (edp_drv->ahb_clk)
		clk_put(edp_drv->ahb_clk);
	if (edp_drv->link_clk)
		clk_put(edp_drv->link_clk);
	if (edp_drv->mdp_core_clk)
		clk_put(edp_drv->mdp_core_clk);
}

int mdss_edp_clk_init(struct mdss_edp_drv_pdata *edp_drv)
{
	struct device *dev = &(edp_drv->pdev->dev);

	edp_drv->aux_clk = clk_get(dev, "core_clk");
	if (IS_ERR(edp_drv->aux_clk)) {
		pr_err("%s: Can't find aux_clk", __func__);
		edp_drv->aux_clk = NULL;
		goto mdss_edp_clk_err;
	}

	edp_drv->pixel_clk = clk_get(dev, "pixel_clk");
	if (IS_ERR(edp_drv->pixel_clk)) {
		pr_err("%s: Can't find pixel_clk", __func__);
		edp_drv->pixel_clk = NULL;
		goto mdss_edp_clk_err;
	}

	edp_drv->ahb_clk = clk_get(dev, "iface_clk");
	if (IS_ERR(edp_drv->ahb_clk)) {
		pr_err("%s: Can't find ahb_clk", __func__);
		edp_drv->ahb_clk = NULL;
		goto mdss_edp_clk_err;
	}

	edp_drv->link_clk = clk_get(dev, "link_clk");
	if (IS_ERR(edp_drv->link_clk)) {
		pr_err("%s: Can't find link_clk", __func__);
		edp_drv->link_clk = NULL;
		goto mdss_edp_clk_err;
	}

	/* need mdss clock to receive irq */
	edp_drv->mdp_core_clk = clk_get(dev, "mdp_core_clk");
	if (IS_ERR(edp_drv->mdp_core_clk)) {
		pr_err("%s: Can't find mdp_core_clk", __func__);
		edp_drv->mdp_core_clk = NULL;
		goto mdss_edp_clk_err;
	}

	return 0;

mdss_edp_clk_err:
	mdss_edp_clk_deinit(edp_drv);
	return -EPERM;
}

int mdss_edp_aux_clk_enable(struct mdss_edp_drv_pdata *edp_drv)
{
	int ret;

	if (clk_set_rate(edp_drv->aux_clk, 19200000) < 0)
		pr_err("%s: aux_clk - clk_set_rate failed\n",
					__func__);

	ret = clk_enable(edp_drv->aux_clk);
	if (ret) {
		pr_err("%s: Failed to enable aux clk\n", __func__);
		goto c2;
	}

	ret = clk_enable(edp_drv->ahb_clk);
	if (ret) {
		pr_err("%s: Failed to enable ahb clk\n", __func__);
		goto c1;
	}

	/* need mdss clock to receive irq */
	ret = clk_enable(edp_drv->mdp_core_clk);
	if (ret) {
		pr_err("%s: Failed to enable mdp_core_clk\n", __func__);
		goto c0;
	}

	return 0;
c0:
	clk_disable(edp_drv->ahb_clk);
c1:
	clk_disable(edp_drv->aux_clk);
c2:
	return ret;

}

void mdss_edp_aux_clk_disable(struct mdss_edp_drv_pdata *edp_drv)
{
	clk_disable(edp_drv->aux_clk);
	clk_disable(edp_drv->ahb_clk);
	clk_disable(edp_drv->mdp_core_clk);
}

static void mdss_edp_clk_set_rate(struct mdss_edp_drv_pdata *edp_drv)
{
	if (clk_set_rate(edp_drv->link_clk, edp_drv->link_rate * 27000000) < 0)
		pr_err("%s: link_clk - clk_set_rate failed\n",
					__func__);

	if (clk_set_rate(edp_drv->pixel_clk, edp_drv->pixel_rate) < 0)
		pr_err("%s: pixel_clk - clk_set_rate failed\n",
					__func__);
}

int mdss_edp_clk_enable(struct mdss_edp_drv_pdata *edp_drv)
{
	int ret;

	if (edp_drv->clk_on) {
		pr_info("%s: edp clks are already ON\n", __func__);
		return 0;
	}

	if (clk_set_rate(edp_drv->link_clk, edp_drv->link_rate * 27000000) < 0)
		pr_err("%s: link_clk - clk_set_rate failed\n",
					__func__);

	if (clk_set_rate(edp_drv->aux_clk, edp_drv->aux_rate) < 0)
		pr_err("%s: aux_clk - clk_set_rate failed\n",
					__func__);

	if (clk_set_rate(edp_drv->pixel_clk, edp_drv->pixel_rate) < 0)
		pr_err("%s: pixel_clk - clk_set_rate failed\n",
					__func__);

	ret = clk_enable(edp_drv->aux_clk);
	if (ret) {
		pr_err("%s: Failed to enable aux clk\n", __func__);
		goto c4;
	}
	ret = clk_enable(edp_drv->pixel_clk);
	if (ret) {
		pr_err("%s: Failed to enable pixel clk\n", __func__);
		goto c3;
	}
	ret = clk_enable(edp_drv->ahb_clk);
	if (ret) {
		pr_err("%s: Failed to enable ahb clk\n", __func__);
		goto c2;
	}
	ret = clk_enable(edp_drv->link_clk);
	if (ret) {
		pr_err("%s: Failed to enable link clk\n", __func__);
		goto c1;
	}
	ret = clk_enable(edp_drv->mdp_core_clk);
	if (ret) {
		pr_err("%s: Failed to enable mdp_core_clk\n", __func__);
		goto c0;
	}

	edp_drv->clk_on = 1;

	return 0;

c0:
	clk_disable(edp_drv->link_clk);
c1:
	clk_disable(edp_drv->ahb_clk);
c2:
	clk_disable(edp_drv->pixel_clk);
c3:
	clk_disable(edp_drv->aux_clk);
c4:
	return ret;
}

void mdss_edp_clk_disable(struct mdss_edp_drv_pdata *edp_drv)
{
	if (edp_drv->clk_on == 0) {
		pr_info("%s: edp clks are already OFF\n", __func__);
		return;
	}

	clk_disable(edp_drv->aux_clk);
	clk_disable(edp_drv->pixel_clk);
	clk_disable(edp_drv->ahb_clk);
	clk_disable(edp_drv->link_clk);
	clk_disable(edp_drv->mdp_core_clk);

	edp_drv->clk_on = 0;
}

int mdss_edp_prepare_aux_clocks(struct mdss_edp_drv_pdata *edp_drv)
{
	int ret;

	/* ahb clock should be prepared first */
	ret = clk_prepare(edp_drv->ahb_clk);
	if (ret) {
		pr_err("%s: Failed to prepare ahb clk\n", __func__);
		goto c3;
	}
	ret = clk_prepare(edp_drv->aux_clk);
	if (ret) {
		pr_err("%s: Failed to prepare aux clk\n", __func__);
		goto c2;
	}

	/* need mdss clock to receive irq */
	ret = clk_prepare(edp_drv->mdp_core_clk);
	if (ret) {
		pr_err("%s: Failed to prepare mdp_core clk\n", __func__);
		goto c1;
	}

	return 0;
c1:
	clk_unprepare(edp_drv->aux_clk);
c2:
	clk_unprepare(edp_drv->ahb_clk);
c3:
	return ret;

}

void mdss_edp_unprepare_aux_clocks(struct mdss_edp_drv_pdata *edp_drv)
{
	clk_unprepare(edp_drv->mdp_core_clk);
	clk_unprepare(edp_drv->aux_clk);
	clk_unprepare(edp_drv->ahb_clk);
}

int mdss_edp_prepare_clocks(struct mdss_edp_drv_pdata *edp_drv)
{
	int ret;

	mdss_edp_clk_set_rate(edp_drv);

	/* ahb clock should be prepared first */
	ret = clk_prepare(edp_drv->ahb_clk);
	if (ret) {
		pr_err("%s: Failed to prepare ahb clk\n", __func__);
		goto c4;
	}
	ret = clk_prepare(edp_drv->aux_clk);
	if (ret) {
		pr_err("%s: Failed to prepare aux clk\n", __func__);
		goto c3;
	}
	ret = clk_prepare(edp_drv->pixel_clk);
	if (ret) {
		pr_err("%s: Failed to prepare pixel clk\n", __func__);
		goto c2;
	}
	ret = clk_prepare(edp_drv->link_clk);
	if (ret) {
		pr_err("%s: Failed to prepare link clk\n", __func__);
		goto c1;
	}
	ret = clk_prepare(edp_drv->mdp_core_clk);
	if (ret) {
		pr_err("%s: Failed to prepare mdp_core clk\n", __func__);
		goto c0;
	}

	return 0;
c0:
	clk_unprepare(edp_drv->link_clk);
c1:
	clk_unprepare(edp_drv->pixel_clk);
c2:
	clk_unprepare(edp_drv->aux_clk);
c3:
	clk_unprepare(edp_drv->ahb_clk);
c4:
	return ret;
}

void mdss_edp_unprepare_clocks(struct mdss_edp_drv_pdata *edp_drv)
{
	clk_unprepare(edp_drv->mdp_core_clk);
	clk_unprepare(edp_drv->aux_clk);
	clk_unprepare(edp_drv->pixel_clk);
	clk_unprepare(edp_drv->link_clk);
	/* ahb clock should be last one to disable */
	clk_unprepare(edp_drv->ahb_clk);
}

void mdss_edp_clk_debug(unsigned char *edp_base, unsigned char *mmss_cc_base)
{
	u32 da4, da0, d32c;
	u32 dc4, dc0, d330;

	/* pixel clk */
	da0  = edp_read(mmss_cc_base + 0x0a0);
	da4  = edp_read(mmss_cc_base + 0x0a4);
	d32c = edp_read(mmss_cc_base + 0x32c);

	/* main link clk */
	dc0  = edp_read(mmss_cc_base + 0x0c0);
	dc4  = edp_read(mmss_cc_base + 0x0c4);
	d330 = edp_read(mmss_cc_base + 0x330);

	pr_err("%s: da0=%x da4=%x d32c=%x dc0=%x dc4=%x d330=%x\n", __func__,
	(int)da0, (int)da4, (int)d32c, (int)dc0, (int)dc4, (int)d330);

}
