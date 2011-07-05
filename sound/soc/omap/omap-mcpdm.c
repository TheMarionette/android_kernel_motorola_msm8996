/*
 * omap-mcpdm.c  --  OMAP ALSA SoC DAI driver using McPDM port
 *
 * Copyright (C) 2009 - 2011 Texas Instruments
 *
 * Author: Misael Lopez Cruz <misael.lopez@ti.com>
 * Contact: Jorge Eduardo Candelaria <x0107209@ti.com>
 *          Margarita Olaya <magi.olaya@ti.com>
 *          Peter Ujfalusi <peter.ujfalusi@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <plat/dma.h>
#include <plat/omap_hwmod.h>
#include "omap-mcpdm.h"
#include "omap-pcm.h"

struct omap_mcpdm {
	struct device *dev;
	unsigned long phys_base;
	void __iomem *io_base;
	int irq;

	struct mutex mutex;

	/* channel data */
	u32 dn_channels;
	u32 up_channels;

	/* McPDM FIFO thresholds */
	u32 dn_threshold;
	u32 up_threshold;
};

/*
 * Stream DMA parameters
 */
static struct omap_pcm_dma_data omap_mcpdm_dai_dma_params[] = {
	{
		.name = "Audio playback",
		.dma_req = OMAP44XX_DMA_MCPDM_DL,
		.data_type = OMAP_DMA_DATA_TYPE_S32,
		.sync_mode = OMAP_DMA_SYNC_PACKET,
		.port_addr = OMAP44XX_MCPDM_L3_BASE + MCPDM_REG_DN_DATA,
	},
	{
		.name = "Audio capture",
		.dma_req = OMAP44XX_DMA_MCPDM_UP,
		.data_type = OMAP_DMA_DATA_TYPE_S32,
		.sync_mode = OMAP_DMA_SYNC_PACKET,
		.port_addr = OMAP44XX_MCPDM_L3_BASE + MCPDM_REG_UP_DATA,
	},
};

static inline void omap_mcpdm_write(struct omap_mcpdm *mcpdm, u16 reg, u32 val)
{
	__raw_writel(val, mcpdm->io_base + reg);
}

static inline int omap_mcpdm_read(struct omap_mcpdm *mcpdm, u16 reg)
{
	return __raw_readl(mcpdm->io_base + reg);
}

