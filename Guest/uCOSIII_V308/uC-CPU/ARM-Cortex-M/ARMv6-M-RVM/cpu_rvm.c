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
*                therefore goes through the RVM virtual interrupt mask functions instead, with
 *                the very same semantics as the official port: CPU_SR_Save() snapshots the
 *                mask flag (RVM_Int_Mask is exported by the virtlib header) and
 *                CPU_SR_Restore() writes it back unconditionally - nested sections are
 *                balanced by the snapshot, not by a counter - while CPU_IntDis()/
 *                CPU_IntEn() are the unconditioned pair, exactly like CPSID/CPSIE.  The "wait for
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

/*
*********************************************************************************************************
*                                           DISABLE INTERRUPTS
*
* Description : Disable the virtual interrupts of this machine.
*********************************************************************************************************
*/

void  CPU_IntDis (void)
{
    RVM_Virt_Int_Mask();
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
    RVM_Virt_Int_Unmask();
}


/*
*********************************************************************************************************
*                                       SAVE CPU STATUS WORD & DISABLE INTERRUPTS
*
* Description : Snapshot the current mask flag and disable the virtual
 *               interrupts, like the official MRS+CPSID pair.
*********************************************************************************************************
*/

CPU_SR  CPU_SR_Save (void)
{
    CPU_SR  cpu_sr;


    cpu_sr = (CPU_SR)RVM_Int_Mask;                      /* The flag seen on entry, not the new one. */
    if (cpu_sr == 0u) {
        RVM_Virt_Int_Mask();
    }

    return (cpu_sr);
}


/*
*********************************************************************************************************
*                                      RESTORE CPU STATUS WORD & ENABLE INTERRUPTS
*
* Description : Write the snapshot that CPU_SR_Save() captured back, like the
*               official MSR: writing back "unmasked" must take the full unmask
 *               path, or pending virtual interrupts would starve on a bare
 *               flag write.
*********************************************************************************************************
*/

void  CPU_SR_Restore (CPU_SR  cpu_sr)
{
    if (cpu_sr == 0u) {
        RVM_Virt_Int_Unmask();
    }
    else {
        RVM_Virt_Int_Mask();
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
