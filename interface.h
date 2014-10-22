/*
 * Greybus interfaces
 *
 * Copyright 2014 Google Inc.
 *
 * Released under the GPLv2 only.
 */

#ifndef __INTERFACE_H
#define __INTERFACE_H

#include <linux/list.h>

struct gb_interface {
	struct gb_module	*gmod;
	u8			id;
	u8			device_id;
	struct list_head	connections;

	struct list_head	links;	/* module->interfaces */
};

struct gb_interface *gb_interface_create(struct gb_module *gmod, u8 module_id);
void gb_interface_destroy(struct gb_interface *interface);

struct gb_interface *gb_interface_find(struct gb_module *gmod, u8 interface_id);

int gb_interface_connections_init(struct gb_interface *interface);
void gb_interface_connections_exit(struct gb_interface *interface);

#endif /* __INTERFACE_H */
