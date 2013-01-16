/*
 * adis16400.c	support Analog Devices ADIS16400/5
 *		3d 2g Linear Accelerometers,
 *		3d Gyroscopes,
 *		3d Magnetometers via SPI
 *
 * Copyright (c) 2009 Manuel Stahl <manuel.stahl@iis.fraunhofer.de>
 * Copyright (c) 2007 Jonathan Cameron <jic23@kernel.org>
 * Copyright (c) 2011 Analog Devices Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/module.h>

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/buffer.h>

#include "adis16400.h"

enum adis16400_chip_variant {
	ADIS16300,
	ADIS16334,
	ADIS16350,
	ADIS16360,
	ADIS16362,
	ADIS16364,
	ADIS16400,
};

static int adis16334_get_freq(struct adis16400_state *st)
{
	int ret;
	u16 t;

	ret = adis_read_reg_16(&st->adis, ADIS16400_SMPL_PRD, &t);
	if (ret < 0)
		return ret;

	t >>= ADIS16334_RATE_DIV_SHIFT;

	return (8192 >> t) / 10;
}

static int adis16334_set_freq(struct adis16400_state *st, unsigned int freq)
{
	unsigned int t;

	freq *= 10;
	if (freq < 8192)
		t = ilog2(8192 / freq);
	else
		t = 0;

	if (t > 0x31)
		t = 0x31;

	t <<= ADIS16334_RATE_DIV_SHIFT;
	t |= ADIS16334_RATE_INT_CLK;

	return adis_write_reg_16(&st->adis, ADIS16400_SMPL_PRD, t);
}

static int adis16400_get_freq(struct adis16400_state *st)
{
	int sps, ret;
	u16 t;

	ret = adis_read_reg_16(&st->adis, ADIS16400_SMPL_PRD, &t);
	if (ret < 0)
		return ret;
	sps =  (t & ADIS16400_SMPL_PRD_TIME_BASE) ? 53 : 1638;
	sps /= (t & ADIS16400_SMPL_PRD_DIV_MASK) + 1;

	return sps;
}

static int adis16400_set_freq(struct adis16400_state *st, unsigned int freq)
{
	unsigned int t;

	t = 1638 / freq;
	if (t > 0)
		t--;
	t &= ADIS16400_SMPL_PRD_DIV_MASK;
	if ((t & ADIS16400_SMPL_PRD_DIV_MASK) >= 0x0A)
		st->adis.spi->max_speed_hz = ADIS16400_SPI_SLOW;
	else
		st->adis.spi->max_speed_hz = ADIS16400_SPI_FAST;

	return adis_write_reg_8(&st->adis, ADIS16400_SMPL_PRD, t);
}

static ssize_t adis16400_read_frequency(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct adis16400_state *st = iio_priv(indio_dev);
	int ret, len = 0;

	ret = st->variant->get_freq(st);
	if (ret < 0)
		return ret;
	len = sprintf(buf, "%d SPS\n", ret);
	return len;
}

static const unsigned adis16400_3db_divisors[] = {
	[0] = 2, /* Special case */
	[1] = 6,
	[2] = 12,
	[3] = 25,
	[4] = 50,
	[5] = 100,
	[6] = 200,
	[7] = 200, /* Not a valid setting */
};

static int adis16400_set_filter(struct iio_dev *indio_dev, int sps, int val)
{
	struct adis16400_state *st = iio_priv(indio_dev);
	int i, ret;
	u16 val16;

	for (i = ARRAY_SIZE(adis16400_3db_divisors) - 1; i >= 1; i--) {
		if (sps / adis16400_3db_divisors[i] >= val)
			break;
	}

	ret = adis_read_reg_16(&st->adis, ADIS16400_SENS_AVG, &val16);
	if (ret < 0)
		return ret;

	ret = adis_write_reg_16(&st->adis, ADIS16400_SENS_AVG,
					 (val16 & ~0x07) | i);
	return ret;
}

static ssize_t adis16400_write_frequency(struct device *dev,
		struct device_attribute *attr,
		const char *buf,
		size_t len)
{
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct adis16400_state *st = iio_priv(indio_dev);
	long val;
	int ret;

	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	if (val == 0)
		return -EINVAL;

	mutex_lock(&indio_dev->mlock);

	st->variant->set_freq(st, val);

	/* Also update the filter */
	mutex_unlock(&indio_dev->mlock);

	return ret ? ret : len;
}

