/*
   tm6000-stds.c - driver for TM5600/TM6000/TM6010 USB video capture devices

   Copyright (C) 2007 Mauro Carvalho Chehab <mchehab@redhat.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation version 2

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include "tm6000.h"
#include "tm6000-regs.h"

struct tm6000_reg_settings {
	unsigned char req;
	unsigned char reg;
	unsigned char value;
};

enum tm6000_audio_std {
	BG_NICAM,
	BTSC,
	BG_A2,
	DK_NICAM,
	EIAJ,
	FM_RADIO,
	I_NICAM,
	KOREA_A2,
	L_NICAM,
};

struct tm6000_std_tv_settings {
	v4l2_std_id id;
	enum tm6000_audio_std audio_default_std;

	struct tm6000_reg_settings sif[12];
	struct tm6000_reg_settings nosif[12];
	struct tm6000_reg_settings common[26];
};

struct tm6000_std_settings {
	v4l2_std_id id;
	enum tm6000_audio_std audio_default_std;
	struct tm6000_reg_settings common[37];
};

static struct tm6000_std_tv_settings tv_stds[] = {
	{
		.id = V4L2_STD_PAL_M,
		.audio_default_std = BTSC,
		.sif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf2},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x08},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x62},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfe},
			{TM6010_REQ07_RFE_POWER_DOWN, 0xcb},
			{0, 0, 0},
		},
		.nosif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x60},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},
			{0, 0, 0},
		},
		.common = {
			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x04},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x00},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x83},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x0a},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xe0},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x88},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x20},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0x61},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},

			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_PAL_Nc,
		.audio_default_std = BTSC,
		.sif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf2},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x08},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x62},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfe},
			{TM6010_REQ07_RFE_POWER_DOWN, 0xcb},
			{0, 0, 0},
		},
		.nosif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x60},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},
			{0, 0, 0},
		},
		.common = {
			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x36},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x02},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x91},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x1f},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0x0c},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x8c},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x2c},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ08_R05_A_STANDARD_MOD, 0x21}, /* FIXME */
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_PAL,
		.audio_default_std = BG_A2,
		.sif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf2},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x08},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x62},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfe},
			{TM6010_REQ07_RFE_POWER_DOWN, 0xcb},
			{0, 0, 0}
		},
		.nosif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x60},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},
			{0, 0, 0},
		},
		.common = {
			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x32},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x02},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x25},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0xd5},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x63},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0x50},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x8c},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x2c},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ08_R05_A_STANDARD_MOD, 0x76}, /* FIXME */
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_SECAM,
		.audio_default_std = BG_NICAM,
		.sif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf2},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x08},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x62},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfe},
			{TM6010_REQ07_RFE_POWER_DOWN, 0xcb},
			{0, 0, 0},
		},
		.nosif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x60},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},
			{0, 0, 0},
		},
		.common = {
			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x38},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x02},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x24},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x92},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0xe8},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xed},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x8c},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x2c},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x2c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x18},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x42},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0xFF},

			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ08_R05_A_STANDARD_MOD, 0x79},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_NTSC,
		.audio_default_std = BTSC,
		.sif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf2},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x08},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x62},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfe},
			{TM6010_REQ07_RFE_POWER_DOWN, 0xcb},
			{0, 0, 0},
		},
		.nosif = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf8},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x60},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},
			{0, 0, 0},
		},
		.common = {
			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x00},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0f},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x00},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x8b},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0xa2},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xe9},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x88},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x22},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0x61},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x1c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x42},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdd},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ08_R05_A_STANDARD_MOD, 0x22}, /* FIXME */
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	},
};

