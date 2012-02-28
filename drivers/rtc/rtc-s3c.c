/* drivers/rtc/rtc-s3c.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * Copyright (c) 2004,2006 Simtec Electronics
 *	Ben Dooks, <ben@simtec.co.uk>
 *	http://armlinux.simtec.co.uk/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * S3C2410/S3C2440/S3C24XX Internal RTC Driver
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/clk.h>
#include <linux/log2.h>
#include <linux/slab.h>
#include <linux/of.h>

#include <mach/hardware.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <plat/regs-rtc.h>

enum s3c_cpu_type {
	TYPE_S3C2410,
	TYPE_S3C64XX,
};

/* I have yet to find an S3C implementation with more than one
 * of these rtc blocks in */

static struct resource *s3c_rtc_mem;

static struct clk *rtc_clk;
static void __iomem *s3c_rtc_base;
static int s3c_rtc_alarmno = NO_IRQ;
static int s3c_rtc_tickno  = NO_IRQ;
static bool wake_en;
static enum s3c_cpu_type s3c_rtc_cpu_type;

static DEFINE_SPINLOCK(s3c_rtc_pie_lock);

static void s3c_rtc_alarm_clk_enable(bool enable)
{
	static DEFINE_SPINLOCK(s3c_rtc_alarm_clk_lock);
	static bool alarm_clk_enabled;
	unsigned long irq_flags;

	spin_lock_irqsave(&s3c_rtc_alarm_clk_lock, irq_flags);
	if (enable) {
		if (!alarm_clk_enabled) {
			clk_enable(rtc_clk);
			alarm_clk_enabled = true;
		}
	} else {
		if (alarm_clk_enabled) {
			clk_disable(rtc_clk);
			alarm_clk_enabled = false;
		}
	}
	spin_unlock_irqrestore(&s3c_rtc_alarm_clk_lock, irq_flags);
}

/* IRQ Handlers */

static irqreturn_t s3c_rtc_alarmirq(int irq, void *id)
{
	struct rtc_device *rdev = id;

	clk_enable(rtc_clk);
	rtc_update_irq(rdev, 1, RTC_AF | RTC_IRQF);

	if (s3c_rtc_cpu_type == TYPE_S3C64XX)
		writeb(S3C2410_INTP_ALM, s3c_rtc_base + S3C2410_INTP);

	clk_disable(rtc_clk);

	s3c_rtc_alarm_clk_enable(false);

	return IRQ_HANDLED;
}

static irqreturn_t s3c_rtc_tickirq(int irq, void *id)
{
	struct rtc_device *rdev = id;

	clk_enable(rtc_clk);
	rtc_update_irq(rdev, 1, RTC_PF | RTC_IRQF);

	if (s3c_rtc_cpu_type == TYPE_S3C64XX)
		writeb(S3C2410_INTP_TIC, s3c_rtc_base + S3C2410_INTP);

	clk_disable(rtc_clk);
	return IRQ_HANDLED;
}

/* Update control registers */
static int s3c_rtc_setaie(struct device *dev, unsigned int enabled)
{
	unsigned int tmp;

	pr_debug("%s: aie=%d\n", __func__, enabled);

	clk_enable(rtc_clk);
	tmp = readb(s3c_rtc_base + S3C2410_RTCALM) & ~S3C2410_RTCALM_ALMEN;

	if (enabled)
		tmp |= S3C2410_RTCALM_ALMEN;

	writeb(tmp, s3c_rtc_base + S3C2410_RTCALM);
	clk_disable(rtc_clk);

	s3c_rtc_alarm_clk_enable(enabled);

	return 0;
}

