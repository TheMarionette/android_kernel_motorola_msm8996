/*
 * Copyright 2013 Advanced Micro Devices, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * Authors: Christian König <christian.koenig@amd.com>
 */

#include <linux/firmware.h>
#include <linux/module.h>
#include <drm/drmP.h>
#include <drm/drm.h>

#include "radeon.h"
#include "radeon_asic.h"
#include "sid.h"

/* Firmware Names */
#define FIRMWARE_BONAIRE	"radeon/BONAIRE_vce.bin"

MODULE_FIRMWARE(FIRMWARE_BONAIRE);

/**
 * radeon_vce_init - allocate memory, load vce firmware
 *
 * @rdev: radeon_device pointer
 *
 * First step to get VCE online, allocate memory and load the firmware
 */
int radeon_vce_init(struct radeon_device *rdev)
{
	unsigned long bo_size;
	const char *fw_name;
	int i, r;

	switch (rdev->family) {
	case CHIP_BONAIRE:
	case CHIP_KAVERI:
	case CHIP_KABINI:
		fw_name = FIRMWARE_BONAIRE;
		break;

	default:
		return -EINVAL;
	}

	r = request_firmware(&rdev->vce_fw, fw_name, rdev->dev);
	if (r) {
		dev_err(rdev->dev, "radeon_vce: Can't load firmware \"%s\"\n",
			fw_name);
		return r;
	}

	bo_size = RADEON_GPU_PAGE_ALIGN(rdev->vce_fw->size) +
		  RADEON_VCE_STACK_SIZE + RADEON_VCE_HEAP_SIZE;
	r = radeon_bo_create(rdev, bo_size, PAGE_SIZE, true,
			     RADEON_GEM_DOMAIN_VRAM, NULL, &rdev->vce.vcpu_bo);
	if (r) {
		dev_err(rdev->dev, "(%d) failed to allocate VCE bo\n", r);
		return r;
	}

	r = radeon_vce_resume(rdev);
	if (r)
		return r;

	memset(rdev->vce.cpu_addr, 0, bo_size);
	memcpy(rdev->vce.cpu_addr, rdev->vce_fw->data, rdev->vce_fw->size);

	r = radeon_vce_suspend(rdev);
	if (r)
		return r;

	for (i = 0; i < RADEON_MAX_VCE_HANDLES; ++i) {
		atomic_set(&rdev->vce.handles[i], 0);
		rdev->vce.filp[i] = NULL;
        }

	return 0;
}

/**
 * radeon_vce_fini - free memory
 *
 * @rdev: radeon_device pointer
 *
 * Last step on VCE teardown, free firmware memory
 */
void radeon_vce_fini(struct radeon_device *rdev)
{
	radeon_vce_suspend(rdev);
	radeon_bo_unref(&rdev->vce.vcpu_bo);
}

/**
 * radeon_vce_suspend - unpin VCE fw memory
 *
 * @rdev: radeon_device pointer
 *
 * TODO: Test VCE suspend/resume
 */
int radeon_vce_suspend(struct radeon_device *rdev)
{
	int r;

	if (rdev->vce.vcpu_bo == NULL)
		return 0;

	r = radeon_bo_reserve(rdev->vce.vcpu_bo, false);
	if (!r) {
		radeon_bo_kunmap(rdev->vce.vcpu_bo);
		radeon_bo_unpin(rdev->vce.vcpu_bo);
		radeon_bo_unreserve(rdev->vce.vcpu_bo);
	}
	return r;
}

/**
 * radeon_vce_resume - pin VCE fw memory
 *
 * @rdev: radeon_device pointer
 *
 * TODO: Test VCE suspend/resume
 */
int radeon_vce_resume(struct radeon_device *rdev)
{
	int r;

	if (rdev->vce.vcpu_bo == NULL)
		return -EINVAL;

	r = radeon_bo_reserve(rdev->vce.vcpu_bo, false);
	if (r) {
		radeon_bo_unref(&rdev->vce.vcpu_bo);
		dev_err(rdev->dev, "(%d) failed to reserve VCE bo\n", r);
		return r;
	}

	r = radeon_bo_pin(rdev->vce.vcpu_bo, RADEON_GEM_DOMAIN_VRAM,
			  &rdev->vce.gpu_addr);
	if (r) {
		radeon_bo_unreserve(rdev->vce.vcpu_bo);
		radeon_bo_unref(&rdev->vce.vcpu_bo);
		dev_err(rdev->dev, "(%d) VCE bo pin failed\n", r);
		return r;
	}

	r = radeon_bo_kmap(rdev->vce.vcpu_bo, &rdev->vce.cpu_addr);
	if (r) {
		dev_err(rdev->dev, "(%d) VCE map failed\n", r);
		return r;
	}

	radeon_bo_unreserve(rdev->vce.vcpu_bo);

	return 0;
}

