/*
 * ITE Tech IT9137 silicon tuner driver
 *
 *  Copyright (C) 2011 Malcolm Priestley (tvboxspy@gmail.com)
 *  IT9137 Copyright (C) ITE Tech Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.=
 */

#ifndef IT913X_PRIV_H
#define IT913X_PRIV_H

#include "it913x.h"

/* Build in tuner types */
#define IT9137 0x38
#define IT9135_38 0x38
#define IT9135_51 0x51
#define IT9135_52 0x52
#define IT9135_60 0x60
#define IT9135_61 0x61
#define IT9135_62 0x62

#define I2C_BASE_ADDR		0x10
#define DEV_0			0x0
#define DEV_1			0x10
#define PRO_LINK		0x0
#define PRO_DMOD		0x1
#define DEV_0_DMOD		(PRO_DMOD << 0x7)
#define DEV_1_DMOD		(DEV_0_DMOD | DEV_1)
#define CHIP2_I2C_ADDR		0x3a

#define	PADODPU			0xd827
#define THIRDODPU		0xd828
#define AGC_O_D			0xd829

#define TRIGGER_OFSM		0x0000


struct it913xset {	u32 pro;
			u32 address;
			u8 reg[15];
			u8 count;
};

/* Version 1 types */
static struct it913xset it9135_v1[] = {
	{PRO_DMOD, 0x0051, {0x01}, 0x01},
	{PRO_DMOD, 0x0070, {0x0a}, 0x01},
	{PRO_DMOD, 0x007e, {0x04}, 0x01},
	{PRO_DMOD, 0x0081, {0x0a}, 0x01},
	{PRO_DMOD, 0x008a, {0x01}, 0x01},
	{PRO_DMOD, 0x008e, {0x01}, 0x01},
	{PRO_DMOD, 0x0092, {0x06}, 0x01},
	{PRO_DMOD, 0x0099, {0x01}, 0x01},
	{PRO_DMOD, 0x009f, {0xe1}, 0x01},
	{PRO_DMOD, 0x00a0, {0xcf}, 0x01},
	{PRO_DMOD, 0x00a3, {0x01}, 0x01},
	{PRO_DMOD, 0x00a5, {0x01}, 0x01},
	{PRO_DMOD, 0x00a6, {0x01}, 0x01},
	{PRO_DMOD, 0x00a9, {0x00}, 0x01},
	{PRO_DMOD, 0x00aa, {0x01}, 0x01},
	{PRO_DMOD, 0x00b0, {0x01}, 0x01},
	{PRO_DMOD, 0x00c2, {0x05}, 0x01},
	{PRO_DMOD, 0x00c6, {0x19}, 0x01},
	{PRO_DMOD, 0xf000, {0x0f}, 0x01},
	{PRO_DMOD, 0xf016, {0x10}, 0x01},
	{PRO_DMOD, 0xf017, {0x04}, 0x01},
	{PRO_DMOD, 0xf018, {0x05}, 0x01},
	{PRO_DMOD, 0xf019, {0x04}, 0x01},
	{PRO_DMOD, 0xf01a, {0x05}, 0x01},
	{PRO_DMOD, 0xf021, {0x03}, 0x01},
	{PRO_DMOD, 0xf022, {0x0a}, 0x01},
	{PRO_DMOD, 0xf023, {0x0a}, 0x01},
	{PRO_DMOD, 0xf02b, {0x00}, 0x01},
	{PRO_DMOD, 0xf02c, {0x01}, 0x01},
	{PRO_DMOD, 0xf064, {0x03}, 0x01},
	{PRO_DMOD, 0xf065, {0xf9}, 0x01},
	{PRO_DMOD, 0xf066, {0x03}, 0x01},
	{PRO_DMOD, 0xf067, {0x01}, 0x01},
	{PRO_DMOD, 0xf06f, {0xe0}, 0x01},
	{PRO_DMOD, 0xf070, {0x03}, 0x01},
	{PRO_DMOD, 0xf072, {0x0f}, 0x01},
	{PRO_DMOD, 0xf073, {0x03}, 0x01},
	{PRO_DMOD, 0xf078, {0x00}, 0x01},
	{PRO_DMOD, 0xf087, {0x00}, 0x01},
	{PRO_DMOD, 0xf09b, {0x3f}, 0x01},
	{PRO_DMOD, 0xf09c, {0x00}, 0x01},
	{PRO_DMOD, 0xf09d, {0x20}, 0x01},
	{PRO_DMOD, 0xf09e, {0x00}, 0x01},
	{PRO_DMOD, 0xf09f, {0x0c}, 0x01},
	{PRO_DMOD, 0xf0a0, {0x00}, 0x01},
	{PRO_DMOD, 0xf130, {0x04}, 0x01},
	{PRO_DMOD, 0xf132, {0x04}, 0x01},
	{PRO_DMOD, 0xf144, {0x1a}, 0x01},
	{PRO_DMOD, 0xf146, {0x00}, 0x01},
	{PRO_DMOD, 0xf14a, {0x01}, 0x01},
	{PRO_DMOD, 0xf14c, {0x00}, 0x01},
	{PRO_DMOD, 0xf14d, {0x00}, 0x01},
	{PRO_DMOD, 0xf14f, {0x04}, 0x01},
	{PRO_DMOD, 0xf158, {0x7f}, 0x01},
	{PRO_DMOD, 0xf15a, {0x00}, 0x01},
	{PRO_DMOD, 0xf15b, {0x08}, 0x01},
	{PRO_DMOD, 0xf15d, {0x03}, 0x01},
	{PRO_DMOD, 0xf15e, {0x05}, 0x01},
	{PRO_DMOD, 0xf163, {0x05}, 0x01},
	{PRO_DMOD, 0xf166, {0x01}, 0x01},
	{PRO_DMOD, 0xf167, {0x40}, 0x01},
	{PRO_DMOD, 0xf168, {0x0f}, 0x01},
	{PRO_DMOD, 0xf17a, {0x00}, 0x01},
	{PRO_DMOD, 0xf17b, {0x00}, 0x01},
	{PRO_DMOD, 0xf183, {0x01}, 0x01},
	{PRO_DMOD, 0xf19d, {0x40}, 0x01},
	{PRO_DMOD, 0xf1bc, {0x36}, 0x01},
	{PRO_DMOD, 0xf1bd, {0x00}, 0x01},
	{PRO_DMOD, 0xf1cb, {0xa0}, 0x01},
	{PRO_DMOD, 0xf1cc, {0x01}, 0x01},
	{PRO_DMOD, 0xf204, {0x10}, 0x01},
	{PRO_DMOD, 0xf214, {0x00}, 0x01},
	{PRO_DMOD, 0xf40e, {0x0a}, 0x01},
	{PRO_DMOD, 0xf40f, {0x40}, 0x01},
	{PRO_DMOD, 0xf410, {0x08}, 0x01},
	{PRO_DMOD, 0xf55f, {0x0a}, 0x01},
	{PRO_DMOD, 0xf561, {0x15}, 0x01},
	{PRO_DMOD, 0xf562, {0x20}, 0x01},
	{PRO_DMOD, 0xf5df, {0xfb}, 0x01},
	{PRO_DMOD, 0xf5e0, {0x00}, 0x01},
	{PRO_DMOD, 0xf5e3, {0x09}, 0x01},
	{PRO_DMOD, 0xf5e4, {0x01}, 0x01},
	{PRO_DMOD, 0xf5e5, {0x01}, 0x01},
	{PRO_DMOD, 0xf5f8, {0x01}, 0x01},
	{PRO_DMOD, 0xf5fd, {0x01}, 0x01},
	{PRO_DMOD, 0xf600, {0x05}, 0x01},
	{PRO_DMOD, 0xf601, {0x08}, 0x01},
	{PRO_DMOD, 0xf602, {0x0b}, 0x01},
	{PRO_DMOD, 0xf603, {0x0e}, 0x01},
	{PRO_DMOD, 0xf604, {0x11}, 0x01},
	{PRO_DMOD, 0xf605, {0x14}, 0x01},
	{PRO_DMOD, 0xf606, {0x17}, 0x01},
	{PRO_DMOD, 0xf607, {0x1f}, 0x01},
	{PRO_DMOD, 0xf60e, {0x00}, 0x01},
	{PRO_DMOD, 0xf60f, {0x04}, 0x01},
	{PRO_DMOD, 0xf610, {0x32}, 0x01},
	{PRO_DMOD, 0xf611, {0x10}, 0x01},
	{PRO_DMOD, 0xf707, {0xfc}, 0x01},
	{PRO_DMOD, 0xf708, {0x00}, 0x01},
	{PRO_DMOD, 0xf709, {0x37}, 0x01},
	{PRO_DMOD, 0xf70a, {0x00}, 0x01},
	{PRO_DMOD, 0xf78b, {0x01}, 0x01},
	{PRO_DMOD, 0xf80f, {0x40}, 0x01},
	{PRO_DMOD, 0xf810, {0x54}, 0x01},
	{PRO_DMOD, 0xf811, {0x5a}, 0x01},
	{PRO_DMOD, 0xf905, {0x01}, 0x01},
	{PRO_DMOD, 0xfb06, {0x03}, 0x01},
	{PRO_DMOD, 0xfd8b, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00} /* Terminating Entry */
};