static struct tm6000_std_settings composite_stds[] = {
	{
		.id = V4L2_STD_PAL_M,
		.audio_default_std = BTSC,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf4},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x04},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x00},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x83},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x0a},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xe0},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x88},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x20},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0x61},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	 }, {
		.id = V4L2_STD_PAL_Nc,
		.audio_default_std = BTSC,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf4},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x36},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x02},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x91},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x1f},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0x0c},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x8c},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x2c},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_PAL,
		.audio_default_std = BG_A2,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf4},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x32},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x02},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x25},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0xd5},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x63},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0x50},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x8c},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x2c},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	 }, {
		.id = V4L2_STD_SECAM,
		.audio_default_std = BG_NICAM,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf4},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x38},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x02},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x24},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x92},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0xe8},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xed},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x8c},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x2c},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x2c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x18},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x42},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0xFF},

			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_NTSC,
		.audio_default_std = BTSC,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xf4},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf3},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x0f},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf1},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xe0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe8},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8b},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x00},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0f},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x00},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x8b},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0xa2},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xe9},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x88},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x22},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0x61},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x1c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x42},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdd},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	},
};

static struct tm6000_std_settings svideo_stds[] = {
	{
		.id = V4L2_STD_PAL_M,
		.audio_default_std = BTSC,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xfc},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf8},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x00},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf2},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xf0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe0},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8a},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x05},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x04},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x83},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x0a},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xe0},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x88},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x22},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0x61},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_PAL_Nc,
		.audio_default_std = BTSC,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xfc},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf8},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x00},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf2},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xf0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe0},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8a},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x37},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x04},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x91},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x1f},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0x0c},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x88},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x22},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_PAL,
		.audio_default_std = BG_A2,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xfc},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf8},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x00},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf2},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xf0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe0},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8a},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x33},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x04},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x30},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x25},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0xd5},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0x63},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0x50},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x8c},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x2a},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x0c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x52},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdc},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	 }, {
		.id = V4L2_STD_SECAM,
		.audio_default_std = BG_NICAM,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xfc},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf8},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x00},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf2},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xf0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe0},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8a},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x39},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0e},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x03},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x31},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x24},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x92},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0xe8},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xed},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x8c},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x2a},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0xc1},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x2c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x18},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x42},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0xFF},

			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	}, {
		.id = V4L2_STD_NTSC,
		.audio_default_std = BTSC,
		.common = {
			{TM6010_REQ08_RE2_POWER_DOWN_CTRL1, 0xf0},
			{TM6010_REQ08_RE3_ADC_IN1_SEL, 0xfc},
			{TM6010_REQ08_RE4_ADC_IN2_SEL, 0xf8},
			{TM6010_REQ08_RE6_POWER_DOWN_CTRL2, 0x00},
			{TM6010_REQ08_REA_BUFF_DRV_CTRL, 0xf2},
			{TM6010_REQ08_REB_SIF_GAIN_CTRL, 0xf0},
			{TM6010_REQ08_REC_REVERSE_YC_CTRL, 0xc2},
			{TM6010_REQ08_RED_GAIN_SEL, 0xe0},
			{TM6010_REQ08_RF0_DAUDIO_INPUT_CONFIG, 0x68},
			{TM6010_REQ08_RF1_AADC_POWER_DOWN, 0xfc},
			{TM6010_REQ07_RFE_POWER_DOWN, 0x8a},

			{TM6010_REQ07_R3F_RESET, 0x01},
			{TM6010_REQ07_R00_VIDEO_CONTROL0, 0x01},
			{TM6010_REQ07_R01_VIDEO_CONTROL1, 0x0f},
			{TM6010_REQ07_R02_VIDEO_CONTROL2, 0x5f},
			{TM6010_REQ07_R03_YC_SEP_CONTROL, 0x03},
			{TM6010_REQ07_R07_OUTPUT_CONTROL, 0x30},
			{TM6010_REQ07_R17_HLOOP_MAXSTATE, 0x8b},
			{TM6010_REQ07_R18_CHROMA_DTO_INCREMENT3, 0x1e},
			{TM6010_REQ07_R19_CHROMA_DTO_INCREMENT2, 0x8b},
			{TM6010_REQ07_R1A_CHROMA_DTO_INCREMENT1, 0xa2},
			{TM6010_REQ07_R1B_CHROMA_DTO_INCREMENT0, 0xe9},
			{TM6010_REQ07_R1C_HSYNC_DTO_INCREMENT3, 0x1c},
			{TM6010_REQ07_R1D_HSYNC_DTO_INCREMENT2, 0xcc},
			{TM6010_REQ07_R1E_HSYNC_DTO_INCREMENT1, 0xcc},
			{TM6010_REQ07_R1F_HSYNC_DTO_INCREMENT0, 0xcd},
			{TM6010_REQ07_R2E_ACTIVE_VIDEO_HSTART, 0x88},
			{TM6010_REQ07_R30_ACTIVE_VIDEO_VSTART, 0x22},
			{TM6010_REQ07_R31_ACTIVE_VIDEO_VHIGHT, 0x61},
			{TM6010_REQ07_R33_VSYNC_HLOCK_MAX, 0x1c},
			{TM6010_REQ07_R35_VSYNC_AGC_MAX, 0x1c},
			{TM6010_REQ07_R82_COMB_FILTER_CONFIG, 0x42},
			{TM6010_REQ07_R83_CHROMA_LOCK_CONFIG, 0x6F},

			{TM6010_REQ07_R04_LUMA_HAGC_CONTROL, 0xdd},
			{TM6010_REQ07_R0D_CHROMA_KILL_LEVEL, 0x07},
			{TM6010_REQ07_R3F_RESET, 0x00},
			{0, 0, 0},
		},
	},
};