/**
 * radeon_vce_free_handles - free still open VCE handles
 *
 * @rdev: radeon_device pointer
 * @filp: drm file pointer
 *
 * Close all VCE handles still open by this file pointer
 */
void radeon_vce_free_handles(struct radeon_device *rdev, struct drm_file *filp)
{
	int i, r;
	for (i = 0; i < RADEON_MAX_VCE_HANDLES; ++i) {
		uint32_t handle = atomic_read(&rdev->vce.handles[i]);
		if (!handle || rdev->vce.filp[i] != filp)
			continue;

		r = radeon_vce_get_destroy_msg(rdev, TN_RING_TYPE_VCE1_INDEX,
					       handle, NULL);
		if (r)
			DRM_ERROR("Error destroying VCE handle (%d)!\n", r);

		rdev->vce.filp[i] = NULL;
		atomic_set(&rdev->vce.handles[i], 0);
	}
}

/**
 * radeon_vce_get_create_msg - generate a VCE create msg
 *
 * @rdev: radeon_device pointer
 * @ring: ring we should submit the msg to
 * @handle: VCE session handle to use
 * @fence: optional fence to return
 *
 * Open up a stream for HW test
 */
int radeon_vce_get_create_msg(struct radeon_device *rdev, int ring,
			      uint32_t handle, struct radeon_fence **fence)
{
	const unsigned ib_size_dw = 1024;
	struct radeon_ib ib;
	uint64_t dummy;
	int i, r;

	r = radeon_ib_get(rdev, ring, &ib, NULL, ib_size_dw * 4);
	if (r) {
		DRM_ERROR("radeon: failed to get ib (%d).\n", r);
		return r;
	}

	dummy = ib.gpu_addr + 1024;

	/* stitch together an VCE create msg */
	ib.length_dw = 0;
	ib.ptr[ib.length_dw++] = 0x0000000c; /* len */
	ib.ptr[ib.length_dw++] = 0x00000001; /* session cmd */
	ib.ptr[ib.length_dw++] = handle;

	ib.ptr[ib.length_dw++] = 0x00000030; /* len */
	ib.ptr[ib.length_dw++] = 0x01000001; /* create cmd */
	ib.ptr[ib.length_dw++] = 0x00000000;
	ib.ptr[ib.length_dw++] = 0x00000042;
	ib.ptr[ib.length_dw++] = 0x0000000a;
	ib.ptr[ib.length_dw++] = 0x00000001;
	ib.ptr[ib.length_dw++] = 0x00000080;
	ib.ptr[ib.length_dw++] = 0x00000060;
	ib.ptr[ib.length_dw++] = 0x00000100;
	ib.ptr[ib.length_dw++] = 0x00000100;
	ib.ptr[ib.length_dw++] = 0x0000000c;
	ib.ptr[ib.length_dw++] = 0x00000000;

	ib.ptr[ib.length_dw++] = 0x00000014; /* len */
	ib.ptr[ib.length_dw++] = 0x05000005; /* feedback buffer */
	ib.ptr[ib.length_dw++] = upper_32_bits(dummy);
	ib.ptr[ib.length_dw++] = dummy;
	ib.ptr[ib.length_dw++] = 0x00000001;

	for (i = ib.length_dw; i < ib_size_dw; ++i)
		ib.ptr[i] = 0x0;

	r = radeon_ib_schedule(rdev, &ib, NULL);
	if (r) {
	        DRM_ERROR("radeon: failed to schedule ib (%d).\n", r);
	}

	if (fence)
		*fence = radeon_fence_ref(ib.fence);

	radeon_ib_free(rdev, &ib);

	return r;
}

/**
 * radeon_vce_get_destroy_msg - generate a VCE destroy msg
 *
 * @rdev: radeon_device pointer
 * @ring: ring we should submit the msg to
 * @handle: VCE session handle to use
 * @fence: optional fence to return
 *
 * Close up a stream for HW test or if userspace failed to do so
 */
