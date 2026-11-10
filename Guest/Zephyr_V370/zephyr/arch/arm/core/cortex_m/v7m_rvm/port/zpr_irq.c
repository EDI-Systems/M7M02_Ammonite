/* SPDX-License-Identifier: Unlicense */

#include <stdint.h>

#include "rvm.h"

static uint32_t zpr_irq_nest;

unsigned int zpr_arch_irq_lock(void)
{
	unsigned int key = zpr_irq_nest;

	++zpr_irq_nest;
	if (key == 0U) {
		RVM_Virt_Int_Mask();
	}

	return key;
}

void zpr_arch_irq_unlock(unsigned int key)
{
	zpr_irq_nest = key;
	if (key == 0U) {
		RVM_Virt_Int_Unmask();
	}
}

void zpr_irq_restore_nest(unsigned int level)
{
	zpr_irq_nest = level;
	if (level == 0U) {
		RVM_Virt_Int_Unmask();
	} else {
		RVM_Virt_Int_Mask();
	}
}

void _RVM_CPU_ISR_Set_level(unsigned int level)
{
	zpr_irq_restore_nest(level);
}

unsigned int zpr_irq_get_nest(void)
{
	return zpr_irq_nest;
}
