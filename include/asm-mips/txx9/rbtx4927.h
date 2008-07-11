/*
 * Author: MontaVista Software, Inc.
 *         source@mvista.com
 *
 * Copyright 2001-2002 MontaVista Software Inc.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __ASM_TXX9_RBTX4927_H
#define __ASM_TXX9_RBTX4927_H

#include <asm/txx9/tx4927.h>

#define RBTX4927_PCIMEM		0x08000000
#define RBTX4927_PCIMEM_SIZE	0x08000000
#define RBTX4927_PCIIO		0x16000000
#define RBTX4927_PCIIO_SIZE	0x01000000

#define rbtx4927_pcireset_addr	((__u8 __iomem *)0xbc00f006UL)

/* bits for ISTAT/IMASK/IMSTAT */
#define RBTX4927_INTB_PCID	0
#define RBTX4927_INTB_PCIC	1
#define RBTX4927_INTB_PCIB	2
#define RBTX4927_INTB_PCIA	3
#define RBTX4927_INTF_PCID	(1 << RBTX4927_INTB_PCID)
#define RBTX4927_INTF_PCIC	(1 << RBTX4927_INTB_PCIC)
#define RBTX4927_INTF_PCIB	(1 << RBTX4927_INTB_PCIB)
#define RBTX4927_INTF_PCIA	(1 << RBTX4927_INTB_PCIA)

#define RBTX4927_NR_IRQ_IOC	8	/* IOC */

#define RBTX4927_IRQ_IOC	(TXX9_IRQ_BASE + TX4927_NUM_IR)
#define RBTX4927_IRQ_IOC_PCID	(RBTX4927_IRQ_IOC + RBTX4927_INTB_PCID)
#define RBTX4927_IRQ_IOC_PCIC	(RBTX4927_IRQ_IOC + RBTX4927_INTB_PCIC)
#define RBTX4927_IRQ_IOC_PCIB	(RBTX4927_IRQ_IOC + RBTX4927_INTB_PCIB)
#define RBTX4927_IRQ_IOC_PCIA	(RBTX4927_IRQ_IOC + RBTX4927_INTB_PCIA)

#define RBTX4927_IRQ_IOCINT	(TXX9_IRQ_BASE + TX4927_IR_INT(1))

#ifdef CONFIG_PCI
#define RBTX4927_ISA_IO_OFFSET RBTX4927_PCIIO
#else
#define RBTX4927_ISA_IO_OFFSET 0
#endif

#define RBTX4927_SW_RESET_DO         (void __iomem *)0xbc00f000UL
#define RBTX4927_SW_RESET_DO_SET                0x01

#define RBTX4927_SW_RESET_ENABLE     (void __iomem *)0xbc00f002UL
#define RBTX4927_SW_RESET_ENABLE_SET            0x01

#define RBTX4927_RTL_8019_BASE (0x1c020280 - RBTX4927_ISA_IO_OFFSET)
#define RBTX4927_RTL_8019_IRQ  (TXX9_IRQ_BASE + TX4927_IR_INT(3))

void rbtx4927_prom_init(void);
void rbtx4927_irq_setup(void);
struct pci_dev;
int rbtx4927_pci_map_irq(const struct pci_dev *dev, u8 slot, u8 pin);

#endif /* __ASM_TXX9_RBTX4927_H */
