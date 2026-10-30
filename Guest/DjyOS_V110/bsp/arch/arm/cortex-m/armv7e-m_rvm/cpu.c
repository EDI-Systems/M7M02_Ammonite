//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: CPU service module (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: Replaces bsp/arch/arm/cortex-m/armv7-m/cortex-m3/src/cpu.c
//             for ARMv7(E)-M systems running as an unprivileged guest of
//             the RVM hypervisor.  The reset and reboot services go
//             through the hypervisor because the guest cannot reach
//             AIRCR; the debug console helpers wrap the per-character
//             print hypercall that the whole port (including the
//             kernel's printk warnings) reports through.
//Notes: Porting sensitive - every entry here is hypervisor mediated.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#include "rvm.h"
#include "stdint.h"
#include "stddef.h"
#include "cpu.h"

/* Print a zero-terminated string through the debug console hypercall.
 * Every user of this port - the kernel's early warnings, the switch
 * error trap and the benchmark - funnels its output through here. */
void djy_dbg_puts(const char *str)
{
    while(*str != '\0')
    {
        RVM_Hyp_Putchar((char)*str);
        str++;
    }
}

/* Print a single character through the debug console. */
void djy_dbg_putc(char c)
{
    RVM_Hyp_Putchar(c);
}

/* Print an unsigned 32-bit value in decimal through the debug console. */
void djy_dbg_putu32(u32 val)
{
    char buf[11];
    u32 len = 0U;

    do
    {
        buf[len] = (char)('0' + (val % 10U));
        val /= 10U;
        len++;
    }while(val != 0U);

    while(len != 0U)
    {
        len--;
        RVM_Hyp_Putchar(buf[len]);
    }
}

//----Reset the CPU------------------------------------------------------------
//Function: reset the CPU
//Input   : none
//Output  : none
//Return  : none
//Notes   : porting sensitive - the reset request is a hypercall.
//-----------------------------------------------------------------------------
void CPU_Reset(void)
{
    RVM_Hyp_Reboot();
}

//----Reboot the system--------------------------------------------------------
//Function: reboot the system
//Input   : none
//Output  : none
//Return  : none
//Notes   : porting sensitive - the reboot request is a hypercall.
//-----------------------------------------------------------------------------
void CPU_Reboot(void)
{
    RVM_Hyp_Reboot();
}

void CPU_RestartSystem(void)
{
    RVM_Hyp_Reboot();
}
