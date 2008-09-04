/*
 *  linux/drivers/mmc/host/omap.c
 *
 *  Copyright (C) 2004 Nokia Corporation
 *  Written by Tuukka Tikkanen and Juha Yrj�l�<juha.yrjola@nokia.com>
 *  Misc hacks here and there by Tony Lindgren <tony@atomide.com>
 *  Other hacks (DMA, SD, etc) by David Brownell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/clk.h>
#include <linux/scatterlist.h>
#include <linux/i2c/tps65010.h>

#include <asm/io.h>
#include <asm/irq.h>

#include <mach/board.h>
#include <mach/mmc.h>
#include <mach/gpio.h>
#include <mach/dma.h>
#include <mach/mux.h>
#include <mach/fpga.h>

#define	OMAP_MMC_REG_CMD	0x00
#define	OMAP_MMC_REG_ARGL	0x04
#define	OMAP_MMC_REG_ARGH	0x08
#define	OMAP_MMC_REG_CON	0x0c
#define	OMAP_MMC_REG_STAT	0x10
#define	OMAP_MMC_REG_IE		0x14
#define	OMAP_MMC_REG_CTO	0x18
#define	OMAP_MMC_REG_DTO	0x1c
#define	OMAP_MMC_REG_DATA	0x20
#define	OMAP_MMC_REG_BLEN	0x24
#define	OMAP_MMC_REG_NBLK	0x28
#define	OMAP_MMC_REG_BUF	0x2c
#define OMAP_MMC_REG_SDIO	0x34
#define	OMAP_MMC_REG_REV	0x3c
#define	OMAP_MMC_REG_RSP0	0x40
#define	OMAP_MMC_REG_RSP1	0x44
#define	OMAP_MMC_REG_RSP2	0x48
#define	OMAP_MMC_REG_RSP3	0x4c
#define	OMAP_MMC_REG_RSP4	0x50
#define	OMAP_MMC_REG_RSP5	0x54
#define	OMAP_MMC_REG_RSP6	0x58
#define	OMAP_MMC_REG_RSP7	0x5c
#define	OMAP_MMC_REG_IOSR	0x60
#define	OMAP_MMC_REG_SYSC	0x64
#define	OMAP_MMC_REG_SYSS	0x68

#define	OMAP_MMC_STAT_CARD_ERR		(1 << 14)
#define	OMAP_MMC_STAT_CARD_IRQ		(1 << 13)
#define	OMAP_MMC_STAT_OCR_BUSY		(1 << 12)
#define	OMAP_MMC_STAT_A_EMPTY		(1 << 11)
#define	OMAP_MMC_STAT_A_FULL		(1 << 10)
#define	OMAP_MMC_STAT_CMD_CRC		(1 <<  8)
#define	OMAP_MMC_STAT_CMD_TOUT		(1 <<  7)
#define	OMAP_MMC_STAT_DATA_CRC		(1 <<  6)
#define	OMAP_MMC_STAT_DATA_TOUT		(1 <<  5)
#define	OMAP_MMC_STAT_END_BUSY		(1 <<  4)
#define	OMAP_MMC_STAT_END_OF_DATA	(1 <<  3)
#define	OMAP_MMC_STAT_CARD_BUSY		(1 <<  2)
#define	OMAP_MMC_STAT_END_OF_CMD	(1 <<  0)

#define OMAP_MMC_READ(host, reg)	__raw_readw((host)->virt_base + OMAP_MMC_REG_##reg)
#define OMAP_MMC_WRITE(host, reg, val)	__raw_writew((val), (host)->virt_base + OMAP_MMC_REG_##reg)

/*
 * Command types
 */
#define OMAP_MMC_CMDTYPE_BC	0
#define OMAP_MMC_CMDTYPE_BCR	1
#define OMAP_MMC_CMDTYPE_AC	2
#define OMAP_MMC_CMDTYPE_ADTC	3


#define DRIVER_NAME "mmci-omap"

/* Specifies how often in millisecs to poll for card status changes
 * when the cover switch is open */
#define OMAP_MMC_COVER_POLL_DELAY	500

struct mmc_omap_host;

struct mmc_omap_slot {
	int			id;
	unsigned int		vdd;
	u16			saved_con;
	u16			bus_mode;
	unsigned int		fclk_freq;
	unsigned		powered:1;

	struct tasklet_struct	cover_tasklet;
	struct timer_list       cover_timer;
	unsigned		cover_open;

	struct mmc_request      *mrq;
	struct mmc_omap_host    *host;
	struct mmc_host		*mmc;
	struct omap_mmc_slot_data *pdata;
};

struct mmc_omap_host {
	int			initialized;
	int			suspended;
	struct mmc_request *	mrq;
	struct mmc_command *	cmd;
	struct mmc_data *	data;
	struct mmc_host *	mmc;
	struct device *		dev;
	unsigned char		id; /* 16xx chips have 2 MMC blocks */
	struct clk *		iclk;
	struct clk *		fclk;
	struct resource		*mem_res;
	void __iomem		*virt_base;
	unsigned int		phys_base;
	int			irq;
	unsigned char		bus_mode;
	unsigned char		hw_bus_mode;

	struct work_struct	cmd_abort_work;
	unsigned		abort:1;
	struct timer_list	cmd_abort_timer;

	struct work_struct      slot_release_work;
	struct mmc_omap_slot    *next_slot;
	struct work_struct      send_stop_work;
	struct mmc_data		*stop_data;

	unsigned int		sg_len;
	int			sg_idx;
	u16 *			buffer;
	u32			buffer_bytes_left;
	u32			total_bytes_left;

	unsigned		use_dma:1;
	unsigned		brs_received:1, dma_done:1;
	unsigned		dma_is_read:1;
	unsigned		dma_in_use:1;
	int			dma_ch;
	spinlock_t		dma_lock;
	struct timer_list	dma_timer;
	unsigned		dma_len;

	short			power_pin;

	struct mmc_omap_slot    *slots[OMAP_MMC_MAX_SLOTS];
	struct mmc_omap_slot    *current_slot;
	spinlock_t              slot_lock;
	wait_queue_head_t       slot_wq;
	int                     nr_slots;

	struct timer_list       clk_timer;
	spinlock_t		clk_lock;     /* for changing enabled state */
	unsigned int            fclk_enabled:1;

	struct omap_mmc_platform_data *pdata;
};

void mmc_omap_fclk_offdelay(struct mmc_omap_slot *slot)
{
	unsigned long tick_ns;

	if (slot != NULL && slot->host->fclk_enabled && slot->fclk_freq > 0) {
		tick_ns = (1000000000 + slot->fclk_freq - 1) / slot->fclk_freq;
		ndelay(8 * tick_ns);
	}
}