static struct it913xset it9135_38[] = {
	{PRO_DMOD, 0x0043, {0x00}, 0x01},
	{PRO_DMOD, 0x0046, {0x38}, 0x01},
	{PRO_DMOD, 0x0051, {0x01}, 0x01},
	{PRO_DMOD, 0x005f, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0x0068, {0x0a}, 0x01},
	{PRO_DMOD, 0x0070, {0x0a, 0x05, 0x02}, 0x03},
	{PRO_DMOD, 0x0075, {0x8c, 0x8c, 0x8c, 0xc8, 0x01}, 0x05},
	{PRO_DMOD, 0x007e, {0x04, 0x00}, 0x02},
	{PRO_DMOD, 0x0081, {	0x0a, 0x12, 0x02, 0x0a, 0x03, 0xc8, 0xb8,
				0xd0, 0xc3, 0x01}, 0x0a},
	{PRO_DMOD, 0x008e, {0x01}, 0x01},
	{PRO_DMOD, 0x0092, {0x06, 0x00, 0x00, 0x00, 0x00}, 0x05},
	{PRO_DMOD, 0x0099, {0x01}, 0x01},
	{PRO_DMOD, 0x009b, {0x3c, 0x28}, 0x02},
	{PRO_DMOD, 0x009f, {0xe1, 0xcf}, 0x02},
	{PRO_DMOD, 0x00a3, {0x01, 0x5a, 0x01, 0x01}, 0x04},
	{PRO_DMOD, 0x00a9, {0x00, 0x01}, 0x02},
	{PRO_DMOD, 0x00b0, {0x01}, 0x01},
	{PRO_DMOD, 0x00b3, {0x02, 0x32}, 0x02},
	{PRO_DMOD, 0x00b6, {0x14}, 0x01},
	{PRO_DMOD, 0x00c0, {0x11, 0x00, 0x05}, 0x03},
	{PRO_DMOD, 0x00c4, {0x00}, 0x01},
	{PRO_DMOD, 0x00c6, {0x19, 0x00}, 0x02},
	{PRO_DMOD, 0x00cc, {0x2e, 0x51, 0x33}, 0x03},
	{PRO_DMOD, 0x00f3, {0x05, 0x8c, 0x8c}, 0x03},
	{PRO_DMOD, 0x00f8, {0x03, 0x06, 0x06}, 0x03},
	{PRO_DMOD, 0x00fc, {	0x02, 0x02, 0x02, 0x09, 0x50, 0x7b, 0x77,
				0x00, 0x02, 0xc8, 0x05, 0x7b}, 0x0c},
	{PRO_DMOD, 0x0109, {0x02}, 0x01},
	{PRO_DMOD, 0x0115, {0x0a, 0x03, 0x02, 0x80}, 0x04},
	{PRO_DMOD, 0x011a, {0xc8, 0x7b, 0x8a, 0xa0}, 0x04},
	{PRO_DMOD, 0x0122, {0x02, 0x18, 0xc3}, 0x03},
	{PRO_DMOD, 0x0127, {0x00, 0x07}, 0x02},
	{PRO_DMOD, 0x012a, {0x53, 0x51, 0x4e, 0x43}, 0x04},
	{PRO_DMOD, 0x0137, {0x01, 0x00, 0x07, 0x00, 0x06}, 0x05},
	{PRO_DMOD, 0x013d, {0x00, 0x01, 0x5b, 0xc8, 0x59}, 0x05},
	{PRO_DMOD, 0xf000, {0x0f}, 0x01},
	{PRO_DMOD, 0xf016, {0x10, 0x04, 0x05, 0x04, 0x05}, 0x05},
	{PRO_DMOD, 0xf01f, {0x8c, 0x00, 0x03, 0x0a, 0x0a}, 0x05},
	{PRO_DMOD, 0xf029, {0x8c, 0x00, 0x00, 0x01}, 0x04},
	{PRO_DMOD, 0xf064, {0x03, 0xf9, 0x03, 0x01}, 0x04},
	{PRO_DMOD, 0xf06f, {0xe0, 0x03}, 0x02},
	{PRO_DMOD, 0xf072, {0x0f, 0x03}, 0x02},
	{PRO_DMOD, 0xf077, {0x01, 0x00}, 0x02},
	{PRO_DMOD, 0xf085, {0x00, 0x02, 0x00}, 0x03},
	{PRO_DMOD, 0xf09b, {0x3f, 0x00, 0x20, 0x00, 0x0c, 0x00}, 0x06},
	{PRO_DMOD, 0xf130, {0x04}, 0x01},
	{PRO_DMOD, 0xf132, {0x04}, 0x01},
	{PRO_DMOD, 0xf144, {0x1a}, 0x01},
	{PRO_DMOD, 0xf146, {0x00}, 0x01},
	{PRO_DMOD, 0xf14a, {0x01}, 0x01},
	{PRO_DMOD, 0xf14c, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf14f, {0x04}, 0x01},
	{PRO_DMOD, 0xf158, {0x7f}, 0x01},
	{PRO_DMOD, 0xf15a, {0x00, 0x08}, 0x02},
	{PRO_DMOD, 0xf15d, {0x03, 0x05}, 0x02},
	{PRO_DMOD, 0xf163, {0x05}, 0x01},
	{PRO_DMOD, 0xf166, {0x01, 0x40, 0x0f}, 0x03},
	{PRO_DMOD, 0xf17a, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf183, {0x01}, 0x01},
	{PRO_DMOD, 0xf19d, {0x40}, 0x01},
	{PRO_DMOD, 0xf1bc, {0x36, 0x00}, 0x02},
	{PRO_DMOD, 0xf1cb, {0xa0, 0x01}, 0x02},
	{PRO_DMOD, 0xf204, {0x10}, 0x01},
	{PRO_DMOD, 0xf214, {0x00}, 0x01},
	{PRO_DMOD, 0xf24c, {0x88, 0x95, 0x9a, 0x90}, 0x04},
	{PRO_DMOD, 0xf25a, {0x07, 0xe8, 0x03, 0xb0, 0x04}, 0x05},
	{PRO_DMOD, 0xf270, {0x01, 0x02, 0x01, 0x02}, 0x04},
	{PRO_DMOD, 0xf40e, {0x0a, 0x40, 0x08}, 0x03},
	{PRO_DMOD, 0xf55f, {0x0a}, 0x01},
	{PRO_DMOD, 0xf561, {0x15, 0x20}, 0x02},
	{PRO_DMOD, 0xf5df, {0xfb, 0x00}, 0x02},
	{PRO_DMOD, 0xf5e3, {0x09, 0x01, 0x01}, 0x03},
	{PRO_DMOD, 0xf5f8, {0x01}, 0x01},
	{PRO_DMOD, 0xf5fd, {0x01}, 0x01},
	{PRO_DMOD, 0xf600, {	0x05, 0x08, 0x0b, 0x0e, 0x11, 0x14, 0x17,
				0x1f}, 0x08},
	{PRO_DMOD, 0xf60e, {0x00, 0x04, 0x32, 0x10}, 0x04},
	{PRO_DMOD, 0xf707, {0xfc, 0x00, 0x37, 0x00}, 0x04},
	{PRO_DMOD, 0xf78b, {0x01}, 0x01},
	{PRO_DMOD, 0xf80f, {0x40, 0x54, 0x5a}, 0x03},
	{PRO_DMOD, 0xf905, {0x01}, 0x01},
	{PRO_DMOD, 0xfb06, {0x03}, 0x01},
	{PRO_DMOD, 0xfd8b, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00} /* Terminating Entry */
};

