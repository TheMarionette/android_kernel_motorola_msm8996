#ifndef _ASM_SPARC64_DMA_MAPPING_H
#define _ASM_SPARC64_DMA_MAPPING_H

#include <linux/scatterlist.h>
#include <linux/mm.h>

struct dma_ops {
	void *(*alloc_coherent)(struct device *dev, size_t size,
				dma_addr_t *dma_handle, gfp_t flag);
	void (*free_coherent)(struct device *dev, size_t size,
			      void *cpu_addr, dma_addr_t dma_handle);
	dma_addr_t (*map_single)(struct device *dev, void *cpu_addr,
				 size_t size,
				 enum dma_data_direction direction);
	void (*unmap_single)(struct device *dev, dma_addr_t dma_addr,
			     size_t size,
			     enum dma_data_direction direction);
	int (*map_sg)(struct device *dev, struct scatterlist *sg, int nents,
		      enum dma_data_direction direction);
	void (*unmap_sg)(struct device *dev, struct scatterlist *sg,
			 int nhwentries,
			 enum dma_data_direction direction);
	void (*sync_single_for_cpu)(struct device *dev,
				    dma_addr_t dma_handle, size_t size,
				    enum dma_data_direction direction);
	void (*sync_single_for_device)(struct device *dev,
				       dma_addr_t dma_handle, size_t size,
				       enum dma_data_direction direction);
	void (*sync_sg_for_cpu)(struct device *dev, struct scatterlist *sg,
				int nelems,
				enum dma_data_direction direction);
	void (*sync_sg_for_device)(struct device *dev,
				   struct scatterlist *sg, int nents,
				   enum dma_data_direction dir);
};
extern const struct dma_ops *dma_ops;

static inline void *dma_alloc_coherent(struct device *dev, size_t size,
				       dma_addr_t *dma_handle, gfp_t flag)
{
	return dma_ops->alloc_coherent(dev, size, dma_handle, flag);
}

static inline void dma_free_coherent(struct device *dev, size_t size,
				     void *cpu_addr, dma_addr_t dma_handle)
{
	dma_ops->free_coherent(dev, size, cpu_addr, dma_handle);
}

static inline dma_addr_t dma_map_single(struct device *dev, void *cpu_addr,
					size_t size,
					enum dma_data_direction direction)
{
	return dma_ops->map_single(dev, cpu_addr, size, direction);
}

static inline void dma_unmap_single(struct device *dev, dma_addr_t dma_addr,
				    size_t size,
				    enum dma_data_direction direction)
{
	dma_ops->unmap_single(dev, dma_addr, size, direction);
}

static inline dma_addr_t dma_map_page(struct device *dev, struct page *page,
				      unsigned long offset, size_t size,
				      enum dma_data_direction direction)
{
	return dma_ops->map_single(dev, page_address(page) + offset,
				   size, direction);
}

static inline void dma_unmap_page(struct device *dev, dma_addr_t dma_address,
				  size_t size,
				  enum dma_data_direction direction)
{
	dma_ops->unmap_single(dev, dma_address, size, direction);
}

static inline int dma_map_sg(struct device *dev, struct scatterlist *sg,
			     int nents, enum dma_data_direction direction)
{
	return dma_ops->map_sg(dev, sg, nents, direction);
}

static inline void dma_unmap_sg(struct device *dev, struct scatterlist *sg,
				int nents, enum dma_data_direction direction)
{
	dma_ops->unmap_sg(dev, sg, nents, direction);
}

static inline void dma_sync_single_for_cpu(struct device *dev,
					   dma_addr_t dma_handle, size_t size,
					   enum dma_data_direction direction)
{
	dma_ops->sync_single_for_cpu(dev, dma_handle, size, direction);
}

static inline void dma_sync_single_for_device(struct device *dev,
					      dma_addr_t dma_handle,
					      size_t size,
					      enum dma_data_direction direction)
{
	/* No flushing needed to sync cpu writes to the device.  */
}

static inline void dma_sync_sg_for_cpu(struct device *dev,
				       struct scatterlist *sg, int nelems,
				       enum dma_data_direction direction)
{
	dma_ops->sync_sg_for_cpu(dev, sg, nelems, direction);
}

static inline void dma_sync_sg_for_device(struct device *dev,
					  struct scatterlist *sg, int nelems,
					  enum dma_data_direction direction)
{
	/* No flushing needed to sync cpu writes to the device.  */
}

#endif /* _ASM_SPARC64_DMA_MAPPING_H */
