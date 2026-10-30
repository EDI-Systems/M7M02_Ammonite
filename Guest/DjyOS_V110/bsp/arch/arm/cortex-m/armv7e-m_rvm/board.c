//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Board support (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: Replaces the BSP board file for the DjyOS-on-RVM port:
//             runs the kernel's SYS_Start sequence, registers the
//             deferred context switch handler and the tick with the
//             hypervisor, and hands the CPU to the scheduler.  Only the
//             glue the kernel itself asks for lives here - Board_Init,
//             Sys_ModuleInit and __DjyInitTick are linked by the
//             kernel's own SYS_Start - so a plain application carries
//             no benchmark timer, no interrupt source and no test
//             header at all.  The benchmark's hardware stays in
//             test/djy_board.c, which only benchmark builds compile.
//Notes: The RVM virtual timer is the only time source the port spends;
//       a guest cannot own the physical SysTick.
//Revision history:
//1. Date: 2026-10-30
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Split out of test/djy_board.c so the plain application does
//           not depend on the benchmark hardware.
//----------------------------------------------------

#include "rvm.h"
#include "djyos.h"
#include "int.h"
#include "systime.h"
#include "cpu.h"

/* The port's register-transfer helpers. */
extern void djy_dbg_puts(const char *str);
extern void djy_port_arm(void);
extern void djy_lp_module_init(void);
extern void Djy_Ctx_Handler(void);

/* Kernel boot entry points, in the official SYS_Start order. */
extern void Heap_StaticModuleInit(void);
extern void __DJY_InitSys(void);
extern ptu32_t __InitMB(void);
extern ptu32_t __InitLock(void);
extern s32 OBJ_ModuleInit(void);
extern s32 Handle_ModuleInit(void);
extern s32 Lock_CreateObject(void);
extern s32 Mb_CreateObject(void);
extern void __CppRuntime(void);
extern void __DJY_StartOs(void);
extern bool_t ModuleInstall_MsgQ(void);

/* The tick fallback: no precise hardware timer is connected, so the
 * microsecond time reads fall back to tick counting. */
extern s64 g_s64OsTicks;
s64 __DjyGetSysTime(void)
{
    return g_s64OsTicks*(s64)CN_CFG_TICK_US;
}

/* The tick source of this port is the RVM virtual timer registered in
 * Board_Init; there is no hardware systick to start here. */
void __DjyInitTick(void)
{
}

/*
 * The virtual timer interrupt: the hypervisor delivers it at the rvp
 * period; the kernel advances by exactly one tick per delivery - the
 * periodic-tick ancestor mode of the kernel, which is the natural fit
 * for the fixed-interval virtual timer.  The tickless overlay is left
 * unregistered (op == NULL), all its call sites are silent no-ops.
 */
static volatile u32 djy_boot_done;
static volatile u32 djy_tick_count;

/* The kernel tick - the RVM virtual timer channel, the same source the
 * proven RMP, FreeRTOS and RT-Thread guests use on this hypervisor.
 * One delivery is one kernel tick, exactly the SysTick handler shape
 * of the official DjyOS BSP. */
void djy_tim_handler(void);
void djy_tim_handler(void)
{
    if(djy_boot_done == 0U)
        return;             /* boot phase: the kernel is not ready */
    if(djy_tick_count == 0U)
        djy_dbg_puts("[tk1]");
    djy_tick_count++;
    if(djy_tick_count == 100U)
        djy_dbg_puts("[tk100]");
    DJY_ScheduleIsr(1U);
}

/* Re-arm the hypervisor handler table.  Idempotent: the vector thread
 * runs RVM_Virt_Init at boot before this process's main, so this is a
 * safety net for restarts, not a workaround - it never touches the
 * interrupt enable or the virtual mask, which belong to the kernel's
 * own critical section bookkeeping. */
void djy_arm_handlers(void)
{
    djy_port_arm();
    RVM_Virt_Tim_Reg(djy_tim_handler);
}

/*
 * Board_Init - the SYS_Start call site: register the guest side hooks of
 * the low power machinery (the idle event parks through it) and the
 * deferred context switch handler plus the tick with the hypervisor.
 */
void Board_Init(void)
{
    djy_lp_module_init();
    djy_arm_handlers();
}

/*
 * Sys_ModuleInit - the SYS_Start call site for the optional modules.
 * The message queue is the only kernel extension this port installs;
 * everything else (shell, black box, stdio, device files) stays out.
 */
void Sys_ModuleInit(void)
{
    ModuleInstall_MsgQ();
}

/*
 * djy_port_boot - the kernel's SYS_Start sequence up to the point where
 * the application may add its own pre-start work.  The kernel's own
 * SYS_Start cannot be reused because it enters the scheduler directly,
 * while an application may need to register event types first.
 */
void djy_port_boot(void)
{
    djy_dbg_puts("\r\nDjyOS V1.1.0 on RVM - starting boot sequence.\r\n");

    Heap_StaticModuleInit();
    djy_dbg_puts("[1]");
    __DjyInitTick();
    djy_dbg_puts("[2]");
    __DJY_InitSys();
    djy_dbg_puts("[3]");
    __InitMB();
    djy_dbg_puts("[4]");
    __InitLock();
    djy_dbg_puts("[5]");
    OBJ_ModuleInit();
    Handle_ModuleInit();
    Lock_CreateObject();
    Mb_CreateObject();
    djy_dbg_puts("[6]");
    __CppRuntime();
    Board_Init();
    Sys_ModuleInit();
}

/*
 * djy_port_start - open the tick gate and enter the scheduler.  From
 * here on every timer delivery advances the kernel by one tick.  The
 * startup context is abandoned inside __DJY_StartOs.
 */
void djy_port_start(void)
{
    djy_boot_done = 1U;

    __DJY_StartOs();        /* never returns */

    /* If control ever falls through to here, the startup context has
     * been resurrected by a fault-recovery SVC return: spinning would
     * hold the user thread runnable forever and the restart the kernel
     * armed in RVM_REG would never happen.  Block in the hypervisor
     * instead - the scheduler then hands the thread to its restarted
     * entry point and the reboot completes. */
    for(;;)
        RVM_Virt_Yield();
}

/* End Of File ***************************************************************/