void mmc_omap_fclk_enable(struct mmc_omap_host *host, unsigned int enable)
{
	unsigned long flags;

	spin_lock_irqsave(&host->clk_lock, flags);
	if (host->fclk_enabled != enable) {
		host->fclk_enabled = enable;
		if (enable)
			clk_enable(host->fclk);
		else
			clk_disable(host->fclk);
	}
	spin_unlock_irqrestore(&host->clk_lock, flags);
}

static void mmc_omap_select_slot(struct mmc_omap_slot *slot, int claimed)
{
	struct mmc_omap_host *host = slot->host;
	unsigned long flags;

	if (claimed)
		goto no_claim;
	spin_lock_irqsave(&host->slot_lock, flags);
	while (host->mmc != NULL) {
		spin_unlock_irqrestore(&host->slot_lock, flags);
		wait_event(host->slot_wq, host->mmc == NULL);
		spin_lock_irqsave(&host->slot_lock, flags);
	}
	host->mmc = slot->mmc;
	spin_unlock_irqrestore(&host->slot_lock, flags);
no_claim:
	del_timer(&host->clk_timer);
	if (host->current_slot != slot || !claimed)
		mmc_omap_fclk_offdelay(host->current_slot);

	if (host->current_slot != slot) {
		OMAP_MMC_WRITE(host, CON, slot->saved_con & 0xFC00);
		if (host->pdata->switch_slot != NULL)
			host->pdata->switch_slot(mmc_dev(slot->mmc), slot->id);
		host->current_slot = slot;
	}

	if (claimed) {
		mmc_omap_fclk_enable(host, 1);

		/* Doing the dummy read here seems to work around some bug
		 * at least in OMAP24xx silicon where the command would not
		 * start after writing the CMD register. Sigh. */
		OMAP_MMC_READ(host, CON);

		OMAP_MMC_WRITE(host, CON, slot->saved_con);
	} else
		mmc_omap_fclk_enable(host, 0);
}

static void mmc_omap_start_request(struct mmc_omap_host *host,
				   struct mmc_request *req);

static void mmc_omap_slot_release_work(struct work_struct *work)
{
	struct mmc_omap_host *host = container_of(work, struct mmc_omap_host,
						  slot_release_work);
	struct mmc_omap_slot *next_slot = host->next_slot;
	struct mmc_request *rq;

	host->next_slot = NULL;
	mmc_omap_select_slot(next_slot, 1);

	rq = next_slot->mrq;
	next_slot->mrq = NULL;
	mmc_omap_start_request(host, rq);
}

static void mmc_omap_release_slot(struct mmc_omap_slot *slot, int clk_enabled)
{
	struct mmc_omap_host *host = slot->host;
	unsigned long flags;
	int i;

	BUG_ON(slot == NULL || host->mmc == NULL);

	if (clk_enabled)
		/* Keeps clock running for at least 8 cycles on valid freq */
		mod_timer(&host->clk_timer, jiffies  + HZ/10);
	else {
		del_timer(&host->clk_timer);
		mmc_omap_fclk_offdelay(slot);
		mmc_omap_fclk_enable(host, 0);
	}

	spin_lock_irqsave(&host->slot_lock, flags);
	/* Check for any pending requests */
	for (i = 0; i < host->nr_slots; i++) {
		struct mmc_omap_slot *new_slot;

		if (host->slots[i] == NULL || host->slots[i]->mrq == NULL)
			continue;

		BUG_ON(host->next_slot != NULL);
		new_slot = host->slots[i];
		/* The current slot should not have a request in queue */
		BUG_ON(new_slot == host->current_slot);

		host->next_slot = new_slot;
		host->mmc = new_slot->mmc;
		spin_unlock_irqrestore(&host->slot_lock, flags);
		schedule_work(&host->slot_release_work);
		return;
	}

	host->mmc = NULL;
	wake_up(&host->slot_wq);
	spin_unlock_irqrestore(&host->slot_lock, flags);
}

static inline
int mmc_omap_cover_is_open(struct mmc_omap_slot *slot)
{
	if (slot->pdata->get_cover_state)
		return slot->pdata->get_cover_state(mmc_dev(slot->mmc),
						    slot->id);
	return 0;
}

static ssize_t
mmc_omap_show_cover_switch(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct mmc_host *mmc = container_of(dev, struct mmc_host, class_dev);
	struct mmc_omap_slot *slot = mmc_priv(mmc);

	return sprintf(buf, "%s\n", mmc_omap_cover_is_open(slot) ? "open" :
		       "closed");
}

static DEVICE_ATTR(cover_switch, S_IRUGO, mmc_omap_show_cover_switch, NULL);

static ssize_t
mmc_omap_show_slot_name(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct mmc_host *mmc = container_of(dev, struct mmc_host, class_dev);
	struct mmc_omap_slot *slot = mmc_priv(mmc);

	return sprintf(buf, "%s\n", slot->pdata->name);
}

static DEVICE_ATTR(slot_name, S_IRUGO, mmc_omap_show_slot_name, NULL);

static void
mmc_omap_start_command(struct mmc_omap_host *host, struct mmc_command *cmd)
{
	u32 cmdreg;
	u32 resptype;
	u32 cmdtype;

	host->cmd = cmd;

	resptype = 0;
	cmdtype = 0;

	/* Our hardware needs to know exact type */
	switch (mmc_resp_type(cmd)) {
	case MMC_RSP_NONE:
		break;
	case MMC_RSP_R1:
	case MMC_RSP_R1B:
		/* resp 1, 1b, 6, 7 */
		resptype = 1;
		break;
	case MMC_RSP_R2:
		resptype = 2;
		break;
	case MMC_RSP_R3:
		resptype = 3;
		break;
	default:
		dev_err(mmc_dev(host->mmc), "Invalid response type: %04x\n", mmc_resp_type(cmd));
		break;
	}

	if (mmc_cmd_type(cmd) == MMC_CMD_ADTC) {
		cmdtype = OMAP_MMC_CMDTYPE_ADTC;
	} else if (mmc_cmd_type(cmd) == MMC_CMD_BC) {
		cmdtype = OMAP_MMC_CMDTYPE_BC;
	} else if (mmc_cmd_type(cmd) == MMC_CMD_BCR) {
		cmdtype = OMAP_MMC_CMDTYPE_BCR;
	} else {
		cmdtype = OMAP_MMC_CMDTYPE_AC;
	}

	cmdreg = cmd->opcode | (resptype << 8) | (cmdtype << 12);

	if (host->current_slot->bus_mode == MMC_BUSMODE_OPENDRAIN)
		cmdreg |= 1 << 6;

	if (cmd->flags & MMC_RSP_BUSY)
		cmdreg |= 1 << 11;

	if (host->data && !(host->data->flags & MMC_DATA_WRITE))
		cmdreg |= 1 << 15;

	mod_timer(&host->cmd_abort_timer, jiffies + HZ/2);

	OMAP_MMC_WRITE(host, CTO, 200);
	OMAP_MMC_WRITE(host, ARGL, cmd->arg & 0xffff);
	OMAP_MMC_WRITE(host, ARGH, cmd->arg >> 16);
	OMAP_MMC_WRITE(host, IE,
		       OMAP_MMC_STAT_A_EMPTY    | OMAP_MMC_STAT_A_FULL    |
		       OMAP_MMC_STAT_CMD_CRC    | OMAP_MMC_STAT_CMD_TOUT  |
		       OMAP_MMC_STAT_DATA_CRC   | OMAP_MMC_STAT_DATA_TOUT |
		       OMAP_MMC_STAT_END_OF_CMD | OMAP_MMC_STAT_CARD_ERR  |
		       OMAP_MMC_STAT_END_OF_DATA);
	OMAP_MMC_WRITE(host, CMD, cmdreg);
}