static int tm6000_set_audio_std(struct tm6000_core *dev,
				enum tm6000_audio_std std)
{
	switch (std) {
	case BG_NICAM:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x11);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	case BTSC:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x04);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x02);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R09_A_MAIN_VOL, 0x08);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R0E_A_MONO_THRES1, 0xf0);
		tm6000_set_reg(dev, TM6010_REQ08_R0F_A_MONO_THRES2, 0x80);
		tm6000_set_reg(dev, TM6010_REQ08_R10_A_MUTE_THRES1, 0xc0);
		tm6000_set_reg(dev, TM6010_REQ08_R11_A_MUTE_THRES2, 0x80);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	case BG_A2:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x04);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x05);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R09_A_MAIN_VOL, 0x08);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R0E_A_MONO_THRES1, 0xf0);
		tm6000_set_reg(dev, TM6010_REQ08_R0F_A_MONO_THRES2, 0x80);
		tm6000_set_reg(dev, TM6010_REQ08_R10_A_MUTE_THRES1, 0xc0);
		tm6000_set_reg(dev, TM6010_REQ08_R11_A_MUTE_THRES2, 0x80);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	case DK_NICAM:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x04);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R09_A_MAIN_VOL, 0x08);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R0C_A_ASD_THRES2, 0x0a);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	case EIAJ:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x04);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x03);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R09_A_MAIN_VOL, 0x08);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	case FM_RADIO:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x0c);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R09_A_MAIN_VOL, 0x10);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	case I_NICAM:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x04);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R09_A_MAIN_VOL, 0x08);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R0C_A_ASD_THRES2, 0x0a);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	case KOREA_A2:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x04);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x04);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R09_A_MAIN_VOL, 0x08);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R0E_A_MONO_THRES1, 0xf0);
		tm6000_set_reg(dev, TM6010_REQ08_R0F_A_MONO_THRES2, 0x80);
		tm6000_set_reg(dev, TM6010_REQ08_R10_A_MUTE_THRES1, 0xc0);
		tm6000_set_reg(dev, TM6010_REQ08_R11_A_MUTE_THRES2, 0xf0);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	case L_NICAM:
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R02_A_FIX_GAIN_CTRL, 0x02);
		tm6000_set_reg(dev, TM6010_REQ08_R03_A_AUTO_GAIN_CTRL, 0x00);
		tm6000_set_reg(dev, TM6010_REQ08_R05_A_STANDARD_MOD, 0x0a);
		tm6000_set_reg(dev, TM6010_REQ08_R06_A_SOUND_MOD, 0x06);
		tm6000_set_reg(dev, TM6010_REQ08_R09_A_MAIN_VOL, 0x08);
		tm6000_set_reg(dev, TM6010_REQ08_R0A_A_I2S_MOD, 0x91);
		tm6000_set_reg(dev, TM6010_REQ08_R16_A_AGC_GAIN_MAX, 0xfe);
		tm6000_set_reg(dev, TM6010_REQ08_R17_A_AGC_GAIN_MIN, 0x01);
		tm6000_set_reg(dev, TM6010_REQ08_R01_A_INIT, 0x80);
		break;
	}
	return 0;
}