static struct it913xset it9135_51[] = {
	{PRO_DMOD, 0x0043, {0x00}, 0x01},
	{PRO_DMOD, 0x0046, {0x51}, 0x01},
	{PRO_DMOD, 0x0051, {0x01}, 0x01},
	{PRO_DMOD, 0x005f, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0x0068, {0x0a}, 0x01},
	{PRO_DMOD, 0x0070, {0x0a, 0x06, 0x02}, 0x03},
	{PRO_DMOD, 0x0075, {0x8c, 0x8c, 0x8c, 0xc8, 0x01}, 0x05},
	{PRO_DMOD, 0x007e, {0x04, 0x00}, 0x02},
	{PRO_DMOD, 0x0081, {	0x0a, 0x12, 0x02, 0x0a, 0x03, 0xc0, 0x96,
				0xcf, 0xc3, 0x01}, 0x0a},
	{PRO_DMOD, 0x008e, {0x01}, 0x01},
	{PRO_DMOD, 0x0092, {0x06, 0x00, 0x00, 0x00, 0x00}, 0x05},
	{PRO_DMOD, 0x0099, {0x01}, 0x01},
	{PRO_DMOD, 0x009b, {0x3c, 0x28}, 0x02},
	{PRO_DMOD, 0x009f, {0xe1, 0xcf}, 0x02},
	{PRO_DMOD, 0x00a3, {0x01, 0x5a, 0x01, 0x01}, 0x04},
	{PRO_DMOD, 0x00a9, {0x00, 0x01}, 0x02},
	{PRO_DMOD, 0x00b0, {0x01}, 0x01},
	{PRO_DMOD, 0x00b3, {0x02, 0x3c}, 0x02},
	{PRO_DMOD, 0x00b6, {0x14}, 0x01},
	{PRO_DMOD, 0x00c0, {0x11, 0x00, 0x05}, 0x03},
	{PRO_DMOD, 0x00c4, {0x00}, 0x01},
	{PRO_DMOD, 0x00c6, {0x19, 0x00}, 0x02},
	{PRO_DMOD, 0x00cc, {0x2e, 0x51, 0x33}, 0x03},
	{PRO_DMOD, 0x00f3, {0x05, 0x8c, 0x8c}, 0x03},
	{PRO_DMOD, 0x00f8, {0x03, 0x06, 0x06}, 0x03},
	{PRO_DMOD, 0x00fc, {	0x03, 0x02, 0x02, 0x09, 0x50, 0x7a, 0x77,
				0x01, 0x02, 0xb0, 0x02, 0x7a}, 0x0c},
	{PRO_DMOD, 0x0109, {0x02}, 0x01},
	{PRO_DMOD, 0x0115, {0x0a, 0x03, 0x02, 0x80}, 0x04},
	{PRO_DMOD, 0x011a, {0xc0, 0x7a, 0xac, 0x8c}, 0x04},
	{PRO_DMOD, 0x0122, {0x02, 0x70, 0xa4}, 0x03},
	{PRO_DMOD, 0x0127, {0x00, 0x07}, 0x02},
	{PRO_DMOD, 0x012a, {0x53, 0x51, 0x4e, 0x43}, 0x04},
	{PRO_DMOD, 0x0137, {0x01, 0x00, 0x07, 0x00, 0x06}, 0x05},
	{PRO_DMOD, 0x013d, {0x00, 0x01, 0x5b, 0xc0, 0x59}, 0x05},
	{PRO_DMOD, 0xf000, {0x0f}, 0x01},
	{PRO_DMOD, 0xf016, {0x10, 0x04, 0x05, 0x04, 0x05}, 0x05},
	{PRO_DMOD, 0xf01f, {0x8c, 0x00, 0x03, 0x0a, 0x0a}, 0x05},
	{PRO_DMOD, 0xf029, {0x8c, 0x00, 0x00, 0x01}, 0x04},
	{PRO_DMOD, 0xf064, {0x03, 0xf9, 0x03, 0x01}, 0x04},
	{PRO_DMOD, 0xf06f, {0xe0, 0x03}, 0x02},
	{PRO_DMOD, 0xf072, {0x0f, 0x03}, 0x02},
	{PRO_DMOD, 0xf077, {0x01, 0x00}, 0x02},
	{PRO_DMOD, 0xf085, {0xc0, 0x01, 0x00}, 0x03},
	{PRO_DMOD, 0xf09b, {0x3f, 0x00, 0x20, 0x00, 0x0c, 0x00}, 0x06},
	{PRO_DMOD, 0xf130, {0x04}, 0x01},
	{PRO_DMOD, 0xf132, {0x04}, 0x01},
	{PRO_DMOD, 0xf144, {0x1a}, 0x01},
	{PRO_DMOD, 0xf146, {0x00}, 0x01},
	{PRO_DMOD, 0xf14a, {0x01}, 0x01},
	{PRO_DMOD, 0xf14c, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf14f, {0x04}, 0x01},
	{PRO_DMOD, 0xf158, {0x7f}, 0x01},
	{PRO_DMOD, 0xf15a, {0x00, 0x08}, 0x02},
	{PRO_DMOD, 0xf15d, {0x03, 0x05}, 0x02},
	{PRO_DMOD, 0xf163, {0x05}, 0x01},
	{PRO_DMOD, 0xf166, {0x01, 0x40, 0x0f}, 0x03},
	{PRO_DMOD, 0xf17a, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf183, {0x01}, 0x01},
	{PRO_DMOD, 0xf19d, {0x40}, 0x01},
	{PRO_DMOD, 0xf1bc, {0x36, 0x00}, 0x02},
	{PRO_DMOD, 0xf1cb, {0xa0, 0x01}, 0x02},
	{PRO_DMOD, 0xf204, {0x10}, 0x01},
	{PRO_DMOD, 0xf214, {0x00}, 0x01},
	{PRO_DMOD, 0xf24c, {0x88, 0x95, 0x9a, 0x90}, 0x04},
	{PRO_DMOD, 0xf25a, {0x07, 0xe8, 0x03, 0xb0, 0x04}, 0x05},
	{PRO_DMOD, 0xf270, {0x01, 0x02, 0x01, 0x02}, 0x04},
	{PRO_DMOD, 0xf40e, {0x0a, 0x40, 0x08}, 0x03},
	{PRO_DMOD, 0xf55f, {0x0a}, 0x01},
	{PRO_DMOD, 0xf561, {0x15, 0x20}, 0x02},
	{PRO_DMOD, 0xf5df, {0xfb, 0x00}, 0x02},
	{PRO_DMOD, 0xf5e3, {0x09, 0x01, 0x01}, 0x03},
	{PRO_DMOD, 0xf5f8, {0x01}, 0x01},
	{PRO_DMOD, 0xf5fd, {0x01}, 0x01},
	{PRO_DMOD, 0xf600, {	0x05, 0x08, 0x0b, 0x0e, 0x11, 0x14, 0x17,
				0x1f}, 0x08},
	{PRO_DMOD, 0xf60e, {0x00, 0x04, 0x32, 0x10}, 0x04},
	{PRO_DMOD, 0xf707, {0xfc, 0x00, 0x37, 0x00}, 0x04},
	{PRO_DMOD, 0xf78b, {0x01}, 0x01},
	{PRO_DMOD, 0xf80f, {0x40, 0x54, 0x5a}, 0x03},
	{PRO_DMOD, 0xf905, {0x01}, 0x01},
	{PRO_DMOD, 0xfb06, {0x03}, 0x01},
	{PRO_DMOD, 0xfd8b, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00} /* Terminating Entry */
};

