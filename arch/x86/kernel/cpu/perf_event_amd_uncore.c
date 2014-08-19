/*
 * Copyright (C) 2013 Advanced Micro Devices, Inc.
 *
 * Author: Jacob Shin <jacob.shin@amd.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/perf_event.h>
#include <linux/percpu.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>

#include <asm/cpufeature.h>
#include <asm/perf_event.h>
#include <asm/msr.h>

#define NUM_COUNTERS_NB		4
#define NUM_COUNTERS_L2		4
#define MAX_COUNTERS		NUM_COUNTERS_NB

#define RDPMC_BASE_NB		6
#define RDPMC_BASE_L2		10

#define COUNTER_SHIFT		16

struct amd_uncore {
	int id;
	int refcnt;
	int cpu;
	int num_counters;
	int rdpmc_base;
	u32 msr_base;
	cpumask_t *active_mask;
	struct pmu *pmu;
	struct perf_event *events[MAX_COUNTERS];
	struct amd_uncore *free_when_cpu_online;
};

static struct amd_uncore * __percpu *amd_uncore_nb;
static struct amd_uncore * __percpu *amd_uncore_l2;

static struct pmu amd_nb_pmu;
static struct pmu amd_l2_pmu;

static cpumask_t amd_nb_active_mask;
static cpumask_t amd_l2_active_mask;

static bool is_nb_event(struct perf_event *event)
{
	return event->pmu->type == amd_nb_pmu.type;
}

static bool is_l2_event(struct perf_event *event)
{
	return event->pmu->type == amd_l2_pmu.type;
}

static struct amd_uncore *event_to_amd_uncore(struct perf_event *event)
{
	if (is_nb_event(event) && amd_uncore_nb)
		return *per_cpu_ptr(amd_uncore_nb, event->cpu);
	else if (is_l2_event(event) && amd_uncore_l2)
		return *per_cpu_ptr(amd_uncore_l2, event->cpu);

	return NULL;
}

static void amd_uncore_read(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 prev, new;
	s64 delta;

	/*
	 * since we do not enable counter overflow interrupts,
	 * we do not have to worry about prev_count changing on us
	 */

	prev = local64_read(&hwc->prev_count);
	rdpmcl(hwc->event_base_rdpmc, new);
	local64_set(&hwc->prev_count, new);
	delta = (new << COUNTER_SHIFT) - (prev << COUNTER_SHIFT);
	delta >>= COUNTER_SHIFT;
	local64_add(delta, &event->count);
}

static void amd_uncore_start(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;

	if (flags & PERF_EF_RELOAD)
		wrmsrl(hwc->event_base, (u64)local64_read(&hwc->prev_count));

	hwc->state = 0;
	wrmsrl(hwc->config_base, (hwc->config | ARCH_PERFMON_EVENTSEL_ENABLE));
	perf_event_update_userpage(event);
}

static void amd_uncore_stop(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;

	wrmsrl(hwc->config_base, hwc->config);
	hwc->state |= PERF_HES_STOPPED;

	if ((flags & PERF_EF_UPDATE) && !(hwc->state & PERF_HES_UPTODATE)) {
		amd_uncore_read(event);
		hwc->state |= PERF_HES_UPTODATE;
	}
}

static int amd_uncore_add(struct perf_event *event, int flags)
{
	int i;
	struct amd_uncore *uncore = event_to_amd_uncore(event);
	struct hw_perf_event *hwc = &event->hw;

	/* are we already assigned? */
	if (hwc->idx != -1 && uncore->events[hwc->idx] == event)
		goto out;

	for (i = 0; i < uncore->num_counters; i++) {
		if (uncore->events[i] == event) {
			hwc->idx = i;
			goto out;
		}
	}

	/* if not, take the first available counter */
	hwc->idx = -1;
	for (i = 0; i < uncore->num_counters; i++) {
		if (cmpxchg(&uncore->events[i], NULL, event) == NULL) {
			hwc->idx = i;
			break;
		}
	}

out:
	if (hwc->idx == -1)
		return -EBUSY;

	hwc->config_base = uncore->msr_base + (2 * hwc->idx);
	hwc->event_base = uncore->msr_base + 1 + (2 * hwc->idx);
	hwc->event_base_rdpmc = uncore->rdpmc_base + hwc->idx;
	hwc->state = PERF_HES_UPTODATE | PERF_HES_STOPPED;

	if (flags & PERF_EF_START)
		amd_uncore_start(event, PERF_EF_RELOAD);

	return 0;
}

