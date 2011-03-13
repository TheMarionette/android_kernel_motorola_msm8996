#include "vb_def.h"
#include "vgatypes.h"
#include "vb_struct.h"

#include "XGIfb.h"
#include <asm/io.h>
#include <linux/types.h>

#include "vb_util.h"

void xgifb_reg_set(unsigned long port, unsigned short index,
		unsigned short data)
{
	outb(index, port);
	outb(data, port + 1);
}

unsigned char xgifb_reg_get(unsigned long port, unsigned short index)
{
	unsigned char data;

	outb(index, port);
	data = inb(port + 1);
	return data;
}

void xgifb_reg_and_or(unsigned long Port, unsigned short Index,
		unsigned short DataAND, unsigned short DataOR)
{
	unsigned short temp;

	temp = xgifb_reg_get(Port, Index); /* XGINew_Part1Port index 02 */
	temp = (temp & (DataAND)) | DataOR;
	xgifb_reg_set(Port, Index, temp);
}

void xgifb_reg_and(unsigned long Port, unsigned short Index,
		unsigned short DataAND)
{
	unsigned short temp;

	temp = xgifb_reg_get(Port, Index); /* XGINew_Part1Port index 02 */
	temp &= DataAND;
	xgifb_reg_set(Port, Index, temp);
}

void xgifb_reg_or(unsigned long Port, unsigned short Index,
		unsigned short DataOR)
{
	unsigned short temp;

	temp = xgifb_reg_get(Port, Index); /* XGINew_Part1Port index 02 */
	temp |= DataOR;
	xgifb_reg_set(Port, Index, temp);
}