static int s3c_rtc_setfreq(struct device *dev, int freq)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct rtc_device *rtc_dev = platform_get_drvdata(pdev);
	unsigned int tmp = 0;

	if (!is_power_of_2(freq))
		return -EINVAL;

	clk_enable(rtc_clk);
	spin_lock_irq(&s3c_rtc_pie_lock);

	if (s3c_rtc_cpu_type == TYPE_S3C2410) {
		tmp = readb(s3c_rtc_base + S3C2410_TICNT);
		tmp &= S3C2410_TICNT_ENABLE;
	}

	tmp |= (rtc_dev->max_user_freq / freq)-1;

	writel(tmp, s3c_rtc_base + S3C2410_TICNT);
	spin_unlock_irq(&s3c_rtc_pie_lock);
	clk_disable(rtc_clk);

	return 0;
}

/* Time read/write */

static int s3c_rtc_gettime(struct device *dev, struct rtc_time *rtc_tm)
{
	unsigned int have_retried = 0;
	void __iomem *base = s3c_rtc_base;

	clk_enable(rtc_clk);
 retry_get_time:
	rtc_tm->tm_min  = readb(base + S3C2410_RTCMIN);
	rtc_tm->tm_hour = readb(base + S3C2410_RTCHOUR);
	rtc_tm->tm_mday = readb(base + S3C2410_RTCDATE);
	rtc_tm->tm_mon  = readb(base + S3C2410_RTCMON);
	rtc_tm->tm_year = readb(base + S3C2410_RTCYEAR);
	rtc_tm->tm_sec  = readb(base + S3C2410_RTCSEC);

	/* the only way to work out wether the system was mid-update
	 * when we read it is to check the second counter, and if it
	 * is zero, then we re-try the entire read
	 */

	if (rtc_tm->tm_sec == 0 && !have_retried) {
		have_retried = 1;
		goto retry_get_time;
	}

	rtc_tm->tm_sec = bcd2bin(rtc_tm->tm_sec);
	rtc_tm->tm_min = bcd2bin(rtc_tm->tm_min);
	rtc_tm->tm_hour = bcd2bin(rtc_tm->tm_hour);
	rtc_tm->tm_mday = bcd2bin(rtc_tm->tm_mday);
	rtc_tm->tm_mon = bcd2bin(rtc_tm->tm_mon);
	rtc_tm->tm_year = bcd2bin(rtc_tm->tm_year);

	rtc_tm->tm_year += 100;

	pr_debug("read time %04d.%02d.%02d %02d:%02d:%02d\n",
		 1900 + rtc_tm->tm_year, rtc_tm->tm_mon, rtc_tm->tm_mday,
		 rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);

	rtc_tm->tm_mon -= 1;

	clk_disable(rtc_clk);
	return rtc_valid_tm(rtc_tm);
}

static int s3c_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	void __iomem *base = s3c_rtc_base;
	int year = tm->tm_year - 100;

	pr_debug("set time %04d.%02d.%02d %02d:%02d:%02d\n",
		 1900 + tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec);

	/* we get around y2k by simply not supporting it */

	if (year < 0 || year >= 100) {
		dev_err(dev, "rtc only supports 100 years\n");
		return -EINVAL;
	}

	clk_enable(rtc_clk);
	writeb(bin2bcd(tm->tm_sec),  base + S3C2410_RTCSEC);
	writeb(bin2bcd(tm->tm_min),  base + S3C2410_RTCMIN);
	writeb(bin2bcd(tm->tm_hour), base + S3C2410_RTCHOUR);
	writeb(bin2bcd(tm->tm_mday), base + S3C2410_RTCDATE);
	writeb(bin2bcd(tm->tm_mon + 1), base + S3C2410_RTCMON);
	writeb(bin2bcd(year), base + S3C2410_RTCYEAR);
	clk_disable(rtc_clk);

	return 0;
}