static void
mmc_omap_release_dma(struct mmc_omap_host *host, struct mmc_data *data,
		     int abort)
{
	enum dma_data_direction dma_data_dir;

	BUG_ON(host->dma_ch < 0);
	if (data->error)
		omap_stop_dma(host->dma_ch);
	/* Release DMA channel lazily */
	mod_timer(&host->dma_timer, jiffies + HZ);
	if (data->flags & MMC_DATA_WRITE)
		dma_data_dir = DMA_TO_DEVICE;
	else
		dma_data_dir = DMA_FROM_DEVICE;
	dma_unmap_sg(mmc_dev(host->mmc), data->sg, host->sg_len,
		     dma_data_dir);
}

static void mmc_omap_send_stop_work(struct work_struct *work)
{
	struct mmc_omap_host *host = container_of(work, struct mmc_omap_host,
						  send_stop_work);
	struct mmc_omap_slot *slot = host->current_slot;
	struct mmc_data *data = host->stop_data;
	unsigned long tick_ns;

	tick_ns = (1000000000 + slot->fclk_freq - 1)/slot->fclk_freq;
	ndelay(8*tick_ns);

	mmc_omap_start_command(host, data->stop);
}

static void
mmc_omap_xfer_done(struct mmc_omap_host *host, struct mmc_data *data)
{
	if (host->dma_in_use)
		mmc_omap_release_dma(host, data, data->error);

	host->data = NULL;
	host->sg_len = 0;

	/* NOTE:  MMC layer will sometimes poll-wait CMD13 next, issuing
	 * dozens of requests until the card finishes writing data.
	 * It'd be cheaper to just wait till an EOFB interrupt arrives...
	 */

	if (!data->stop) {
		struct mmc_host *mmc;

		host->mrq = NULL;
		mmc = host->mmc;
		mmc_omap_release_slot(host->current_slot, 1);
		mmc_request_done(mmc, data->mrq);
		return;
	}

	host->stop_data = data;
	schedule_work(&host->send_stop_work);
}

static void
mmc_omap_send_abort(struct mmc_omap_host *host, int maxloops)
{
	struct mmc_omap_slot *slot = host->current_slot;
	unsigned int restarts, passes, timeout;
	u16 stat = 0;

	/* Sending abort takes 80 clocks. Have some extra and round up */
	timeout = (120*1000000 + slot->fclk_freq - 1)/slot->fclk_freq;
	restarts = 0;
	while (restarts < maxloops) {
		OMAP_MMC_WRITE(host, STAT, 0xFFFF);
		OMAP_MMC_WRITE(host, CMD, (3 << 12) | (1 << 7));

		passes = 0;
		while (passes < timeout) {
			stat = OMAP_MMC_READ(host, STAT);
			if (stat & OMAP_MMC_STAT_END_OF_CMD)
				goto out;
			udelay(1);
			passes++;
		}

		restarts++;
	}
out:
	OMAP_MMC_WRITE(host, STAT, stat);
}

static void
mmc_omap_abort_xfer(struct mmc_omap_host *host, struct mmc_data *data)
{
	if (host->dma_in_use)
		mmc_omap_release_dma(host, data, 1);

	host->data = NULL;
	host->sg_len = 0;

	mmc_omap_send_abort(host, 10000);
}

static void
mmc_omap_end_of_data(struct mmc_omap_host *host, struct mmc_data *data)
{
	unsigned long flags;
	int done;

	if (!host->dma_in_use) {
		mmc_omap_xfer_done(host, data);
		return;
	}
	done = 0;
	spin_lock_irqsave(&host->dma_lock, flags);
	if (host->dma_done)
		done = 1;
	else
		host->brs_received = 1;
	spin_unlock_irqrestore(&host->dma_lock, flags);
	if (done)
		mmc_omap_xfer_done(host, data);
}

static void
mmc_omap_dma_timer(unsigned long data)
{
	struct mmc_omap_host *host = (struct mmc_omap_host *) data;

	BUG_ON(host->dma_ch < 0);
	omap_free_dma(host->dma_ch);
	host->dma_ch = -1;
}

static void
mmc_omap_dma_done(struct mmc_omap_host *host, struct mmc_data *data)
{
	unsigned long flags;
	int done;

	done = 0;
	spin_lock_irqsave(&host->dma_lock, flags);
	if (host->brs_received)
		done = 1;
	else
		host->dma_done = 1;
	spin_unlock_irqrestore(&host->dma_lock, flags);
	if (done)
		mmc_omap_xfer_done(host, data);
}

static void
mmc_omap_cmd_done(struct mmc_omap_host *host, struct mmc_command *cmd)
{
	host->cmd = NULL;

	del_timer(&host->cmd_abort_timer);

	if (cmd->flags & MMC_RSP_PRESENT) {
		if (cmd->flags & MMC_RSP_136) {
			/* response type 2 */
			cmd->resp[3] =
				OMAP_MMC_READ(host, RSP0) |
				(OMAP_MMC_READ(host, RSP1) << 16);
			cmd->resp[2] =
				OMAP_MMC_READ(host, RSP2) |
				(OMAP_MMC_READ(host, RSP3) << 16);
			cmd->resp[1] =
				OMAP_MMC_READ(host, RSP4) |
				(OMAP_MMC_READ(host, RSP5) << 16);
			cmd->resp[0] =
				OMAP_MMC_READ(host, RSP6) |
				(OMAP_MMC_READ(host, RSP7) << 16);
		} else {
			/* response types 1, 1b, 3, 4, 5, 6 */
			cmd->resp[0] =
				OMAP_MMC_READ(host, RSP6) |
				(OMAP_MMC_READ(host, RSP7) << 16);
		}
	}

	if (host->data == NULL || cmd->error) {
		struct mmc_host *mmc;

		if (host->data != NULL)
			mmc_omap_abort_xfer(host, host->data);
		host->mrq = NULL;
		mmc = host->mmc;
		mmc_omap_release_slot(host->current_slot, 1);
		mmc_request_done(mmc, cmd->mrq);
	}
}