void tm6000_get_std_res(struct tm6000_core *dev)
{
	/* Currently, those are the only supported resoltions */
	if (dev->norm & V4L2_STD_525_60)
		dev->height = 480;
	else
		dev->height = 576;

	dev->width = 720;
}

static int tm6000_load_std(struct tm6000_core *dev,
			   struct tm6000_reg_settings *set, int max_size)
{
	int i, rc;

	/* Load board's initialization table */
	for (i = 0; max_size; i++) {
		if (!set[i].req)
			return 0;

		if ((dev->dev_type != TM6010) &&
		    (set[i].req == REQ_08_SET_GET_AVREG_BIT))
				continue;

		rc = tm6000_set_reg(dev, set[i].req, set[i].reg, set[i].value);
		if (rc < 0) {
			printk(KERN_ERR "Error %i while setting "
			       "req %d, reg %d to value %d\n",
			       rc, set[i].req, set[i].reg, set[i].value);
			return rc;
		}
	}

	return 0;
}

static int tm6000_set_tv(struct tm6000_core *dev, int pos)
{
	int rc;

	/* FIXME: This code is for tm6010 - not tested yet - doesn't work with
	   tm5600
	 */

	/* FIXME: This is tuner-dependent */
	int nosif = 0;

	if (nosif) {
		rc = tm6000_load_std(dev, tv_stds[pos].nosif,
				     sizeof(tv_stds[pos].nosif));
	} else {
		rc = tm6000_load_std(dev, tv_stds[pos].sif,
				     sizeof(tv_stds[pos].sif));
	}
	if (rc < 0)
		return rc;
	rc = tm6000_load_std(dev, tv_stds[pos].common,
			     sizeof(tv_stds[pos].common));

	tm6000_set_audio_std(dev, tv_stds[pos].audio_default_std);

	return rc;
}

int tm6000_set_standard(struct tm6000_core *dev, v4l2_std_id * norm)
{
	int i, rc = 0;

	dev->norm = *norm;
	tm6000_get_std_res(dev);

	switch (dev->input) {
	case TM6000_INPUT_TV:
		for (i = 0; i < ARRAY_SIZE(tv_stds); i++) {
			if (*norm & tv_stds[i].id) {
				rc = tm6000_set_tv(dev, i);
				goto ret;
			}
		}
		return -EINVAL;
	case TM6000_INPUT_SVIDEO:
		for (i = 0; i < ARRAY_SIZE(svideo_stds); i++) {
			if (*norm & svideo_stds[i].id) {
				rc = tm6000_load_std(dev, svideo_stds[i].common,
						     sizeof(svideo_stds[i].
							    common));
				tm6000_set_audio_std(dev, svideo_stds[i].audio_default_std);

				goto ret;
			}
		}
		return -EINVAL;
	case TM6000_INPUT_COMPOSITE:
		for (i = 0; i < ARRAY_SIZE(composite_stds); i++) {
			if (*norm & composite_stds[i].id) {
				rc = tm6000_load_std(dev,
						     composite_stds[i].common,
						     sizeof(composite_stds[i].
							    common));
				tm6000_set_audio_std(dev, composite_stds[i].audio_default_std);
				goto ret;
			}
		}
		return -EINVAL;
	}

ret:
	if (rc < 0)
		return rc;

	msleep(40);


	return 0;
}