static int s3c_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_time *alm_tm = &alrm->time;
	void __iomem *base = s3c_rtc_base;
	unsigned int alm_en;

	clk_enable(rtc_clk);
	alm_tm->tm_sec  = readb(base + S3C2410_ALMSEC);
	alm_tm->tm_min  = readb(base + S3C2410_ALMMIN);
	alm_tm->tm_hour = readb(base + S3C2410_ALMHOUR);
	alm_tm->tm_mon  = readb(base + S3C2410_ALMMON);
	alm_tm->tm_mday = readb(base + S3C2410_ALMDATE);
	alm_tm->tm_year = readb(base + S3C2410_ALMYEAR);

	alm_en = readb(base + S3C2410_RTCALM);

	alrm->enabled = (alm_en & S3C2410_RTCALM_ALMEN) ? 1 : 0;

	pr_debug("read alarm %d, %04d.%02d.%02d %02d:%02d:%02d\n",
		 alm_en,
		 1900 + alm_tm->tm_year, alm_tm->tm_mon, alm_tm->tm_mday,
		 alm_tm->tm_hour, alm_tm->tm_min, alm_tm->tm_sec);


	/* decode the alarm enable field */

	if (alm_en & S3C2410_RTCALM_SECEN)
		alm_tm->tm_sec = bcd2bin(alm_tm->tm_sec);
	else
		alm_tm->tm_sec = -1;

	if (alm_en & S3C2410_RTCALM_MINEN)
		alm_tm->tm_min = bcd2bin(alm_tm->tm_min);
	else
		alm_tm->tm_min = -1;

	if (alm_en & S3C2410_RTCALM_HOUREN)
		alm_tm->tm_hour = bcd2bin(alm_tm->tm_hour);
	else
		alm_tm->tm_hour = -1;

	if (alm_en & S3C2410_RTCALM_DAYEN)
		alm_tm->tm_mday = bcd2bin(alm_tm->tm_mday);
	else
		alm_tm->tm_mday = -1;

	if (alm_en & S3C2410_RTCALM_MONEN) {
		alm_tm->tm_mon = bcd2bin(alm_tm->tm_mon);
		alm_tm->tm_mon -= 1;
	} else {
		alm_tm->tm_mon = -1;
	}

	if (alm_en & S3C2410_RTCALM_YEAREN)
		alm_tm->tm_year = bcd2bin(alm_tm->tm_year);
	else
		alm_tm->tm_year = -1;

	clk_disable(rtc_clk);
	return 0;
}

static int s3c_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct rtc_time *tm = &alrm->time;
	void __iomem *base = s3c_rtc_base;
	unsigned int alrm_en;

	clk_enable(rtc_clk);
	pr_debug("s3c_rtc_setalarm: %d, %04d.%02d.%02d %02d:%02d:%02d\n",
		 alrm->enabled,
		 1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec);

	alrm_en = readb(base + S3C2410_RTCALM) & S3C2410_RTCALM_ALMEN;
	writeb(0x00, base + S3C2410_RTCALM);

	if (tm->tm_sec < 60 && tm->tm_sec >= 0) {
		alrm_en |= S3C2410_RTCALM_SECEN;
		writeb(bin2bcd(tm->tm_sec), base + S3C2410_ALMSEC);
	}

	if (tm->tm_min < 60 && tm->tm_min >= 0) {
		alrm_en |= S3C2410_RTCALM_MINEN;
		writeb(bin2bcd(tm->tm_min), base + S3C2410_ALMMIN);
	}

	if (tm->tm_hour < 24 && tm->tm_hour >= 0) {
		alrm_en |= S3C2410_RTCALM_HOUREN;
		writeb(bin2bcd(tm->tm_hour), base + S3C2410_ALMHOUR);
	}

	pr_debug("setting S3C2410_RTCALM to %08x\n", alrm_en);

	writeb(alrm_en, base + S3C2410_RTCALM);

	s3c_rtc_setaie(dev, alrm->enabled);

	clk_disable(rtc_clk);
	return 0;
}