int radeon_vce_get_destroy_msg(struct radeon_device *rdev, int ring,
			       uint32_t handle, struct radeon_fence **fence)
{
	const unsigned ib_size_dw = 1024;
	struct radeon_ib ib;
	uint64_t dummy;
	int i, r;

	r = radeon_ib_get(rdev, ring, &ib, NULL, ib_size_dw * 4);
	if (r) {
		DRM_ERROR("radeon: failed to get ib (%d).\n", r);
		return r;
	}

	dummy = ib.gpu_addr + 1024;

	/* stitch together an VCE destroy msg */
	ib.length_dw = 0;
	ib.ptr[ib.length_dw++] = 0x0000000c; /* len */
	ib.ptr[ib.length_dw++] = 0x00000001; /* session cmd */
	ib.ptr[ib.length_dw++] = handle;

	ib.ptr[ib.length_dw++] = 0x00000014; /* len */
	ib.ptr[ib.length_dw++] = 0x05000005; /* feedback buffer */
	ib.ptr[ib.length_dw++] = upper_32_bits(dummy);
	ib.ptr[ib.length_dw++] = dummy;
	ib.ptr[ib.length_dw++] = 0x00000001;

	ib.ptr[ib.length_dw++] = 0x00000008; /* len */
	ib.ptr[ib.length_dw++] = 0x02000001; /* destroy cmd */

	for (i = ib.length_dw; i < ib_size_dw; ++i)
		ib.ptr[i] = 0x0;

	r = radeon_ib_schedule(rdev, &ib, NULL);
	if (r) {
	        DRM_ERROR("radeon: failed to schedule ib (%d).\n", r);
	}

	if (fence)
		*fence = radeon_fence_ref(ib.fence);

	radeon_ib_free(rdev, &ib);

	return r;
}

/**
 * radeon_vce_cs_reloc - command submission relocation
 *
 * @p: parser context
 * @lo: address of lower dword
 * @hi: address of higher dword
 *
 * Patch relocation inside command stream with real buffer address
 */
int radeon_vce_cs_reloc(struct radeon_cs_parser *p, int lo, int hi)
{
	struct radeon_cs_chunk *relocs_chunk;
	uint64_t offset;
	unsigned idx;

	relocs_chunk = &p->chunks[p->chunk_relocs_idx];
	offset = radeon_get_ib_value(p, lo);
	idx = radeon_get_ib_value(p, hi);

	if (idx >= relocs_chunk->length_dw) {
		DRM_ERROR("Relocs at %d after relocations chunk end %d !\n",
			  idx, relocs_chunk->length_dw);
		return -EINVAL;
	}

	offset += p->relocs_ptr[(idx / 4)]->lobj.gpu_offset;

        p->ib.ptr[lo] = offset & 0xFFFFFFFF;
        p->ib.ptr[hi] = offset >> 32;

	return 0;
}

/**
 * radeon_vce_cs_parse - parse and validate the command stream
 *
 * @p: parser context
 *
 */
int radeon_vce_cs_parse(struct radeon_cs_parser *p)
{
	uint32_t handle = 0;
	bool destroy = false;
	int i, r;

	while (p->idx < p->chunks[p->chunk_ib_idx].length_dw) {
		uint32_t len = radeon_get_ib_value(p, p->idx);
		uint32_t cmd = radeon_get_ib_value(p, p->idx + 1);

		if ((len < 8) || (len & 3)) {
			DRM_ERROR("invalid VCE command length (%d)!\n", len);
                	return -EINVAL;
		}

		switch (cmd) {
		case 0x00000001: // session
			handle = radeon_get_ib_value(p, p->idx + 2);
			break;

		case 0x00000002: // task info
		case 0x01000001: // create
		case 0x04000001: // config extension
		case 0x04000002: // pic control
		case 0x04000005: // rate control
		case 0x04000007: // motion estimation
		case 0x04000008: // rdo
			break;

		case 0x03000001: // encode
			r = radeon_vce_cs_reloc(p, p->idx + 10, p->idx + 9);
			if (r)
				return r;

			r = radeon_vce_cs_reloc(p, p->idx + 12, p->idx + 11);
			if (r)
				return r;
			break;

		case 0x02000001: // destroy
			destroy = true;
			break;

		case 0x05000001: // context buffer
		case 0x05000004: // video bitstream buffer
		case 0x05000005: // feedback buffer
			r = radeon_vce_cs_reloc(p, p->idx + 3, p->idx + 2);
			if (r)
				return r;
			break;

		default:
			DRM_ERROR("invalid VCE command (0x%x)!\n", cmd);
			return -EINVAL;
		}

		p->idx += len / 4;
	}

	if (destroy) {
		/* IB contains a destroy msg, free the handle */
		for (i = 0; i < RADEON_MAX_VCE_HANDLES; ++i)
			atomic_cmpxchg(&p->rdev->vce.handles[i], handle, 0);

		return 0;
        }

	/* create or encode, validate the handle */
	for (i = 0; i < RADEON_MAX_VCE_HANDLES; ++i) {
		if (atomic_read(&p->rdev->vce.handles[i]) == handle)
			return 0;
	}

	/* handle not found try to alloc a new one */
	for (i = 0; i < RADEON_MAX_VCE_HANDLES; ++i) {
		if (!atomic_cmpxchg(&p->rdev->vce.handles[i], 0, handle)) {
			p->rdev->vce.filp[i] = p->filp;
			return 0;
		}
	}

	DRM_ERROR("No more free VCE handles!\n");
	return -EINVAL;
}