/* Power down the device */
static int adis16400_stop_device(struct iio_dev *indio_dev)
{
	struct adis16400_state *st = iio_priv(indio_dev);
	int ret;
	u16 val = ADIS16400_SLP_CNT_POWER_OFF;

	ret = adis_write_reg_16(&st->adis, ADIS16400_SLP_CNT, val);
	if (ret)
		dev_err(&indio_dev->dev,
			"problem with turning device off: SLP_CNT");

	return ret;
}

static int adis16400_initial_setup(struct iio_dev *indio_dev)
{
	int ret;
	u16 prod_id, smp_prd;
	unsigned int device_id;
	struct adis16400_state *st = iio_priv(indio_dev);

	/* use low spi speed for init if the device has a slow mode */
	if (st->variant->flags & ADIS16400_HAS_SLOW_MODE)
		st->adis.spi->max_speed_hz = ADIS16400_SPI_SLOW;
	else
		st->adis.spi->max_speed_hz = ADIS16400_SPI_FAST;
	st->adis.spi->mode = SPI_MODE_3;
	spi_setup(st->adis.spi);

	ret = adis_initial_startup(&st->adis);
	if (ret)
		return ret;

	if (st->variant->flags & ADIS16400_HAS_PROD_ID) {
		ret = adis_read_reg_16(&st->adis,
						ADIS16400_PRODUCT_ID, &prod_id);
		if (ret)
			goto err_ret;

		sscanf(indio_dev->name, "adis%u\n", &device_id);

		if (prod_id != device_id)
			dev_warn(&indio_dev->dev, "Device ID(%u) and product ID(%u) do not match.",
					device_id, prod_id);

		dev_info(&indio_dev->dev, "%s: prod_id 0x%04x at CS%d (irq %d)\n",
		       indio_dev->name, prod_id,
		       st->adis.spi->chip_select, st->adis.spi->irq);
	}
	/* use high spi speed if possible */
	if (st->variant->flags & ADIS16400_HAS_SLOW_MODE) {
		ret = adis_read_reg_16(&st->adis, ADIS16400_SMPL_PRD, &smp_prd);
		if (ret)
			goto err_ret;

		if ((smp_prd & ADIS16400_SMPL_PRD_DIV_MASK) < 0x0A) {
			st->adis.spi->max_speed_hz = ADIS16400_SPI_FAST;
			spi_setup(st->adis.spi);
		}
	}

err_ret:
	return ret;
}

static IIO_DEV_ATTR_SAMP_FREQ(S_IWUSR | S_IRUGO,
			      adis16400_read_frequency,
			      adis16400_write_frequency);

static IIO_CONST_ATTR_SAMP_FREQ_AVAIL("409 546 819 1638");

static const u8 adis16400_addresses[] = {
	[ADIS16400_SCAN_GYRO_X] = ADIS16400_XGYRO_OFF,
	[ADIS16400_SCAN_GYRO_Y] = ADIS16400_YGYRO_OFF,
	[ADIS16400_SCAN_GYRO_Z] = ADIS16400_ZGYRO_OFF,
	[ADIS16400_SCAN_ACC_X] = ADIS16400_XACCL_OFF,
	[ADIS16400_SCAN_ACC_Y] = ADIS16400_YACCL_OFF,
	[ADIS16400_SCAN_ACC_Z] = ADIS16400_ZACCL_OFF,
};

static int adis16400_write_raw(struct iio_dev *indio_dev,
			       struct iio_chan_spec const *chan,
			       int val,
			       int val2,
			       long mask)
{
	struct adis16400_state *st = iio_priv(indio_dev);
	int ret, sps;

	switch (mask) {
	case IIO_CHAN_INFO_CALIBBIAS:
		mutex_lock(&indio_dev->mlock);
		ret = adis_write_reg_16(&st->adis,
				adis16400_addresses[chan->scan_index], val);
		mutex_unlock(&indio_dev->mlock);
		return ret;
	case IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY:
		/* Need to cache values so we can update if the frequency
		   changes */
		mutex_lock(&indio_dev->mlock);
		st->filt_int = val;
		/* Work out update to current value */
		sps = st->variant->get_freq(st);
		if (sps < 0) {
			mutex_unlock(&indio_dev->mlock);
			return sps;
		}

		ret = adis16400_set_filter(indio_dev, sps, val);
		mutex_unlock(&indio_dev->mlock);
		return ret;
	default:
		return -EINVAL;
	}
}