static int s3c_rtc_proc(struct device *dev, struct seq_file *seq)
{
	unsigned int ticnt;

	clk_enable(rtc_clk);
	if (s3c_rtc_cpu_type == TYPE_S3C64XX) {
		ticnt = readw(s3c_rtc_base + S3C2410_RTCCON);
		ticnt &= S3C64XX_RTCCON_TICEN;
	} else {
		ticnt = readb(s3c_rtc_base + S3C2410_TICNT);
		ticnt &= S3C2410_TICNT_ENABLE;
	}

	seq_printf(seq, "periodic_IRQ\t: %s\n", ticnt  ? "yes" : "no");
	clk_disable(rtc_clk);
	return 0;
}

static const struct rtc_class_ops s3c_rtcops = {
	.read_time	= s3c_rtc_gettime,
	.set_time	= s3c_rtc_settime,
	.read_alarm	= s3c_rtc_getalarm,
	.set_alarm	= s3c_rtc_setalarm,
	.proc		= s3c_rtc_proc,
	.alarm_irq_enable = s3c_rtc_setaie,
};

static void s3c_rtc_enable(struct platform_device *pdev, int en)
{
	void __iomem *base = s3c_rtc_base;
	unsigned int tmp;

	if (s3c_rtc_base == NULL)
		return;

	clk_enable(rtc_clk);
	if (!en) {
		tmp = readw(base + S3C2410_RTCCON);
		if (s3c_rtc_cpu_type == TYPE_S3C64XX)
			tmp &= ~S3C64XX_RTCCON_TICEN;
		tmp &= ~S3C2410_RTCCON_RTCEN;
		writew(tmp, base + S3C2410_RTCCON);

		if (s3c_rtc_cpu_type == TYPE_S3C2410) {
			tmp = readb(base + S3C2410_TICNT);
			tmp &= ~S3C2410_TICNT_ENABLE;
			writeb(tmp, base + S3C2410_TICNT);
		}
	} else {
		/* re-enable the device, and check it is ok */

		if ((readw(base+S3C2410_RTCCON) & S3C2410_RTCCON_RTCEN) == 0) {
			dev_info(&pdev->dev, "rtc disabled, re-enabling\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp | S3C2410_RTCCON_RTCEN,
				base + S3C2410_RTCCON);
		}

		if ((readw(base + S3C2410_RTCCON) & S3C2410_RTCCON_CNTSEL)) {
			dev_info(&pdev->dev, "removing RTCCON_CNTSEL\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp & ~S3C2410_RTCCON_CNTSEL,
				base + S3C2410_RTCCON);
		}

		if ((readw(base + S3C2410_RTCCON) & S3C2410_RTCCON_CLKRST)) {
			dev_info(&pdev->dev, "removing RTCCON_CLKRST\n");

			tmp = readw(base + S3C2410_RTCCON);
			writew(tmp & ~S3C2410_RTCCON_CLKRST,
				base + S3C2410_RTCCON);
		}
	}
	clk_disable(rtc_clk);
}

static int __devexit s3c_rtc_remove(struct platform_device *dev)
{
	struct rtc_device *rtc = platform_get_drvdata(dev);

	free_irq(s3c_rtc_alarmno, rtc);
	free_irq(s3c_rtc_tickno, rtc);

	platform_set_drvdata(dev, NULL);
	rtc_device_unregister(rtc);

	s3c_rtc_setaie(&dev->dev, 0);

	clk_put(rtc_clk);
	rtc_clk = NULL;

	iounmap(s3c_rtc_base);
	release_resource(s3c_rtc_mem);
	kfree(s3c_rtc_mem);

	return 0;
}