/*
 * Abort stuck command. Can occur when card is removed while it is being
 * read.
 */
static void mmc_omap_abort_command(struct work_struct *work)
{
	struct mmc_omap_host *host = container_of(work, struct mmc_omap_host,
						  cmd_abort_work);
	BUG_ON(!host->cmd);

	dev_dbg(mmc_dev(host->mmc), "Aborting stuck command CMD%d\n",
		host->cmd->opcode);

	if (host->cmd->error == 0)
		host->cmd->error = -ETIMEDOUT;

	if (host->data == NULL) {
		struct mmc_command *cmd;
		struct mmc_host    *mmc;

		cmd = host->cmd;
		host->cmd = NULL;
		mmc_omap_send_abort(host, 10000);

		host->mrq = NULL;
		mmc = host->mmc;
		mmc_omap_release_slot(host->current_slot, 1);
		mmc_request_done(mmc, cmd->mrq);
	} else
		mmc_omap_cmd_done(host, host->cmd);

	host->abort = 0;
	enable_irq(host->irq);
}

static void
mmc_omap_cmd_timer(unsigned long data)
{
	struct mmc_omap_host *host = (struct mmc_omap_host *) data;
	unsigned long flags;

	spin_lock_irqsave(&host->slot_lock, flags);
	if (host->cmd != NULL && !host->abort) {
		OMAP_MMC_WRITE(host, IE, 0);
		disable_irq(host->irq);
		host->abort = 1;
		schedule_work(&host->cmd_abort_work);
	}
	spin_unlock_irqrestore(&host->slot_lock, flags);
}

/* PIO only */
static void
mmc_omap_sg_to_buf(struct mmc_omap_host *host)
{
	struct scatterlist *sg;

	sg = host->data->sg + host->sg_idx;
	host->buffer_bytes_left = sg->length;
	host->buffer = sg_virt(sg);
	if (host->buffer_bytes_left > host->total_bytes_left)
		host->buffer_bytes_left = host->total_bytes_left;
}

static void
mmc_omap_clk_timer(unsigned long data)
{
	struct mmc_omap_host *host = (struct mmc_omap_host *) data;

	mmc_omap_fclk_enable(host, 0);
}

/* PIO only */
static void
mmc_omap_xfer_data(struct mmc_omap_host *host, int write)
{
	int n;

	if (host->buffer_bytes_left == 0) {
		host->sg_idx++;
		BUG_ON(host->sg_idx == host->sg_len);
		mmc_omap_sg_to_buf(host);
	}
	n = 64;
	if (n > host->buffer_bytes_left)
		n = host->buffer_bytes_left;
	host->buffer_bytes_left -= n;
	host->total_bytes_left -= n;
	host->data->bytes_xfered += n;

	if (write) {
		__raw_writesw(host->virt_base + OMAP_MMC_REG_DATA, host->buffer, n);
	} else {
		__raw_readsw(host->virt_base + OMAP_MMC_REG_DATA, host->buffer, n);
	}
}

static inline void mmc_omap_report_irq(u16 status)
{
	static const char *mmc_omap_status_bits[] = {
		"EOC", "CD", "CB", "BRS", "EOFB", "DTO", "DCRC", "CTO",
		"CCRC", "CRW", "AF", "AE", "OCRB", "CIRQ", "CERR"
	};
	int i, c = 0;

	for (i = 0; i < ARRAY_SIZE(mmc_omap_status_bits); i++)
		if (status & (1 << i)) {
			if (c)
				printk(" ");
			printk("%s", mmc_omap_status_bits[i]);
			c++;
		}
}

static irqreturn_t mmc_omap_irq(int irq, void *dev_id)
{
	struct mmc_omap_host * host = (struct mmc_omap_host *)dev_id;
	u16 status;
	int end_command;
	int end_transfer;
	int transfer_error, cmd_error;

	if (host->cmd == NULL && host->data == NULL) {
		status = OMAP_MMC_READ(host, STAT);
		dev_info(mmc_dev(host->slots[0]->mmc),
			 "Spurious IRQ 0x%04x\n", status);
		if (status != 0) {
			OMAP_MMC_WRITE(host, STAT, status);
			OMAP_MMC_WRITE(host, IE, 0);
		}
		return IRQ_HANDLED;
	}

	end_command = 0;
	end_transfer = 0;
	transfer_error = 0;
	cmd_error = 0;

	while ((status = OMAP_MMC_READ(host, STAT)) != 0) {
		int cmd;

		OMAP_MMC_WRITE(host, STAT, status);
		if (host->cmd != NULL)
			cmd = host->cmd->opcode;
		else
			cmd = -1;
#ifdef CONFIG_MMC_DEBUG
		dev_dbg(mmc_dev(host->mmc), "MMC IRQ %04x (CMD %d): ",
			status, cmd);
		mmc_omap_report_irq(status);
		printk("\n");
#endif
		if (host->total_bytes_left) {
			if ((status & OMAP_MMC_STAT_A_FULL) ||
			    (status & OMAP_MMC_STAT_END_OF_DATA))
				mmc_omap_xfer_data(host, 0);
			if (status & OMAP_MMC_STAT_A_EMPTY)
				mmc_omap_xfer_data(host, 1);
		}

		if (status & OMAP_MMC_STAT_END_OF_DATA)
			end_transfer = 1;

		if (status & OMAP_MMC_STAT_DATA_TOUT) {
			dev_dbg(mmc_dev(host->mmc), "data timeout (CMD%d)\n",
				cmd);
			if (host->data) {
				host->data->error = -ETIMEDOUT;
				transfer_error = 1;
			}
		}

		if (status & OMAP_MMC_STAT_DATA_CRC) {
			if (host->data) {
				host->data->error = -EILSEQ;
				dev_dbg(mmc_dev(host->mmc),
					 "data CRC error, bytes left %d\n",
					host->total_bytes_left);
				transfer_error = 1;
			} else {
				dev_dbg(mmc_dev(host->mmc), "data CRC error\n");
			}
		}

		if (status & OMAP_MMC_STAT_CMD_TOUT) {
			/* Timeouts are routine with some commands */
			if (host->cmd) {
				struct mmc_omap_slot *slot =
					host->current_slot;
				if (slot == NULL ||
				    !mmc_omap_cover_is_open(slot))
					dev_err(mmc_dev(host->mmc),
						"command timeout (CMD%d)\n",
						cmd);
				host->cmd->error = -ETIMEDOUT;
				end_command = 1;
				cmd_error = 1;
			}
		}

		if (status & OMAP_MMC_STAT_CMD_CRC) {
			if (host->cmd) {
				dev_err(mmc_dev(host->mmc),
					"command CRC error (CMD%d, arg 0x%08x)\n",
					cmd, host->cmd->arg);
				host->cmd->error = -EILSEQ;
				end_command = 1;
				cmd_error = 1;
			} else
				dev_err(mmc_dev(host->mmc),
					"command CRC error without cmd?\n");
		}

		if (status & OMAP_MMC_STAT_CARD_ERR) {
			dev_dbg(mmc_dev(host->mmc),
				"ignoring card status error (CMD%d)\n",
				cmd);
			end_command = 1;
		}

		/*
		 * NOTE: On 1610 the END_OF_CMD may come too early when
		 * starting a write
		 */
		if ((status & OMAP_MMC_STAT_END_OF_CMD) &&
		    (!(status & OMAP_MMC_STAT_A_EMPTY))) {
			end_command = 1;
		}
	}

	if (cmd_error && host->data) {
		del_timer(&host->cmd_abort_timer);
		host->abort = 1;
		OMAP_MMC_WRITE(host, IE, 0);
		disable_irq(host->irq);
		schedule_work(&host->cmd_abort_work);
		return IRQ_HANDLED;
	}

	if (end_command)
		mmc_omap_cmd_done(host, host->cmd);
	if (host->data != NULL) {
		if (transfer_error)
			mmc_omap_xfer_done(host, host->data);
		else if (end_transfer)
			mmc_omap_end_of_data(host, host->data);
	}

	return IRQ_HANDLED;
}