static int adis16400_read_raw(struct iio_dev *indio_dev,
			      struct iio_chan_spec const *chan,
			      int *val,
			      int *val2,
			      long mask)
{
	struct adis16400_state *st = iio_priv(indio_dev);
	int ret;
	s16 val16;

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		return adis_single_conversion(indio_dev, chan, 0, val);
	case IIO_CHAN_INFO_SCALE:
		switch (chan->type) {
		case IIO_ANGL_VEL:
			*val = 0;
			*val2 = st->variant->gyro_scale_micro;
			return IIO_VAL_INT_PLUS_MICRO;
		case IIO_VOLTAGE:
			*val = 0;
			if (chan->channel == 0) {
				*val = 2;
				*val2 = 418000; /* 2.418 mV */
			} else {
				*val = 0;
				*val2 = 805800; /* 805.8 uV */
			}
			return IIO_VAL_INT_PLUS_MICRO;
		case IIO_ACCEL:
			*val = 0;
			*val2 = st->variant->accel_scale_micro;
			return IIO_VAL_INT_PLUS_MICRO;
		case IIO_MAGN:
			*val = 0;
			*val2 = 500; /* 0.5 mgauss */
			return IIO_VAL_INT_PLUS_MICRO;
		case IIO_TEMP:
			*val = st->variant->temp_scale_nano / 1000000;
			*val2 = (st->variant->temp_scale_nano % 1000000);
			return IIO_VAL_INT_PLUS_MICRO;
		default:
			return -EINVAL;
		}
	case IIO_CHAN_INFO_CALIBBIAS:
		mutex_lock(&indio_dev->mlock);
		ret = adis_read_reg_16(&st->adis,
				adis16400_addresses[chan->scan_index], &val16);
		mutex_unlock(&indio_dev->mlock);
		if (ret)
			return ret;
		val16 = ((val16 & 0xFFF) << 4) >> 4;
		*val = val16;
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_OFFSET:
		/* currently only temperature */
		*val = st->variant->temp_offset;
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY:
		mutex_lock(&indio_dev->mlock);
		/* Need both the number of taps and the sampling frequency */
		ret = adis_read_reg_16(&st->adis,
						ADIS16400_SENS_AVG,
						&val16);
		if (ret < 0) {
			mutex_unlock(&indio_dev->mlock);
			return ret;
		}
		ret = st->variant->get_freq(st);
		if (ret >= 0)
			*val = ret / adis16400_3db_divisors[val16 & 0x07];
		*val2 = 0;
		mutex_unlock(&indio_dev->mlock);
		if (ret < 0)
			return ret;
		return IIO_VAL_INT_PLUS_MICRO;
	default:
		return -EINVAL;
	}
}