static void amd_uncore_del(struct perf_event *event, int flags)
{
	int i;
	struct amd_uncore *uncore = event_to_amd_uncore(event);
	struct hw_perf_event *hwc = &event->hw;

	amd_uncore_stop(event, PERF_EF_UPDATE);

	for (i = 0; i < uncore->num_counters; i++) {
		if (cmpxchg(&uncore->events[i], event, NULL) == event)
			break;
	}

	hwc->idx = -1;
}

static int amd_uncore_event_init(struct perf_event *event)
{
	struct amd_uncore *uncore;
	struct hw_perf_event *hwc = &event->hw;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	/*
	 * NB and L2 counters (MSRs) are shared across all cores that share the
	 * same NB / L2 cache. Interrupts can be directed to a single target
	 * core, however, event counts generated by processes running on other
	 * cores cannot be masked out. So we do not support sampling and
	 * per-thread events.
	 */
	if (is_sampling_event(event) || event->attach_state & PERF_ATTACH_TASK)
		return -EINVAL;

	/* NB and L2 counters do not have usr/os/guest/host bits */
	if (event->attr.exclude_user || event->attr.exclude_kernel ||
	    event->attr.exclude_host || event->attr.exclude_guest)
		return -EINVAL;

	/* and we do not enable counter overflow interrupts */
	hwc->config = event->attr.config & AMD64_RAW_EVENT_MASK_NB;
	hwc->idx = -1;

	if (event->cpu < 0)
		return -EINVAL;

	uncore = event_to_amd_uncore(event);
	if (!uncore)
		return -ENODEV;

	/*
	 * since request can come in to any of the shared cores, we will remap
	 * to a single common cpu.
	 */
	event->cpu = uncore->cpu;

	return 0;
}

static ssize_t amd_uncore_attr_show_cpumask(struct device *dev,
					    struct device_attribute *attr,
					    char *buf)
{
	int n;
	cpumask_t *active_mask;
	struct pmu *pmu = dev_get_drvdata(dev);

	if (pmu->type == amd_nb_pmu.type)
		active_mask = &amd_nb_active_mask;
	else if (pmu->type == amd_l2_pmu.type)
		active_mask = &amd_l2_active_mask;
	else
		return 0;

	n = cpulist_scnprintf(buf, PAGE_SIZE - 2, active_mask);
	buf[n++] = '\n';
	buf[n] = '\0';
	return n;
}
static DEVICE_ATTR(cpumask, S_IRUGO, amd_uncore_attr_show_cpumask, NULL);

static struct attribute *amd_uncore_attrs[] = {
	&dev_attr_cpumask.attr,
	NULL,
};

static struct attribute_group amd_uncore_attr_group = {
	.attrs = amd_uncore_attrs,
};

PMU_FORMAT_ATTR(event, "config:0-7,32-35");
PMU_FORMAT_ATTR(umask, "config:8-15");

static struct attribute *amd_uncore_format_attr[] = {
	&format_attr_event.attr,
	&format_attr_umask.attr,
	NULL,
};

static struct attribute_group amd_uncore_format_group = {
	.name = "format",
	.attrs = amd_uncore_format_attr,
};

static const struct attribute_group *amd_uncore_attr_groups[] = {
	&amd_uncore_attr_group,
	&amd_uncore_format_group,
	NULL,
};

static struct pmu amd_nb_pmu = {
	.attr_groups	= amd_uncore_attr_groups,
	.name		= "amd_nb",
	.event_init	= amd_uncore_event_init,
	.add		= amd_uncore_add,
	.del		= amd_uncore_del,
	.start		= amd_uncore_start,
	.stop		= amd_uncore_stop,
	.read		= amd_uncore_read,
};

static struct pmu amd_l2_pmu = {
	.attr_groups	= amd_uncore_attr_groups,
	.name		= "amd_l2",
	.event_init	= amd_uncore_event_init,
	.add		= amd_uncore_add,
	.del		= amd_uncore_del,
	.start		= amd_uncore_start,
	.stop		= amd_uncore_stop,
	.read		= amd_uncore_read,
};

static struct amd_uncore *amd_uncore_alloc(unsigned int cpu)
{
	return kzalloc_node(sizeof(struct amd_uncore), GFP_KERNEL,
			cpu_to_node(cpu));
}

