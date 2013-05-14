/*
 *  Copyright (C) 2004 Bernd Porr, Bernd.Porr@f2s.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * I must give credit here to Chris Baugher who
 * wrote the driver for AT-MIO-16d. I used some parts of this
 * driver. I also must give credits to David Brownell
 * who supported me with the USB development.
 *
 * Bernd Porr
 *
 *
 * Revision history:
 * 0.9: Dropping the first data packet which seems to be from the last transfer.
 *      Buffer overflows in the FX2 are handed over to comedi.
 * 0.92: Dropping now 4 packets. The quad buffer has to be emptied.
 *       Added insn command basically for testing. Sample rate is
 *       1MHz/16ch=62.5kHz
 * 0.99: Ian Abbott pointed out a bug which has been corrected. Thanks!
 * 0.99a: added external trigger.
 * 1.00: added firmware kernel request to the driver which fixed
 *       udev coldplug problem
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/firmware.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/usb.h>
#include <linux/fcntl.h>
#include <linux/compiler.h>
#include "comedi_fc.h"
#include "../comedidev.h"

/*
 * timeout for the USB-transfer
 */
#define EZTIMEOUT	30

/*
 * constants for "firmware" upload and download
 */
#define FIRMWARE		"usbduxfast_firmware.bin"
#define FIRMWARE_MAX_LEN	0x2000
#define USBDUXFASTSUB_FIRMWARE	0xA0
#define VENDOR_DIR_IN		0xC0
#define VENDOR_DIR_OUT		0x40

/*
 * internal addresses of the 8051 processor
 */
#define USBDUXFASTSUB_CPUCS	0xE600

/*
 * max lenghth of the transfer-buffer for software upload
 */
#define TB_LEN	0x2000

/*
 * input endpoint number
 */
#define BULKINEP	6

/*
 * endpoint for the A/D channellist: bulk OUT
 */
#define CHANNELLISTEP	4

/*
 * number of channels
 */
#define NUMCHANNELS	32

/*
 * size of the waveform descriptor
 */
#define WAVESIZE	0x20

/*
 * size of one A/D value
 */
#define SIZEADIN	(sizeof(int16_t))

/*
 * size of the input-buffer IN BYTES
 */
#define SIZEINBUF	512

/*
 * 16 bytes
 */
#define SIZEINSNBUF	512

/*
 * size of the buffer for the dux commands in bytes
 */
#define SIZEOFDUXBUF	256

/*
 * number of in-URBs which receive the data: min=5
 */
#define NUMOFINBUFFERSHIGH	10

/*
 * min delay steps for more than one channel
 * basically when the mux gives up ;-)
 *
 * steps at 30MHz in the FX2
 */
#define MIN_SAMPLING_PERIOD	9

/*
 * max number of 1/30MHz delay steps
 */
#define MAX_SAMPLING_PERIOD	500

/*
 * number of received packets to ignore before we start handing data
 * over to comedi, it's quad buffering and we have to ignore 4 packets
 */
#define PACKETS_TO_IGNORE	4

/*
 * comedi constants
 */
static const struct comedi_lrange range_usbduxfast_ai_range = {
	2, {BIP_RANGE(0.75), BIP_RANGE(0.5)}
};

/*
 * private structure of one subdevice
 *
 * this is the structure which holds all the data of this driver
 * one sub device just now: A/D
 */
struct usbduxfast_private {
	struct urb *urb;	/* BULK-transfer handling: urb */
	uint8_t *duxbuf;
	int8_t *inbuf;
	short int ai_cmd_running;	/* asynchronous command is running */
	short int ai_continous;	/* continous acquisition */
	long int ai_sample_count;	/* number of samples to acquire */
	int ignore;		/* counter which ignores the first
				   buffers */
	struct semaphore sem;
};

/*
 * bulk transfers to usbduxfast
 */
#define SENDADCOMMANDS            0
#define SENDINITEP6               1

static int usbduxfast_send_cmd(struct comedi_device *dev, int cmd_type)
{
	struct usb_interface *intf = comedi_to_usb_interface(dev);
	struct usb_device *usb = interface_to_usbdev(intf);
	struct usbduxfast_private *devpriv = dev->private;
	int nsent;
	int ret;

	devpriv->duxbuf[0] = cmd_type;

	ret = usb_bulk_msg(usb, usb_sndbulkpipe(usb, CHANNELLISTEP),
			   devpriv->duxbuf, SIZEOFDUXBUF,
			   &nsent, 10000);
	if (ret < 0)
		dev_err(dev->class_dev,
			"could not transmit command to the usb-device, err=%d\n",
			ret);
	return ret;
}