static struct it913xset it9135_52[] = {
	{PRO_DMOD, 0x0043, {0x00}, 0x01},
	{PRO_DMOD, 0x0046, {0x52}, 0x01},
	{PRO_DMOD, 0x0051, {0x01}, 0x01},
	{PRO_DMOD, 0x005f, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0x0068, {0x10}, 0x01},
	{PRO_DMOD, 0x0070, {0x0a, 0x05, 0x02}, 0x03},
	{PRO_DMOD, 0x0075, {0x8c, 0x8c, 0x8c, 0xa0, 0x01}, 0x05},
	{PRO_DMOD, 0x007e, {0x04, 0x00}, 0x02},
	{PRO_DMOD, 0x0081, {	0x0a, 0x12, 0x03, 0x0a, 0x03, 0xb3, 0x97,
				0xc0, 0x9e, 0x01}, 0x0a},
	{PRO_DMOD, 0x008e, {0x01}, 0x01},
	{PRO_DMOD, 0x0092, {0x06, 0x00, 0x00, 0x00, 0x00}, 0x05},
	{PRO_DMOD, 0x0099, {0x01}, 0x01},
	{PRO_DMOD, 0x009b, {0x3c, 0x28}, 0x02},
	{PRO_DMOD, 0x009f, {0xe1, 0xcf}, 0x02},
	{PRO_DMOD, 0x00a3, {0x01, 0x5c, 0x01, 0x01}, 0x04},
	{PRO_DMOD, 0x00a9, {0x00, 0x01}, 0x02},
	{PRO_DMOD, 0x00b0, {0x01}, 0x01},
	{PRO_DMOD, 0x00b3, {0x02, 0x3c}, 0x02},
	{PRO_DMOD, 0x00b6, {0x14}, 0x01},
	{PRO_DMOD, 0x00c0, {0x11, 0x00, 0x05}, 0x03},
	{PRO_DMOD, 0x00c4, {0x00}, 0x01},
	{PRO_DMOD, 0x00c6, {0x19, 0x00}, 0x02},
	{PRO_DMOD, 0x00cc, {0x2e, 0x51, 0x33}, 0x03},
	{PRO_DMOD, 0x00f3, {0x05, 0x91, 0x8c}, 0x03},
	{PRO_DMOD, 0x00f8, {0x03, 0x06, 0x06}, 0x03},
	{PRO_DMOD, 0x00fc, {	0x03, 0x02, 0x02, 0x09, 0x50, 0x74, 0x77,
				0x02, 0x02, 0xae, 0x02, 0x6e}, 0x0c},
	{PRO_DMOD, 0x0109, {0x02}, 0x01},
	{PRO_DMOD, 0x0115, {0x0a, 0x03, 0x02, 0x80}, 0x04},
	{PRO_DMOD, 0x011a, {0xcd, 0x62, 0xa4, 0x8c}, 0x04},
	{PRO_DMOD, 0x0122, {0x03, 0x18, 0x9e}, 0x03},
	{PRO_DMOD, 0x0127, {0x00, 0x07}, 0x02},
	{PRO_DMOD, 0x012a, {0x53, 0x51, 0x4e, 0x43}, 0x04},
	{PRO_DMOD, 0x0137, {0x00, 0x00, 0x07, 0x00, 0x06}, 0x05},
	{PRO_DMOD, 0x013d, {0x00, 0x01, 0x5b, 0xb6, 0x59}, 0x05},
	{PRO_DMOD, 0xf000, {0x0f}, 0x01},
	{PRO_DMOD, 0xf016, {0x10, 0x04, 0x05, 0x04, 0x05}, 0x05},
	{PRO_DMOD, 0xf01f, {0x8c, 0x00, 0x03, 0x0a, 0x0a}, 0x05},
	{PRO_DMOD, 0xf029, {0x8c, 0x00, 0x00, 0x01}, 0x04},
	{PRO_DMOD, 0xf064, {0x03, 0xf9, 0x03, 0x01}, 0x04},
	{PRO_DMOD, 0xf06f, {0xe0, 0x03}, 0x02},
	{PRO_DMOD, 0xf072, {0x0f, 0x03}, 0x02},
	{PRO_DMOD, 0xf077, {0x01, 0x00}, 0x02},
	{PRO_DMOD, 0xf085, {0xc0, 0x01, 0x00}, 0x03},
	{PRO_DMOD, 0xf09b, {0x3f, 0x00, 0x20, 0x00, 0x0c, 0x00}, 0x06},
	{PRO_DMOD, 0xf130, {0x04}, 0x01},
	{PRO_DMOD, 0xf132, {0x04}, 0x01},
	{PRO_DMOD, 0xf144, {0x1a}, 0x01},
	{PRO_DMOD, 0xf146, {0x00}, 0x01},
	{PRO_DMOD, 0xf14a, {0x01}, 0x01},
	{PRO_DMOD, 0xf14c, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf14f, {0x04}, 0x01},
	{PRO_DMOD, 0xf158, {0x7f}, 0x01},
	{PRO_DMOD, 0xf15a, {0x00, 0x08}, 0x02},
	{PRO_DMOD, 0xf15d, {0x03, 0x05}, 0x02},
	{PRO_DMOD, 0xf163, {0x05}, 0x01},
	{PRO_DMOD, 0xf166, {0x01, 0x40, 0x0f}, 0x03},
	{PRO_DMOD, 0xf17a, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf183, {0x01}, 0x01},
	{PRO_DMOD, 0xf19d, {0x40}, 0x01},
	{PRO_DMOD, 0xf1bc, {0x36, 0x00}, 0x02},
	{PRO_DMOD, 0xf1cb, {0xa0, 0x01}, 0x02},
	{PRO_DMOD, 0xf204, {0x10}, 0x01},
	{PRO_DMOD, 0xf214, {0x00}, 0x01},
	{PRO_DMOD, 0xf24c, {0x88, 0x95, 0x9a, 0x90}, 0x04},
	{PRO_DMOD, 0xf25a, {0x07, 0xe8, 0x03, 0xb0, 0x04}, 0x05},
	{PRO_DMOD, 0xf270, {0x01, 0x02, 0x01, 0x02}, 0x04},
	{PRO_DMOD, 0xf40e, {0x0a, 0x40, 0x08}, 0x03},
	{PRO_DMOD, 0xf55f, {0x0a}, 0x01},
	{PRO_DMOD, 0xf561, {0x15, 0x20}, 0x02},
	{PRO_DMOD, 0xf5df, {0xfb, 0x00}, 0x02},
	{PRO_DMOD, 0xf5e3, {0x09, 0x01, 0x01}, 0x03},
	{PRO_DMOD, 0xf5f8, {0x01}, 0x01},
	{PRO_DMOD, 0xf5fd, {0x01}, 0x01},
	{PRO_DMOD, 0xf600, {0x05, 0x08, 0x0b, 0x0e, 0x11, 0x14, 0x17,
				0x1f}, 0x08},
	{PRO_DMOD, 0xf60e, {0x00, 0x04, 0x32, 0x10}, 0x04},
	{PRO_DMOD, 0xf707, {0xfc, 0x00, 0x37, 0x00}, 0x04},
	{PRO_DMOD, 0xf78b, {0x01}, 0x01},
	{PRO_DMOD, 0xf80f, {0x40, 0x54, 0x5a}, 0x03},
	{PRO_DMOD, 0xf905, {0x01}, 0x01},
	{PRO_DMOD, 0xfb06, {0x03}, 0x01},
	{PRO_DMOD, 0xfd8b, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00} /* Terminating Entry */
};