static const struct iio_chan_spec adis16400_channels[] = {
	{
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.extend_name = "supply",
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16400_SUPPLY_OUT,
		.scan_index = ADIS16400_SCAN_SUPPLY,
		.scan_type = IIO_ST('u', 14, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_YGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_Y,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_Z,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_ZGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_Z,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_YACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_Y,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_Z,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_ZACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_Z,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_MAGN,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XMAGN_OUT,
		.scan_index = ADIS16400_SCAN_MAGN_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_MAGN,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_YMAGN_OUT,
		.scan_index = ADIS16400_SCAN_MAGN_Y,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_MAGN,
		.modified = 1,
		.channel2 = IIO_MOD_Z,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_ZMAGN_OUT,
		.scan_index = ADIS16400_SCAN_MAGN_Z,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_TEMP,
		.indexed = 1,
		.channel = 0,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_OFFSET_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16400_TEMP_OUT,
		.scan_index = ADIS16350_SCAN_TEMP_X,
		.scan_type = IIO_ST('s', 12, 16, 0),
	}, {
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 1,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16400_AUX_ADC,
		.scan_index = ADIS16400_SCAN_ADC,
		.scan_type = IIO_ST('s', 12, 16, 0),
	},
	IIO_CHAN_SOFT_TIMESTAMP(12)
};

static const struct iio_chan_spec adis16350_channels[] = {
	{
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.extend_name = "supply",
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16400_SUPPLY_OUT,
		.scan_index = ADIS16400_SCAN_SUPPLY,
		.scan_type = IIO_ST('u', 12, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_YGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_Y,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_Z,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_ZGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_Z,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_YACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_Y,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_Z,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_ZACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_Z,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_TEMP,
		.indexed = 1,
		.channel = 0,
		.extend_name = "x",
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_OFFSET_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16350_XTEMP_OUT,
		.scan_index = ADIS16350_SCAN_TEMP_X,
		.scan_type = IIO_ST('s', 12, 16, 0),
	}, {
		.type = IIO_TEMP,
		.indexed = 1,
		.channel = 1,
		.extend_name = "y",
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_OFFSET_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16350_YTEMP_OUT,
		.scan_index = ADIS16350_SCAN_TEMP_Y,
		.scan_type = IIO_ST('s', 12, 16, 0),
	}, {
		.type = IIO_TEMP,
		.indexed = 1,
		.channel = 2,
		.extend_name = "z",
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_OFFSET_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16350_ZTEMP_OUT,
		.scan_index = ADIS16350_SCAN_TEMP_Z,
		.scan_type = IIO_ST('s', 12, 16, 0),
	}, {
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 1,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16300_AUX_ADC,
		.scan_index = ADIS16400_SCAN_ADC,
		.scan_type = IIO_ST('s', 12, 16, 0),
	},
	IIO_CHAN_SOFT_TIMESTAMP(11)
};

static const struct iio_chan_spec adis16300_channels[] = {
	{
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.extend_name = "supply",
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16400_SUPPLY_OUT,
		.scan_index = ADIS16400_SCAN_SUPPLY,
		.scan_type = IIO_ST('u', 12, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_YACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_Y,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_Z,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_ZACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_Z,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_TEMP,
		.indexed = 1,
		.channel = 0,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_OFFSET_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16350_XTEMP_OUT,
		.scan_index = ADIS16350_SCAN_TEMP_X,
		.scan_type = IIO_ST('s', 12, 16, 0),
	}, {
		.type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 1,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SEPARATE_BIT,
		.address = ADIS16300_AUX_ADC,
		.scan_index = ADIS16400_SCAN_ADC,
		.scan_type = IIO_ST('s', 12, 16, 0),
	}, {
		.type = IIO_INCLI,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT,
		.address = ADIS16300_PITCH_OUT,
		.scan_index = ADIS16300_SCAN_INCLI_X,
		.scan_type = IIO_ST('s', 13, 16, 0),
	}, {
		.type = IIO_INCLI,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT,
		.address = ADIS16300_ROLL_OUT,
		.scan_index = ADIS16300_SCAN_INCLI_Y,
		.scan_type = IIO_ST('s', 13, 16, 0),
	},
	IIO_CHAN_SOFT_TIMESTAMP(14)
};

static const struct iio_chan_spec adis16334_channels[] = {
	{
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_YGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_Y,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ANGL_VEL,
		.modified = 1,
		.channel2 = IIO_MOD_Z,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_ZGYRO_OUT,
		.scan_index = ADIS16400_SCAN_GYRO_Z,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_X,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_XACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_Y,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_YACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_Y,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_ACCEL,
		.modified = 1,
		.channel2 = IIO_MOD_Z,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_CALIBBIAS_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT |
		IIO_CHAN_INFO_LOW_PASS_FILTER_3DB_FREQUENCY_SHARED_BIT,
		.address = ADIS16400_ZACCL_OUT,
		.scan_index = ADIS16400_SCAN_ACC_Z,
		.scan_type = IIO_ST('s', 14, 16, 0),
	}, {
		.type = IIO_TEMP,
		.indexed = 1,
		.channel = 0,
		.info_mask = IIO_CHAN_INFO_RAW_SEPARATE_BIT |
		IIO_CHAN_INFO_OFFSET_SEPARATE_BIT |
		IIO_CHAN_INFO_SCALE_SHARED_BIT,
		.address = ADIS16350_XTEMP_OUT,
		.scan_index = ADIS16350_SCAN_TEMP_X,
		.scan_type = IIO_ST('s', 14, 16, 0),
	},
	IIO_CHAN_SOFT_TIMESTAMP(12)
};

static struct attribute *adis16400_attributes[] = {
	&iio_dev_attr_sampling_frequency.dev_attr.attr,
	&iio_const_attr_sampling_frequency_available.dev_attr.attr,
	NULL
};

static const struct attribute_group adis16400_attribute_group = {
	.attrs = adis16400_attributes,
};

static struct adis16400_chip_info adis16400_chips[] = {
	[ADIS16300] = {
		.channels = adis16300_channels,
		.num_channels = ARRAY_SIZE(adis16300_channels),
		.flags = ADIS16400_HAS_SLOW_MODE,
		.gyro_scale_micro = IIO_DEGREE_TO_RAD(50000), /* 0.05 deg/s */
		.accel_scale_micro = 5884,
		.temp_scale_nano = 140000000, /* 0.14 C */
		.temp_offset = 25000000 / 140000, /* 25 C = 0x00 */
		.set_freq = adis16400_set_freq,
		.get_freq = adis16400_get_freq,
	},
	[ADIS16334] = {
		.channels = adis16334_channels,
		.num_channels = ARRAY_SIZE(adis16334_channels),
		.flags = ADIS16400_HAS_PROD_ID,
		.gyro_scale_micro = IIO_DEGREE_TO_RAD(50000), /* 0.05 deg/s */
		.accel_scale_micro = IIO_G_TO_M_S_2(1000), /* 1 mg */
		.temp_scale_nano = 67850000, /* 0.06785 C */
		.temp_offset = 25000000 / 67850, /* 25 C = 0x00 */
		.set_freq = adis16334_set_freq,
		.get_freq = adis16334_get_freq,
	},
	[ADIS16350] = {
		.channels = adis16350_channels,
		.num_channels = ARRAY_SIZE(adis16350_channels),
		.gyro_scale_micro = IIO_DEGREE_TO_RAD(73260), /* 0.07326 deg/s */
		.accel_scale_micro = IIO_G_TO_M_S_2(2522), /* 0.002522 g */
		.temp_scale_nano = 145300000, /* 0.1453 C */
		.temp_offset = 25000000 / 145300, /* 25 C = 0x00 */
		.flags = ADIS16400_NO_BURST | ADIS16400_HAS_SLOW_MODE,
		.set_freq = adis16400_set_freq,
		.get_freq = adis16400_get_freq,
	},
	[ADIS16360] = {
		.channels = adis16350_channels,
		.num_channels = ARRAY_SIZE(adis16350_channels),
		.flags = ADIS16400_HAS_PROD_ID | ADIS16400_HAS_SLOW_MODE,
		.gyro_scale_micro = IIO_DEGREE_TO_RAD(50000), /* 0.05 deg/s */
		.accel_scale_micro = IIO_G_TO_M_S_2(3333), /* 3.333 mg */
		.temp_scale_nano = 136000000, /* 0.136 C */
		.temp_offset = 25000000 / 136000, /* 25 C = 0x00 */
		.set_freq = adis16400_set_freq,
		.get_freq = adis16400_get_freq,
	},
	[ADIS16362] = {
		.channels = adis16350_channels,
		.num_channels = ARRAY_SIZE(adis16350_channels),
		.flags = ADIS16400_HAS_PROD_ID | ADIS16400_HAS_SLOW_MODE,
		.gyro_scale_micro = IIO_DEGREE_TO_RAD(50000), /* 0.05 deg/s */
		.accel_scale_micro = IIO_G_TO_M_S_2(333), /* 0.333 mg */
		.temp_scale_nano = 136000000, /* 0.136 C */
		.temp_offset = 25000000 / 136000, /* 25 C = 0x00 */
		.set_freq = adis16400_set_freq,
		.get_freq = adis16400_get_freq,
	},
	[ADIS16364] = {
		.channels = adis16350_channels,
		.num_channels = ARRAY_SIZE(adis16350_channels),
		.flags = ADIS16400_HAS_PROD_ID | ADIS16400_HAS_SLOW_MODE,
		.gyro_scale_micro = IIO_DEGREE_TO_RAD(50000), /* 0.05 deg/s */
		.accel_scale_micro = IIO_G_TO_M_S_2(1000), /* 1 mg */
		.temp_scale_nano = 136000000, /* 0.136 C */
		.temp_offset = 25000000 / 136000, /* 25 C = 0x00 */
		.set_freq = adis16400_set_freq,
		.get_freq = adis16400_get_freq,
	},
	[ADIS16400] = {
		.channels = adis16400_channels,
		.num_channels = ARRAY_SIZE(adis16400_channels),
		.flags = ADIS16400_HAS_PROD_ID | ADIS16400_HAS_SLOW_MODE,
		.gyro_scale_micro = IIO_DEGREE_TO_RAD(50000), /* 0.05 deg/s */
		.accel_scale_micro = IIO_G_TO_M_S_2(3333), /* 3.333 mg */
		.temp_scale_nano = 140000000, /* 0.14 C */
		.temp_offset = 25000000 / 140000, /* 25 C = 0x00 */
		.set_freq = adis16400_set_freq,
		.get_freq = adis16400_get_freq,
	}
};

static const struct iio_info adis16400_info = {
	.driver_module = THIS_MODULE,
	.read_raw = &adis16400_read_raw,
	.write_raw = &adis16400_write_raw,
	.attrs = &adis16400_attribute_group,
};

static const char * const adis16400_status_error_msgs[] = {
	[ADIS16400_DIAG_STAT_ZACCL_FAIL] = "Z-axis accelerometer self-test failure",
	[ADIS16400_DIAG_STAT_YACCL_FAIL] = "Y-axis accelerometer self-test failure",
	[ADIS16400_DIAG_STAT_XACCL_FAIL] = "X-axis accelerometer self-test failure",
	[ADIS16400_DIAG_STAT_XGYRO_FAIL] = "X-axis gyroscope self-test failure",
	[ADIS16400_DIAG_STAT_YGYRO_FAIL] = "Y-axis gyroscope self-test failure",
	[ADIS16400_DIAG_STAT_ZGYRO_FAIL] = "Z-axis gyroscope self-test failure",
	[ADIS16400_DIAG_STAT_ALARM2] = "Alarm 2 active",
	[ADIS16400_DIAG_STAT_ALARM1] = "Alarm 1 active",
	[ADIS16400_DIAG_STAT_FLASH_CHK] = "Flash checksum error",
	[ADIS16400_DIAG_STAT_SELF_TEST] = "Self test error",
	[ADIS16400_DIAG_STAT_OVERFLOW] = "Sensor overrange",
	[ADIS16400_DIAG_STAT_SPI_FAIL] = "SPI failure",
	[ADIS16400_DIAG_STAT_FLASH_UPT] = "Flash update failed",
	[ADIS16400_DIAG_STAT_POWER_HIGH] = "Power supply above 5.25V",
	[ADIS16400_DIAG_STAT_POWER_LOW] = "Power supply below 4.75V",
};

static const struct adis_data adis16400_data = {
	.msc_ctrl_reg = ADIS16400_MSC_CTRL,
	.glob_cmd_reg = ADIS16400_GLOB_CMD,
	.diag_stat_reg = ADIS16400_DIAG_STAT,

	.read_delay = 50,
	.write_delay = 50,

	.self_test_mask = ADIS16400_MSC_CTRL_MEM_TEST,
	.startup_delay = ADIS16400_STARTUP_DELAY,

	.status_error_msgs = adis16400_status_error_msgs,
	.status_error_mask = BIT(ADIS16400_DIAG_STAT_ZACCL_FAIL) |
		BIT(ADIS16400_DIAG_STAT_YACCL_FAIL) |
		BIT(ADIS16400_DIAG_STAT_XACCL_FAIL) |
		BIT(ADIS16400_DIAG_STAT_XGYRO_FAIL) |
		BIT(ADIS16400_DIAG_STAT_YGYRO_FAIL) |
		BIT(ADIS16400_DIAG_STAT_ZGYRO_FAIL) |
		BIT(ADIS16400_DIAG_STAT_ALARM2) |
		BIT(ADIS16400_DIAG_STAT_ALARM1) |
		BIT(ADIS16400_DIAG_STAT_FLASH_CHK) |
		BIT(ADIS16400_DIAG_STAT_SELF_TEST) |
		BIT(ADIS16400_DIAG_STAT_OVERFLOW) |
		BIT(ADIS16400_DIAG_STAT_SPI_FAIL) |
		BIT(ADIS16400_DIAG_STAT_FLASH_UPT) |
		BIT(ADIS16400_DIAG_STAT_POWER_HIGH) |
		BIT(ADIS16400_DIAG_STAT_POWER_LOW),
};

static int adis16400_probe(struct spi_device *spi)
{
	int ret;
	struct adis16400_state *st;
	struct iio_dev *indio_dev = iio_device_alloc(sizeof(*st));
	if (indio_dev == NULL) {
		ret =  -ENOMEM;
		goto error_ret;
	}
	st = iio_priv(indio_dev);
	/* this is only used for removal purposes */
	spi_set_drvdata(spi, indio_dev);

	/* setup the industrialio driver allocated elements */
	st->variant = &adis16400_chips[spi_get_device_id(spi)->driver_data];
	indio_dev->dev.parent = &spi->dev;
	indio_dev->name = spi_get_device_id(spi)->name;
	indio_dev->channels = st->variant->channels;
	indio_dev->num_channels = st->variant->num_channels;
	indio_dev->info = &adis16400_info;
	indio_dev->modes = INDIO_DIRECT_MODE;

	ret = adis_init(&st->adis, indio_dev, spi, &adis16400_data);
	if (ret)
		goto error_free_dev;

	ret = adis16400_configure_ring(indio_dev);
	if (ret)
		goto error_free_dev;

	if (spi->irq) {
		ret = adis_probe_trigger(&st->adis, indio_dev);
		if (ret)
			goto error_unreg_ring_funcs;
	}

	/* Get the device into a sane initial state */
	ret = adis16400_initial_setup(indio_dev);
	if (ret)
		goto error_remove_trigger;
	ret = iio_device_register(indio_dev);
	if (ret)
		goto error_remove_trigger;

	return 0;

error_remove_trigger:
	if (spi->irq)
		adis_remove_trigger(&st->adis);
error_unreg_ring_funcs:
	adis16400_unconfigure_ring(indio_dev);
error_free_dev:
	iio_device_free(indio_dev);
error_ret:
	return ret;
}

/* fixme, confirm ordering in this function */
static int adis16400_remove(struct spi_device *spi)
{
	struct iio_dev *indio_dev =  spi_get_drvdata(spi);
	struct adis16400_state *st = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
	adis16400_stop_device(indio_dev);

	if (spi->irq)
		adis_remove_trigger(&st->adis);
	adis16400_unconfigure_ring(indio_dev);

	iio_device_free(indio_dev);

	return 0;
}

static const struct spi_device_id adis16400_id[] = {
	{"adis16300", ADIS16300},
	{"adis16334", ADIS16334},
	{"adis16350", ADIS16350},
	{"adis16354", ADIS16350},
	{"adis16355", ADIS16350},
	{"adis16360", ADIS16360},
	{"adis16362", ADIS16362},
	{"adis16364", ADIS16364},
	{"adis16365", ADIS16360},
	{"adis16400", ADIS16400},
	{"adis16405", ADIS16400},
	{}
};
MODULE_DEVICE_TABLE(spi, adis16400_id);

static struct spi_driver adis16400_driver = {
	.driver = {
		.name = "adis16400",
		.owner = THIS_MODULE,
	},
	.id_table = adis16400_id,
	.probe = adis16400_probe,
	.remove = adis16400_remove,
};
module_spi_driver(adis16400_driver);

MODULE_AUTHOR("Manuel Stahl <manuel.stahl@iis.fraunhofer.de>");
MODULE_DESCRIPTION("Analog Devices ADIS16400/5 IMU SPI driver");
MODULE_LICENSE("GPL v2");