#ifdef DEBUG
static void omap_mcpdm_reg_dump(struct omap_mcpdm *mcpdm)
{
	dev_dbg(mcpdm->dev, "***********************\n");
	dev_dbg(mcpdm->dev, "IRQSTATUS_RAW:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_IRQSTATUS_RAW));
	dev_dbg(mcpdm->dev, "IRQSTATUS:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_IRQSTATUS));
	dev_dbg(mcpdm->dev, "IRQENABLE_SET:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_IRQENABLE_SET));
	dev_dbg(mcpdm->dev, "IRQENABLE_CLR:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_IRQENABLE_CLR));
	dev_dbg(mcpdm->dev, "IRQWAKE_EN: 0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_IRQWAKE_EN));
	dev_dbg(mcpdm->dev, "DMAENABLE_SET: 0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_DMAENABLE_SET));
	dev_dbg(mcpdm->dev, "DMAENABLE_CLR:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_DMAENABLE_CLR));
	dev_dbg(mcpdm->dev, "DMAWAKEEN:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_DMAWAKEEN));
	dev_dbg(mcpdm->dev, "CTRL:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_CTRL));
	dev_dbg(mcpdm->dev, "DN_DATA:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_DN_DATA));
	dev_dbg(mcpdm->dev, "UP_DATA: 0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_UP_DATA));
	dev_dbg(mcpdm->dev, "FIFO_CTRL_DN: 0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_FIFO_CTRL_DN));
	dev_dbg(mcpdm->dev, "FIFO_CTRL_UP:  0x%04x\n",
			omap_mcpdm_read(mcpdm, MCPDM_REG_FIFO_CTRL_UP));
	dev_dbg(mcpdm->dev, "***********************\n");
}
#else
static void omap_mcpdm_reg_dump(struct omap_mcpdm *mcpdm) {}
#endif

/*
 * Enables the transfer through the PDM interface to/from the Phoenix
 * codec by enabling the corresponding UP or DN channels.
 */
static void omap_mcpdm_start(struct omap_mcpdm *mcpdm)
{
	u32 ctrl = omap_mcpdm_read(mcpdm, MCPDM_REG_CTRL);

	ctrl |= (MCPDM_SW_DN_RST | MCPDM_SW_UP_RST);
	omap_mcpdm_write(mcpdm, MCPDM_REG_CTRL, ctrl);

	ctrl |= mcpdm->dn_channels | mcpdm->up_channels;
	omap_mcpdm_write(mcpdm, MCPDM_REG_CTRL, ctrl);

	ctrl &= ~(MCPDM_SW_DN_RST | MCPDM_SW_UP_RST);
	omap_mcpdm_write(mcpdm, MCPDM_REG_CTRL, ctrl);
}

/*
 * Disables the transfer through the PDM interface to/from the Phoenix
 * codec by disabling the corresponding UP or DN channels.
 */
static void omap_mcpdm_stop(struct omap_mcpdm *mcpdm)
{
	u32 ctrl = omap_mcpdm_read(mcpdm, MCPDM_REG_CTRL);

	ctrl |= (MCPDM_SW_DN_RST | MCPDM_SW_UP_RST);
	omap_mcpdm_write(mcpdm, MCPDM_REG_CTRL, ctrl);

	ctrl &= ~(mcpdm->dn_channels | mcpdm->up_channels);
	omap_mcpdm_write(mcpdm, MCPDM_REG_CTRL, ctrl);

	ctrl &= ~(MCPDM_SW_DN_RST | MCPDM_SW_UP_RST);
	omap_mcpdm_write(mcpdm, MCPDM_REG_CTRL, ctrl);

}

/*
 * Is the physical McPDM interface active.
 */
static inline int omap_mcpdm_active(struct omap_mcpdm *mcpdm)
{
	return omap_mcpdm_read(mcpdm, MCPDM_REG_CTRL) &
					(MCPDM_PDM_DN_MASK | MCPDM_PDM_UP_MASK);
}

/*
 * Configures McPDM uplink, and downlink for audio.
 * This function should be called before omap_mcpdm_start.
 */
static void omap_mcpdm_open_streams(struct omap_mcpdm *mcpdm)
{
	omap_mcpdm_write(mcpdm, MCPDM_REG_IRQENABLE_SET,
			MCPDM_DN_IRQ_EMPTY | MCPDM_DN_IRQ_FULL |
			MCPDM_UP_IRQ_EMPTY | MCPDM_UP_IRQ_FULL);

	omap_mcpdm_write(mcpdm, MCPDM_REG_FIFO_CTRL_DN, mcpdm->dn_threshold);
	omap_mcpdm_write(mcpdm, MCPDM_REG_FIFO_CTRL_UP, mcpdm->up_threshold);

	omap_mcpdm_write(mcpdm, MCPDM_REG_DMAENABLE_SET,
			MCPDM_DMA_DN_ENABLE | MCPDM_DMA_UP_ENABLE);
}

/*
 * Cleans McPDM uplink, and downlink configuration.
 * This function should be called when the stream is closed.
 */
static void omap_mcpdm_close_streams(struct omap_mcpdm *mcpdm)
{
	/* Disable irq request generation for downlink */
	omap_mcpdm_write(mcpdm, MCPDM_REG_IRQENABLE_CLR,
			MCPDM_DN_IRQ_EMPTY | MCPDM_DN_IRQ_FULL);

	/* Disable DMA request generation for downlink */
	omap_mcpdm_write(mcpdm, MCPDM_REG_DMAENABLE_CLR, MCPDM_DMA_DN_ENABLE);

	/* Disable irq request generation for uplink */
	omap_mcpdm_write(mcpdm, MCPDM_REG_IRQENABLE_CLR,
			MCPDM_UP_IRQ_EMPTY | MCPDM_UP_IRQ_FULL);

	/* Disable DMA request generation for uplink */
	omap_mcpdm_write(mcpdm, MCPDM_REG_DMAENABLE_CLR, MCPDM_DMA_UP_ENABLE);
}

static irqreturn_t omap_mcpdm_irq_handler(int irq, void *dev_id)
{
	struct omap_mcpdm *mcpdm = dev_id;
	int irq_status;

	irq_status = omap_mcpdm_read(mcpdm, MCPDM_REG_IRQSTATUS);

	/* Acknowledge irq event */
	omap_mcpdm_write(mcpdm, MCPDM_REG_IRQSTATUS, irq_status);

	if (irq_status & MCPDM_DN_IRQ_FULL)
		dev_dbg(mcpdm->dev, "DN (playback) FIFO Full\n");

	if (irq_status & MCPDM_DN_IRQ_EMPTY)
		dev_dbg(mcpdm->dev, "DN (playback) FIFO Empty\n");

	if (irq_status & MCPDM_DN_IRQ)
		dev_dbg(mcpdm->dev, "DN (playback) write request\n");

	if (irq_status & MCPDM_UP_IRQ_FULL)
		dev_dbg(mcpdm->dev, "UP (capture) FIFO Full\n");

	if (irq_status & MCPDM_UP_IRQ_EMPTY)
		dev_dbg(mcpdm->dev, "UP (capture) FIFO Empty\n");

	if (irq_status & MCPDM_UP_IRQ)
		dev_dbg(mcpdm->dev, "UP (capture) write request\n");

	return IRQ_HANDLED;
}

static int omap_mcpdm_dai_startup(struct snd_pcm_substream *substream,
				  struct snd_soc_dai *dai)
{
	struct omap_mcpdm *mcpdm = snd_soc_dai_get_drvdata(dai);

	mutex_lock(&mcpdm->mutex);

	if (!dai->active) {
		pm_runtime_get_sync(mcpdm->dev);

		/* Enable watch dog for ES above ES 1.0 to avoid saturation */
		if (omap_rev() != OMAP4430_REV_ES1_0) {
			u32 ctrl = omap_mcpdm_read(mcpdm, MCPDM_REG_CTRL);

			omap_mcpdm_write(mcpdm, MCPDM_REG_CTRL,
					 ctrl | MCPDM_WD_EN);
		}
		omap_mcpdm_open_streams(mcpdm);
	}

	mutex_unlock(&mcpdm->mutex);

	return 0;
}

static void omap_mcpdm_dai_shutdown(struct snd_pcm_substream *substream,
				  struct snd_soc_dai *dai)
{
	struct omap_mcpdm *mcpdm = snd_soc_dai_get_drvdata(dai);

	mutex_lock(&mcpdm->mutex);

	if (!dai->active) {
		if (omap_mcpdm_active(mcpdm)) {
			omap_mcpdm_stop(mcpdm);
			omap_mcpdm_close_streams(mcpdm);
		}

		if (!omap_mcpdm_active(mcpdm))
			pm_runtime_put_sync(mcpdm->dev);
	}

	mutex_unlock(&mcpdm->mutex);
}

static int omap_mcpdm_dai_hw_params(struct snd_pcm_substream *substream,
				    struct snd_pcm_hw_params *params,
				    struct snd_soc_dai *dai)
{
	struct omap_mcpdm *mcpdm = snd_soc_dai_get_drvdata(dai);
	int stream = substream->stream;
	struct omap_pcm_dma_data *dma_data;
	int channels;
	int link_mask = 0;

	channels = params_channels(params);
	switch (channels) {
	case 4:
		if (stream == SNDRV_PCM_STREAM_CAPTURE)
			/* up to 2 channels for capture */
			return -EINVAL;
		link_mask |= 1 << 3;
	case 3:
		if (stream == SNDRV_PCM_STREAM_CAPTURE)
			/* up to 2 channels for capture */
			return -EINVAL;
		link_mask |= 1 << 2;
	case 2:
		link_mask |= 1 << 1;
	case 1:
		link_mask |= 1 << 0;
		break;
	default:
		/* unsupported number of channels */
		return -EINVAL;
	}

	dma_data = &omap_mcpdm_dai_dma_params[stream];

	/* Configure McPDM channels, and DMA packet size */
	if (stream == SNDRV_PCM_STREAM_PLAYBACK) {
		mcpdm->dn_channels = link_mask << 3;
		dma_data->packet_size =
			(MCPDM_DN_THRES_MAX - mcpdm->dn_threshold) * channels;
	} else {
		mcpdm->up_channels = link_mask << 0;
		dma_data->packet_size = mcpdm->up_threshold * channels;
	}

	snd_soc_dai_set_dma_data(dai, substream, dma_data);

	return 0;
}

static int omap_mcpdm_prepare(struct snd_pcm_substream *substream,
				  struct snd_soc_dai *dai)
{
	struct omap_mcpdm *mcpdm = snd_soc_dai_get_drvdata(dai);

	if (!omap_mcpdm_active(mcpdm)) {
		omap_mcpdm_start(mcpdm);
		omap_mcpdm_reg_dump(mcpdm);
	}

	return 0;
}

static struct snd_soc_dai_ops omap_mcpdm_dai_ops = {
	.startup	= omap_mcpdm_dai_startup,
	.shutdown	= omap_mcpdm_dai_shutdown,
	.hw_params	= omap_mcpdm_dai_hw_params,
	.prepare	= omap_mcpdm_prepare,
};

static int omap_mcpdm_probe(struct snd_soc_dai *dai)
{
	struct omap_mcpdm *mcpdm = snd_soc_dai_get_drvdata(dai);
	int ret;

	pm_runtime_enable(mcpdm->dev);

	/* Disable lines while request is ongoing */
	pm_runtime_get_sync(mcpdm->dev);
	omap_mcpdm_write(mcpdm, MCPDM_REG_CTRL, 0x00);

	ret = request_irq(mcpdm->irq, omap_mcpdm_irq_handler,
				0, "McPDM", (void *)mcpdm);

	pm_runtime_put_sync(mcpdm->dev);

	if (ret) {
		dev_err(mcpdm->dev, "Request for IRQ failed\n");
		pm_runtime_disable(mcpdm->dev);
	}

	/* Configure McPDM threshold values */
	mcpdm->dn_threshold = 2;
	mcpdm->up_threshold = MCPDM_UP_THRES_MAX - 3;
	return ret;
}

static int omap_mcpdm_remove(struct snd_soc_dai *dai)
{
	struct omap_mcpdm *mcpdm = snd_soc_dai_get_drvdata(dai);

	free_irq(mcpdm->irq, (void *)mcpdm);
	pm_runtime_disable(mcpdm->dev);

	return 0;
}

#define OMAP_MCPDM_RATES	(SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)
#define OMAP_MCPDM_FORMATS	SNDRV_PCM_FMTBIT_S32_LE

static struct snd_soc_dai_driver omap_mcpdm_dai = {
	.probe = omap_mcpdm_probe,
	.remove = omap_mcpdm_remove,
	.probe_order = SND_SOC_COMP_ORDER_LATE,
	.remove_order = SND_SOC_COMP_ORDER_EARLY,
	.playback = {
		.channels_min = 1,
		.channels_max = 4,
		.rates = OMAP_MCPDM_RATES,
		.formats = OMAP_MCPDM_FORMATS,
	},
	.capture = {
		.channels_min = 1,
		.channels_max = 2,
		.rates = OMAP_MCPDM_RATES,
		.formats = OMAP_MCPDM_FORMATS,
	},
	.ops = &omap_mcpdm_dai_ops,
};

static __devinit int asoc_mcpdm_probe(struct platform_device *pdev)
{
	struct omap_mcpdm *mcpdm;
	struct resource *res;
	int ret = 0;

	mcpdm = kzalloc(sizeof(struct omap_mcpdm), GFP_KERNEL);
	if (!mcpdm)
		return -ENOMEM;

	platform_set_drvdata(pdev, mcpdm);

	mutex_init(&mcpdm->mutex);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "no resource\n");
		goto err_res;
	}

	if (!request_mem_region(res->start, resource_size(res), "McPDM")) {
		ret = -EBUSY;
		goto err_res;
	}

	mcpdm->io_base = ioremap(res->start, resource_size(res));
	if (!mcpdm->io_base) {
		ret = -ENOMEM;
		goto err_iomap;
	}

	mcpdm->irq = platform_get_irq(pdev, 0);
	if (mcpdm->irq < 0) {
		ret = mcpdm->irq;
		goto err_irq;
	}

	mcpdm->dev = &pdev->dev;

	ret = snd_soc_register_dai(&pdev->dev, &omap_mcpdm_dai);
	if (!ret)
		return 0;

err_irq:
	iounmap(mcpdm->io_base);
err_iomap:
	release_mem_region(res->start, resource_size(res));
err_res:
	kfree(mcpdm);
	return ret;
}

static int __devexit asoc_mcpdm_remove(struct platform_device *pdev)
{
	struct omap_mcpdm *mcpdm = platform_get_drvdata(pdev);
	struct resource *res;

	snd_soc_unregister_dai(&pdev->dev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	iounmap(mcpdm->io_base);
	release_mem_region(res->start, resource_size(res));

	kfree(mcpdm);
	return 0;
}

static struct platform_driver asoc_mcpdm_driver = {
	.driver = {
		.name	= "omap-mcpdm",
		.owner	= THIS_MODULE,
	},

	.probe	= asoc_mcpdm_probe,
	.remove	= __devexit_p(asoc_mcpdm_remove),
};

static int __init snd_omap_mcpdm_init(void)
{
	return platform_driver_register(&asoc_mcpdm_driver);
}
module_init(snd_omap_mcpdm_init);

static void __exit snd_omap_mcpdm_exit(void)
{
	platform_driver_unregister(&asoc_mcpdm_driver);
}
module_exit(snd_omap_mcpdm_exit);

MODULE_AUTHOR("Misael Lopez Cruz <misael.lopez@ti.com>");
MODULE_DESCRIPTION("OMAP PDM SoC Interface");
MODULE_LICENSE("GPL");
