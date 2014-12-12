/*
 * Greybus kernel "version" glue logic.
 *
 * Copyright 2014 Google Inc.
 * Copyright 2014 Linaro Ltd.
 *
 * Released under the GPLv2 only.
 *
 * Backports of newer kernel apis to allow the code to build properly on older
 * kernel versions.  Remove this file when merging to upstream, it should not be
 * needed at all
 */

#ifndef __GREYBUS_KERNEL_VER_H
#define __GREYBUS_KERNEL_VER_H

#ifndef __ATTR_WO
#define __ATTR_WO(_name) {						\
        .attr   = { .name = __stringify(_name), .mode = S_IWUSR },      \
        .store  = _name##_store,                                        \
}
#endif

#ifndef DEVICE_ATTR_RO
#define DEVICE_ATTR_RO(_name) \
	struct device_attribute dev_attr_##_name = __ATTR_RO(_name)
#endif

#ifndef DEVICE_ATTR_WO
#define DEVICE_ATTR_WO(_name) \
	struct device_attribute dev_attr_##_name = __ATTR_WO(_name)
#endif

#ifndef U8_MAX
#define U8_MAX	((u8)~0U)
#endif /* ! U8_MAX */

#ifndef U16_MAX
#define U16_MAX	((u16)(~0U))
#endif /* !U16_MAX */

/*
 * The GPIO api sucks rocks in places, like removal, so work around their
 * explicit requirements of catching the return value for kernels older than
 * 3.17, which they explicitly changed in the 3.17 kernel.  Consistency is
 * overrated.
 */
#include <linux/version.h>
#include <linux/gpio.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)
static inline void gb_gpiochip_remove(struct gpio_chip *chip)
{
	gpiochip_remove(chip);
}
#else
static inline void gb_gpiochip_remove(struct gpio_chip *chip)
{
	int ret;
	ret = gpiochip_remove(chip);
}
#endif

/*
 * ATTRIBUTE_GROUPS showed up in 3.11-rc2, but we need to build on 3.10, so add
 * it here.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,11,0)
#define ATTRIBUTE_GROUPS(name)					\
static const struct attribute_group name##_group = {		\
	.attrs = name##_attrs,					\
};								\
static const struct attribute_group *name##_groups[] = {	\
	&name##_group,						\
	NULL,							\
}
#endif

#endif	/* __GREYBUS_KERNEL_VER_H */