static int __devinit s3c_rtc_probe(struct platform_device *pdev)
{
	struct rtc_device *rtc;
	struct rtc_time rtc_tm;
	struct resource *res;
	int ret;

	pr_debug("%s: probe=%p\n", __func__, pdev);

	/* find the IRQs */

	s3c_rtc_tickno = platform_get_irq(pdev, 1);
	if (s3c_rtc_tickno < 0) {
		dev_err(&pdev->dev, "no irq for rtc tick\n");
		return -ENOENT;
	}

	s3c_rtc_alarmno = platform_get_irq(pdev, 0);
	if (s3c_rtc_alarmno < 0) {
		dev_err(&pdev->dev, "no irq for alarm\n");
		return -ENOENT;
	}

	pr_debug("s3c2410_rtc: tick irq %d, alarm irq %d\n",
		 s3c_rtc_tickno, s3c_rtc_alarmno);

	/* get the memory region */

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "failed to get memory region resource\n");
		return -ENOENT;
	}

	s3c_rtc_mem = request_mem_region(res->start, resource_size(res),
					 pdev->name);

	if (s3c_rtc_mem == NULL) {
		dev_err(&pdev->dev, "failed to reserve memory region\n");
		ret = -ENOENT;
		goto err_nores;
	}

	s3c_rtc_base = ioremap(res->start, resource_size(res));
	if (s3c_rtc_base == NULL) {
		dev_err(&pdev->dev, "failed ioremap()\n");
		ret = -EINVAL;
		goto err_nomap;
	}

	rtc_clk = clk_get(&pdev->dev, "rtc");
	if (IS_ERR(rtc_clk)) {
		dev_err(&pdev->dev, "failed to find rtc clock source\n");
		ret = PTR_ERR(rtc_clk);
		rtc_clk = NULL;
		goto err_clk;
	}

	clk_enable(rtc_clk);

	/* check to see if everything is setup correctly */

	s3c_rtc_enable(pdev, 1);

	pr_debug("s3c2410_rtc: RTCCON=%02x\n",
		 readw(s3c_rtc_base + S3C2410_RTCCON));

	device_init_wakeup(&pdev->dev, 1);

	/* register RTC and exit */

	rtc = rtc_device_register("s3c", &pdev->dev, &s3c_rtcops,
				  THIS_MODULE);

	if (IS_ERR(rtc)) {
		dev_err(&pdev->dev, "cannot attach rtc\n");
		ret = PTR_ERR(rtc);
		goto err_nortc;
	}

#ifdef CONFIG_OF
	if (pdev->dev.of_node)
		s3c_rtc_cpu_type = of_device_is_compatible(pdev->dev.of_node,
			"samsung,s3c6410-rtc") ? TYPE_S3C64XX : TYPE_S3C2410;
	else
#endif
		s3c_rtc_cpu_type = platform_get_device_id(pdev)->driver_data;

	/* Check RTC Time */

	s3c_rtc_gettime(NULL, &rtc_tm);

	if (rtc_valid_tm(&rtc_tm)) {
		rtc_tm.tm_year	= 100;
		rtc_tm.tm_mon	= 0;
		rtc_tm.tm_mday	= 1;
		rtc_tm.tm_hour	= 0;
		rtc_tm.tm_min	= 0;
		rtc_tm.tm_sec	= 0;

		s3c_rtc_settime(NULL, &rtc_tm);

		dev_warn(&pdev->dev, "warning: invalid RTC value so initializing it\n");
	}

	if (s3c_rtc_cpu_type == TYPE_S3C64XX)
		rtc->max_user_freq = 32768;
	else
		rtc->max_user_freq = 128;

	platform_set_drvdata(pdev, rtc);

	s3c_rtc_setfreq(&pdev->dev, 1);

	ret = request_irq(s3c_rtc_alarmno, s3c_rtc_alarmirq,
			  IRQF_DISABLED,  "s3c2410-rtc alarm", rtc);
	if (ret) {
		dev_err(&pdev->dev, "IRQ%d error %d\n", s3c_rtc_alarmno, ret);
		goto err_alarm_irq;
	}

	ret = request_irq(s3c_rtc_tickno, s3c_rtc_tickirq,
			  IRQF_DISABLED,  "s3c2410-rtc tick", rtc);
	if (ret) {
		dev_err(&pdev->dev, "IRQ%d error %d\n", s3c_rtc_tickno, ret);
		free_irq(s3c_rtc_alarmno, rtc);
		goto err_tick_irq;
	}

	clk_disable(rtc_clk);

	return 0;

 err_tick_irq:
	free_irq(s3c_rtc_alarmno, rtc);

 err_alarm_irq:
	platform_set_drvdata(pdev, NULL);
	rtc_device_unregister(rtc);

 err_nortc:
	s3c_rtc_enable(pdev, 0);
	clk_disable(rtc_clk);
	clk_put(rtc_clk);

 err_clk:
	iounmap(s3c_rtc_base);

 err_nomap:
	release_resource(s3c_rtc_mem);

 err_nores:
	return ret;
}

