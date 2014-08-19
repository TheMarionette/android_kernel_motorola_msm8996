/*
 * s2mpu02.h
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd
 *              http://www.samsung.com
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __LINUX_MFD_S2MPU02_H
#define __LINUX_MFD_S2MPU02_H

/* S2MPU02 registers */
enum S2MPU02_reg {
	S2MPU02_REG_ID,
	S2MPU02_REG_INT1,
	S2MPU02_REG_INT2,
	S2MPU02_REG_INT3,
	S2MPU02_REG_INT1M,
	S2MPU02_REG_INT2M,
	S2MPU02_REG_INT3M,
	S2MPU02_REG_ST1,
	S2MPU02_REG_ST2,
	S2MPU02_REG_PWRONSRC,
	S2MPU02_REG_OFFSRC,
	S2MPU02_REG_BU_CHG,
	S2MPU02_REG_RTCCTRL,
	S2MPU02_REG_PMCTRL1,
	S2MPU02_REG_RSVD1,
	S2MPU02_REG_RSVD2,
	S2MPU02_REG_RSVD3,
	S2MPU02_REG_RSVD4,
	S2MPU02_REG_RSVD5,
	S2MPU02_REG_RSVD6,
	S2MPU02_REG_RSVD7,
	S2MPU02_REG_WRSTEN,
	S2MPU02_REG_RSVD8,
	S2MPU02_REG_RSVD9,
	S2MPU02_REG_RSVD10,
	S2MPU02_REG_B1CTRL1,
	S2MPU02_REG_B1CTRL2,
	S2MPU02_REG_B2CTRL1,
	S2MPU02_REG_B2CTRL2,
	S2MPU02_REG_B3CTRL1,
	S2MPU02_REG_B3CTRL2,
	S2MPU02_REG_B4CTRL1,
	S2MPU02_REG_B4CTRL2,
	S2MPU02_REG_B5CTRL1,
	S2MPU02_REG_B5CTRL2,
	S2MPU02_REG_B5CTRL3,
	S2MPU02_REG_B5CTRL4,
	S2MPU02_REG_B5CTRL5,
	S2MPU02_REG_B6CTRL1,
	S2MPU02_REG_B6CTRL2,
	S2MPU02_REG_B7CTRL1,
	S2MPU02_REG_B7CTRL2,
	S2MPU02_REG_RAMP1,
	S2MPU02_REG_RAMP2,
	S2MPU02_REG_L1CTRL,
	S2MPU02_REG_L2CTRL1,
	S2MPU02_REG_L2CTRL2,
	S2MPU02_REG_L2CTRL3,
	S2MPU02_REG_L2CTRL4,
	S2MPU02_REG_L3CTRL,
	S2MPU02_REG_L4CTRL,
	S2MPU02_REG_L5CTRL,
	S2MPU02_REG_L6CTRL,
	S2MPU02_REG_L7CTRL,
	S2MPU02_REG_L8CTRL,
	S2MPU02_REG_L9CTRL,
	S2MPU02_REG_L10CTRL,
	S2MPU02_REG_L11CTRL,
	S2MPU02_REG_L12CTRL,
	S2MPU02_REG_L13CTRL,
	S2MPU02_REG_L14CTRL,
	S2MPU02_REG_L15CTRL,
	S2MPU02_REG_L16CTRL,
	S2MPU02_REG_L17CTRL,
	S2MPU02_REG_L18CTRL,
	S2MPU02_REG_L19CTRL,
	S2MPU02_REG_L20CTRL,
	S2MPU02_REG_L21CTRL,
	S2MPU02_REG_L22CTRL,
	S2MPU02_REG_L23CTRL,
	S2MPU02_REG_L24CTRL,
	S2MPU02_REG_L25CTRL,
	S2MPU02_REG_L26CTRL,
	S2MPU02_REG_L27CTRL,
	S2MPU02_REG_L28CTRL,
	S2MPU02_REG_LDODSCH1,
	S2MPU02_REG_LDODSCH2,
	S2MPU02_REG_LDODSCH3,
	S2MPU02_REG_LDODSCH4,
	S2MPU02_REG_SELMIF,
	S2MPU02_REG_RSVD11,
	S2MPU02_REG_RSVD12,
	S2MPU02_REG_RSVD13,
	S2MPU02_REG_DVSSEL,
	S2MPU02_REG_DVSPTR,
	S2MPU02_REG_DVSDATA,
};