static int amd_uncore_cpu_up_prepare(unsigned int cpu)
{
	struct amd_uncore *uncore_nb = NULL, *uncore_l2;

	if (amd_uncore_nb) {
		uncore_nb = amd_uncore_alloc(cpu);
		if (!uncore_nb)
			goto fail;
		uncore_nb->cpu = cpu;
		uncore_nb->num_counters = NUM_COUNTERS_NB;
		uncore_nb->rdpmc_base = RDPMC_BASE_NB;
		uncore_nb->msr_base = MSR_F15H_NB_PERF_CTL;
		uncore_nb->active_mask = &amd_nb_active_mask;
		uncore_nb->pmu = &amd_nb_pmu;
		*per_cpu_ptr(amd_uncore_nb, cpu) = uncore_nb;
	}

	if (amd_uncore_l2) {
		uncore_l2 = amd_uncore_alloc(cpu);
		if (!uncore_l2)
			goto fail;
		uncore_l2->cpu = cpu;
		uncore_l2->num_counters = NUM_COUNTERS_L2;
		uncore_l2->rdpmc_base = RDPMC_BASE_L2;
		uncore_l2->msr_base = MSR_F16H_L2I_PERF_CTL;
		uncore_l2->active_mask = &amd_l2_active_mask;
		uncore_l2->pmu = &amd_l2_pmu;
		*per_cpu_ptr(amd_uncore_l2, cpu) = uncore_l2;
	}

	return 0;

fail:
	kfree(uncore_nb);
	return -ENOMEM;
}

static struct amd_uncore *
amd_uncore_find_online_sibling(struct amd_uncore *this,
			       struct amd_uncore * __percpu *uncores)
{
	unsigned int cpu;
	struct amd_uncore *that;

	for_each_online_cpu(cpu) {
		that = *per_cpu_ptr(uncores, cpu);

		if (!that)
			continue;

		if (this == that)
			continue;

		if (this->id == that->id) {
			that->free_when_cpu_online = this;
			this = that;
			break;
		}
	}

	this->refcnt++;
	return this;
}

static void amd_uncore_cpu_starting(unsigned int cpu)
{
	unsigned int eax, ebx, ecx, edx;
	struct amd_uncore *uncore;

	if (amd_uncore_nb) {
		uncore = *per_cpu_ptr(amd_uncore_nb, cpu);
		cpuid(0x8000001e, &eax, &ebx, &ecx, &edx);
		uncore->id = ecx & 0xff;

		uncore = amd_uncore_find_online_sibling(uncore, amd_uncore_nb);
		*per_cpu_ptr(amd_uncore_nb, cpu) = uncore;
	}

	if (amd_uncore_l2) {
		unsigned int apicid = cpu_data(cpu).apicid;
		unsigned int nshared;

		uncore = *per_cpu_ptr(amd_uncore_l2, cpu);
		cpuid_count(0x8000001d, 2, &eax, &ebx, &ecx, &edx);
		nshared = ((eax >> 14) & 0xfff) + 1;
		uncore->id = apicid - (apicid % nshared);

		uncore = amd_uncore_find_online_sibling(uncore, amd_uncore_l2);
		*per_cpu_ptr(amd_uncore_l2, cpu) = uncore;
	}
}

static void uncore_online(unsigned int cpu,
			  struct amd_uncore * __percpu *uncores)
{
	struct amd_uncore *uncore = *per_cpu_ptr(uncores, cpu);

	kfree(uncore->free_when_cpu_online);
	uncore->free_when_cpu_online = NULL;

	if (cpu == uncore->cpu)
		cpumask_set_cpu(cpu, uncore->active_mask);
}

static void amd_uncore_cpu_online(unsigned int cpu)
{
	if (amd_uncore_nb)
		uncore_online(cpu, amd_uncore_nb);

	if (amd_uncore_l2)
		uncore_online(cpu, amd_uncore_l2);
}

static void uncore_down_prepare(unsigned int cpu,
				struct amd_uncore * __percpu *uncores)
{
	unsigned int i;
	struct amd_uncore *this = *per_cpu_ptr(uncores, cpu);

	if (this->cpu != cpu)
		return;

	/* this cpu is going down, migrate to a shared sibling if possible */
	for_each_online_cpu(i) {
		struct amd_uncore *that = *per_cpu_ptr(uncores, i);

		if (cpu == i)
			continue;

		if (this == that) {
			perf_pmu_migrate_context(this->pmu, cpu, i);
			cpumask_clear_cpu(cpu, that->active_mask);
			cpumask_set_cpu(i, that->active_mask);
			that->cpu = i;
			break;
		}
	}
}

static void amd_uncore_cpu_down_prepare(unsigned int cpu)
{
	if (amd_uncore_nb)
		uncore_down_prepare(cpu, amd_uncore_nb);

	if (amd_uncore_l2)
		uncore_down_prepare(cpu, amd_uncore_l2);
}

