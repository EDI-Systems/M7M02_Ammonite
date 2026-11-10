/* SPDX-License-Identifier: Unlicense */

#include "rvm.h"

int arch_printk_char_out(int c)
{
	if (c == '\n') {
		RVM_Hyp_Putchar('\r');
	}

	RVM_Hyp_Putchar((char) c);
	return c;
}