/* S2MPU02 regulator ids */
enum S2MPU02_regulators {
	S2MPU02_LDO1,
	S2MPU02_LDO2,
	S2MPU02_LDO3,
	S2MPU02_LDO4,
	S2MPU02_LDO5,
	S2MPU02_LDO6,
	S2MPU02_LDO7,
	S2MPU02_LDO8,
	S2MPU02_LDO9,
	S2MPU02_LDO10,
	S2MPU02_LDO11,
	S2MPU02_LDO12,
	S2MPU02_LDO13,
	S2MPU02_LDO14,
	S2MPU02_LDO15,
	S2MPU02_LDO16,
	S2MPU02_LDO17,
	S2MPU02_LDO18,
	S2MPU02_LDO19,
	S2MPU02_LDO20,
	S2MPU02_LDO21,
	S2MPU02_LDO22,
	S2MPU02_LDO23,
	S2MPU02_LDO24,
	S2MPU02_LDO25,
	S2MPU02_LDO26,
	S2MPU02_LDO27,
	S2MPU02_LDO28,
	S2MPU02_BUCK1,
	S2MPU02_BUCK2,
	S2MPU02_BUCK3,
	S2MPU02_BUCK4,
	S2MPU02_BUCK5,
	S2MPU02_BUCK6,
	S2MPU02_BUCK7,

	S2MPU02_REGULATOR_MAX,
};

/* Regulator constraints for BUCKx */
#define S2MPU02_BUCK1234_MIN_600MV	600000
#define S2MPU02_BUCK5_MIN_1081_25MV	1081250
#define S2MPU02_BUCK6_MIN_1700MV	1700000
#define S2MPU02_BUCK7_MIN_900MV		900000

#define S2MPU02_BUCK1234_STEP_6_25MV	6250
#define S2MPU02_BUCK5_STEP_6_25MV	6250
#define S2MPU02_BUCK6_STEP_2_50MV	2500
#define S2MPU02_BUCK7_STEP_6_25MV	6250

#define S2MPU02_BUCK1234_START_SEL	0x00
#define S2MPU02_BUCK5_START_SEL		0x4D
#define S2MPU02_BUCK6_START_SEL		0x28
#define S2MPU02_BUCK7_START_SEL		0x30

#define S2MPU02_BUCK_RAMP_DELAY		12500

/* Regulator constraints for different types of LDOx */
#define S2MPU02_LDO_MIN_900MV		900000
#define S2MPU02_LDO_MIN_1050MV		1050000
#define S2MPU02_LDO_MIN_1600MV		1600000
#define S2MPU02_LDO_STEP_12_5MV		12500
#define S2MPU02_LDO_STEP_25MV		25000
#define S2MPU02_LDO_STEP_50MV		50000

#define S2MPU02_LDO_GROUP1_START_SEL	0x8
#define S2MPU02_LDO_GROUP2_START_SEL	0xA
#define S2MPU02_LDO_GROUP3_START_SEL	0x10

#define S2MPU02_LDO_VSEL_MASK		0x3F
#define S2MPU02_BUCK_VSEL_MASK		0xFF
#define S2MPU02_ENABLE_MASK		(0x03 << S2MPU02_ENABLE_SHIFT)
#define S2MPU02_ENABLE_SHIFT		6

/* On/Off controlled by PWREN */
#define S2MPU02_ENABLE_SUSPEND		(0x01 << S2MPU02_ENABLE_SHIFT)
#define S2MPU02_DISABLE_SUSPEND		(0x11 << S2MPU02_ENABLE_SHIFT)
#define S2MPU02_LDO_N_VOLTAGES		(S2MPU02_LDO_VSEL_MASK + 1)
#define S2MPU02_BUCK_N_VOLTAGES		(S2MPU02_BUCK_VSEL_MASK + 1)

/* RAMP delay for BUCK1234*/
#define S2MPU02_BUCK1_RAMP_SHIFT	6
#define S2MPU02_BUCK2_RAMP_SHIFT	4
#define S2MPU02_BUCK3_RAMP_SHIFT	2
#define S2MPU02_BUCK4_RAMP_SHIFT	0
#define S2MPU02_BUCK1234_RAMP_MASK	0x3

#endif /*  __LINUX_MFD_S2MPU02_H */