static void usbduxfast_cmd_data(struct comedi_device *dev, int index,
				uint8_t len, uint8_t op, uint8_t out,
				uint8_t log)
{
	struct usbduxfast_private *devpriv = dev->private;

	/* Set the GPIF bytes, the first byte is the command byte */
	devpriv->duxbuf[1 + 0x00 + index] = len;
	devpriv->duxbuf[1 + 0x08 + index] = op;
	devpriv->duxbuf[1 + 0x10 + index] = out;
	devpriv->duxbuf[1 + 0x18 + index] = log;
}

static int usbduxfast_ai_stop(struct comedi_device *dev, int do_unlink)
{
	struct usbduxfast_private *devpriv = dev->private;

	/* stop aquistion */
	devpriv->ai_cmd_running = 0;

	if (do_unlink && devpriv->urb) {
		/* kill the running transfer */
		usb_kill_urb(devpriv->urb);
	}

	return 0;
}

static int usbduxfast_ai_cancel(struct comedi_device *dev,
				struct comedi_subdevice *s)
{
	struct usbduxfast_private *devpriv = dev->private;
	int ret;

	if (!devpriv)
		return -EFAULT;

	down(&devpriv->sem);
	ret = usbduxfast_ai_stop(dev, 1);
	up(&devpriv->sem);

	return ret;
}

/*
 * analogue IN
 * interrupt service routine
 */
static void usbduxfast_ai_interrupt(struct urb *urb)
{
	struct comedi_device *dev = urb->context;
	struct comedi_subdevice *s = dev->read_subdev;
	struct comedi_async *async = s->async;
	struct usb_interface *intf = comedi_to_usb_interface(dev);
	struct usb_device *usb = interface_to_usbdev(intf);
	struct usbduxfast_private *devpriv = dev->private;
	int n, err;

	/* are we running a command? */
	if (unlikely(!devpriv->ai_cmd_running)) {
		/*
		 * not running a command
		 * do not continue execution if no asynchronous command
		 * is running in particular not resubmit
		 */
		return;
	}

	/* first we test if something unusual has just happened */
	switch (urb->status) {
	case 0:
		break;

		/*
		 * happens after an unlink command or when the device
		 * is plugged out
		 */
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
	case -ECONNABORTED:
		/* tell this comedi */
		async->events |= COMEDI_CB_EOA;
		async->events |= COMEDI_CB_ERROR;
		comedi_event(dev, s);
		/* stop the transfer w/o unlink */
		usbduxfast_ai_stop(dev, 0);
		return;

	default:
		pr_err("non-zero urb status received in ai intr context: %d\n",
		       urb->status);
		async->events |= COMEDI_CB_EOA;
		async->events |= COMEDI_CB_ERROR;
		comedi_event(dev, s);
		usbduxfast_ai_stop(dev, 0);
		return;
	}

	if (!devpriv->ignore) {
		if (!devpriv->ai_continous) {
			/* not continuous, fixed number of samples */
			n = urb->actual_length / sizeof(uint16_t);
			if (unlikely(devpriv->ai_sample_count < n)) {
				unsigned int num_bytes;

				/* partial sample received */
				num_bytes = devpriv->ai_sample_count *
					    sizeof(uint16_t);
				cfc_write_array_to_buffer(s,
							  urb->transfer_buffer,
							  num_bytes);
				usbduxfast_ai_stop(dev, 0);
				/* tell comedi that the acquistion is over */
				async->events |= COMEDI_CB_EOA;
				comedi_event(dev, s);
				return;
			}
			devpriv->ai_sample_count -= n;
		}
		/* write the full buffer to comedi */
		err = cfc_write_array_to_buffer(s, urb->transfer_buffer,
						urb->actual_length);
		if (unlikely(err == 0)) {
			/* buffer overflow */
			usbduxfast_ai_stop(dev, 0);
			return;
		}

		/* tell comedi that data is there */
		comedi_event(dev, s);
	} else {
		/* ignore this packet */
		devpriv->ignore--;
	}

	/*
	 * command is still running
	 * resubmit urb for BULK transfer
	 */
	urb->dev = usb;
	urb->status = 0;
	err = usb_submit_urb(urb, GFP_ATOMIC);
	if (err < 0) {
		dev_err(dev->class_dev,
			"urb resubm failed: %d", err);
		async->events |= COMEDI_CB_EOA;
		async->events |= COMEDI_CB_ERROR;
		comedi_event(dev, s);
		usbduxfast_ai_stop(dev, 0);
	}
}