/**
 * radeon_vce_semaphore_emit - emit a semaphore command
 *
 * @rdev: radeon_device pointer
 * @ring: engine to use
 * @semaphore: address of semaphore
 * @emit_wait: true=emit wait, false=emit signal
 *
 */
bool radeon_vce_semaphore_emit(struct radeon_device *rdev,
			       struct radeon_ring *ring,
			       struct radeon_semaphore *semaphore,
			       bool emit_wait)
{
	uint64_t addr = semaphore->gpu_addr;

	radeon_ring_write(ring, VCE_CMD_SEMAPHORE);
	radeon_ring_write(ring, (addr >> 3) & 0x000FFFFF);
	radeon_ring_write(ring, (addr >> 23) & 0x000FFFFF);
	radeon_ring_write(ring, 0x01003000 | (emit_wait ? 1 : 0));
	if (!emit_wait)
		radeon_ring_write(ring, VCE_CMD_END);

	return true;
}

/**
 * radeon_vce_ib_execute - execute indirect buffer
 *
 * @rdev: radeon_device pointer
 * @ib: the IB to execute
 *
 */
void radeon_vce_ib_execute(struct radeon_device *rdev, struct radeon_ib *ib)
{
	struct radeon_ring *ring = &rdev->ring[ib->ring];
	radeon_ring_write(ring, VCE_CMD_IB);
	radeon_ring_write(ring, ib->gpu_addr);
	radeon_ring_write(ring, upper_32_bits(ib->gpu_addr));
	radeon_ring_write(ring, ib->length_dw);
}

/**
 * radeon_vce_fence_emit - add a fence command to the ring
 *
 * @rdev: radeon_device pointer
 * @fence: the fence
 *
 */
void radeon_vce_fence_emit(struct radeon_device *rdev,
			   struct radeon_fence *fence)
{
	struct radeon_ring *ring = &rdev->ring[fence->ring];
	uint32_t addr = rdev->fence_drv[fence->ring].gpu_addr;

	radeon_ring_write(ring, VCE_CMD_FENCE);
	radeon_ring_write(ring, addr);
	radeon_ring_write(ring, upper_32_bits(addr));
	radeon_ring_write(ring, fence->seq);
	radeon_ring_write(ring, VCE_CMD_TRAP);
	radeon_ring_write(ring, VCE_CMD_END);
}

/**
 * radeon_vce_ring_test - test if VCE ring is working
 *
 * @rdev: radeon_device pointer
 * @ring: the engine to test on
 *
 */
int radeon_vce_ring_test(struct radeon_device *rdev, struct radeon_ring *ring)
{
	uint32_t rptr = vce_v1_0_get_rptr(rdev, ring);
	unsigned i;
	int r;

	r = radeon_ring_lock(rdev, ring, 16);
	if (r) {
		DRM_ERROR("radeon: vce failed to lock ring %d (%d).\n",
			  ring->idx, r);
		return r;
	}
	radeon_ring_write(ring, VCE_CMD_END);
	radeon_ring_unlock_commit(rdev, ring);

	for (i = 0; i < rdev->usec_timeout; i++) {
	        if (vce_v1_0_get_rptr(rdev, ring) != rptr)
	                break;
	        DRM_UDELAY(1);
	}

	if (i < rdev->usec_timeout) {
	        DRM_INFO("ring test on %d succeeded in %d usecs\n",
	                 ring->idx, i);
	} else {
	        DRM_ERROR("radeon: ring %d test failed\n",
	                  ring->idx);
	        r = -ETIMEDOUT;
	}

	return r;
}

/**
 * radeon_vce_ib_test - test if VCE IBs are working
 *
 * @rdev: radeon_device pointer
 * @ring: the engine to test on
 *
 */
int radeon_vce_ib_test(struct radeon_device *rdev, struct radeon_ring *ring)
{
	struct radeon_fence *fence = NULL;
	int r;

	r = radeon_vce_get_create_msg(rdev, ring->idx, 1, NULL);
	if (r) {
		DRM_ERROR("radeon: failed to get create msg (%d).\n", r);
		goto error;
	}

	r = radeon_vce_get_destroy_msg(rdev, ring->idx, 1, &fence);
	if (r) {
		DRM_ERROR("radeon: failed to get destroy ib (%d).\n", r);
		goto error;
	}

	r = radeon_fence_wait(fence, false);
	if (r) {
		DRM_ERROR("radeon: fence wait failed (%d).\n", r);
	} else {
	        DRM_INFO("ib test on ring %d succeeded\n", ring->idx);
	}
error:
	radeon_fence_unref(&fence);
	return r;
}