void omap_mmc_notify_cover_event(struct device *dev, int num, int is_closed)
{
	int cover_open;
	struct mmc_omap_host *host = dev_get_drvdata(dev);
	struct mmc_omap_slot *slot = host->slots[num];

	BUG_ON(num >= host->nr_slots);

	/* Other subsystems can call in here before we're initialised. */
	if (host->nr_slots == 0 || !host->slots[num])
		return;

	cover_open = mmc_omap_cover_is_open(slot);
	if (cover_open != slot->cover_open) {
		slot->cover_open = cover_open;
		sysfs_notify(&slot->mmc->class_dev.kobj, NULL, "cover_switch");
	}

	tasklet_hi_schedule(&slot->cover_tasklet);
}

static void mmc_omap_cover_timer(unsigned long arg)
{
	struct mmc_omap_slot *slot = (struct mmc_omap_slot *) arg;
	tasklet_schedule(&slot->cover_tasklet);
}

static void mmc_omap_cover_handler(unsigned long param)
{
	struct mmc_omap_slot *slot = (struct mmc_omap_slot *)param;
	int cover_open = mmc_omap_cover_is_open(slot);

	mmc_detect_change(slot->mmc, 0);
	if (!cover_open)
		return;

	/*
	 * If no card is inserted, we postpone polling until
	 * the cover has been closed.
	 */
	if (slot->mmc->card == NULL || !mmc_card_present(slot->mmc->card))
		return;

	mod_timer(&slot->cover_timer,
		  jiffies + msecs_to_jiffies(OMAP_MMC_COVER_POLL_DELAY));
}

/* Prepare to transfer the next segment of a scatterlist */
static void
mmc_omap_prepare_dma(struct mmc_omap_host *host, struct mmc_data *data)
{
	int dma_ch = host->dma_ch;
	unsigned long data_addr;
	u16 buf, frame;
	u32 count;
	struct scatterlist *sg = &data->sg[host->sg_idx];
	int src_port = 0;
	int dst_port = 0;
	int sync_dev = 0;

	data_addr = host->phys_base + OMAP_MMC_REG_DATA;
	frame = data->blksz;
	count = sg_dma_len(sg);

	if ((data->blocks == 1) && (count > data->blksz))
		count = frame;

	host->dma_len = count;

	/* FIFO is 16x2 bytes on 15xx, and 32x2 bytes on 16xx and 24xx.
	 * Use 16 or 32 word frames when the blocksize is at least that large.
	 * Blocksize is usually 512 bytes; but not for some SD reads.
	 */
	if (cpu_is_omap15xx() && frame > 32)
		frame = 32;
	else if (frame > 64)
		frame = 64;
	count /= frame;
	frame >>= 1;

	if (!(data->flags & MMC_DATA_WRITE)) {
		buf = 0x800f | ((frame - 1) << 8);

		if (cpu_class_is_omap1()) {
			src_port = OMAP_DMA_PORT_TIPB;
			dst_port = OMAP_DMA_PORT_EMIFF;
		}
		if (cpu_is_omap24xx())
			sync_dev = OMAP24XX_DMA_MMC1_RX;

		omap_set_dma_src_params(dma_ch, src_port,
					OMAP_DMA_AMODE_CONSTANT,
					data_addr, 0, 0);
		omap_set_dma_dest_params(dma_ch, dst_port,
					 OMAP_DMA_AMODE_POST_INC,
					 sg_dma_address(sg), 0, 0);
		omap_set_dma_dest_data_pack(dma_ch, 1);
		omap_set_dma_dest_burst_mode(dma_ch, OMAP_DMA_DATA_BURST_4);
	} else {
		buf = 0x0f80 | ((frame - 1) << 0);

		if (cpu_class_is_omap1()) {
			src_port = OMAP_DMA_PORT_EMIFF;
			dst_port = OMAP_DMA_PORT_TIPB;
		}
		if (cpu_is_omap24xx())
			sync_dev = OMAP24XX_DMA_MMC1_TX;

		omap_set_dma_dest_params(dma_ch, dst_port,
					 OMAP_DMA_AMODE_CONSTANT,
					 data_addr, 0, 0);
		omap_set_dma_src_params(dma_ch, src_port,
					OMAP_DMA_AMODE_POST_INC,
					sg_dma_address(sg), 0, 0);
		omap_set_dma_src_data_pack(dma_ch, 1);
		omap_set_dma_src_burst_mode(dma_ch, OMAP_DMA_DATA_BURST_4);
	}

	/* Max limit for DMA frame count is 0xffff */
	BUG_ON(count > 0xffff);

	OMAP_MMC_WRITE(host, BUF, buf);
	omap_set_dma_transfer_params(dma_ch, OMAP_DMA_DATA_TYPE_S16,
				     frame, count, OMAP_DMA_SYNC_FRAME,
				     sync_dev, 0);
}