/* Version 2 types */
static struct it913xset it9135_v2[] = {
	{PRO_DMOD, 0x0051, {0x01}, 0x01},
	{PRO_DMOD, 0x0070, {0x0a}, 0x01},
	{PRO_DMOD, 0x007e, {0x04}, 0x01},
	{PRO_DMOD, 0x0081, {0x0a}, 0x01},
	{PRO_DMOD, 0x008a, {0x01}, 0x01},
	{PRO_DMOD, 0x008e, {0x01}, 0x01},
	{PRO_DMOD, 0x0092, {0x06}, 0x01},
	{PRO_DMOD, 0x0099, {0x01}, 0x01},
	{PRO_DMOD, 0x009f, {0xe1}, 0x01},
	{PRO_DMOD, 0x00a0, {0xcf}, 0x01},
	{PRO_DMOD, 0x00a3, {0x01}, 0x01},
	{PRO_DMOD, 0x00a5, {0x01}, 0x01},
	{PRO_DMOD, 0x00a6, {0x01}, 0x01},
	{PRO_DMOD, 0x00a9, {0x00}, 0x01},
	{PRO_DMOD, 0x00aa, {0x01}, 0x01},
	{PRO_DMOD, 0x00b0, {0x01}, 0x01},
	{PRO_DMOD, 0x00c2, {0x05}, 0x01},
	{PRO_DMOD, 0x00c6, {0x19}, 0x01},
	{PRO_DMOD, 0xf000, {0x0f}, 0x01},
	{PRO_DMOD, 0xf02b, {0x00}, 0x01},
	{PRO_DMOD, 0xf064, {0x03}, 0x01},
	{PRO_DMOD, 0xf065, {0xf9}, 0x01},
	{PRO_DMOD, 0xf066, {0x03}, 0x01},
	{PRO_DMOD, 0xf067, {0x01}, 0x01},
	{PRO_DMOD, 0xf06f, {0xe0}, 0x01},
	{PRO_DMOD, 0xf070, {0x03}, 0x01},
	{PRO_DMOD, 0xf072, {0x0f}, 0x01},
	{PRO_DMOD, 0xf073, {0x03}, 0x01},
	{PRO_DMOD, 0xf078, {0x00}, 0x01},
	{PRO_DMOD, 0xf087, {0x00}, 0x01},
	{PRO_DMOD, 0xf09b, {0x3f}, 0x01},
	{PRO_DMOD, 0xf09c, {0x00}, 0x01},
	{PRO_DMOD, 0xf09d, {0x20}, 0x01},
	{PRO_DMOD, 0xf09e, {0x00}, 0x01},
	{PRO_DMOD, 0xf09f, {0x0c}, 0x01},
	{PRO_DMOD, 0xf0a0, {0x00}, 0x01},
	{PRO_DMOD, 0xf130, {0x04}, 0x01},
	{PRO_DMOD, 0xf132, {0x04}, 0x01},
	{PRO_DMOD, 0xf144, {0x1a}, 0x01},
	{PRO_DMOD, 0xf146, {0x00}, 0x01},
	{PRO_DMOD, 0xf14a, {0x01}, 0x01},
	{PRO_DMOD, 0xf14c, {0x00}, 0x01},
	{PRO_DMOD, 0xf14d, {0x00}, 0x01},
	{PRO_DMOD, 0xf14f, {0x04}, 0x01},
	{PRO_DMOD, 0xf158, {0x7f}, 0x01},
	{PRO_DMOD, 0xf15a, {0x00}, 0x01},
	{PRO_DMOD, 0xf15b, {0x08}, 0x01},
	{PRO_DMOD, 0xf15d, {0x03}, 0x01},
	{PRO_DMOD, 0xf15e, {0x05}, 0x01},
	{PRO_DMOD, 0xf163, {0x05}, 0x01},
	{PRO_DMOD, 0xf166, {0x01}, 0x01},
	{PRO_DMOD, 0xf167, {0x40}, 0x01},
	{PRO_DMOD, 0xf168, {0x0f}, 0x01},
	{PRO_DMOD, 0xf17a, {0x00}, 0x01},
	{PRO_DMOD, 0xf17b, {0x00}, 0x01},
	{PRO_DMOD, 0xf183, {0x01}, 0x01},
	{PRO_DMOD, 0xf19d, {0x40}, 0x01},
	{PRO_DMOD, 0xf1bc, {0x36}, 0x01},
	{PRO_DMOD, 0xf1bd, {0x00}, 0x01},
	{PRO_DMOD, 0xf1cb, {0xa0}, 0x01},
	{PRO_DMOD, 0xf1cc, {0x01}, 0x01},
	{PRO_DMOD, 0xf204, {0x10}, 0x01},
	{PRO_DMOD, 0xf214, {0x00}, 0x01},
	{PRO_DMOD, 0xf40e, {0x0a}, 0x01},
	{PRO_DMOD, 0xf40f, {0x40}, 0x01},
	{PRO_DMOD, 0xf410, {0x08}, 0x01},
	{PRO_DMOD, 0xf55f, {0x0a}, 0x01},
	{PRO_DMOD, 0xf561, {0x15}, 0x01},
	{PRO_DMOD, 0xf562, {0x20}, 0x01},
	{PRO_DMOD, 0xf5e3, {0x09}, 0x01},
	{PRO_DMOD, 0xf5e4, {0x01}, 0x01},
	{PRO_DMOD, 0xf5e5, {0x01}, 0x01},
	{PRO_DMOD, 0xf600, {0x05}, 0x01},
	{PRO_DMOD, 0xf601, {0x08}, 0x01},
	{PRO_DMOD, 0xf602, {0x0b}, 0x01},
	{PRO_DMOD, 0xf603, {0x0e}, 0x01},
	{PRO_DMOD, 0xf604, {0x11}, 0x01},
	{PRO_DMOD, 0xf605, {0x14}, 0x01},
	{PRO_DMOD, 0xf606, {0x17}, 0x01},
	{PRO_DMOD, 0xf607, {0x1f}, 0x01},
	{PRO_DMOD, 0xf60e, {0x00}, 0x01},
	{PRO_DMOD, 0xf60f, {0x04}, 0x01},
	{PRO_DMOD, 0xf610, {0x32}, 0x01},
	{PRO_DMOD, 0xf611, {0x10}, 0x01},
	{PRO_DMOD, 0xf707, {0xfc}, 0x01},
	{PRO_DMOD, 0xf708, {0x00}, 0x01},
	{PRO_DMOD, 0xf709, {0x37}, 0x01},
	{PRO_DMOD, 0xf70a, {0x00}, 0x01},
	{PRO_DMOD, 0xf78b, {0x01}, 0x01},
	{PRO_DMOD, 0xf80f, {0x40}, 0x01},
	{PRO_DMOD, 0xf810, {0x54}, 0x01},
	{PRO_DMOD, 0xf811, {0x5a}, 0x01},
	{PRO_DMOD, 0xf905, {0x01}, 0x01},
	{PRO_DMOD, 0xfb06, {0x03}, 0x01},
	{PRO_DMOD, 0xfd8b, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00} /* Terminating Entry */
};