static int usbduxfast_submit_urb(struct comedi_device *dev)
{
	struct usb_interface *intf = comedi_to_usb_interface(dev);
	struct usb_device *usb = interface_to_usbdev(intf);
	struct usbduxfast_private *devpriv = dev->private;
	int ret;

	if (!devpriv)
		return -EFAULT;

	usb_fill_bulk_urb(devpriv->urb, usb, usb_rcvbulkpipe(usb, BULKINEP),
			  devpriv->inbuf, SIZEINBUF,
			  usbduxfast_ai_interrupt, dev);

	ret = usb_submit_urb(devpriv->urb, GFP_ATOMIC);
	if (ret) {
		dev_err(dev->class_dev, "usb_submit_urb error %d\n", ret);
		return ret;
	}
	return 0;
}

static int usbduxfast_ai_cmdtest(struct comedi_device *dev,
				 struct comedi_subdevice *s,
				 struct comedi_cmd *cmd)
{
	int err = 0;
	long int steps, tmp;
	int min_sample_period;

	/* Step 1 : check if triggers are trivially valid */

	err |= cfc_check_trigger_src(&cmd->start_src,
					TRIG_NOW | TRIG_EXT | TRIG_INT);
	err |= cfc_check_trigger_src(&cmd->scan_begin_src,
					TRIG_TIMER | TRIG_FOLLOW | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->convert_src, TRIG_TIMER | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= cfc_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);

	if (err)
		return 1;

	/* Step 2a : make sure trigger sources are unique */

	err |= cfc_check_trigger_is_unique(cmd->start_src);
	err |= cfc_check_trigger_is_unique(cmd->scan_begin_src);
	err |= cfc_check_trigger_is_unique(cmd->convert_src);
	err |= cfc_check_trigger_is_unique(cmd->stop_src);

	/* Step 2b : and mutually compatible */

	/* can't have external stop and start triggers at once */
	if (cmd->start_src == TRIG_EXT && cmd->stop_src == TRIG_EXT)
		err |= -EINVAL;

	if (err)
		return 2;

	/* Step 3: check if arguments are trivially valid */

	if (cmd->start_src == TRIG_NOW)
		err |= cfc_check_trigger_arg_is(&cmd->start_arg, 0);

	if (!cmd->chanlist_len)
		err |= -EINVAL;

	err |= cfc_check_trigger_arg_is(&cmd->scan_end_arg, cmd->chanlist_len);

	if (cmd->chanlist_len == 1)
		min_sample_period = 1;
	else
		min_sample_period = MIN_SAMPLING_PERIOD;

	if (cmd->convert_src == TRIG_TIMER) {
		steps = cmd->convert_arg * 30;
		if (steps < (min_sample_period * 1000))
			steps = min_sample_period * 1000;

		if (steps > (MAX_SAMPLING_PERIOD * 1000))
			steps = MAX_SAMPLING_PERIOD * 1000;

		/* calc arg again */
		tmp = steps / 30;
		err |= cfc_check_trigger_arg_is(&cmd->convert_arg, tmp);
	}

	if (cmd->scan_begin_src == TRIG_TIMER)
		err |= -EINVAL;

	/* stop source */
	switch (cmd->stop_src) {
	case TRIG_COUNT:
		err |= cfc_check_trigger_arg_min(&cmd->stop_arg, 1);
		break;
	case TRIG_NONE:
		err |= cfc_check_trigger_arg_is(&cmd->stop_arg, 0);
		break;
		/*
		 * TRIG_EXT doesn't care since it doesn't trigger
		 * off a numbered channel
		 */
	default:
		break;
	}

	if (err)
		return 3;

	/* step 4: fix up any arguments */

	return 0;

}

static int usbduxfast_ai_inttrig(struct comedi_device *dev,
				 struct comedi_subdevice *s,
				 unsigned int trignum)
{
	struct usbduxfast_private *devpriv = dev->private;
	int ret;

	if (!devpriv)
		return -EFAULT;

	down(&devpriv->sem);

	if (trignum != 0) {
		dev_err(dev->class_dev, "invalid trignum\n");
		up(&devpriv->sem);
		return -EINVAL;
	}
	if (!devpriv->ai_cmd_running) {
		devpriv->ai_cmd_running = 1;
		ret = usbduxfast_submit_urb(dev);
		if (ret < 0) {
			dev_err(dev->class_dev, "urbSubmit: err=%d\n", ret);
			devpriv->ai_cmd_running = 0;
			up(&devpriv->sem);
			return ret;
		}
		s->async->inttrig = NULL;
	} else {
		dev_err(dev->class_dev, "ai is already running\n");
	}
	up(&devpriv->sem);
	return 1;
}