static void uncore_dead(unsigned int cpu, struct amd_uncore * __percpu *uncores)
{
	struct amd_uncore *uncore = *per_cpu_ptr(uncores, cpu);

	if (cpu == uncore->cpu)
		cpumask_clear_cpu(cpu, uncore->active_mask);

	if (!--uncore->refcnt)
		kfree(uncore);
	*per_cpu_ptr(uncores, cpu) = NULL;
}

static void amd_uncore_cpu_dead(unsigned int cpu)
{
	if (amd_uncore_nb)
		uncore_dead(cpu, amd_uncore_nb);

	if (amd_uncore_l2)
		uncore_dead(cpu, amd_uncore_l2);
}

static int
amd_uncore_cpu_notifier(struct notifier_block *self, unsigned long action,
			void *hcpu)
{
	unsigned int cpu = (long)hcpu;

	switch (action & ~CPU_TASKS_FROZEN) {
	case CPU_UP_PREPARE:
		if (amd_uncore_cpu_up_prepare(cpu))
			return notifier_from_errno(-ENOMEM);
		break;

	case CPU_STARTING:
		amd_uncore_cpu_starting(cpu);
		break;

	case CPU_ONLINE:
		amd_uncore_cpu_online(cpu);
		break;

	case CPU_DOWN_PREPARE:
		amd_uncore_cpu_down_prepare(cpu);
		break;

	case CPU_UP_CANCELED:
	case CPU_DEAD:
		amd_uncore_cpu_dead(cpu);
		break;

	default:
		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block amd_uncore_cpu_notifier_block = {
	.notifier_call	= amd_uncore_cpu_notifier,
	.priority	= CPU_PRI_PERF + 1,
};

static void __init init_cpu_already_online(void *dummy)
{
	unsigned int cpu = smp_processor_id();

	amd_uncore_cpu_starting(cpu);
	amd_uncore_cpu_online(cpu);
}

static void cleanup_cpu_online(void *dummy)
{
	unsigned int cpu = smp_processor_id();

	amd_uncore_cpu_dead(cpu);
}

static int __init amd_uncore_init(void)
{
	unsigned int cpu, cpu2;
	int ret = -ENODEV;

	if (boot_cpu_data.x86_vendor != X86_VENDOR_AMD)
		goto fail_nodev;

	if (!cpu_has_topoext)
		goto fail_nodev;

	if (cpu_has_perfctr_nb) {
		amd_uncore_nb = alloc_percpu(struct amd_uncore *);
		if (!amd_uncore_nb) {
			ret = -ENOMEM;
			goto fail_nb;
		}
		ret = perf_pmu_register(&amd_nb_pmu, amd_nb_pmu.name, -1);
		if (ret)
			goto fail_nb;

		printk(KERN_INFO "perf: AMD NB counters detected\n");
		ret = 0;
	}

	if (cpu_has_perfctr_l2) {
		amd_uncore_l2 = alloc_percpu(struct amd_uncore *);
		if (!amd_uncore_l2) {
			ret = -ENOMEM;
			goto fail_l2;
		}
		ret = perf_pmu_register(&amd_l2_pmu, amd_l2_pmu.name, -1);
		if (ret)
			goto fail_l2;

		printk(KERN_INFO "perf: AMD L2I counters detected\n");
		ret = 0;
	}

	if (ret)
		goto fail_nodev;

	cpu_notifier_register_begin();

	/* init cpus already online before registering for hotplug notifier */
	for_each_online_cpu(cpu) {
		ret = amd_uncore_cpu_up_prepare(cpu);
		if (ret)
			goto fail_online;
		smp_call_function_single(cpu, init_cpu_already_online, NULL, 1);
	}

	__register_cpu_notifier(&amd_uncore_cpu_notifier_block);
	cpu_notifier_register_done();

	return 0;


fail_online:
	for_each_online_cpu(cpu2) {
		if (cpu2 == cpu)
			break;
		smp_call_function_single(cpu, cleanup_cpu_online, NULL, 1);
	}
	cpu_notifier_register_done();

	/* amd_uncore_nb/l2 should have been freed by cleanup_cpu_online */
	amd_uncore_nb = amd_uncore_l2 = NULL;
	if (cpu_has_perfctr_l2)
		perf_pmu_unregister(&amd_l2_pmu);
fail_l2:
	if (cpu_has_perfctr_nb)
		perf_pmu_unregister(&amd_nb_pmu);
	if (amd_uncore_l2)
		free_percpu(amd_uncore_l2);
fail_nb:
	if (amd_uncore_nb)
		free_percpu(amd_uncore_nb);

fail_nodev:
	return ret;
}
device_initcall(amd_uncore_init);