#ifdef CONFIG_PM

/* RTC Power management control */

static int ticnt_save, ticnt_en_save;

static int s3c_rtc_suspend(struct platform_device *pdev, pm_message_t state)
{
	clk_enable(rtc_clk);
	/* save TICNT for anyone using periodic interrupts */
	ticnt_save = readb(s3c_rtc_base + S3C2410_TICNT);
	if (s3c_rtc_cpu_type == TYPE_S3C64XX) {
		ticnt_en_save = readw(s3c_rtc_base + S3C2410_RTCCON);
		ticnt_en_save &= S3C64XX_RTCCON_TICEN;
	}
	s3c_rtc_enable(pdev, 0);

	if (device_may_wakeup(&pdev->dev) && !wake_en) {
		if (enable_irq_wake(s3c_rtc_alarmno) == 0)
			wake_en = true;
		else
			dev_err(&pdev->dev, "enable_irq_wake failed\n");
	}
	clk_disable(rtc_clk);

	return 0;
}

static int s3c_rtc_resume(struct platform_device *pdev)
{
	unsigned int tmp;

	clk_enable(rtc_clk);
	s3c_rtc_enable(pdev, 1);
	writeb(ticnt_save, s3c_rtc_base + S3C2410_TICNT);
	if (s3c_rtc_cpu_type == TYPE_S3C64XX && ticnt_en_save) {
		tmp = readw(s3c_rtc_base + S3C2410_RTCCON);
		writew(tmp | ticnt_en_save, s3c_rtc_base + S3C2410_RTCCON);
	}

	if (device_may_wakeup(&pdev->dev) && wake_en) {
		disable_irq_wake(s3c_rtc_alarmno);
		wake_en = false;
	}
	clk_disable(rtc_clk);

	return 0;
}
#else
#define s3c_rtc_suspend NULL
#define s3c_rtc_resume  NULL
#endif

#ifdef CONFIG_OF
static const struct of_device_id s3c_rtc_dt_match[] = {
	{ .compatible = "samsung,s3c2410-rtc" },
	{ .compatible = "samsung,s3c6410-rtc" },
	{},
};
MODULE_DEVICE_TABLE(of, s3c_rtc_dt_match);
#else
#define s3c_rtc_dt_match NULL
#endif

static struct platform_device_id s3c_rtc_driver_ids[] = {
	{
		.name		= "s3c2410-rtc",
		.driver_data	= TYPE_S3C2410,
	}, {
		.name		= "s3c64xx-rtc",
		.driver_data	= TYPE_S3C64XX,
	},
	{ }
};

MODULE_DEVICE_TABLE(platform, s3c_rtc_driver_ids);

static struct platform_driver s3c_rtc_driver = {
	.probe		= s3c_rtc_probe,
	.remove		= __devexit_p(s3c_rtc_remove),
	.suspend	= s3c_rtc_suspend,
	.resume		= s3c_rtc_resume,
	.id_table	= s3c_rtc_driver_ids,
	.driver		= {
		.name	= "s3c-rtc",
		.owner	= THIS_MODULE,
		.of_match_table	= s3c_rtc_dt_match,
	},
};

module_platform_driver(s3c_rtc_driver);

MODULE_DESCRIPTION("Samsung S3C RTC Driver");
MODULE_AUTHOR("Ben Dooks <ben@simtec.co.uk>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:s3c2410-rtc");