static int usbduxfast_ai_cmd(struct comedi_device *dev,
			     struct comedi_subdevice *s)
{
	struct usbduxfast_private *devpriv = dev->private;
	struct comedi_cmd *cmd = &s->async->cmd;
	unsigned int chan, gain, rngmask = 0xff;
	int i, j, ret;
	int result;
	long steps, steps_tmp;

	if (!devpriv)
		return -EFAULT;

	down(&devpriv->sem);
	if (devpriv->ai_cmd_running) {
		dev_err(dev->class_dev, "ai_cmd not possible\n");
		up(&devpriv->sem);
		return -EBUSY;
	}
	/* set current channel of the running acquisition to zero */
	s->async->cur_chan = 0;

	/*
	 * ignore the first buffers from the device if there
	 * is an error condition
	 */
	devpriv->ignore = PACKETS_TO_IGNORE;

	if (cmd->chanlist_len > 0) {
		gain = CR_RANGE(cmd->chanlist[0]);
		for (i = 0; i < cmd->chanlist_len; ++i) {
			chan = CR_CHAN(cmd->chanlist[i]);
			if (chan != i) {
				dev_err(dev->class_dev,
					"channels are not consecutive\n");
				up(&devpriv->sem);
				return -EINVAL;
			}
			if ((gain != CR_RANGE(cmd->chanlist[i]))
			    && (cmd->chanlist_len > 3)) {
				dev_err(dev->class_dev,
					"gain must be the same for all channels\n");
				up(&devpriv->sem);
				return -EINVAL;
			}
			if (i >= NUMCHANNELS) {
				dev_err(dev->class_dev, "chanlist too long\n");
				break;
			}
		}
	}
	steps = 0;
	if (cmd->scan_begin_src == TRIG_TIMER) {
		dev_err(dev->class_dev,
			"scan_begin_src==TRIG_TIMER not valid\n");
		up(&devpriv->sem);
		return -EINVAL;
	}
	if (cmd->convert_src == TRIG_TIMER)
		steps = (cmd->convert_arg * 30) / 1000;

	if ((steps < MIN_SAMPLING_PERIOD) && (cmd->chanlist_len != 1)) {
		dev_err(dev->class_dev,
			"steps=%ld, scan_begin_arg=%d. Not properly tested by cmdtest?\n",
			steps, cmd->scan_begin_arg);
		up(&devpriv->sem);
		return -EINVAL;
	}
	if (steps > MAX_SAMPLING_PERIOD) {
		dev_err(dev->class_dev, "sampling rate too low\n");
		up(&devpriv->sem);
		return -EINVAL;
	}
	if ((cmd->start_src == TRIG_EXT) && (cmd->chanlist_len != 1)
	    && (cmd->chanlist_len != 16)) {
		dev_err(dev->class_dev,
			"TRIG_EXT only with 1 or 16 channels possible\n");
		up(&devpriv->sem);
		return -EINVAL;
	}

