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

#ifndef IT913X_H
#define IT913X_H

#include "dvb_frontend.h"

struct ite_config {
	u8 chip_ver;
	u16 chip_type;
	u32 firmware;
	u8 firmware_ver;
	u8 adc_x2;
	u8 tuner_id_0;
	u8 tuner_id_1;
	u8 dual_mode;
	u8 adf;
	/* option to read SIGNAL_LEVEL */
	u8 read_slevel;
};

#if defined(CONFIG_MEDIA_TUNER_IT913X) || \
	(defined(CONFIG_MEDIA_TUNER_IT913X_MODULE) && defined(MODULE))
extern struct dvb_frontend *it913x_attach(struct dvb_frontend *fe,
	struct i2c_adapter *i2c_adap, u8 i2c_addr, struct ite_config *config);
#else
static inline struct dvb_frontend *it913x_attach(struct dvb_frontend *fe,
	struct i2c_adapter *i2c_adap, u8 i2c_addr, struct ite_config *config)
{
	pr_warn("%s: driver disabled by Kconfig\n", __func__);
	return NULL;
}
#endif

#endif
