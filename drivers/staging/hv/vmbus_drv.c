/*
 *
 * Copyright (c) 2009, Microsoft Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307 USA.
 *
 * Authors:
 *   Haiyang Zhang <haiyangz@microsoft.com>
 *   Hank Janssen  <hjanssen@microsoft.com>
 *
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sysctl.h>
#include "osd.h"
#include "logging.h"
#include "vmbus.h"


/* Defines */


/* FIXME! We need to do this dynamically for PIC and APIC system */
#define VMBUS_IRQ				0x5
#define VMBUS_IRQ_VECTOR     IRQ5_VECTOR

/* Data types */


/* Main vmbus driver data structure */
struct vmbus_driver_context {
	/* !! These must be the first 2 fields !! */
	/* The driver field is not used in here. Instead, the bus field is */
	/* used to represent the driver */
	struct driver_context	drv_ctx;
	struct vmbus_driver drv_obj;

	struct bus_type			bus;
	struct tasklet_struct	msg_dpc;
	struct tasklet_struct	event_dpc;

	/* The bus root device */
	struct device_context	device_ctx;
};


/* Static decl */

static int vmbus_match(struct device *device, struct device_driver *driver);
static int vmbus_probe(struct device *device);
static int vmbus_remove(struct device *device);
static void vmbus_shutdown(struct device *device);
static int vmbus_uevent(struct device *device, struct kobj_uevent_env *env);
static void vmbus_msg_dpc(unsigned long data);
static void vmbus_event_dpc(unsigned long data);

static irqreturn_t vmbus_isr(int irq, void* dev_id);

static void vmbus_device_release(struct device *device);
static void vmbus_bus_release(struct device *device);

static struct hv_device *vmbus_child_device_create(struct hv_guid *type, struct hv_guid *instance, void *context);
static void vmbus_child_device_destroy(struct hv_device *device_obj);
static int vmbus_child_device_register(struct hv_device *root_device_obj, struct hv_device *child_device_obj);
static void vmbus_child_device_unregister(struct hv_device *child_device_obj);
static void vmbus_child_device_get_info(struct hv_device *device_obj, struct hv_device_info *device_info);

/* static ssize_t vmbus_show_class_id(struct device *dev, struct device_attribute *attr, char *buf); */
/* static ssize_t vmbus_show_device_id(struct device *dev, struct device_attribute *attr, char *buf); */

static ssize_t vmbus_show_device_attr(struct device *dev, struct device_attribute *dev_attr, char *buf);


/* Global */


/* Global logging setting */

/* unsigned int vmbus_loglevel= (((VMBUS | VMBUS_DRV)<<16) | DEBUG_LVL_ENTEREXIT); */
/* unsigned int vmbus_loglevel= (ALL_MODULES << 16 | DEBUG_LVL_ENTEREXIT); */
unsigned int vmbus_loglevel= (ALL_MODULES << 16 | INFO_LVL);
EXPORT_SYMBOL(vmbus_loglevel);

static int vmbus_irq = VMBUS_IRQ;

/* Setup /proc/sys/bus/vmbus/vmbus_loglevel */
/* Allow usage of sysctl cmd to set the logging level */

/* Set up per device attributes in /sys/bus/vmbus/devices/<bus device> */

