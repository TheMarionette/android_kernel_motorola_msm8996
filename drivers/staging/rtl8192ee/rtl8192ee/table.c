/******************************************************************************
 *
 * Copyright(c) 2009-2010  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Created on  2010/ 5/18,  1:41
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#include "table.h"
u32 RTL8192EE_PHY_REG_ARRAY[] = {
		0x800, 0x80040000,
		0x804, 0x00000003,
		0x808, 0x0000FC00,
		0x80C, 0x0000000A,
		0x810, 0x10001331,
		0x814, 0x020C3D10,
		0x818, 0x02220385,
		0x81C, 0x00000000,
		0x820, 0x01000100,
		0x824, 0x00390204,
		0x828, 0x01000100,
		0x82C, 0x00390204,
		0x830, 0x32323232,
		0x834, 0x30303030,
		0x838, 0x30303030,
		0x83C, 0x30303030,
		0x840, 0x00010000,
		0x844, 0x00010000,
		0x848, 0x28282828,
		0x84C, 0x28282828,
		0x850, 0x00000000,
		0x854, 0x00000000,
		0x858, 0x009A009A,
		0x85C, 0x01000014,
		0x860, 0x66F60000,
		0x864, 0x061F0000,
		0x868, 0x30303030,
		0x86C, 0x30303030,
		0x870, 0x00000000,
		0x874, 0x55004200,
		0x878, 0x08080808,
		0x87C, 0x00000000,
		0x880, 0xB0000C1C,
		0x884, 0x00000001,
		0x888, 0x00000000,
		0x88C, 0xCC0000C0,
		0x890, 0x00000800,
		0x894, 0xFFFFFFFE,
		0x898, 0x40302010,
		0x900, 0x00000000,
		0x904, 0x00000023,
		0x908, 0x00000000,
		0x90C, 0x81121313,
		0x910, 0x806C0001,
		0x914, 0x00000001,
		0x918, 0x00000000,
		0x91C, 0x00010000,
		0x924, 0x00000001,
		0x928, 0x00000000,
		0x92C, 0x00000000,
		0x930, 0x00000000,
		0x934, 0x00000000,
		0x938, 0x00000000,
		0x93C, 0x00000000,
		0x940, 0x00000000,
		0x944, 0x00000000,
		0x94C, 0x00000008,
		0xA00, 0x00D0C7C8,
		0xA04, 0x81FF000C,
		0xA08, 0x8C838300,
		0xA0C, 0x2E68120F,
		0xA10, 0x95009B78,
		0xA14, 0x1114D028,
		0xA18, 0x00881117,
		0xA1C, 0x89140F00,
		0xA20, 0x1A1B0000,
		0xA24, 0x090E1317,
		0xA28, 0x00000204,
		0xA2C, 0x00D30000,
		0xA70, 0x101FBF00,
		0xA74, 0x00000007,
		0xA78, 0x00000900,
		0xA7C, 0x225B0606,
		0xA80, 0x218075B1,
		0xB38, 0x00000000,
		0xC00, 0x48071D40,
		0xC04, 0x03A05633,
		0xC08, 0x000000E4,
		0xC0C, 0x6C6C6C6C,
		0xC10, 0x08800000,
		0xC14, 0x40000100,
		0xC18, 0x08800000,
		0xC1C, 0x40000100,
		0xC20, 0x00000000,
		0xC24, 0x00000000,
		0xC28, 0x00000000,
		0xC2C, 0x00000000,
		0xC30, 0x69E9AC47,
		0xC34, 0x469652AF,
		0xC38, 0x49795994,
		0xC3C, 0x0A97971C,
		0xC40, 0x1F7C403F,
		0xC44, 0x000100B7,
		0xC48, 0xEC020107,
		0xC4C, 0x007F037F,
	0xFF010718, 0xABCD,
		0xC50, 0x00340220,
	0xCDCDCDCD, 0xCDCD,
		0xC50, 0x00340020,
	0xFF010718, 0xDEAD,
		0xC54, 0x0080801F,
	0xFF010718, 0xABCD,
		0xC58, 0x00000220,
	0xCDCDCDCD, 0xCDCD,
		0xC58, 0x00000020,
	0xFF010718, 0xDEAD,
		0xC5C, 0x00248492,
		0xC60, 0x00000000,
		0xC64, 0x7112848B,
		0xC68, 0x47C00BFF,
		0xC6C, 0x00000036,
		0xC70, 0x00000600,
		0xC74, 0x02013169,
		0xC78, 0x0000001F,
		0xC7C, 0x00B91612,
	0xFF010718, 0xABCD,
		0xC80, 0x2D4000B5,
	0xCDCDCDCD, 0xCDCD,
		0xC80, 0x40000100,
	0xFF010718, 0xDEAD,
		0xC84, 0x21F60000,
	0xFF010718, 0xABCD,
		0xC88, 0x2D4000B5,
	0xCDCDCDCD, 0xCDCD,
		0xC88, 0x40000100,
	0xFF010718, 0xDEAD,
		0xC8C, 0xA0E40000,
		0xC90, 0x00121820,
		0xC94, 0x00000000,
		0xC98, 0x00121820,
		0xC9C, 0x00007F7F,
		0xCA0, 0x00000000,
		0xCA4, 0x000300A0,
		0xCA8, 0x00000000,
		0xCAC, 0x00000000,
		0xCB0, 0x00000000,
		0xCB4, 0x00000000,
		0xCB8, 0x00000000,
		0xCBC, 0x28000000,
		0xCC0, 0x00000000,
		0xCC4, 0x00000000,
		0xCC8, 0x00000000,
		0xCCC, 0x00000000,
		0xCD0, 0x00000000,
		0xCD4, 0x00000000,
		0xCD8, 0x64B22427,
		0xCDC, 0x00766932,
		0xCE0, 0x00222222,
		0xCE4, 0x00040000,
		0xCE8, 0x77644302,
		0xCEC, 0x2F97D40C,
		0xD00, 0x00080740,
		0xD04, 0x00020403,
		0xD08, 0x0000907F,
		0xD0C, 0x20010201,
		0xD10, 0xA0633333,
		0xD14, 0x3333BC43,
		0xD18, 0x7A8F5B6B,
		0xD1C, 0x0000007F,
		0xD2C, 0xCC979975,
		0xD30, 0x00000000,
		0xD34, 0x80608000,
		0xD38, 0x00000000,
		0xD3C, 0x00127353,
		0xD40, 0x00000000,
		0xD44, 0x00000000,
		0xD48, 0x00000000,
		0xD4C, 0x00000000,
		0xD50, 0x6437140A,
		0xD54, 0x00000000,
		0xD58, 0x00000282,
		0xD5C, 0x30032064,
		0xD60, 0x4653DE68,
		0xD64, 0x04518A3C,
		0xD68, 0x00002101,
		0xD6C, 0x2A201C16,
		0xD70, 0x1812362E,
		0xD74, 0x322C2220,
		0xD78, 0x000E3C24,
		0xD80, 0x01081008,
		0xD84, 0x00000800,
		0xD88, 0xF0B50000,
		0xE00, 0x30303030,
		0xE04, 0x30303030,
		0xE08, 0x03903030,
		0xE10, 0x30303030,
		0xE14, 0x30303030,
		0xE18, 0x30303030,
		0xE1C, 0x30303030,
		0xE28, 0x00000000,
		0xE30, 0x1000DC1F,
		0xE34, 0x10008C1F,
		0xE38, 0x02140102,
		0xE3C, 0x681604C2,
		0xE40, 0x01007C00,
		0xE44, 0x01004800,
		0xE48, 0xFB000000,
		0xE4C, 0x000028D1,
		0xE50, 0x1000DC1F,
		0xE54, 0x10008C1F,
		0xE58, 0x02140102,
		0xE5C, 0x28160D05,
		0xE60, 0x00000008,
		0xE68, 0x0FC05656,
		0xE6C, 0x03C09696,
		0xE70, 0x03C09696,
		0xE74, 0x0C005656,
		0xE78, 0x0C005656,
		0xE7C, 0x0C005656,
		0xE80, 0x0C005656,
		0xE84, 0x03C09696,
		0xE88, 0x0C005656,
		0xE8C, 0x03C09696,
		0xED0, 0x03C09696,
		0xED4, 0x03C09696,
		0xED8, 0x03C09696,
		0xEDC, 0x0000D6D6,
		0xEE0, 0x0000D6D6,
		0xEEC, 0x0FC01616,
		0xEE4, 0xB0000C1C,
		0xEE8, 0x00000001,
		0xF14, 0x00000003,
		0xF4C, 0x00000000,
		0xF00, 0x00000300,
};

u32 RTL8192EE_PHY_REG_ARRAY_PG[] = {
	0, 0, 0, 0x00000e08, 0x0000ff00, 0x00003200,
	0, 0, 1, 0x00000e08, 0x0000ff00, 0x00003200,
	0, 0, 0, 0x0000086c, 0xffffff00, 0x32323200,
	0, 0, 1, 0x0000086c, 0xffffff00, 0x32323200,
	0, 0, 0, 0x00000e00, 0xffffffff, 0x34343636,
	0, 0, 1, 0x00000e00, 0xffffffff, 0x34343636,
	0, 0, 0, 0x00000e04, 0xffffffff, 0x28283032,
	0, 0, 1, 0x00000e04, 0xffffffff, 0x28283032,
	0, 0, 0, 0x00000e10, 0xffffffff, 0x34363840,
	0, 0, 1, 0x00000e10, 0xffffffff, 0x34363840,
	0, 0, 0, 0x00000e14, 0xffffffff, 0x26283032,
	0, 0, 1, 0x00000e14, 0xffffffff, 0x26283032,
	0, 0, 1, 0x00000e18, 0xffffffff, 0x36384040,
	0, 0, 1, 0x00000e1c, 0xffffffff, 0x24262832,
	0, 1, 0, 0x00000838, 0xffffff00, 0x32323200,
	0, 1, 1, 0x00000838, 0xffffff00, 0x32323200,
	0, 1, 0, 0x0000086c, 0x000000ff, 0x00000032,
	0, 1, 1, 0x0000086c, 0x000000ff, 0x00000032,
	0, 1, 0, 0x00000830, 0xffffffff, 0x34343636,
	0, 1, 1, 0x00000830, 0xffffffff, 0x34343636,
	0, 1, 0, 0x00000834, 0xffffffff, 0x28283032,
	0, 1, 1, 0x00000834, 0xffffffff, 0x28283032,
	0, 1, 0, 0x0000083c, 0xffffffff, 0x34363840,
	0, 1, 1, 0x0000083c, 0xffffffff, 0x34363840,
	0, 1, 0, 0x00000848, 0xffffffff, 0x26283032,
	0, 1, 1, 0x00000848, 0xffffffff, 0x26283032,
	0, 1, 1, 0x0000084c, 0xffffffff, 0x36384040,
	0, 1, 1, 0x00000868, 0xffffffff, 0x24262832
};

u32 RTL8192EE_RADIOA_ARRAY[] = {
		0x07F, 0x00000082,
		0x081, 0x0003FC00,
		0x000, 0x00030000,
		0x008, 0x00008400,
		0x018, 0x00000407,
		0x019, 0x00000012,
		0x01B, 0x00000064,
		0x01E, 0x00080009,
		0x01F, 0x00000880,
		0x02F, 0x0001A060,
		0x03F, 0x00000000,
		0x042, 0x000060C0,
		0x057, 0x000D0000,
		0x058, 0x000BE180,
		0x067, 0x00001552,
		0x083, 0x00000000,
		0x0B0, 0x000FF9F1,
		0x0B1, 0x00055418,
		0x0B2, 0x0008CC00,
		0x0B4, 0x00043083,
		0x0B5, 0x00008166,
		0x0B6, 0x0000803E,
		0x0B7, 0x0001C69F,
		0x0B8, 0x0000407F,
		0x0B9, 0x00080001,
		0x0BA, 0x00040001,
		0x0BB, 0x00000400,
		0x0BF, 0x000C0000,
		0x0C2, 0x00002400,
		0x0C3, 0x00000009,
		0x0C4, 0x00040C91,
		0x0C5, 0x00099999,
		0x0C6, 0x000000A3,
		0x0C7, 0x00088820,
		0x0C8, 0x00076C06,
		0x0C9, 0x00000000,
		0x0CA, 0x00080000,
		0x0DF, 0x00000180,
		0x0EF, 0x000001A0,
		0x051, 0x00069545,
		0x052, 0x0007E45E,
		0x053, 0x00000071,
		0x056, 0x00051FF3,
		0x035, 0x000000A8,
		0x035, 0x000001E2,
		0x035, 0x000002A8,
		0x036, 0x00001C24,
		0x036, 0x00009C24,
		0x036, 0x00011C24,
		0x036, 0x00019C24,
		0x018, 0x00000C07,
		0x05A, 0x00048000,
		0x019, 0x000739D0,
	0xFF010718, 0xABCD,
		0x034, 0x0000A093,
		0x034, 0x0000908F,
		0x034, 0x0000808C,
		0x034, 0x0000704D,
		0x034, 0x0000604A,
		0x034, 0x00005047,
		0x034, 0x0000400A,
		0x034, 0x00003007,
		0x034, 0x00002004,
		0x034, 0x00001001,
		0x034, 0x00000000,
	0xCDCDCDCD, 0xCDCD,
		0x034, 0x0000ADD7,
		0x034, 0x00009DD4,
		0x034, 0x00008DD1,
		0x034, 0x00007DCE,
		0x034, 0x00006DCB,
		0x034, 0x00005DC8,
		0x034, 0x00004DC5,
		0x034, 0x000034CC,
		0x034, 0x0000244F,
		0x034, 0x0000144C,
		0x034, 0x00000014,
	0xFF010718, 0xDEAD,
		0x000, 0x00030159,
		0x084, 0x00068180,
		0x086, 0x0000014E,
		0x087, 0x00048E00,
		0x08E, 0x00065540,
		0x08F, 0x00088000,
		0x0EF, 0x000020A0,
	0xFF010718, 0xABCD,
		0x03B, 0x000F07B0,
	0xCDCDCDCD, 0xCDCD,
		0x03B, 0x000F02B0,
	0xFF010718, 0xDEAD,
		0x03B, 0x000EF7B0,
		0x03B, 0x000D4FB0,
		0x03B, 0x000CF060,
		0x03B, 0x000B0090,
		0x03B, 0x000A0080,
		0x03B, 0x00090080,
		0x03B, 0x0008F780,
	0xFF010718, 0xABCD,
		0x03B, 0x000787B0,
	0xCDCDCDCD, 0xCDCD,
		0x03B, 0x00078730,
	0xFF010718, 0xDEAD,
		0x03B, 0x00060FB0,
		0x03B, 0x0005FFA0,
		0x03B, 0x00040620,
		0x03B, 0x00037090,
		0x03B, 0x00020080,
		0x03B, 0x0001F060,
		0x03B, 0x0000FFB0,
		0x0EF, 0x000000A0,
		0x0FE, 0x00000000,
		0x018, 0x0000FC07,
		0x0FE, 0x00000000,
		0x0FE, 0x00000000,
		0x0FE, 0x00000000,
		0x0FE, 0x00000000,
		0x01E, 0x00000001,
		0x01F, 0x00080000,
		0x000, 0x00033E70,
};

u32 RTL8192EE_RADIOB_ARRAY[] = {
		0x07F, 0x00000082,
		0x081, 0x0003FC00,
		0x000, 0x00030000,
		0x008, 0x00008400,
		0x018, 0x00000407,
		0x019, 0x00000012,
		0x01B, 0x00000064,
		0x01E, 0x00080009,
		0x01F, 0x00000880,
		0x02F, 0x0001A060,
		0x03F, 0x00000000,
		0x042, 0x000060C0,
		0x057, 0x000D0000,
		0x058, 0x000BE180,
		0x067, 0x00001552,
		0x07F, 0x00000082,
		0x081, 0x0003F000,
		0x083, 0x00000000,
		0x0DF, 0x00000180,
		0x0EF, 0x000001A0,
		0x051, 0x00069545,
		0x052, 0x0007E42E,
		0x053, 0x00000071,
		0x056, 0x00051FF3,
		0x035, 0x000000A8,
		0x035, 0x000001E0,
		0x035, 0x000002A8,
		0x036, 0x00001CA8,
		0x036, 0x00009C24,
		0x036, 0x00011C24,
		0x036, 0x00019C24,
		0x018, 0x00000C07,
		0x05A, 0x00048000,
		0x019, 0x000739D0,
	0xFF010718, 0xABCD,
		0x034, 0x0000A093,
		0x034, 0x0000908F,
		0x034, 0x0000808C,
		0x034, 0x0000704D,
		0x034, 0x0000604A,
		0x034, 0x00005047,
		0x034, 0x0000400A,
		0x034, 0x00003007,
		0x034, 0x00002004,
		0x034, 0x00001001,
		0x034, 0x00000000,
	0xCDCDCDCD, 0xCDCD,
		0x034, 0x0000ADD7,
		0x034, 0x00009DD4,
		0x034, 0x00008DD1,
		0x034, 0x00007DCE,
		0x034, 0x00006DCB,
		0x034, 0x00005DC8,
		0x034, 0x00004DC5,
		0x034, 0x000034CC,
		0x034, 0x0000244F,
		0x034, 0x0000144C,
		0x034, 0x00000014,
	0xFF010718, 0xDEAD,
		0x000, 0x00030159,
		0x084, 0x00068180,
		0x086, 0x000000CE,
		0x087, 0x00048A00,
		0x08E, 0x00065540,
		0x08F, 0x00088000,
		0x0EF, 0x000020A0,
	0xFF010718, 0xABCD,
		0x03B, 0x000F07B0,
	0xCDCDCDCD, 0xCDCD,
		0x03B, 0x000F02B0,
	0xFF010718, 0xDEAD,
		0x03B, 0x000EF7B0,
		0x03B, 0x000D4FB0,
		0x03B, 0x000CF060,
		0x03B, 0x000B0090,
		0x03B, 0x000A0080,
		0x03B, 0x00090080,
		0x03B, 0x0008F780,
	0xFF010718, 0xABCD,
		0x03B, 0x000787B0,
	0xCDCDCDCD, 0xCDCD,
		0x03B, 0x00078730,
	0xFF010718, 0xDEAD,
		0x03B, 0x00060FB0,
		0x03B, 0x0005FFA0,
		0x03B, 0x00040620,
		0x03B, 0x00037090,
		0x03B, 0x00020080,
		0x03B, 0x0001F060,
		0x03B, 0x0000FFB0,
		0x0EF, 0x000000A0,
		0x000, 0x00010159,
		0x0FE, 0x00000000,
		0x0FE, 0x00000000,
		0x0FE, 0x00000000,
		0x0FE, 0x00000000,
		0x01E, 0x00000001,
		0x01F, 0x00080000,
		0x000, 0x00033E70,
};

u32 RTL8192EE_MAC_ARRAY[] = {
		0x011, 0x000000EB,
		0x012, 0x00000007,
		0x014, 0x00000075,
		0x303, 0x000000A7,
		0x428, 0x0000000A,
		0x429, 0x00000010,
		0x430, 0x00000000,
		0x431, 0x00000000,
		0x432, 0x00000000,
		0x433, 0x00000001,
		0x434, 0x00000004,
		0x435, 0x00000005,
		0x436, 0x00000007,
		0x437, 0x00000008,
		0x43C, 0x00000004,
		0x43D, 0x00000005,
		0x43E, 0x00000007,
		0x43F, 0x00000008,
		0x440, 0x0000005D,
		0x441, 0x00000001,
		0x442, 0x00000000,
		0x444, 0x00000010,
		0x445, 0x00000000,
		0x446, 0x00000000,
		0x447, 0x00000000,
		0x448, 0x00000000,
		0x449, 0x000000F0,
		0x44A, 0x0000000F,
		0x44B, 0x0000003E,
		0x44C, 0x00000010,
		0x44D, 0x00000000,
		0x44E, 0x00000000,
		0x44F, 0x00000000,
		0x450, 0x00000000,
		0x451, 0x000000F0,
		0x452, 0x0000000F,
		0x453, 0x00000000,
		0x456, 0x0000005E,
		0x460, 0x00000066,
		0x461, 0x00000066,
		0x4C8, 0x000000FF,
		0x4C9, 0x00000008,
		0x4CC, 0x000000FF,
		0x4CD, 0x000000FF,
		0x4CE, 0x00000001,
		0x500, 0x00000026,
		0x501, 0x000000A2,
		0x502, 0x0000002F,
		0x503, 0x00000000,
		0x504, 0x00000028,
		0x505, 0x000000A3,
		0x506, 0x0000005E,
		0x507, 0x00000000,
		0x508, 0x0000002B,
		0x509, 0x000000A4,
		0x50A, 0x0000005E,
		0x50B, 0x00000000,
		0x50C, 0x0000004F,
		0x50D, 0x000000A4,
		0x50E, 0x00000000,
		0x50F, 0x00000000,
		0x512, 0x0000001C,
		0x514, 0x0000000A,
		0x516, 0x0000000A,
		0x525, 0x0000004F,
		0x540, 0x00000012,
		0x541, 0x00000064,
		0x550, 0x00000010,
		0x551, 0x00000010,
		0x559, 0x00000002,
		0x55C, 0x00000050,
		0x55D, 0x000000FF,
		0x605, 0x00000030,
		0x608, 0x0000000E,
		0x609, 0x0000002A,
		0x620, 0x000000FF,
		0x621, 0x000000FF,
		0x622, 0x000000FF,
		0x623, 0x000000FF,
		0x624, 0x000000FF,
		0x625, 0x000000FF,
		0x626, 0x000000FF,
		0x627, 0x000000FF,
		0x638, 0x00000050,
		0x63C, 0x0000000A,
		0x63D, 0x0000000A,
		0x63E, 0x0000000E,
		0x63F, 0x0000000E,
		0x640, 0x00000040,
		0x642, 0x00000040,
		0x643, 0x00000000,
		0x652, 0x000000C8,
		0x66E, 0x00000005,
		0x700, 0x00000021,
		0x701, 0x00000043,
		0x702, 0x00000065,
		0x703, 0x00000087,
		0x708, 0x00000021,
		0x709, 0x00000043,
		0x70A, 0x00000065,
		0x70B, 0x00000087,
};

u32 RTL8192EE_AGC_TAB_ARRAY[] = {
	0xFF010718, 0xABCD,
		0xC78, 0xFA000001,
		0xC78, 0xF9010001,
		0xC78, 0xF8020001,
		0xC78, 0xF7030001,
		0xC78, 0xF6040001,
		0xC78, 0xF5050001,
		0xC78, 0xF4060001,
		0xC78, 0xF3070001,
		0xC78, 0xF2080001,
		0xC78, 0xF1090001,
		0xC78, 0xF00A0001,
		0xC78, 0xEF0B0001,
		0xC78, 0xEE0C0001,
		0xC78, 0xED0D0001,
		0xC78, 0xEC0E0001,
		0xC78, 0xEB0F0001,
		0xC78, 0xEA100001,
		0xC78, 0xE9110001,
		0xC78, 0xE8120001,
		0xC78, 0xE7130001,
		0xC78, 0xE6140001,
		0xC78, 0xE5150001,
		0xC78, 0xE4160001,
		0xC78, 0xE3170001,
		0xC78, 0xE2180001,
		0xC78, 0xE1190001,
		0xC78, 0x8A1A0001,
		0xC78, 0x891B0001,
		0xC78, 0x881C0001,
		0xC78, 0x871D0001,
		0xC78, 0x861E0001,
		0xC78, 0x851F0001,
		0xC78, 0x84200001,
		0xC78, 0x83210001,
		0xC78, 0x82220001,
		0xC78, 0x6A230001,
		0xC78, 0x69240001,
		0xC78, 0x68250001,
		0xC78, 0x67260001,
		0xC78, 0x66270001,
		0xC78, 0x65280001,
		0xC78, 0x64290001,
		0xC78, 0x632A0001,
		0xC78, 0x622B0001,
		0xC78, 0x612C0001,
		0xC78, 0x602D0001,
		0xC78, 0x472E0001,
		0xC78, 0x462F0001,
		0xC78, 0x45300001,
		0xC78, 0x44310001,
		0xC78, 0x43320001,
		0xC78, 0x42330001,
		0xC78, 0x41340001,
		0xC78, 0x40350001,
		0xC78, 0x40360001,
		0xC78, 0x40370001,
		0xC78, 0x40380001,
		0xC78, 0x40390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0xCDCDCDCD, 0xCDCD,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFB040001,
		0xC78, 0xFB050001,
		0xC78, 0xFA060001,
		0xC78, 0xF9070001,
		0xC78, 0xF8080001,
		0xC78, 0xF7090001,
		0xC78, 0xF60A0001,
		0xC78, 0xF50B0001,
		0xC78, 0xF40C0001,
		0xC78, 0xF30D0001,
		0xC78, 0xF20E0001,
		0xC78, 0xF10F0001,
		0xC78, 0xF0100001,
		0xC78, 0xEF110001,
		0xC78, 0xEE120001,
		0xC78, 0xED130001,
		0xC78, 0xEC140001,
		0xC78, 0xEB150001,
		0xC78, 0xEA160001,
		0xC78, 0xE9170001,
		0xC78, 0xE8180001,
		0xC78, 0xE7190001,
		0xC78, 0xC81A0001,
		0xC78, 0xC71B0001,
		0xC78, 0xC61C0001,
		0xC78, 0x071D0001,
		0xC78, 0x061E0001,
		0xC78, 0x051F0001,
		0xC78, 0x04200001,
		0xC78, 0x03210001,
		0xC78, 0xAA220001,
		0xC78, 0xA9230001,
		0xC78, 0xA8240001,
		0xC78, 0xA7250001,
		0xC78, 0xA6260001,
		0xC78, 0x85270001,
		0xC78, 0x84280001,
		0xC78, 0x83290001,
		0xC78, 0x252A0001,
		0xC78, 0x242B0001,
		0xC78, 0x232C0001,
		0xC78, 0x222D0001,
		0xC78, 0x672E0001,
		0xC78, 0x662F0001,
		0xC78, 0x65300001,
		0xC78, 0x64310001,
		0xC78, 0x63320001,
		0xC78, 0x62330001,
		0xC78, 0x61340001,
		0xC78, 0x45350001,
		0xC78, 0x44360001,
		0xC78, 0x43370001,
		0xC78, 0x42380001,
		0xC78, 0x41390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0xFF010718, 0xDEAD,
	0xFF010718, 0xABCD,
		0xC78, 0xFA400001,
		0xC78, 0xF9410001,
		0xC78, 0xF8420001,
		0xC78, 0xF7430001,
		0xC78, 0xF6440001,
		0xC78, 0xF5450001,
		0xC78, 0xF4460001,
		0xC78, 0xF3470001,
		0xC78, 0xF2480001,
		0xC78, 0xF1490001,
		0xC78, 0xF04A0001,
		0xC78, 0xEF4B0001,
		0xC78, 0xEE4C0001,
		0xC78, 0xED4D0001,
		0xC78, 0xEC4E0001,
		0xC78, 0xEB4F0001,
		0xC78, 0xEA500001,
		0xC78, 0xE9510001,
		0xC78, 0xE8520001,
		0xC78, 0xE7530001,
		0xC78, 0xE6540001,
		0xC78, 0xE5550001,
		0xC78, 0xE4560001,
		0xC78, 0xE3570001,
		0xC78, 0xE2580001,
		0xC78, 0xE1590001,
		0xC78, 0x8A5A0001,
		0xC78, 0x895B0001,
		0xC78, 0x885C0001,
		0xC78, 0x875D0001,
		0xC78, 0x865E0001,
		0xC78, 0x855F0001,
		0xC78, 0x84600001,
		0xC78, 0x83610001,
		0xC78, 0x82620001,
		0xC78, 0x6A630001,
		0xC78, 0x69640001,
		0xC78, 0x68650001,
		0xC78, 0x67660001,
		0xC78, 0x66670001,
		0xC78, 0x65680001,
		0xC78, 0x64690001,
		0xC78, 0x636A0001,
		0xC78, 0x626B0001,
		0xC78, 0x616C0001,
		0xC78, 0x606D0001,
		0xC78, 0x476E0001,
		0xC78, 0x466F0001,
		0xC78, 0x45700001,
		0xC78, 0x44710001,
		0xC78, 0x43720001,
		0xC78, 0x42730001,
		0xC78, 0x41740001,
		0xC78, 0x40750001,
		0xC78, 0x40760001,
		0xC78, 0x40770001,
		0xC78, 0x40780001,
		0xC78, 0x40790001,
		0xC78, 0x407A0001,
		0xC78, 0x407B0001,
		0xC78, 0x407C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040222,
		0xC50, 0x00040220,
	0xCDCDCDCD, 0xCDCD,
		0xC78, 0xFB400001,
		0xC78, 0xFB410001,
		0xC78, 0xFB420001,
		0xC78, 0xFB430001,
		0xC78, 0xFB440001,
		0xC78, 0xFB450001,
		0xC78, 0xFA460001,
		0xC78, 0xF9470001,
		0xC78, 0xF8480001,
		0xC78, 0xF7490001,
		0xC78, 0xF64A0001,
		0xC78, 0xF54B0001,
		0xC78, 0xF44C0001,
		0xC78, 0xF34D0001,
		0xC78, 0xF24E0001,
		0xC78, 0xF14F0001,
		0xC78, 0xF0500001,
		0xC78, 0xEF510001,
		0xC78, 0xEE520001,
		0xC78, 0xED530001,
		0xC78, 0xEC540001,
		0xC78, 0xEB550001,
		0xC78, 0xEA560001,
		0xC78, 0xE9570001,
		0xC78, 0xE8580001,
		0xC78, 0xE7590001,
		0xC78, 0xE65A0001,
		0xC78, 0xE55B0001,
		0xC78, 0xE45C0001,
		0xC78, 0xE35D0001,
		0xC78, 0xE25E0001,
		0xC78, 0xE15F0001,
		0xC78, 0x8A600001,
		0xC78, 0x89610001,
		0xC78, 0x88620001,
		0xC78, 0x87630001,
		0xC78, 0x86640001,
		0xC78, 0x85650001,
		0xC78, 0x84660001,
		0xC78, 0x83670001,
		0xC78, 0x82680001,
		0xC78, 0x6B690001,
		0xC78, 0x6A6A0001,
		0xC78, 0x696B0001,
		0xC78, 0x686C0001,
		0xC78, 0x676D0001,
		0xC78, 0x666E0001,
		0xC78, 0x656F0001,
		0xC78, 0x64700001,
		0xC78, 0x63710001,
		0xC78, 0x62720001,
		0xC78, 0x61730001,
		0xC78, 0x49740001,
		0xC78, 0x48750001,
		0xC78, 0x47760001,
		0xC78, 0x46770001,
		0xC78, 0x45780001,
		0xC78, 0x44790001,
		0xC78, 0x437A0001,
		0xC78, 0x427B0001,
		0xC78, 0x417C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040022,
		0xC50, 0x00040020,
	0xFF010718, 0xDEAD,
};