	switch (cmd->chanlist_len) {
	case 1:
		/*
		 * one channel
		 */

		if (CR_RANGE(cmd->chanlist[0]) > 0)
			rngmask = 0xff - 0x04;
		else
			rngmask = 0xff;

		/*
		 * for external trigger: looping in this state until
		 * the RDY0 pin becomes zero
		 */

		/* we loop here until ready has been set */
		if (cmd->start_src == TRIG_EXT) {
			/* branch back to state 0 */
			/* deceision state w/o data */
			/* RDY0 = 0 */
			usbduxfast_cmd_data(dev, 0, 0x01, 0x01, rngmask, 0x00);
		} else {	/* we just proceed to state 1 */
			usbduxfast_cmd_data(dev, 0, 0x01, 0x00, rngmask, 0x00);
		}

		if (steps < MIN_SAMPLING_PERIOD) {
			/* for fast single channel aqu without mux */
			if (steps <= 1) {
				/*
				 * we just stay here at state 1 and rexecute
				 * the same state this gives us 30MHz sampling
				 * rate
				 */

				/* branch back to state 1 */
				/* deceision state with data */
				/* doesn't matter */
				usbduxfast_cmd_data(dev, 1,
						    0x89, 0x03, rngmask, 0xff);
			} else {
				/*
				 * we loop through two states: data and delay
				 * max rate is 15MHz
				 */
				/* data */
				/* doesn't matter */
				usbduxfast_cmd_data(dev, 1, steps - 1,
						    0x02, rngmask, 0x00);

				/* branch back to state 1 */
				/* deceision state w/o data */
				/* doesn't matter */
				usbduxfast_cmd_data(dev, 2,
						    0x09, 0x01, rngmask, 0xff);
			}
		} else {
			/*
			 * we loop through 3 states: 2x delay and 1x data
			 * this gives a min sampling rate of 60kHz
			 */

			/* we have 1 state with duration 1 */
			steps = steps - 1;

			/* do the first part of the delay */
			usbduxfast_cmd_data(dev, 1,
					    steps / 2, 0x00, rngmask, 0x00);

			/* and the second part */
			usbduxfast_cmd_data(dev, 2, steps - steps / 2,
					    0x00, rngmask, 0x00);

			/* get the data and branch back */

			/* branch back to state 1 */
			/* deceision state w data */
			/* doesn't matter */
			usbduxfast_cmd_data(dev, 3,
					    0x09, 0x03, rngmask, 0xff);
		}
		break;

	case 2:
		/*
		 * two channels
		 * commit data to the FIFO
		 */

		if (CR_RANGE(cmd->chanlist[0]) > 0)
			rngmask = 0xff - 0x04;
		else
			rngmask = 0xff;

		/* data */
		usbduxfast_cmd_data(dev, 0, 0x01, 0x02, rngmask, 0x00);

		/* we have 1 state with duration 1: state 0 */
		steps_tmp = steps - 1;

		if (CR_RANGE(cmd->chanlist[1]) > 0)
			rngmask = 0xff - 0x04;
		else
			rngmask = 0xff;

		/* do the first part of the delay */
		/* count */
		usbduxfast_cmd_data(dev, 1, steps_tmp / 2,
				    0x00, 0xfe & rngmask, 0x00);

		/* and the second part */
		usbduxfast_cmd_data(dev, 2, steps_tmp  - steps_tmp / 2,
				    0x00, rngmask, 0x00);

		/* data */
		usbduxfast_cmd_data(dev, 3, 0x01, 0x02, rngmask, 0x00);

		/*
		 * we have 2 states with duration 1: step 6 and
		 * the IDLE state
		 */
		steps_tmp = steps - 2;

		if (CR_RANGE(cmd->chanlist[0]) > 0)
			rngmask = 0xff - 0x04;
		else
			rngmask = 0xff;

		/* do the first part of the delay */
		/* reset */
		usbduxfast_cmd_data(dev, 4, steps_tmp / 2,
				    0x00, (0xff - 0x02) & rngmask, 0x00);

		/* and the second part */
		usbduxfast_cmd_data(dev, 5, steps_tmp - steps_tmp / 2,
				    0x00, rngmask, 0x00);

		usbduxfast_cmd_data(dev, 6, 0x01, 0x00, rngmask, 0x00);
		break;

	case 3:
		/*
		 * three channels
		 */
		for (j = 0; j < 1; j++) {
			int index = j * 2;

			if (CR_RANGE(cmd->chanlist[j]) > 0)
				rngmask = 0xff - 0x04;
			else
				rngmask = 0xff;
			/*
			 * commit data to the FIFO and do the first part
			 * of the delay
			 */
			/* data */
			/* no change */
			usbduxfast_cmd_data(dev, index, steps / 2,
					    0x02, rngmask, 0x00);

			if (CR_RANGE(cmd->chanlist[j + 1]) > 0)
				rngmask = 0xff - 0x04;
			else
				rngmask = 0xff;

			/* do the second part of the delay */
			/* no data */
			/* count */
			usbduxfast_cmd_data(dev, index + 1, steps - steps / 2,
					    0x00, 0xfe & rngmask, 0x00);
		}

		/* 2 steps with duration 1: the idele step and step 6: */
		steps_tmp = steps - 2;

		/* commit data to the FIFO and do the first part of the delay */
		/* data */
		usbduxfast_cmd_data(dev, 4, steps_tmp / 2,
				    0x02, rngmask, 0x00);

		if (CR_RANGE(cmd->chanlist[0]) > 0)
			rngmask = 0xff - 0x04;
		else
			rngmask = 0xff;

		/* do the second part of the delay */
		/* no data */
		/* reset */
		usbduxfast_cmd_data(dev, 5, steps_tmp - steps_tmp / 2,
				    0x00, (0xff - 0x02) & rngmask, 0x00);

		usbduxfast_cmd_data(dev, 6, 0x01, 0x00, rngmask, 0x00);

	case 16:
		if (CR_RANGE(cmd->chanlist[0]) > 0)
			rngmask = 0xff - 0x04;
		else
			rngmask = 0xff;

		if (cmd->start_src == TRIG_EXT) {
			/*
			 * we loop here until ready has been set
			 */

			/* branch back to state 0 */
			/* deceision state w/o data */
			/* reset */
			/* RDY0 = 0 */
			usbduxfast_cmd_data(dev, 0, 0x01, 0x01,
					    (0xff - 0x02) & rngmask, 0x00);
		} else {
			/*
			 * we just proceed to state 1
			 */

			/* 30us reset pulse */
			/* reset */
			usbduxfast_cmd_data(dev, 0, 0xff, 0x00,
					    (0xff - 0x02) & rngmask, 0x00);
		}

		/* commit data to the FIFO */
		/* data */
		usbduxfast_cmd_data(dev, 1, 0x01, 0x02, rngmask, 0x00);

		/* we have 2 states with duration 1 */
		steps = steps - 2;

		/* do the first part of the delay */
		usbduxfast_cmd_data(dev, 2, steps / 2,
				    0x00, 0xfe & rngmask, 0x00);

		/* and the second part */
		usbduxfast_cmd_data(dev, 3, steps - steps / 2,
				    0x00, rngmask, 0x00);

		/* branch back to state 1 */
		/* deceision state w/o data */
		/* doesn't matter */
		usbduxfast_cmd_data(dev, 4, 0x09, 0x01, rngmask, 0xff);

		break;

	default:
		dev_err(dev->class_dev, "unsupported combination of channels\n");
		up(&devpriv->sem);
		return -EFAULT;
	}