static struct it913xset it9135_60[] = {
	{PRO_DMOD, 0x0043, {0x00}, 0x01},
	{PRO_DMOD, 0x0046, {0x60}, 0x01},
	{PRO_DMOD, 0x0051, {0x01}, 0x01},
	{PRO_DMOD, 0x005f, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0x0068, {0x0a}, 0x01},
	{PRO_DMOD, 0x006a, {0x03}, 0x01},
	{PRO_DMOD, 0x0070, {0x0a, 0x05, 0x02}, 0x03},
	{PRO_DMOD, 0x0075, {0x8c, 0x8c, 0x8c, 0x8c, 0x01}, 0x05},
	{PRO_DMOD, 0x007e, {0x04}, 0x01},
	{PRO_DMOD, 0x0081, {0x0a, 0x12}, 0x02},
	{PRO_DMOD, 0x0084, {0x0a, 0x33, 0xbe, 0xa0, 0xc6, 0xb6, 0x01}, 0x07},
	{PRO_DMOD, 0x008e, {0x01}, 0x01},
	{PRO_DMOD, 0x0092, {0x06, 0x00, 0x00, 0x00, 0x00}, 0x05},
	{PRO_DMOD, 0x0099, {0x01}, 0x01},
	{PRO_DMOD, 0x009b, {0x3c, 0x28}, 0x02},
	{PRO_DMOD, 0x009f, {0xe1, 0xcf}, 0x02},
	{PRO_DMOD, 0x00a3, {0x01, 0x5a, 0x01, 0x01}, 0x04},
	{PRO_DMOD, 0x00a9, {0x00, 0x01}, 0x02},
	{PRO_DMOD, 0x00b0, {0x01}, 0x01},
	{PRO_DMOD, 0x00b3, {0x02, 0x3a}, 0x02},
	{PRO_DMOD, 0x00b6, {0x14}, 0x01},
	{PRO_DMOD, 0x00c0, {0x11, 0x00, 0x05, 0x01, 0x00}, 0x05},
	{PRO_DMOD, 0x00c6, {0x19, 0x00}, 0x02},
	{PRO_DMOD, 0x00cb, {0x32, 0x2c, 0x4f, 0x30}, 0x04},
	{PRO_DMOD, 0x00f3, {0x05, 0xa0, 0x8c}, 0x03},
	{PRO_DMOD, 0x00f8, {0x03, 0x06, 0x06}, 0x03},
	{PRO_DMOD, 0x00fc, {	0x03, 0x03, 0x02, 0x0a, 0x50, 0x7b, 0x8c,
				0x00, 0x02, 0xbe, 0x00}, 0x0b},
	{PRO_DMOD, 0x0109, {0x02}, 0x01},
	{PRO_DMOD, 0x0115, {0x0a, 0x03}, 0x02},
	{PRO_DMOD, 0x011a, {0xbe}, 0x01},
	{PRO_DMOD, 0x0124, {0xae}, 0x01},
	{PRO_DMOD, 0x0127, {0x00}, 0x01},
	{PRO_DMOD, 0x012a, {0x56, 0x50, 0x47, 0x42}, 0x04},
	{PRO_DMOD, 0x0137, {0x00}, 0x01},
	{PRO_DMOD, 0x013b, {0x08}, 0x01},
	{PRO_DMOD, 0x013f, {0x5b}, 0x01},
	{PRO_DMOD, 0x0141, {	0x59, 0xf9, 0x19, 0x19, 0x8c, 0x8c, 0x8c,
				0x6e, 0x8c, 0x50, 0x8c, 0x8c, 0xac, 0xc6,
				0x33}, 0x0f},
	{PRO_DMOD, 0x0151, {0x28}, 0x01},
	{PRO_DMOD, 0x0153, {0xbc}, 0x01},
	{PRO_DMOD, 0x0178, {0x09}, 0x01},
	{PRO_DMOD, 0x0181, {0x94, 0x6e}, 0x02},
	{PRO_DMOD, 0x0185, {0x24}, 0x01},
	{PRO_DMOD, 0x0187, {0x00, 0x00, 0xbe, 0x02, 0x80}, 0x05},
	{PRO_DMOD, 0xed02, {0xff}, 0x01},
	{PRO_DMOD, 0xee42, {0xff}, 0x01},
	{PRO_DMOD, 0xee82, {0xff}, 0x01},
	{PRO_DMOD, 0xf000, {0x0f}, 0x01},
	{PRO_DMOD, 0xf01f, {0x8c, 0x00}, 0x02},
	{PRO_DMOD, 0xf029, {0x8c, 0x00, 0x00}, 0x03},
	{PRO_DMOD, 0xf064, {0x03, 0xf9, 0x03, 0x01}, 0x04},
	{PRO_DMOD, 0xf06f, {0xe0, 0x03}, 0x02},
	{PRO_DMOD, 0xf072, {0x0f, 0x03}, 0x02},
	{PRO_DMOD, 0xf077, {0x01, 0x00}, 0x02},
	{PRO_DMOD, 0xf087, {0x00}, 0x01},
	{PRO_DMOD, 0xf09b, {0x3f, 0x00, 0x20, 0x00, 0x0c, 0x00}, 0x06},
	{PRO_DMOD, 0xf130, {0x04}, 0x01},
	{PRO_DMOD, 0xf132, {0x04}, 0x01},
	{PRO_DMOD, 0xf144, {0x1a}, 0x01},
	{PRO_DMOD, 0xf146, {0x00}, 0x01},
	{PRO_DMOD, 0xf14a, {0x01}, 0x01},
	{PRO_DMOD, 0xf14c, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf14f, {0x04}, 0x01},
	{PRO_DMOD, 0xf158, {0x7f}, 0x01},
	{PRO_DMOD, 0xf15a, {0x00, 0x08}, 0x02},
	{PRO_DMOD, 0xf15d, {0x03, 0x05}, 0x02},
	{PRO_DMOD, 0xf163, {0x05}, 0x01},
	{PRO_DMOD, 0xf166, {0x01, 0x40, 0x0f}, 0x03},
	{PRO_DMOD, 0xf17a, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf183, {0x01}, 0x01},
	{PRO_DMOD, 0xf19d, {0x40}, 0x01},
	{PRO_DMOD, 0xf1bc, {0x36, 0x00}, 0x02},
	{PRO_DMOD, 0xf1cb, {0xa0, 0x01}, 0x02},
	{PRO_DMOD, 0xf204, {0x10}, 0x01},
	{PRO_DMOD, 0xf214, {0x00}, 0x01},
	{PRO_DMOD, 0xf24c, {0x88, 0x95, 0x9a, 0x90}, 0x04},
	{PRO_DMOD, 0xf25a, {0x07, 0xe8, 0x03, 0xb0, 0x04}, 0x05},
	{PRO_DMOD, 0xf270, {0x01, 0x02, 0x01, 0x02}, 0x04},
	{PRO_DMOD, 0xf40e, {0x0a, 0x40, 0x08}, 0x03},
	{PRO_DMOD, 0xf55f, {0x0a}, 0x01},
	{PRO_DMOD, 0xf561, {0x15, 0x20}, 0x02},
	{PRO_DMOD, 0xf5e3, {0x09, 0x01, 0x01}, 0x03},
	{PRO_DMOD, 0xf600, {0x05, 0x08, 0x0b, 0x0e, 0x11, 0x14, 0x17
		, 0x1f}, 0x08},
	{PRO_DMOD, 0xf60e, {0x00, 0x04, 0x32, 0x10}, 0x04},
	{PRO_DMOD, 0xf707, {0xfc, 0x00, 0x37, 0x00}, 0x04},
	{PRO_DMOD, 0xf78b, {0x01}, 0x01},
	{PRO_DMOD, 0xf80f, {0x40, 0x54, 0x5a}, 0x03},
	{PRO_DMOD, 0xf905, {0x01}, 0x01},
	{PRO_DMOD, 0xfb06, {0x03}, 0x01},
	{PRO_DMOD, 0xfd8b, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00} /* Terminating Entry */
};

