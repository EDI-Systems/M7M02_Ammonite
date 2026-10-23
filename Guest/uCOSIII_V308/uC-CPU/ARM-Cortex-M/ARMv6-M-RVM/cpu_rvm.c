/*
*********************************************************************************************************
*                                                 uC/CPU
*                                     CPU CONFIGURATION & PORT LAYER
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/CPU distribution.  It belongs
*                to the uC/OS-III-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-III and uC/CPU distributions are copyright Silicon
*                Laboratories Inc. and are not modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        ARMv6-M Port - cpu_rvm.c
*                                       RVM Hypervisor Guest Layer
*
* File      : cpu_rvm.c
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file replaces the six assembly functions of the official uC/CPU ARMv6-M port
*                (cpu_a.s): under RVM the guest cannot disable interrupts through CPSID/PRIMASK,
*                that would only mask the physical interrupts of the core, not the virtual
*                interrupts that the hypervisor delivers to this virtual machine.  All masking
*                therefore goes through the RVM virtual interrupt mask functions instead.  The
*                masking is nestable: the nesting depth is carried in the CPU_SR value that
*                CPU_SR_Save() returns, so the critical sections of the kernel remain balanced
*                even when a task switch happens while the mask is held.  The "wait for
*                interrupt" functions become stubs, because a guest WFI would halt the whole core
*                and starve every other process in the system.
*********************************************************************************************************
*/

#define  MICRIUM_SOURCE
#include  <cpu_core.h>
#include  "rvm.h"


/*
*********************************************************************************************************
*                                              LOCAL VARIABLES
*********************************************************************************************************
*/

static  CPU_SR  CPU_SR_Nesting  = 0u;                /* The current virtual interrupt mask nesting depth. */


/*
*********************************************************************************************************
*                                           DISABLE INTERRUPTS
*
* Description : Disable the virtual interrupts of this machine.
*********************************************************************************************************
*/

void  CPU_IntDis (void)
{
    if (CPU_SR_Nesting == 0u) {
        RVM_Virt_Int_Mask();
    }
    CPU_SR_Nesting++;
}


/*
*********************************************************************************************************
*                                            ENABLE INTERRUPTS
*
* Description : Re-enable the virtual interrupts of this machine.
*********************************************************************************************************
*/

void  CPU_IntEn (void)
{
    if (CPU_SR_Nesting > 0u) {
        CPU_SR_Nesting--;
        if (CPU_SR_Nesting == 0u) {
            RVM_Virt_Int_Unmask();
        }
    }
}


/*
*********************************************************************************************************
*                                       SAVE CPU STATUS WORD & DISABLE INTERRUPTS
*
* Description : Save the current mask nesting depth and disable the virtual
*               interrupts.  The depth is returned so that CPU_SR_Restore()
*               can unwind the exact number of nested critical sections.
*********************************************************************************************************
*/

CPU_SR  CPU_SR_Save (void)
{
    CPU_SR  cpu_sr;


    if (CPU_SR_Nesting == 0u) {
        RVM_Virt_Int_Mask();
    }
    cpu_sr         = CPU_SR_Nesting++;                   /* Return the depth seen on entry, not the new one. */

    return (cpu_sr);
}


/*
*********************************************************************************************************
*                                      RESTORE CPU STATUS WORD & ENABLE INTERRUPTS
*
* Description : Restore the mask nesting depth that CPU_SR_Save() captured
*               and unmask the virtual interrupts once the outermost critical
*               section has been left.
*********************************************************************************************************
*/

void  CPU_SR_Restore (CPU_SR  cpu_sr)
{
    CPU_SR_Nesting = cpu_sr;
    if (CPU_SR_Nesting == 0u) {
        RVM_Virt_Int_Unmask();
    }
}


/*
*********************************************************************************************************
*                                             WAIT FOR INTERRUPT
*
* Description : Enter the wait-for-interrupt state.  This is stubbed out: an
*               unprivileged WFI would halt the physical core and starve all
*               the other processes of the hypervisor, so the guest just
*               returns and lets the RVM idle thread sleep instead.
*********************************************************************************************************
*/

void  CPU_WaitForInt (void)
{
}


/*
*********************************************************************************************************
*                                            WAIT FOR EXCEPTION
*
* Description : Enter the wait-for-exception state - see CPU_WaitForInt().
*********************************************************************************************************
*/

void  CPU_WaitForExcept (void)
{
}