	/* 0 means that the AD commands are sent */
	result = usbduxfast_send_cmd(dev, SENDADCOMMANDS);
	if (result < 0) {
		up(&devpriv->sem);
		return result;
	}
	if (cmd->stop_src == TRIG_COUNT) {
		devpriv->ai_sample_count = cmd->stop_arg * cmd->scan_end_arg;
		if (devpriv->ai_sample_count < 1) {
			dev_err(dev->class_dev,
				"(cmd->stop_arg)*(cmd->scan_end_arg)<1, aborting\n");
			up(&devpriv->sem);
			return -EFAULT;
		}
		devpriv->ai_continous = 0;
	} else {
		/* continous acquisition */
		devpriv->ai_continous = 1;
		devpriv->ai_sample_count = 0;
	}

	if ((cmd->start_src == TRIG_NOW) || (cmd->start_src == TRIG_EXT)) {
		/* enable this acquisition operation */
		devpriv->ai_cmd_running = 1;
		ret = usbduxfast_submit_urb(dev);
		if (ret < 0) {
			devpriv->ai_cmd_running = 0;
			/* fixme: unlink here?? */
			up(&devpriv->sem);
			return ret;
		}
		s->async->inttrig = NULL;
	} else {
		/*
		 * TRIG_INT
		 * don't enable the acquision operation
		 * wait for an internal signal
		 */
		s->async->inttrig = usbduxfast_ai_inttrig;
	}
	up(&devpriv->sem);

	return 0;
}

/*
 * Mode 0 is used to get a single conversion on demand.
 */
static int usbduxfast_ai_insn_read(struct comedi_device *dev,
				   struct comedi_subdevice *s,
				   struct comedi_insn *insn,
				   unsigned int *data)
{
	struct usb_interface *intf = comedi_to_usb_interface(dev);
	struct usb_device *usb = interface_to_usbdev(intf);
	struct usbduxfast_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
	uint8_t rngmask = range ? (0xff - 0x04) : 0xff;
	int i, j, n, actual_length;
	int ret;

	down(&devpriv->sem);

	if (devpriv->ai_cmd_running) {
		dev_err(dev->class_dev,
			"ai_insn_read not possible, async cmd is running\n");
		up(&devpriv->sem);
		return -EBUSY;
	}

	/* set command for the first channel */

	/* commit data to the FIFO */
	/* data */
	usbduxfast_cmd_data(dev, 0, 0x01, 0x02, rngmask, 0x00);

	/* do the first part of the delay */
	usbduxfast_cmd_data(dev, 1, 0x0c, 0x00, 0xfe & rngmask, 0x00);
	usbduxfast_cmd_data(dev, 2, 0x01, 0x00, 0xfe & rngmask, 0x00);
	usbduxfast_cmd_data(dev, 3, 0x01, 0x00, 0xfe & rngmask, 0x00);
	usbduxfast_cmd_data(dev, 4, 0x01, 0x00, 0xfe & rngmask, 0x00);

	/* second part */
	usbduxfast_cmd_data(dev, 5, 0x0c, 0x00, rngmask, 0x00);
	usbduxfast_cmd_data(dev, 6, 0x01, 0x00, rngmask, 0x00);

	ret = usbduxfast_send_cmd(dev, SENDADCOMMANDS);
	if (ret < 0) {
		up(&devpriv->sem);
		return ret;
	}

	for (i = 0; i < PACKETS_TO_IGNORE; i++) {
		ret = usb_bulk_msg(usb, usb_rcvbulkpipe(usb, BULKINEP),
				   devpriv->inbuf, SIZEINBUF,
				   &actual_length, 10000);
		if (ret < 0) {
			dev_err(dev->class_dev, "insn timeout, no data\n");
			up(&devpriv->sem);
			return ret;
		}
	}

	for (i = 0; i < insn->n;) {
		ret = usb_bulk_msg(usb, usb_rcvbulkpipe(usb, BULKINEP),
				   devpriv->inbuf, SIZEINBUF,
				   &actual_length, 10000);
		if (ret < 0) {
			dev_err(dev->class_dev, "insn data error: %d\n", ret);
			up(&devpriv->sem);
			return ret;
		}
		n = actual_length / sizeof(uint16_t);
		if ((n % 16) != 0) {
			dev_err(dev->class_dev, "insn data packet corrupted\n");
			up(&devpriv->sem);
			return -EINVAL;
		}
		for (j = chan; (j < n) && (i < insn->n); j = j + 16) {
			data[i] = ((uint16_t *) (devpriv->inbuf))[j];
			i++;
		}
	}

	up(&devpriv->sem);

	return insn->n;
}