static struct it913xset it9135_61[] = {
	{PRO_DMOD, 0x0043, {0x00}, 0x01},
	{PRO_DMOD, 0x0046, {0x61}, 0x01},
	{PRO_DMOD, 0x0051, {0x01}, 0x01},
	{PRO_DMOD, 0x005f, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0x0068, {0x06}, 0x01},
	{PRO_DMOD, 0x006a, {0x03}, 0x01},
	{PRO_DMOD, 0x0070, {0x0a, 0x05, 0x02}, 0x03},
	{PRO_DMOD, 0x0075, {0x8c, 0x8c, 0x8c, 0x90, 0x01}, 0x05},
	{PRO_DMOD, 0x007e, {0x04}, 0x01},
	{PRO_DMOD, 0x0081, {0x0a, 0x12}, 0x02},
	{PRO_DMOD, 0x0084, {0x0a, 0x33, 0xbc, 0x9c, 0xcc, 0xa8, 0x01}, 0x07},
	{PRO_DMOD, 0x008e, {0x01}, 0x01},
	{PRO_DMOD, 0x0092, {0x06, 0x00, 0x00, 0x00, 0x00}, 0x05},
	{PRO_DMOD, 0x0099, {0x01}, 0x01},
	{PRO_DMOD, 0x009b, {0x3c, 0x28}, 0x02},
	{PRO_DMOD, 0x009f, {0xe1, 0xcf}, 0x02},
	{PRO_DMOD, 0x00a3, {0x01, 0x5c, 0x01, 0x01}, 0x04},
	{PRO_DMOD, 0x00a9, {0x00, 0x01}, 0x02},
	{PRO_DMOD, 0x00b0, {0x01}, 0x01},
	{PRO_DMOD, 0x00b3, {0x02, 0x3a}, 0x02},
	{PRO_DMOD, 0x00b6, {0x14}, 0x01},
	{PRO_DMOD, 0x00c0, {0x11, 0x00, 0x05, 0x01, 0x00}, 0x05},
	{PRO_DMOD, 0x00c6, {0x19, 0x00}, 0x02},
	{PRO_DMOD, 0x00cb, {0x32, 0x2c, 0x4f, 0x30}, 0x04},
	{PRO_DMOD, 0x00f3, {0x05, 0xa0, 0x8c}, 0x03},
	{PRO_DMOD, 0x00f8, {0x03, 0x06, 0x06}, 0x03},
	{PRO_DMOD, 0x00fc, {	0x03, 0x03, 0x02, 0x08, 0x50, 0x7b, 0x8c,
				0x01, 0x02, 0xc8, 0x00}, 0x0b},
	{PRO_DMOD, 0x0109, {0x02}, 0x01},
	{PRO_DMOD, 0x0115, {0x0a, 0x03}, 0x02},
	{PRO_DMOD, 0x011a, {0xc6}, 0x01},
	{PRO_DMOD, 0x0124, {0xa8}, 0x01},
	{PRO_DMOD, 0x0127, {0x00}, 0x01},
	{PRO_DMOD, 0x012a, {0x59, 0x50, 0x47, 0x42}, 0x04},
	{PRO_DMOD, 0x0137, {0x00}, 0x01},
	{PRO_DMOD, 0x013b, {0x05}, 0x01},
	{PRO_DMOD, 0x013f, {0x5b}, 0x01},
	{PRO_DMOD, 0x0141, {	0x59, 0xf9, 0x59, 0x59, 0x8c, 0x8c, 0x8c,
				0x7b, 0x8c, 0x50, 0x8c, 0x8c, 0xa8, 0xc6,
				0x33}, 0x0f},
	{PRO_DMOD, 0x0151, {0x28}, 0x01},
	{PRO_DMOD, 0x0153, {0xcc}, 0x01},
	{PRO_DMOD, 0x0178, {0x09}, 0x01},
	{PRO_DMOD, 0x0181, {0x9c, 0x76}, 0x02},
	{PRO_DMOD, 0x0185, {0x28}, 0x01},
	{PRO_DMOD, 0x0187, {0x01, 0x00, 0xaa, 0x02, 0x80}, 0x05},
	{PRO_DMOD, 0xed02, {0xff}, 0x01},
	{PRO_DMOD, 0xee42, {0xff}, 0x01},
	{PRO_DMOD, 0xee82, {0xff}, 0x01},
	{PRO_DMOD, 0xf000, {0x0f}, 0x01},
	{PRO_DMOD, 0xf01f, {0x8c, 0x00}, 0x02},
	{PRO_DMOD, 0xf029, {0x8c, 0x00, 0x00}, 0x03},
	{PRO_DMOD, 0xf064, {0x03, 0xf9, 0x03, 0x01}, 0x04},
	{PRO_DMOD, 0xf06f, {0xe0, 0x03}, 0x02},
	{PRO_DMOD, 0xf072, {0x0f, 0x03}, 0x02},
	{PRO_DMOD, 0xf077, {0x01, 0x00}, 0x02},
	{PRO_DMOD, 0xf087, {0x00}, 0x01},
	{PRO_DMOD, 0xf09b, {0x3f, 0x00, 0x20, 0x00, 0x0c, 0x00}, 0x06},
	{PRO_DMOD, 0xf130, {0x04}, 0x01},
	{PRO_DMOD, 0xf132, {0x04}, 0x01},
	{PRO_DMOD, 0xf144, {0x1a}, 0x01},
	{PRO_DMOD, 0xf146, {0x00}, 0x01},
	{PRO_DMOD, 0xf14a, {0x01}, 0x01},
	{PRO_DMOD, 0xf14c, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf14f, {0x04}, 0x01},
	{PRO_DMOD, 0xf158, {0x7f}, 0x01},
	{PRO_DMOD, 0xf15a, {0x00, 0x08}, 0x02},
	{PRO_DMOD, 0xf15d, {0x03, 0x05}, 0x02},
	{PRO_DMOD, 0xf163, {0x05}, 0x01},
	{PRO_DMOD, 0xf166, {0x01, 0x40, 0x0f}, 0x03},
	{PRO_DMOD, 0xf17a, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf183, {0x01}, 0x01},
	{PRO_DMOD, 0xf19d, {0x40}, 0x01},
	{PRO_DMOD, 0xf1bc, {0x36, 0x00}, 0x02},
	{PRO_DMOD, 0xf1cb, {0xa0, 0x01}, 0x02},
	{PRO_DMOD, 0xf204, {0x10}, 0x01},
	{PRO_DMOD, 0xf214, {0x00}, 0x01},
	{PRO_DMOD, 0xf24c, {0x88, 0x95, 0x9a, 0x90}, 0x04},
	{PRO_DMOD, 0xf25a, {0x07, 0xe8, 0x03, 0xb0, 0x04}, 0x05},
	{PRO_DMOD, 0xf270, {0x01, 0x02, 0x01, 0x02}, 0x04},
	{PRO_DMOD, 0xf40e, {0x0a, 0x40, 0x08}, 0x03},
	{PRO_DMOD, 0xf55f, {0x0a}, 0x01},
	{PRO_DMOD, 0xf561, {0x15, 0x20}, 0x02},
	{PRO_DMOD, 0xf5e3, {0x09, 0x01, 0x01}, 0x03},
	{PRO_DMOD, 0xf600, {	0x05, 0x08, 0x0b, 0x0e, 0x11, 0x14, 0x17,
				0x1f}, 0x08},
	{PRO_DMOD, 0xf60e, {0x00, 0x04, 0x32, 0x10}, 0x04},
	{PRO_DMOD, 0xf707, {0xfc, 0x00, 0x37, 0x00}, 0x04},
	{PRO_DMOD, 0xf78b, {0x01}, 0x01},
	{PRO_DMOD, 0xf80f, {0x40, 0x54, 0x5a}, 0x03},
	{PRO_DMOD, 0xf905, {0x01}, 0x01},
	{PRO_DMOD, 0xfb06, {0x03}, 0x01},
	{PRO_DMOD, 0xfd8b, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00} /* Terminating Entry */
};