/* A scatterlist segment completed */
static void mmc_omap_dma_cb(int lch, u16 ch_status, void *data)
{
	struct mmc_omap_host *host = (struct mmc_omap_host *) data;
	struct mmc_data *mmcdat = host->data;

	if (unlikely(host->dma_ch < 0)) {
		dev_err(mmc_dev(host->mmc),
			"DMA callback while DMA not enabled\n");
		return;
	}
	/* FIXME: We really should do something to _handle_ the errors */
	if (ch_status & OMAP1_DMA_TOUT_IRQ) {
		dev_err(mmc_dev(host->mmc),"DMA timeout\n");
		return;
	}
	if (ch_status & OMAP_DMA_DROP_IRQ) {
		dev_err(mmc_dev(host->mmc), "DMA sync error\n");
		return;
	}
	if (!(ch_status & OMAP_DMA_BLOCK_IRQ)) {
		return;
	}
	mmcdat->bytes_xfered += host->dma_len;
	host->sg_idx++;
	if (host->sg_idx < host->sg_len) {
		mmc_omap_prepare_dma(host, host->data);
		omap_start_dma(host->dma_ch);
	} else
		mmc_omap_dma_done(host, host->data);
}

static int mmc_omap_get_dma_channel(struct mmc_omap_host *host, struct mmc_data *data)
{
	const char *dma_dev_name;
	int sync_dev, dma_ch, is_read, r;

	is_read = !(data->flags & MMC_DATA_WRITE);
	del_timer_sync(&host->dma_timer);
	if (host->dma_ch >= 0) {
		if (is_read == host->dma_is_read)
			return 0;
		omap_free_dma(host->dma_ch);
		host->dma_ch = -1;
	}

	if (is_read) {
		if (host->id == 1) {
			sync_dev = OMAP_DMA_MMC_RX;
			dma_dev_name = "MMC1 read";
		} else {
			sync_dev = OMAP_DMA_MMC2_RX;
			dma_dev_name = "MMC2 read";
		}
	} else {
		if (host->id == 1) {
			sync_dev = OMAP_DMA_MMC_TX;
			dma_dev_name = "MMC1 write";
		} else {
			sync_dev = OMAP_DMA_MMC2_TX;
			dma_dev_name = "MMC2 write";
		}
	}
	r = omap_request_dma(sync_dev, dma_dev_name, mmc_omap_dma_cb,
			     host, &dma_ch);
	if (r != 0) {
		dev_dbg(mmc_dev(host->mmc), "omap_request_dma() failed with %d\n", r);
		return r;
	}
	host->dma_ch = dma_ch;
	host->dma_is_read = is_read;

	return 0;
}

static inline void set_cmd_timeout(struct mmc_omap_host *host, struct mmc_request *req)
{
	u16 reg;

	reg = OMAP_MMC_READ(host, SDIO);
	reg &= ~(1 << 5);
	OMAP_MMC_WRITE(host, SDIO, reg);
	/* Set maximum timeout */
	OMAP_MMC_WRITE(host, CTO, 0xff);
}

static inline void set_data_timeout(struct mmc_omap_host *host, struct mmc_request *req)
{
	unsigned int timeout, cycle_ns;
	u16 reg;

	cycle_ns = 1000000000 / host->current_slot->fclk_freq;
	timeout = req->data->timeout_ns / cycle_ns;
	timeout += req->data->timeout_clks;

	/* Check if we need to use timeout multiplier register */
	reg = OMAP_MMC_READ(host, SDIO);
	if (timeout > 0xffff) {
		reg |= (1 << 5);
		timeout /= 1024;
	} else
		reg &= ~(1 << 5);
	OMAP_MMC_WRITE(host, SDIO, reg);
	OMAP_MMC_WRITE(host, DTO, timeout);
}

static void
mmc_omap_prepare_data(struct mmc_omap_host *host, struct mmc_request *req)
{
	struct mmc_data *data = req->data;
	int i, use_dma, block_size;
	unsigned sg_len;

	host->data = data;
	if (data == NULL) {
		OMAP_MMC_WRITE(host, BLEN, 0);
		OMAP_MMC_WRITE(host, NBLK, 0);
		OMAP_MMC_WRITE(host, BUF, 0);
		host->dma_in_use = 0;
		set_cmd_timeout(host, req);
		return;
	}

	block_size = data->blksz;

	OMAP_MMC_WRITE(host, NBLK, data->blocks - 1);
	OMAP_MMC_WRITE(host, BLEN, block_size - 1);
	set_data_timeout(host, req);

	/* cope with calling layer confusion; it issues "single
	 * block" writes using multi-block scatterlists.
	 */
	sg_len = (data->blocks == 1) ? 1 : data->sg_len;

	/* Only do DMA for entire blocks */
	use_dma = host->use_dma;
	if (use_dma) {
		for (i = 0; i < sg_len; i++) {
			if ((data->sg[i].length % block_size) != 0) {
				use_dma = 0;
				break;
			}
		}
	}

	host->sg_idx = 0;
	if (use_dma) {
		if (mmc_omap_get_dma_channel(host, data) == 0) {
			enum dma_data_direction dma_data_dir;

			if (data->flags & MMC_DATA_WRITE)
				dma_data_dir = DMA_TO_DEVICE;
			else
				dma_data_dir = DMA_FROM_DEVICE;

			host->sg_len = dma_map_sg(mmc_dev(host->mmc), data->sg,
						sg_len, dma_data_dir);
			host->total_bytes_left = 0;
			mmc_omap_prepare_dma(host, req->data);
			host->brs_received = 0;
			host->dma_done = 0;
			host->dma_in_use = 1;
		} else
			use_dma = 0;
	}

	/* Revert to PIO? */
	if (!use_dma) {
		OMAP_MMC_WRITE(host, BUF, 0x1f1f);
		host->total_bytes_left = data->blocks * block_size;
		host->sg_len = sg_len;
		mmc_omap_sg_to_buf(host);
		host->dma_in_use = 0;
	}
}

static void mmc_omap_start_request(struct mmc_omap_host *host,
				   struct mmc_request *req)
{
	BUG_ON(host->mrq != NULL);

	host->mrq = req;

	/* only touch fifo AFTER the controller readies it */
	mmc_omap_prepare_data(host, req);
	mmc_omap_start_command(host, req->cmd);
	if (host->dma_in_use)
		omap_start_dma(host->dma_ch);
	BUG_ON(irqs_disabled());
}

static void mmc_omap_request(struct mmc_host *mmc, struct mmc_request *req)
{
	struct mmc_omap_slot *slot = mmc_priv(mmc);
	struct mmc_omap_host *host = slot->host;
	unsigned long flags;

	spin_lock_irqsave(&host->slot_lock, flags);
	if (host->mmc != NULL) {
		BUG_ON(slot->mrq != NULL);
		slot->mrq = req;
		spin_unlock_irqrestore(&host->slot_lock, flags);
		return;
	} else
		host->mmc = mmc;
	spin_unlock_irqrestore(&host->slot_lock, flags);
	mmc_omap_select_slot(slot, 1);
	mmc_omap_start_request(host, req);
}