static int usbduxfast_attach_common(struct comedi_device *dev)
{
	struct usbduxfast_private *devpriv = dev->private;
	struct comedi_subdevice *s;
	int ret;

	down(&devpriv->sem);

	ret = comedi_alloc_subdevices(dev, 1);
	if (ret) {
		up(&devpriv->sem);
		return ret;
	}

	/* Analog Input subdevice */
	s = &dev->subdevices[0];
	dev->read_subdev = s;
	s->type		= COMEDI_SUBD_AI;
	s->subdev_flags	= SDF_READABLE | SDF_GROUND | SDF_CMD_READ;
	s->n_chan	= 16;
	s->len_chanlist	= 16;
	s->insn_read	= usbduxfast_ai_insn_read;
	s->do_cmdtest	= usbduxfast_ai_cmdtest;
	s->do_cmd	= usbduxfast_ai_cmd;
	s->cancel	= usbduxfast_ai_cancel;
	s->maxdata	= 0x1000;
	s->range_table	= &range_usbduxfast_ai_range;

	up(&devpriv->sem);

	return 0;
}

static int usbduxfast_upload_firmware(struct comedi_device *dev,
				      const struct firmware *fw)
{
	struct usb_interface *intf = comedi_to_usb_interface(dev);
	struct usb_device *usb = interface_to_usbdev(intf);
	uint8_t *buf;
	unsigned char *tmp;
	int ret;

	if (!fw->data)
		return 0;

	if (fw->size > FIRMWARE_MAX_LEN) {
		dev_err(dev->class_dev, "firmware binary too large for FX2\n");
		return -ENOMEM;
	}

	/* we generate a local buffer for the firmware */
	buf = kmemdup(fw->data, fw->size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	/* we need a malloc'ed buffer for usb_control_msg() */
	tmp = kmalloc(1, GFP_KERNEL);
	if (!tmp) {
		kfree(buf);
		return -ENOMEM;
	}

	/* stop the current firmware on the device */
	*tmp = 1;	/* 7f92 to one */
	ret = usb_control_msg(usb, usb_sndctrlpipe(usb, 0),
			      USBDUXFASTSUB_FIRMWARE,
			      VENDOR_DIR_OUT,
			      USBDUXFASTSUB_CPUCS, 0x0000,
			      tmp, 1,
			      EZTIMEOUT);
	if (ret < 0) {
		dev_err(dev->class_dev, "can not stop firmware\n");
		goto done;
	}

	/* upload the new firmware to the device */
	ret = usb_control_msg(usb, usb_sndctrlpipe(usb, 0),
			      USBDUXFASTSUB_FIRMWARE,
			      VENDOR_DIR_OUT,
			      0, 0x0000,
			      buf, fw->size,
			      EZTIMEOUT);
	if (ret < 0) {
		dev_err(dev->class_dev, "firmware upload failed\n");
		goto done;
	}

	/* start the new firmware on the device */
	*tmp = 0;	/* 7f92 to zero */
	ret = usb_control_msg(usb, usb_sndctrlpipe(usb, 0),
			      USBDUXFASTSUB_FIRMWARE,
			      VENDOR_DIR_OUT,
			      USBDUXFASTSUB_CPUCS, 0x0000,
			      tmp, 1,
			      EZTIMEOUT);
	if (ret < 0)
		dev_err(dev->class_dev, "can not start firmware\n");

done:
	kfree(tmp);
	kfree(buf);
	return ret;
}

static int usbduxfast_request_firmware(struct comedi_device *dev)
{
	struct usb_interface *intf = comedi_to_usb_interface(dev);
	struct usb_device *usb = interface_to_usbdev(intf);
	const struct firmware *fw;
	int ret;

	ret = request_firmware(&fw, FIRMWARE, &usb->dev);
	if (ret)
		return ret;

	ret = usbduxfast_upload_firmware(dev, fw);
	release_firmware(fw);

	return ret;
}

static int usbduxfast_auto_attach(struct comedi_device *dev,
				  unsigned long context_unused)
{
	struct usb_interface *intf = comedi_to_usb_interface(dev);
	struct usb_device *usb = interface_to_usbdev(intf);
	struct usbduxfast_private *devpriv;
	int ret;

	if (usb->speed != USB_SPEED_HIGH) {
		dev_err(dev->class_dev,
			"This driver needs USB 2.0 to operate. Aborting...\n");
		return -ENODEV;
	}

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	sema_init(&devpriv->sem, 1);
	usb_set_intfdata(intf, devpriv);

	devpriv->duxbuf = kmalloc(SIZEOFDUXBUF, GFP_KERNEL);
	if (!devpriv->duxbuf)
		return -ENOMEM;

	ret = usb_set_interface(usb,
				intf->altsetting->desc.bInterfaceNumber, 1);
	if (ret < 0) {
		dev_err(dev->class_dev,
			"could not switch to alternate setting 1\n");
		return -ENODEV;
	}

	devpriv->urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!devpriv->urb) {
		dev_err(dev->class_dev, "Could not alloc. urb\n");
		return -ENOMEM;
	}

	devpriv->inbuf = kmalloc(SIZEINBUF, GFP_KERNEL);
	if (!devpriv->inbuf)
		return -ENOMEM;

	/*
	 * Request, and upload, the firmware so we can
	 * complete the comedi_driver (*auto_attach).
	 */
	ret = usbduxfast_request_firmware(dev);
	if (ret) {
		dev_err(dev->class_dev, "could not load firmware (err=%d)\n",
			ret);
		return ret;
	}

	return usbduxfast_attach_common(dev);
}