static struct it913xset it9135_62[] = {
	{PRO_DMOD, 0x0043, {0x00}, 0x01},
	{PRO_DMOD, 0x0046, {0x62}, 0x01},
	{PRO_DMOD, 0x0051, {0x01}, 0x01},
	{PRO_DMOD, 0x005f, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0x0068, {0x0a}, 0x01},
	{PRO_DMOD, 0x006a, {0x03}, 0x01},
	{PRO_DMOD, 0x0070, {0x0a, 0x05, 0x02}, 0x03},
	{PRO_DMOD, 0x0075, {0x8c, 0x8c, 0x8c, 0x8c, 0x01}, 0x05},
	{PRO_DMOD, 0x007e, {0x04}, 0x01},
	{PRO_DMOD, 0x0081, {0x0a, 0x12}, 0x02},
	{PRO_DMOD, 0x0084, {	0x0a, 0x33, 0xb8, 0x9c, 0xb2, 0xa6, 0x01},
				0x07},
	{PRO_DMOD, 0x008e, {0x01}, 0x01},
	{PRO_DMOD, 0x0092, {0x06, 0x00, 0x00, 0x00, 0x00}, 0x05},
	{PRO_DMOD, 0x0099, {0x01}, 0x01},
	{PRO_DMOD, 0x009b, {0x3c, 0x28}, 0x02},
	{PRO_DMOD, 0x009f, {0xe1, 0xcf}, 0x02},
	{PRO_DMOD, 0x00a3, {0x01, 0x5a, 0x01, 0x01}, 0x04},
	{PRO_DMOD, 0x00a9, {0x00, 0x01}, 0x02},
	{PRO_DMOD, 0x00b0, {0x01}, 0x01},
	{PRO_DMOD, 0x00b3, {0x02, 0x3a}, 0x02},
	{PRO_DMOD, 0x00b6, {0x14}, 0x01},
	{PRO_DMOD, 0x00c0, {0x11, 0x00, 0x05, 0x01, 0x00}, 0x05},
	{PRO_DMOD, 0x00c6, {0x19, 0x00}, 0x02},
	{PRO_DMOD, 0x00cb, {0x32, 0x2c, 0x4f, 0x30}, 0x04},
	{PRO_DMOD, 0x00f3, {0x05, 0x8c, 0x8c}, 0x03},
	{PRO_DMOD, 0x00f8, {0x03, 0x06, 0x06}, 0x03},
	{PRO_DMOD, 0x00fc, {	0x02, 0x03, 0x02, 0x09, 0x50, 0x6e, 0x8c,
				0x02, 0x02, 0xc2, 0x00}, 0x0b},
	{PRO_DMOD, 0x0109, {0x02}, 0x01},
	{PRO_DMOD, 0x0115, {0x0a, 0x03}, 0x02},
	{PRO_DMOD, 0x011a, {0xb8}, 0x01},
	{PRO_DMOD, 0x0124, {0xa8}, 0x01},
	{PRO_DMOD, 0x0127, {0x00}, 0x01},
	{PRO_DMOD, 0x012a, {0x53, 0x51, 0x4e, 0x43}, 0x04},
	{PRO_DMOD, 0x0137, {0x00}, 0x01},
	{PRO_DMOD, 0x013b, {0x05}, 0x01},
	{PRO_DMOD, 0x013f, {0x5b}, 0x01},
	{PRO_DMOD, 0x0141, {	0x59, 0xf9, 0x59, 0x19, 0x8c, 0x8c, 0x8c,
				0x7b, 0x8c, 0x50, 0x70, 0x8c, 0x96, 0xd0,
				0x33}, 0x0f},
	{PRO_DMOD, 0x0151, {0x28}, 0x01},
	{PRO_DMOD, 0x0153, {0xb2}, 0x01},
	{PRO_DMOD, 0x0178, {0x09}, 0x01},
	{PRO_DMOD, 0x0181, {0x9c, 0x6e}, 0x02},
	{PRO_DMOD, 0x0185, {0x24}, 0x01},
	{PRO_DMOD, 0x0187, {0x00, 0x00, 0xb8, 0x02, 0x80}, 0x05},
	{PRO_DMOD, 0xed02, {0xff}, 0x01},
	{PRO_DMOD, 0xee42, {0xff}, 0x01},
	{PRO_DMOD, 0xee82, {0xff}, 0x01},
	{PRO_DMOD, 0xf000, {0x0f}, 0x01},
	{PRO_DMOD, 0xf01f, {0x8c, 0x00}, 0x02},
	{PRO_DMOD, 0xf029, {0x8c, 0x00, 0x00}, 0x03},
	{PRO_DMOD, 0xf064, {0x03, 0xf9, 0x03, 0x01}, 0x04},
	{PRO_DMOD, 0xf06f, {0xe0, 0x03}, 0x02},
	{PRO_DMOD, 0xf072, {0x0f, 0x03}, 0x02},
	{PRO_DMOD, 0xf077, {0x01, 0x00}, 0x02},
	{PRO_DMOD, 0xf087, {0x00}, 0x01},
	{PRO_DMOD, 0xf09b, {0x3f, 0x00, 0x20, 0x00, 0x0c, 0x00}, 0x06},
	{PRO_DMOD, 0xf130, {0x04}, 0x01},
	{PRO_DMOD, 0xf132, {0x04}, 0x01},
	{PRO_DMOD, 0xf144, {0x1a}, 0x01},
	{PRO_DMOD, 0xf146, {0x00}, 0x01},
	{PRO_DMOD, 0xf14a, {0x01}, 0x01},
	{PRO_DMOD, 0xf14c, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf14f, {0x04}, 0x01},
	{PRO_DMOD, 0xf158, {0x7f}, 0x01},
	{PRO_DMOD, 0xf15a, {0x00, 0x08}, 0x02},
	{PRO_DMOD, 0xf15d, {0x03, 0x05}, 0x02},
	{PRO_DMOD, 0xf163, {0x05}, 0x01},
	{PRO_DMOD, 0xf166, {0x01, 0x40, 0x0f}, 0x03},
	{PRO_DMOD, 0xf17a, {0x00, 0x00}, 0x02},
	{PRO_DMOD, 0xf183, {0x01}, 0x01},
	{PRO_DMOD, 0xf19d, {0x40}, 0x01},
	{PRO_DMOD, 0xf1bc, {0x36, 0x00}, 0x02},
	{PRO_DMOD, 0xf1cb, {0xa0, 0x01}, 0x02},
	{PRO_DMOD, 0xf204, {0x10}, 0x01},
	{PRO_DMOD, 0xf214, {0x00}, 0x01},
	{PRO_DMOD, 0xf24c, {0x88, 0x95, 0x9a, 0x90}, 0x04},
	{PRO_DMOD, 0xf25a, {0x07, 0xe8, 0x03, 0xb0, 0x04}, 0x05},
	{PRO_DMOD, 0xf270, {0x01, 0x02, 0x01, 0x02}, 0x04},
	{PRO_DMOD, 0xf40e, {0x0a, 0x40, 0x08}, 0x03},
	{PRO_DMOD, 0xf55f, {0x0a}, 0x01},
	{PRO_DMOD, 0xf561, {0x15, 0x20}, 0x02},
	{PRO_DMOD, 0xf5e3, {0x09, 0x01, 0x01}, 0x03},
	{PRO_DMOD, 0xf600, {	0x05, 0x08, 0x0b, 0x0e, 0x11, 0x14, 0x17,
				0x1f}, 0x08},
	{PRO_DMOD, 0xf60e, {0x00, 0x04, 0x32, 0x10}, 0x04},
	{PRO_DMOD, 0xf707, {0xfc, 0x00, 0x37, 0x00}, 0x04},
	{PRO_DMOD, 0xf78b, {0x01}, 0x01},
	{PRO_DMOD, 0xf80f, {0x40, 0x54, 0x5a}, 0x03},
	{PRO_DMOD, 0xf905, {0x01}, 0x01},
	{PRO_DMOD, 0xfb06, {0x03}, 0x01},
	{PRO_DMOD, 0xfd8b, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00} /* Terminating Entry */
};

/* Tuner setting scripts (still keeping it9137) */
static struct it913xset it9137_tuner_off[] = {
	{PRO_DMOD, 0xfba8, {0x01}, 0x01}, /* Tuner Clock Off  */
	{PRO_DMOD, 0xec40, {0x00}, 0x01}, /* Power Down Tuner */
	{PRO_DMOD, 0xec02, {0x3f, 0x1f, 0x3f, 0x3f}, 0x04},
	{PRO_DMOD, 0xec06, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00}, 0x0c},
	{PRO_DMOD, 0xec12, {0x00, 0x00, 0x00, 0x00}, 0x04},
	{PRO_DMOD, 0xec17, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00}, 0x09},
	{PRO_DMOD, 0xec22, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00}, 0x0a},
	{PRO_DMOD, 0xec20, {0x00}, 0x01},
	{PRO_DMOD, 0xec3f, {0x01}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00}, /* Terminating Entry */
};

static struct it913xset set_it9135_template[] = {
	{PRO_DMOD, 0xee06, {0x00}, 0x01},
	{PRO_DMOD, 0xec56, {0x00}, 0x01},
	{PRO_DMOD, 0xec4c, {0x00}, 0x01},
	{PRO_DMOD, 0xec4d, {0x00}, 0x01},
	{PRO_DMOD, 0xec4e, {0x00}, 0x01},
	{PRO_DMOD, 0x011e, {0x00}, 0x01}, /* Older Devices */
	{PRO_DMOD, 0x011f, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00}, /* Terminating Entry */
};

static struct it913xset set_it9137_template[] = {
	{PRO_DMOD, 0xee06, {0x00}, 0x01},
	{PRO_DMOD, 0xec56, {0x00}, 0x01},
	{PRO_DMOD, 0xec4c, {0x00}, 0x01},
	{PRO_DMOD, 0xec4d, {0x00}, 0x01},
	{PRO_DMOD, 0xec4e, {0x00}, 0x01},
	{PRO_DMOD, 0xec4f, {0x00}, 0x01},
	{PRO_DMOD, 0xec50, {0x00}, 0x01},
	{0xff, 0x0000, {0x00}, 0x00}, /* Terminating Entry */
};

#endif