static void mmc_omap_set_power(struct mmc_omap_slot *slot, int power_on,
				int vdd)
{
	struct mmc_omap_host *host;

	host = slot->host;

	if (slot->pdata->set_power != NULL)
		slot->pdata->set_power(mmc_dev(slot->mmc), slot->id, power_on,
					vdd);

	if (cpu_is_omap24xx()) {
		u16 w;

		if (power_on) {
			w = OMAP_MMC_READ(host, CON);
			OMAP_MMC_WRITE(host, CON, w | (1 << 11));
		} else {
			w = OMAP_MMC_READ(host, CON);
			OMAP_MMC_WRITE(host, CON, w & ~(1 << 11));
		}
	}
}

static int mmc_omap_calc_divisor(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct mmc_omap_slot *slot = mmc_priv(mmc);
	struct mmc_omap_host *host = slot->host;
	int func_clk_rate = clk_get_rate(host->fclk);
	int dsor;

	if (ios->clock == 0)
		return 0;

	dsor = func_clk_rate / ios->clock;
	if (dsor < 1)
		dsor = 1;

	if (func_clk_rate / dsor > ios->clock)
		dsor++;

	if (dsor > 250)
		dsor = 250;

	slot->fclk_freq = func_clk_rate / dsor;

	if (ios->bus_width == MMC_BUS_WIDTH_4)
		dsor |= 1 << 15;

	return dsor;
}

static void mmc_omap_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct mmc_omap_slot *slot = mmc_priv(mmc);
	struct mmc_omap_host *host = slot->host;
	int i, dsor;
	int clk_enabled;

	mmc_omap_select_slot(slot, 0);

	dsor = mmc_omap_calc_divisor(mmc, ios);

	if (ios->vdd != slot->vdd)
		slot->vdd = ios->vdd;

	clk_enabled = 0;
	switch (ios->power_mode) {
	case MMC_POWER_OFF:
		mmc_omap_set_power(slot, 0, ios->vdd);
		break;
	case MMC_POWER_UP:
		/* Cannot touch dsor yet, just power up MMC */
		mmc_omap_set_power(slot, 1, ios->vdd);
		goto exit;
	case MMC_POWER_ON:
		mmc_omap_fclk_enable(host, 1);
		clk_enabled = 1;
		dsor |= 1 << 11;
		break;
	}

	if (slot->bus_mode != ios->bus_mode) {
		if (slot->pdata->set_bus_mode != NULL)
			slot->pdata->set_bus_mode(mmc_dev(mmc), slot->id,
						  ios->bus_mode);
		slot->bus_mode = ios->bus_mode;
	}

	/* On insanely high arm_per frequencies something sometimes
	 * goes somehow out of sync, and the POW bit is not being set,
	 * which results in the while loop below getting stuck.
	 * Writing to the CON register twice seems to do the trick. */
	for (i = 0; i < 2; i++)
		OMAP_MMC_WRITE(host, CON, dsor);
	slot->saved_con = dsor;
	if (ios->power_mode == MMC_POWER_ON) {
		/* worst case at 400kHz, 80 cycles makes 200 microsecs */
		int usecs = 250;

		/* Send clock cycles, poll completion */
		OMAP_MMC_WRITE(host, IE, 0);
		OMAP_MMC_WRITE(host, STAT, 0xffff);
		OMAP_MMC_WRITE(host, CMD, 1 << 7);
		while (usecs > 0 && (OMAP_MMC_READ(host, STAT) & 1) == 0) {
			udelay(1);
			usecs--;
		}
		OMAP_MMC_WRITE(host, STAT, 1);
	}

exit:
	mmc_omap_release_slot(slot, clk_enabled);
}

static const struct mmc_host_ops mmc_omap_ops = {
	.request	= mmc_omap_request,
	.set_ios	= mmc_omap_set_ios,
};

static int __init mmc_omap_new_slot(struct mmc_omap_host *host, int id)
{
	struct mmc_omap_slot *slot = NULL;
	struct mmc_host *mmc;
	int r;

	mmc = mmc_alloc_host(sizeof(struct mmc_omap_slot), host->dev);
	if (mmc == NULL)
		return -ENOMEM;

	slot = mmc_priv(mmc);
	slot->host = host;
	slot->mmc = mmc;
	slot->id = id;
	slot->pdata = &host->pdata->slots[id];

	host->slots[id] = slot;

	mmc->caps = 0;
	if (host->pdata->conf.wire4)
		mmc->caps |= MMC_CAP_4_BIT_DATA;

	mmc->ops = &mmc_omap_ops;
	mmc->f_min = 400000;

	if (cpu_class_is_omap2())
		mmc->f_max = 48000000;
	else
		mmc->f_max = 24000000;
	if (host->pdata->max_freq)
		mmc->f_max = min(host->pdata->max_freq, mmc->f_max);
	mmc->ocr_avail = slot->pdata->ocr_mask;

	/* Use scatterlist DMA to reduce per-transfer costs.
	 * NOTE max_seg_size assumption that small blocks aren't
	 * normally used (except e.g. for reading SD registers).
	 */
	mmc->max_phys_segs = 32;
	mmc->max_hw_segs = 32;
	mmc->max_blk_size = 2048;	/* BLEN is 11 bits (+1) */
	mmc->max_blk_count = 2048;	/* NBLK is 11 bits (+1) */
	mmc->max_req_size = mmc->max_blk_size * mmc->max_blk_count;
	mmc->max_seg_size = mmc->max_req_size;

	r = mmc_add_host(mmc);
	if (r < 0)
		goto err_remove_host;

	if (slot->pdata->name != NULL) {
		r = device_create_file(&mmc->class_dev,
					&dev_attr_slot_name);
		if (r < 0)
			goto err_remove_host;
	}

	if (slot->pdata->get_cover_state != NULL) {
		r = device_create_file(&mmc->class_dev,
					&dev_attr_cover_switch);
		if (r < 0)
			goto err_remove_slot_name;

		setup_timer(&slot->cover_timer, mmc_omap_cover_timer,
			    (unsigned long)slot);
		tasklet_init(&slot->cover_tasklet, mmc_omap_cover_handler,
			     (unsigned long)slot);
		tasklet_schedule(&slot->cover_tasklet);
	}

	return 0;

err_remove_slot_name:
	if (slot->pdata->name != NULL)
		device_remove_file(&mmc->class_dev, &dev_attr_slot_name);
err_remove_host:
	mmc_remove_host(mmc);
	mmc_free_host(mmc);
	return r;
}

static void mmc_omap_remove_slot(struct mmc_omap_slot *slot)
{
	struct mmc_host *mmc = slot->mmc;

	if (slot->pdata->name != NULL)
		device_remove_file(&mmc->class_dev, &dev_attr_slot_name);
	if (slot->pdata->get_cover_state != NULL)
		device_remove_file(&mmc->class_dev, &dev_attr_cover_switch);

	tasklet_kill(&slot->cover_tasklet);
	del_timer_sync(&slot->cover_timer);
	flush_scheduled_work();

	mmc_remove_host(mmc);
	mmc_free_host(mmc);
}