static void usbduxfast_detach(struct comedi_device *dev)
{
	struct usb_interface *intf = comedi_to_usb_interface(dev);
	struct usbduxfast_private *devpriv = dev->private;

	if (!devpriv)
		return;

	down(&devpriv->sem);

	usb_set_intfdata(intf, NULL);

	if (devpriv->urb) {
		/* waits until a running transfer is over */
		usb_kill_urb(devpriv->urb);

		kfree(devpriv->inbuf);
		devpriv->inbuf = NULL;

		usb_free_urb(devpriv->urb);
		devpriv->urb = NULL;
	}

	kfree(devpriv->duxbuf);
	devpriv->duxbuf = NULL;

	devpriv->ai_cmd_running = 0;

	up(&devpriv->sem);
}

static struct comedi_driver usbduxfast_driver = {
	.driver_name	= "usbduxfast",
	.module		= THIS_MODULE,
	.auto_attach	= usbduxfast_auto_attach,
	.detach		= usbduxfast_detach,
};

static int usbduxfast_usb_probe(struct usb_interface *intf,
				const struct usb_device_id *id)
{
	return comedi_usb_auto_config(intf, &usbduxfast_driver, 0);
}

static const struct usb_device_id usbduxfast_usb_table[] = {
	/* { USB_DEVICE(0x4b4, 0x8613) }, testing */
	{ USB_DEVICE(0x13d8, 0x0010) },	/* real ID */
	{ USB_DEVICE(0x13d8, 0x0011) },	/* real ID */
	{ }
};
MODULE_DEVICE_TABLE(usb, usbduxfast_usb_table);

static struct usb_driver usbduxfast_usb_driver = {
	.name		= "usbduxfast",
	.probe		= usbduxfast_usb_probe,
	.disconnect	= comedi_usb_auto_unconfig,
	.id_table	= usbduxfast_usb_table,
};
module_comedi_usb_driver(usbduxfast_driver, usbduxfast_usb_driver);

MODULE_AUTHOR("Bernd Porr, BerndPorr@f2s.com");
MODULE_DESCRIPTION("USB-DUXfast, BerndPorr@f2s.com");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(FIRMWARE);