static struct device_attribute vmbus_device_attrs[] = {
	__ATTR(id, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(state, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(class_id, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(device_id, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(monitor_id, S_IRUGO, vmbus_show_device_attr, NULL),

	__ATTR(server_monitor_pending, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(server_monitor_latency, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(server_monitor_conn_id, S_IRUGO, vmbus_show_device_attr, NULL),

	__ATTR(client_monitor_pending, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(client_monitor_latency, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(client_monitor_conn_id, S_IRUGO, vmbus_show_device_attr, NULL),

	__ATTR(out_intr_mask, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(out_read_index, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(out_write_index, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(out_read_bytes_avail, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(out_write_bytes_avail, S_IRUGO, vmbus_show_device_attr, NULL),

	__ATTR(in_intr_mask, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(in_read_index, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(in_write_index, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(in_read_bytes_avail, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR(in_write_bytes_avail, S_IRUGO, vmbus_show_device_attr, NULL),
	__ATTR_NULL
};

/* The one and only one */
static struct vmbus_driver_context g_vmbus_drv={
	.bus.name	= "vmbus",
	.bus.match	= vmbus_match,
	.bus.shutdown = vmbus_shutdown,
	.bus.remove = vmbus_remove,
	.bus.probe	= vmbus_probe,
	.bus.uevent = vmbus_uevent,
	.bus.dev_attrs = vmbus_device_attrs,
};


/* Routines */



/*++

Name:	vmbus_show_device_attr()

Desc:	Show the device attribute in sysfs. This is invoked when user does a "cat /sys/bus/vmbus/devices/<bus device>/<attr name>"

--*/
static ssize_t vmbus_show_device_attr(struct device *dev, struct device_attribute *dev_attr, char *buf)
{
	struct device_context *device_ctx = device_to_device_context(dev);
	struct hv_device_info device_info;

	memset(&device_info, 0, sizeof(struct hv_device_info));

	vmbus_child_device_get_info(&device_ctx->device_obj, &device_info);

	if (!strcmp(dev_attr->attr.name, "class_id"))
	{
		return sprintf(buf, "{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x}\n",
			device_info.ChannelType.data[3], device_info.ChannelType.data[2],
			device_info.ChannelType.data[1], device_info.ChannelType.data[0],
			device_info.ChannelType.data[5], device_info.ChannelType.data[4],
			device_info.ChannelType.data[7], device_info.ChannelType.data[6],
			device_info.ChannelType.data[8], device_info.ChannelType.data[9],
			device_info.ChannelType.data[10], device_info.ChannelType.data[11],
			device_info.ChannelType.data[12], device_info.ChannelType.data[13],
			device_info.ChannelType.data[14], device_info.ChannelType.data[15]);

	}
	else if (!strcmp(dev_attr->attr.name, "device_id"))
	{
		return sprintf(buf, "{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x}\n",
			device_info.ChannelInstance.data[3], device_info.ChannelInstance.data[2],
			device_info.ChannelInstance.data[1], device_info.ChannelInstance.data[0],
			device_info.ChannelInstance.data[5], device_info.ChannelInstance.data[4],
			device_info.ChannelInstance.data[7], device_info.ChannelInstance.data[6],
			device_info.ChannelInstance.data[8], device_info.ChannelInstance.data[9],
			device_info.ChannelInstance.data[10], device_info.ChannelInstance.data[11],
			device_info.ChannelInstance.data[12], device_info.ChannelInstance.data[13],
			device_info.ChannelInstance.data[14], device_info.ChannelInstance.data[15]);
	}
	else if (!strcmp(dev_attr->attr.name, "state"))
	{
		return sprintf(buf, "%d\n", device_info.ChannelState);
	}
	else if (!strcmp(dev_attr->attr.name, "id"))
	{
		return sprintf(buf, "%d\n", device_info.ChannelId);
	}
	else if (!strcmp(dev_attr->attr.name, "out_intr_mask"))
	{
		return sprintf(buf, "%d\n", device_info.Outbound.InterruptMask);
	}
	else if (!strcmp(dev_attr->attr.name, "out_read_index"))
	{
		return sprintf(buf, "%d\n", device_info.Outbound.ReadIndex);
	}
	else if (!strcmp(dev_attr->attr.name, "out_write_index"))
	{
		return sprintf(buf, "%d\n", device_info.Outbound.WriteIndex);
	}
	else if (!strcmp(dev_attr->attr.name, "out_read_bytes_avail"))
	{
		return sprintf(buf, "%d\n", device_info.Outbound.BytesAvailToRead);
	}
	else if (!strcmp(dev_attr->attr.name, "out_write_bytes_avail"))
	{
		return sprintf(buf, "%d\n", device_info.Outbound.BytesAvailToWrite);
	}
	else if (!strcmp(dev_attr->attr.name, "in_intr_mask"))
	{
		return sprintf(buf, "%d\n", device_info.Inbound.InterruptMask);
	}
	else if (!strcmp(dev_attr->attr.name, "in_read_index"))
	{
		return sprintf(buf, "%d\n", device_info.Inbound.ReadIndex);
	}
	else if (!strcmp(dev_attr->attr.name, "in_write_index"))
	{
		return sprintf(buf, "%d\n", device_info.Inbound.WriteIndex);
	}
	else if (!strcmp(dev_attr->attr.name, "in_read_bytes_avail"))
	{
		return sprintf(buf, "%d\n", device_info.Inbound.BytesAvailToRead);
	}
	else if (!strcmp(dev_attr->attr.name, "in_write_bytes_avail"))
	{
		return sprintf(buf, "%d\n", device_info.Inbound.BytesAvailToWrite);
	}
	else if (!strcmp(dev_attr->attr.name, "monitor_id"))
	{
		return sprintf(buf, "%d\n", device_info.MonitorId);
	}
	else if (!strcmp(dev_attr->attr.name, "server_monitor_pending"))
	{
		return sprintf(buf, "%d\n", device_info.ServerMonitorPending);
	}
	else if (!strcmp(dev_attr->attr.name, "server_monitor_latency"))
	{
		return sprintf(buf, "%d\n", device_info.ServerMonitorLatency);
	}
	else if (!strcmp(dev_attr->attr.name, "server_monitor_conn_id"))
	{
		return sprintf(buf, "%d\n", device_info.ServerMonitorConnectionId);
	}
	else if (!strcmp(dev_attr->attr.name, "client_monitor_pending"))
	{
		return sprintf(buf, "%d\n", device_info.ClientMonitorPending);
	}
	else if (!strcmp(dev_attr->attr.name, "client_monitor_latency"))
	{
		return sprintf(buf, "%d\n", device_info.ClientMonitorLatency);
	}
	else if (!strcmp(dev_attr->attr.name, "client_monitor_conn_id"))
	{
		return sprintf(buf, "%d\n", device_info.ClientMonitorConnectionId);
	}
	else
	{
		return 0;
	}
}

/*++

Name:	vmbus_show_class_id()

Desc:	Show the device class id in sysfs

--*/
/* static ssize_t vmbus_show_class_id(struct device *dev, struct device_attribute *attr, char *buf) */
/* { */
/* struct device_context *device_ctx = device_to_device_context(dev); */
/* return sprintf(buf, "{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x}\n", */
/*	device_ctx->class_id[3], device_ctx->class_id[2], device_ctx->class_id[1], device_ctx->class_id[0], */
/*	device_ctx->class_id[5], device_ctx->class_id[4], */
/*	device_ctx->class_id[7], device_ctx->class_id[6], */
/*	device_ctx->class_id[8], device_ctx->class_id[9], device_ctx->class_id[10], device_ctx->class_id[11], device_ctx->class_id[12], device_ctx->class_id[13], device_ctx->class_id[14], device_ctx->class_id[15]); */
/* } */

/*++

Name:	vmbus_show_device_id()

Desc:	Show the device instance id in sysfs

--*/
/* static ssize_t vmbus_show_device_id(struct device *dev, struct device_attribute *attr, char *buf) */
/* { */
/* struct device_context *device_ctx = device_to_device_context(dev); */
/* return sprintf(buf, "{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x}\n", */
/*	device_ctx->device_id[3], device_ctx->device_id[2], device_ctx->device_id[1], device_ctx->device_id[0], */
/*	device_ctx->device_id[5], device_ctx->device_id[4], */
/*	device_ctx->device_id[7], device_ctx->device_id[6], */
/*	device_ctx->device_id[8], device_ctx->device_id[9], device_ctx->device_id[10], device_ctx->device_id[11], device_ctx->device_id[12], device_ctx->device_id[13], device_ctx->device_id[14], device_ctx->device_id[15]); */
/* } */

/*++

Name:	vmbus_bus_init()

Desc:	Main vmbus driver initialization routine. Here, we
		- initialize the vmbus driver context
		- setup various driver entry points
		- invoke the vmbus hv main init routine
		- get the irq resource
		- invoke the vmbus to add the vmbus root device
		- setup the vmbus root device
		- retrieve the channel offers
--*/
static int vmbus_bus_init(PFN_DRIVERINITIALIZE pfn_drv_init)
{
	int ret=0;
	unsigned int vector=0;

	struct vmbus_driver_context *vmbus_drv_ctx=&g_vmbus_drv;
	struct vmbus_driver *vmbus_drv_obj = &g_vmbus_drv.drv_obj;

	struct device_context *dev_ctx=&g_vmbus_drv.device_ctx;

	DPRINT_ENTER(VMBUS_DRV);

	/* Set this up to allow lower layer to callback to add/remove child devices on the bus */
	vmbus_drv_obj->OnChildDeviceCreate = vmbus_child_device_create;
	vmbus_drv_obj->OnChildDeviceDestroy = vmbus_child_device_destroy;
	vmbus_drv_obj->OnChildDeviceAdd = vmbus_child_device_register;
	vmbus_drv_obj->OnChildDeviceRemove = vmbus_child_device_unregister;

	/* Call to bus driver to initialize */
	ret = pfn_drv_init(&vmbus_drv_obj->Base);
	if (ret != 0)
	{
		DPRINT_ERR(VMBUS_DRV, "Unable to initialize vmbus (%d)", ret);
		goto cleanup;
	}

	/* Sanity checks */
	if (!vmbus_drv_obj->Base.OnDeviceAdd)
	{
		DPRINT_ERR(VMBUS_DRV, "OnDeviceAdd() routine not set");
		ret = -1;
		goto cleanup;
	}

	vmbus_drv_ctx->bus.name = vmbus_drv_obj->Base.name;

	/* Initialize the bus context */
	tasklet_init(&vmbus_drv_ctx->msg_dpc, vmbus_msg_dpc, (unsigned long)vmbus_drv_obj);
	tasklet_init(&vmbus_drv_ctx->event_dpc, vmbus_event_dpc, (unsigned long)vmbus_drv_obj);

	/* Now, register the bus driver with LDM */
	ret = bus_register(&vmbus_drv_ctx->bus);
	if (ret)
	{
		ret = -1;
		goto cleanup;
	}

	/* Get the interrupt resource */
	ret = request_irq(vmbus_irq,
			  vmbus_isr,
			  IRQF_SAMPLE_RANDOM,
			  vmbus_drv_obj->Base.name,
			  NULL);

	if (ret != 0)
	{
		DPRINT_ERR(VMBUS_DRV, "ERROR - Unable to request IRQ %d", vmbus_irq);

		bus_unregister(&vmbus_drv_ctx->bus);

		ret = -1;
		goto cleanup;
	}
	vector = VMBUS_IRQ_VECTOR;

	DPRINT_INFO(VMBUS_DRV, "irq 0x%x vector 0x%x", vmbus_irq, vector);

	/* Call to bus driver to add the root device */
	memset(dev_ctx, 0, sizeof(struct device_context));

	ret = vmbus_drv_obj->Base.OnDeviceAdd(&dev_ctx->device_obj, &vector);
	if (ret != 0)
	{
		DPRINT_ERR(VMBUS_DRV, "ERROR - Unable to add vmbus root device");

		free_irq(vmbus_irq, NULL);

		bus_unregister(&vmbus_drv_ctx->bus);

		ret = -1;
		goto cleanup;
	}
	/* strcpy(dev_ctx->device.bus_id, dev_ctx->device_obj.name); */
	dev_set_name(&dev_ctx->device, "vmbus_0_0");
	memcpy(&dev_ctx->class_id, &dev_ctx->device_obj.deviceType, sizeof(struct hv_guid));
	memcpy(&dev_ctx->device_id, &dev_ctx->device_obj.deviceInstance, sizeof(struct hv_guid));

	/* No need to bind a driver to the root device. */
	dev_ctx->device.parent = NULL;
	dev_ctx->device.bus = &vmbus_drv_ctx->bus; /* NULL; vmbus_remove() does not get invoked */

	/* Setup the device dispatch table */
	dev_ctx->device.release = vmbus_bus_release;

	/* Setup the bus as root device */
	ret = device_register(&dev_ctx->device);
	if (ret)
	{
		DPRINT_ERR(VMBUS_DRV, "ERROR - Unable to register vmbus root device");

		free_irq(vmbus_irq, NULL);
		bus_unregister(&vmbus_drv_ctx->bus);

		ret = -1;
		goto cleanup;
	}


	vmbus_drv_obj->GetChannelOffers();

cleanup:
	DPRINT_EXIT(VMBUS_DRV);

	return ret;
}


/*++

Name:	vmbus_bus_exit()

Desc:	Terminate the vmbus driver. This routine is opposite of vmbus_bus_init()

--*/
static void vmbus_bus_exit(void)
{
	struct vmbus_driver *vmbus_drv_obj = &g_vmbus_drv.drv_obj;
	struct vmbus_driver_context *vmbus_drv_ctx=&g_vmbus_drv;

	struct device_context *dev_ctx=&g_vmbus_drv.device_ctx;

	DPRINT_ENTER(VMBUS_DRV);

	/* Remove the root device */
	if (vmbus_drv_obj->Base.OnDeviceRemove)
		vmbus_drv_obj->Base.OnDeviceRemove(&dev_ctx->device_obj);

	if (vmbus_drv_obj->Base.OnCleanup)
		vmbus_drv_obj->Base.OnCleanup(&vmbus_drv_obj->Base);

	/* Unregister the root bus device */
	device_unregister(&dev_ctx->device);

	bus_unregister(&vmbus_drv_ctx->bus);

	free_irq(vmbus_irq, NULL);

	tasklet_kill(&vmbus_drv_ctx->msg_dpc);
	tasklet_kill(&vmbus_drv_ctx->event_dpc);

	DPRINT_EXIT(VMBUS_DRV);

	return;
}

/*++

Name:	vmbus_child_driver_register()

Desc:	Register a vmbus's child driver

--*/
int vmbus_child_driver_register(struct driver_context* driver_ctx)
{
	struct vmbus_driver *vmbus_drv_obj = &g_vmbus_drv.drv_obj;
	int ret;

	DPRINT_ENTER(VMBUS_DRV);

	DPRINT_INFO(VMBUS_DRV, "child driver (%p) registering - name %s", driver_ctx, driver_ctx->driver.name);

	/* The child driver on this vmbus */
	driver_ctx->driver.bus = &g_vmbus_drv.bus;

	ret = driver_register(&driver_ctx->driver);

	vmbus_drv_obj->GetChannelOffers();

	DPRINT_EXIT(VMBUS_DRV);

	return ret;
}

EXPORT_SYMBOL(vmbus_child_driver_register);

/*++

Name:	vmbus_child_driver_unregister()

Desc:	Unregister a vmbus's child driver

--*/
void vmbus_child_driver_unregister(struct driver_context* driver_ctx)
{
	DPRINT_ENTER(VMBUS_DRV);

	DPRINT_INFO(VMBUS_DRV, "child driver (%p) unregistering - name %s", driver_ctx, driver_ctx->driver.name);

	driver_unregister(&driver_ctx->driver);

	driver_ctx->driver.bus = NULL;

	DPRINT_EXIT(VMBUS_DRV);
}

EXPORT_SYMBOL(vmbus_child_driver_unregister);

/*++

Name:	vmbus_get_interface()

Desc:	Get the vmbus channel interface. This is invoked by child/client driver that sits
		above vmbus
--*/
void vmbus_get_interface(struct vmbus_channel_interface *interface)
{
	struct vmbus_driver *vmbus_drv_obj = &g_vmbus_drv.drv_obj;

	vmbus_drv_obj->GetChannelInterface(interface);
}

EXPORT_SYMBOL(vmbus_get_interface);


/*++

Name:	vmbus_child_device_get_info()

Desc:	Get the vmbus child device info. This is invoked to display various device attributes in sysfs.
--*/
static void vmbus_child_device_get_info(struct hv_device *device_obj, struct hv_device_info *device_info)
{
	struct vmbus_driver *vmbus_drv_obj = &g_vmbus_drv.drv_obj;

	vmbus_drv_obj->GetChannelInfo(device_obj, device_info);
}


/*++

Name:	vmbus_child_device_create()

Desc:	Creates and registers a new child device on the vmbus.

--*/
static struct hv_device *vmbus_child_device_create(struct hv_guid *type,
						   struct hv_guid *instance,
						   void *context)
{
	struct device_context *child_device_ctx;
	struct hv_device *child_device_obj;

	DPRINT_ENTER(VMBUS_DRV);

	/* Allocate the new child device */
	child_device_ctx = kzalloc(sizeof(struct device_context), GFP_KERNEL);
	if (!child_device_ctx)
	{
		DPRINT_ERR(VMBUS_DRV, "unable to allocate device_context for child device");
		DPRINT_EXIT(VMBUS_DRV);

		return NULL;
	}

	DPRINT_DBG(VMBUS_DRV, "child device (%p) allocated - "
		"type {%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x},"
		"id {%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x}",
		&child_device_ctx->device,
		type->data[3], type->data[2], type->data[1], type->data[0],
		type->data[5], type->data[4], type->data[7], type->data[6],
		type->data[8], type->data[9], type->data[10], type->data[11],
		type->data[12], type->data[13], type->data[14], type->data[15],
		instance->data[3], instance->data[2], instance->data[1], instance->data[0],
		instance->data[5], instance->data[4], instance->data[7], instance->data[6],
		instance->data[8], instance->data[9], instance->data[10], instance->data[11],
		instance->data[12], instance->data[13], instance->data[14], instance->data[15]);

	child_device_obj = &child_device_ctx->device_obj;
	child_device_obj->context = context;
	memcpy(&child_device_obj->deviceType, &type, sizeof(struct hv_guid));
	memcpy(&child_device_obj->deviceInstance, &instance, sizeof(struct hv_guid));

	memcpy(&child_device_ctx->class_id, &type, sizeof(struct hv_guid));
	memcpy(&child_device_ctx->device_id, &instance, sizeof(struct hv_guid));

	DPRINT_EXIT(VMBUS_DRV);

	return child_device_obj;
}

/*++

Name:	vmbus_child_device_register()

Desc:	Register the child device on the specified bus

--*/
static int vmbus_child_device_register(struct hv_device *root_device_obj, struct hv_device *child_device_obj)
{
	int ret=0;
	struct device_context *root_device_ctx = to_device_context(root_device_obj);
	struct device_context *child_device_ctx = to_device_context(child_device_obj);
	static atomic_t device_num = ATOMIC_INIT(0);

	DPRINT_ENTER(VMBUS_DRV);

	DPRINT_DBG(VMBUS_DRV, "child device (%p) registering", child_device_ctx);

	/* Make sure we are not registered already */

	if (strlen(dev_name(&child_device_ctx->device)) != 0)
	{
		DPRINT_ERR(VMBUS_DRV, "child device (%p) already registered - busid %s", child_device_ctx, dev_name(&child_device_ctx->device));

		ret = -1;
		goto Cleanup;
	}

	/* Set the device bus id. Otherwise, device_register()will fail. */
	dev_set_name(&child_device_ctx->device, "vmbus_0_%d", atomic_inc_return(&device_num));

	/* The new device belongs to this bus */
	child_device_ctx->device.bus = &g_vmbus_drv.bus; /* device->dev.bus; */
	child_device_ctx->device.parent = &root_device_ctx->device;
	child_device_ctx->device.release = vmbus_device_release;

	/* Register with the LDM. This will kick off the driver/device binding...which will */
	/* eventually call vmbus_match() and vmbus_probe() */
	ret = device_register(&child_device_ctx->device);

	/* vmbus_probe() error does not get propergate to device_register(). */
	ret = child_device_ctx->probe_error;

	if (ret)
		DPRINT_ERR(VMBUS_DRV, "unable to register child device (%p)", &child_device_ctx->device);
	else
		DPRINT_INFO(VMBUS_DRV, "child device (%p) registered", &child_device_ctx->device);

Cleanup:
	DPRINT_EXIT(VMBUS_DRV);

	return ret;
}

/*++

Name:	vmbus_child_device_unregister()

Desc:	Remove the specified child device from the vmbus.

--*/
static void vmbus_child_device_unregister(struct hv_device *device_obj)
{
	struct device_context *device_ctx = to_device_context(device_obj);

	DPRINT_ENTER(VMBUS_DRV);

	DPRINT_INFO(VMBUS_DRV, "unregistering child device (%p)", &device_ctx->device);

	/* Kick off the process of unregistering the device. */
	/* This will call vmbus_remove() and eventually vmbus_device_release() */
	device_unregister(&device_ctx->device);

	DPRINT_INFO(VMBUS_DRV, "child device (%p) unregistered", &device_ctx->device);

	DPRINT_EXIT(VMBUS_DRV);
}


/*++

Name:	vmbus_child_device_destroy()

Desc:	Destroy the specified child device on the vmbus.

--*/
static void vmbus_child_device_destroy(struct hv_device *device_obj)
{
	DPRINT_ENTER(VMBUS_DRV);

	DPRINT_EXIT(VMBUS_DRV);
}

/*++

Name:	vmbus_uevent()

Desc:	This routine is invoked when a device is added or removed on the vmbus to generate a uevent to udev in the
		userspace. The udev will then look at its rule and the uevent generated here to load the appropriate driver

--*/
static int vmbus_uevent(struct device *device, struct kobj_uevent_env *env)
{
	struct device_context *device_ctx = device_to_device_context(device);
	int i=0;
	int len=0;
	int ret;

	DPRINT_ENTER(VMBUS_DRV);

	DPRINT_INFO(VMBUS_DRV, "generating uevent - VMBUS_DEVICE_CLASS_GUID={%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x}",
		device_ctx->class_id.data[3], device_ctx->class_id.data[2],
		device_ctx->class_id.data[1], device_ctx->class_id.data[0],
		device_ctx->class_id.data[5], device_ctx->class_id.data[4],
		device_ctx->class_id.data[7], device_ctx->class_id.data[6],
		device_ctx->class_id.data[8], device_ctx->class_id.data[9],
		device_ctx->class_id.data[10], device_ctx->class_id.data[11],
		device_ctx->class_id.data[12], device_ctx->class_id.data[13],
		device_ctx->class_id.data[14], device_ctx->class_id.data[15]);

	env->envp_idx = i;
	env->buflen = len;
	ret = add_uevent_var(env,
		"VMBUS_DEVICE_CLASS_GUID={%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x}",
		device_ctx->class_id.data[3], device_ctx->class_id.data[2],
		device_ctx->class_id.data[1], device_ctx->class_id.data[0],
		device_ctx->class_id.data[5], device_ctx->class_id.data[4],
		device_ctx->class_id.data[7], device_ctx->class_id.data[6],
		device_ctx->class_id.data[8], device_ctx->class_id.data[9],
		device_ctx->class_id.data[10], device_ctx->class_id.data[11],
		device_ctx->class_id.data[12], device_ctx->class_id.data[13],
		device_ctx->class_id.data[14], device_ctx->class_id.data[15]);

	if (ret)
	{
		return ret;
	}

	ret = add_uevent_var(env,
		"VMBUS_DEVICE_DEVICE_GUID={%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x%02x%02x}",
		device_ctx->device_id.data[3], device_ctx->device_id.data[2],
		device_ctx->device_id.data[1], device_ctx->device_id.data[0],
		device_ctx->device_id.data[5], device_ctx->device_id.data[4],
		device_ctx->device_id.data[7], device_ctx->device_id.data[6],
		device_ctx->device_id.data[8], device_ctx->device_id.data[9],
		device_ctx->device_id.data[10], device_ctx->device_id.data[11],
		device_ctx->device_id.data[12], device_ctx->device_id.data[13],
		device_ctx->device_id.data[14], device_ctx->device_id.data[15]);

	if (ret)
	{
		return ret;
	}

	env->envp[env->envp_idx] = NULL;

	DPRINT_EXIT(VMBUS_DRV);

	return 0;
}

/*++

Name:	vmbus_match()

Desc:	Attempt to match the specified device to the specified driver

--*/
static int vmbus_match(struct device *device, struct device_driver *driver)
{
	int match=0;
	struct driver_context *driver_ctx = driver_to_driver_context(driver);
	struct device_context *device_ctx = device_to_device_context(device);

	DPRINT_ENTER(VMBUS_DRV);

	/* We found our driver ? */
	if (memcmp(&device_ctx->class_id, &driver_ctx->class_id, sizeof(struct hv_guid)) == 0)
	{
		/* !! NOTE: The driver_ctx is not a vmbus_drv_ctx. We typecast it here to access the */
		/* struct hv_driver field */
		struct vmbus_driver_context *vmbus_drv_ctx = (struct vmbus_driver_context*)driver_ctx;
		device_ctx->device_obj.Driver = &vmbus_drv_ctx->drv_obj.Base;
		DPRINT_INFO(VMBUS_DRV, "device object (%p) set to driver object (%p)", &device_ctx->device_obj, device_ctx->device_obj.Driver);

		match = 1;
	}

	DPRINT_EXIT(VMBUS_DRV);

	return match;
}


/*++

Name:	vmbus_probe_failed_cb()

Desc:	Callback when a driver probe failed in vmbus_probe(). We need a callback because
		we cannot invoked device_unregister() inside vmbus_probe() since vmbus_probe() may be
		invoked inside device_register() i.e. we cannot call device_unregister() inside
		device_register()
--*/
static void vmbus_probe_failed_cb(struct work_struct *context)
{
	struct device_context *device_ctx = (struct device_context*)context;


	DPRINT_ENTER(VMBUS_DRV);

	/* Kick off the process of unregistering the device. */
	/* This will call vmbus_remove() and eventually vmbus_device_release() */
	device_unregister(&device_ctx->device);

	/* put_device(&device_ctx->device); */
	DPRINT_EXIT(VMBUS_DRV);
}


/*++

Name:	vmbus_probe()

Desc:	Add the new vmbus's child device

--*/
static int vmbus_probe(struct device *child_device)
{
	int ret=0;
	struct driver_context *driver_ctx = driver_to_driver_context(child_device->driver);
	struct device_context *device_ctx = device_to_device_context(child_device);

	DPRINT_ENTER(VMBUS_DRV);

	/* Let the specific open-source driver handles the probe if it can */
	if (driver_ctx->probe)
	{
		ret = device_ctx->probe_error = driver_ctx->probe(child_device);
		if (ret != 0)
		{
			DPRINT_ERR(VMBUS_DRV, "probe() failed for device %s (%p) on driver %s (%d)...", dev_name(child_device), child_device, child_device->driver->name, ret);

			INIT_WORK(&device_ctx->probe_failed_work_item, vmbus_probe_failed_cb);
			schedule_work(&device_ctx->probe_failed_work_item);
		}
	}
	else
	{
		DPRINT_ERR(VMBUS_DRV, "probe() method not set for driver - %s", child_device->driver->name);
		ret = -1;
	}

	DPRINT_EXIT(VMBUS_DRV);
	return ret;
}


/*++

Name:	vmbus_remove()

Desc:	Remove a vmbus device

--*/
static int vmbus_remove(struct device *child_device)
{
	int ret=0;
	struct driver_context *driver_ctx;

	DPRINT_ENTER(VMBUS_DRV);

	/* Special case root bus device */
	if (child_device->parent == NULL)
	{
		/* No-op since it is statically defined and handle in vmbus_bus_exit() */
		DPRINT_EXIT(VMBUS_DRV);
		return 0;
	}

	if (child_device->driver)
	{
		driver_ctx = driver_to_driver_context(child_device->driver);

		/* Let the specific open-source driver handles the removal if it can */
		if (driver_ctx->remove)
		{
			ret = driver_ctx->remove(child_device);
		}
		else
		{
			DPRINT_ERR(VMBUS_DRV, "remove() method not set for driver - %s", child_device->driver->name);
			ret = -1;
		}
	}
	else
	{

	}

	DPRINT_EXIT(VMBUS_DRV);

	return 0;
}

/*++

Name:	vmbus_shutdown()

Desc:	Shutdown a vmbus device

--*/
static void vmbus_shutdown(struct device *child_device)
{
	struct driver_context *driver_ctx;

	DPRINT_ENTER(VMBUS_DRV);

	/* Special case root bus device */
	if (child_device->parent == NULL)
	{
		/* No-op since it is statically defined and handle in vmbus_bus_exit() */
		DPRINT_EXIT(VMBUS_DRV);
		return;
	}

	/* The device may not be attached yet */
	if (!child_device->driver)
	{
		DPRINT_EXIT(VMBUS_DRV);
		return;
	}

	driver_ctx = driver_to_driver_context(child_device->driver);

	/* Let the specific open-source driver handles the removal if it can */
	if (driver_ctx->shutdown)
	{
		driver_ctx->shutdown(child_device);
	}

	DPRINT_EXIT(VMBUS_DRV);

	return;
}

/*++

Name:	vmbus_bus_release()

Desc:	Final callback release of the vmbus root device

--*/
static void vmbus_bus_release(struct device *device)
{
	DPRINT_ENTER(VMBUS_DRV);
	/* FIXME */
	/* Empty release functions are a bug, or a major sign
	 * of a problem design, this MUST BE FIXED! */
	dev_err(device, "%s needs to be fixed!\n", __func__);
	WARN_ON(1);
	DPRINT_EXIT(VMBUS_DRV);
}

/*++

Name:	vmbus_device_release()

Desc:	Final callback release of the vmbus child device

--*/
static void vmbus_device_release(struct device *device)
{
	struct device_context *device_ctx = device_to_device_context(device);

	DPRINT_ENTER(VMBUS_DRV);

	/* vmbus_child_device_destroy(&device_ctx->device_obj); */
	kfree(device_ctx);

	/* !!DO NOT REFERENCE device_ctx anymore at this point!! */

	DPRINT_EXIT(VMBUS_DRV);

	return;
}

/*++

Name:	vmbus_msg_dpc()

Desc:	Tasklet routine to handle hypervisor messages

--*/
static void vmbus_msg_dpc(unsigned long data)
{
	struct vmbus_driver *vmbus_drv_obj = (struct vmbus_driver *)data;

	DPRINT_ENTER(VMBUS_DRV);

	ASSERT(vmbus_drv_obj->OnMsgDpc != NULL);

	/* Call to bus driver to handle interrupt */
	vmbus_drv_obj->OnMsgDpc(&vmbus_drv_obj->Base);

	DPRINT_EXIT(VMBUS_DRV);
}

/*++

Name:	vmbus_msg_dpc()

Desc:	Tasklet routine to handle hypervisor events

--*/
static void vmbus_event_dpc(unsigned long data)
{
	struct vmbus_driver *vmbus_drv_obj = (struct vmbus_driver *)data;

	DPRINT_ENTER(VMBUS_DRV);

	ASSERT(vmbus_drv_obj->OnEventDpc != NULL);

	/* Call to bus driver to handle interrupt */
	vmbus_drv_obj->OnEventDpc(&vmbus_drv_obj->Base);

	DPRINT_EXIT(VMBUS_DRV);
}

/*++

Name:	vmbus_msg_dpc()

Desc:	ISR routine

--*/
static irqreturn_t vmbus_isr(int irq, void* dev_id)
{
	int ret=0;
	struct vmbus_driver *vmbus_driver_obj = &g_vmbus_drv.drv_obj;

	DPRINT_ENTER(VMBUS_DRV);

	ASSERT(vmbus_driver_obj->OnIsr != NULL);

	/* Call to bus driver to handle interrupt */
	ret = vmbus_driver_obj->OnIsr(&vmbus_driver_obj->Base);

	/* Schedules a dpc if necessary */
	if (ret > 0)
	{
		if (test_bit(0, (unsigned long*)&ret))
		{
			tasklet_schedule(&g_vmbus_drv.msg_dpc);
		}

		if (test_bit(1, (unsigned long*)&ret))
		{
			tasklet_schedule(&g_vmbus_drv.event_dpc);
		}

		DPRINT_EXIT(VMBUS_DRV);
		return IRQ_HANDLED;
	}
	else
	{
		DPRINT_EXIT(VMBUS_DRV);
		return IRQ_NONE;
	}
}

MODULE_LICENSE("GPL");


/*++

Name:	vmbus_init()

Desc:	Main vmbus driver entry routine

--*/
static int __init vmbus_init(void)
{
	int ret=0;

	DPRINT_ENTER(VMBUS_DRV);

	DPRINT_INFO(VMBUS_DRV,
		"Vmbus initializing.... current log level 0x%x (%x,%x)",
		vmbus_loglevel, HIWORD(vmbus_loglevel), LOWORD(vmbus_loglevel));
/* Todo: it is used for loglevel, to be ported to new kernel. */

	ret = vmbus_bus_init(VmbusInitialize);

	DPRINT_EXIT(VMBUS_DRV);
	return ret;
}



/*++

Name:	vmbus_init()

Desc:	Main vmbus driver exit routine

--*/
static void __exit vmbus_exit(void)
{
	DPRINT_ENTER(VMBUS_DRV);

	vmbus_bus_exit();
/* Todo: it is used for loglevel, to be ported to new kernel. */
	DPRINT_EXIT(VMBUS_DRV);

	return;
}

module_param(vmbus_irq, int, S_IRUGO);
module_param(vmbus_loglevel, int, S_IRUGO);

module_init(vmbus_init);
module_exit(vmbus_exit);
/* eof */