static int __init mmc_omap_probe(struct platform_device *pdev)
{
	struct omap_mmc_platform_data *pdata = pdev->dev.platform_data;
	struct mmc_omap_host *host = NULL;
	struct resource *res;
	int i, ret = 0;
	int irq;

	if (pdata == NULL) {
		dev_err(&pdev->dev, "platform data missing\n");
		return -ENXIO;
	}
	if (pdata->nr_slots == 0) {
		dev_err(&pdev->dev, "no slots\n");
		return -ENXIO;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	irq = platform_get_irq(pdev, 0);
	if (res == NULL || irq < 0)
		return -ENXIO;

	res = request_mem_region(res->start, res->end - res->start + 1,
				 pdev->name);
	if (res == NULL)
		return -EBUSY;

	host = kzalloc(sizeof(struct mmc_omap_host), GFP_KERNEL);
	if (host == NULL) {
		ret = -ENOMEM;
		goto err_free_mem_region;
	}

	INIT_WORK(&host->slot_release_work, mmc_omap_slot_release_work);
	INIT_WORK(&host->send_stop_work, mmc_omap_send_stop_work);

	INIT_WORK(&host->cmd_abort_work, mmc_omap_abort_command);
	setup_timer(&host->cmd_abort_timer, mmc_omap_cmd_timer,
		    (unsigned long) host);

	spin_lock_init(&host->clk_lock);
	setup_timer(&host->clk_timer, mmc_omap_clk_timer, (unsigned long) host);

	spin_lock_init(&host->dma_lock);
	setup_timer(&host->dma_timer, mmc_omap_dma_timer, (unsigned long) host);
	spin_lock_init(&host->slot_lock);
	init_waitqueue_head(&host->slot_wq);

	host->pdata = pdata;
	host->dev = &pdev->dev;
	platform_set_drvdata(pdev, host);

	host->id = pdev->id;
	host->mem_res = res;
	host->irq = irq;

	host->use_dma = 1;
	host->dma_ch = -1;

	host->irq = irq;
	host->phys_base = host->mem_res->start;
	host->virt_base = ioremap(res->start, res->end - res->start + 1);
	if (!host->virt_base)
		goto err_ioremap;

	if (cpu_is_omap24xx()) {
		host->iclk = clk_get(&pdev->dev, "mmc_ick");
		if (IS_ERR(host->iclk))
			goto err_free_mmc_host;
		clk_enable(host->iclk);
	}

	if (!cpu_is_omap24xx())
		host->fclk = clk_get(&pdev->dev, "mmc_ck");
	else
		host->fclk = clk_get(&pdev->dev, "mmc_fck");

	if (IS_ERR(host->fclk)) {
		ret = PTR_ERR(host->fclk);
		goto err_free_iclk;
	}

	ret = request_irq(host->irq, mmc_omap_irq, 0, DRIVER_NAME, host);
	if (ret)
		goto err_free_fclk;

	if (pdata->init != NULL) {
		ret = pdata->init(&pdev->dev);
		if (ret < 0)
			goto err_free_irq;
	}

	host->nr_slots = pdata->nr_slots;
	for (i = 0; i < pdata->nr_slots; i++) {
		ret = mmc_omap_new_slot(host, i);
		if (ret < 0) {
			while (--i >= 0)
				mmc_omap_remove_slot(host->slots[i]);

			goto err_plat_cleanup;
		}
	}

	return 0;

err_plat_cleanup:
	if (pdata->cleanup)
		pdata->cleanup(&pdev->dev);
err_free_irq:
	free_irq(host->irq, host);
err_free_fclk:
	clk_put(host->fclk);
err_free_iclk:
	if (host->iclk != NULL) {
		clk_disable(host->iclk);
		clk_put(host->iclk);
	}
err_free_mmc_host:
	iounmap(host->virt_base);
err_ioremap:
	kfree(host);
err_free_mem_region:
	release_mem_region(res->start, res->end - res->start + 1);
	return ret;
}

static int mmc_omap_remove(struct platform_device *pdev)
{
	struct mmc_omap_host *host = platform_get_drvdata(pdev);
	int i;

	platform_set_drvdata(pdev, NULL);

	BUG_ON(host == NULL);

	for (i = 0; i < host->nr_slots; i++)
		mmc_omap_remove_slot(host->slots[i]);

	if (host->pdata->cleanup)
		host->pdata->cleanup(&pdev->dev);

	if (host->iclk && !IS_ERR(host->iclk))
		clk_put(host->iclk);
	if (host->fclk && !IS_ERR(host->fclk))
		clk_put(host->fclk);

	iounmap(host->virt_base);
	release_mem_region(pdev->resource[0].start,
			   pdev->resource[0].end - pdev->resource[0].start + 1);

	kfree(host);

	return 0;
}

#ifdef CONFIG_PM
static int mmc_omap_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	int i, ret = 0;
	struct mmc_omap_host *host = platform_get_drvdata(pdev);

	if (host == NULL || host->suspended)
		return 0;

	for (i = 0; i < host->nr_slots; i++) {
		struct mmc_omap_slot *slot;

		slot = host->slots[i];
		ret = mmc_suspend_host(slot->mmc, mesg);
		if (ret < 0) {
			while (--i >= 0) {
				slot = host->slots[i];
				mmc_resume_host(slot->mmc);
			}
			return ret;
		}
	}
	host->suspended = 1;
	return 0;
}

static int mmc_omap_resume(struct platform_device *pdev)
{
	int i, ret = 0;
	struct mmc_omap_host *host = platform_get_drvdata(pdev);

	if (host == NULL || !host->suspended)
		return 0;

	for (i = 0; i < host->nr_slots; i++) {
		struct mmc_omap_slot *slot;
		slot = host->slots[i];
		ret = mmc_resume_host(slot->mmc);
		if (ret < 0)
			return ret;

		host->suspended = 0;
	}
	return 0;
}
#else
#define mmc_omap_suspend	NULL
#define mmc_omap_resume		NULL
#endif

static struct platform_driver mmc_omap_driver = {
	.probe		= mmc_omap_probe,
	.remove		= mmc_omap_remove,
	.suspend	= mmc_omap_suspend,
	.resume		= mmc_omap_resume,
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init mmc_omap_init(void)
{
	return platform_driver_register(&mmc_omap_driver);
}

static void __exit mmc_omap_exit(void)
{
	platform_driver_unregister(&mmc_omap_driver);
}

module_init(mmc_omap_init);
module_exit(mmc_omap_exit);

MODULE_DESCRIPTION("OMAP Multimedia Card driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRIVER_NAME);
MODULE_AUTHOR("Juha Yrj�l�");
