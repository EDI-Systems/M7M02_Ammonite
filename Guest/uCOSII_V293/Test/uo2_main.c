/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-II distribution.  It belongs
*                to the uC/OS-II-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-II kernel is copyright Weston Embedded and is not
*                modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        ARMv6-M Port - uo2_main.c
*
* File      : uo2_main.c
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file is copied into a generated virtual machine as its "main.c" when the
*                project is generated without the benchmark mode.  It boots the kernel and parks
*                the idle task in the hypervisor through OSTaskIdleHook.
*********************************************************************************************************
*/

#include  <ucos_ii.h>
#include  "rvm.h"

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO    5u
#define  APP_TASK_START_STK     128u
#define  APP_TASK_PRIO          10u
#define  APP_TASK_STK           128u

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

static  OS_STK  App_TaskStartStk[APP_TASK_START_STK];
static  OS_STK  App_TaskStk[APP_TASK_STK];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg);
static  void  App_Task     (void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*********************************************************************************************************
*/

int  main (void)
{
    INT8U  os_err;

    OSInit();

    os_err = OSTaskCreateExt(App_TaskStart,
                             (void *)0,
                             &App_TaskStartStk[APP_TASK_START_STK - 1u],
                             APP_TASK_START_PRIO,
                             APP_TASK_START_PRIO,
                             &App_TaskStartStk[0],
                             APP_TASK_START_STK,
                             (void *)0,
                             OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSStart();

    return (0);
}

/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description: The startup task that creates the application tasks.
*
* Arguments  : p_arg   Unused.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    INT8U  os_err;

    (void)p_arg;

    os_err = OSTaskCreateExt(App_Task,
                             (void *)0,
                             &App_TaskStk[APP_TASK_STK - 1u],
                             APP_TASK_PRIO,
                             APP_TASK_PRIO,
                             &App_TaskStk[0],
                             APP_TASK_STK,
                             (void *)0,
                             OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskDel(OS_PRIO_SELF);
}

/*
*********************************************************************************************************
*                                          App_Task()
*
* Description: The application task - replace the body with your own code.
*
* Arguments  : p_arg   Unused.
*********************************************************************************************************
*/

static  void  App_Task (void *p_arg)
{
    (void)p_arg;

    for (;;) {
        OSTimeDly(100u);
    }
}

/*
*********************************************************************************************************
*                                       App_TaskIdleHook()
*
* Description: The idle parking hook - called by OSTaskIdleHook whenever the kernel has nothing
*              to run.  The RVM user manual requires the guest's idle path to park in the
*              hypervisor so that the core is not spun away.
*********************************************************************************************************
*/

void  App_TaskIdleHook (void)
{
    RVM_Hyp_Vct_Wait();
}

/* Empty application hooks - uC/OS-II calls these when OS_APP_HOOKS_EN is
 * set; only the idle hook carries the parking policy. */
void  App_TaskCreateHook (OS_TCB *ptcb) { (void)ptcb; }
void  App_TaskDelHook    (OS_TCB *ptcb) { (void)ptcb; }
void  App_TaskReturnHook (OS_TCB *ptcb) { (void)ptcb; }
void  App_TaskStatHook   (void)         { }
void  App_TaskSwHook     (void)         { }
void  App_TCBInitHook    (OS_TCB *ptcb) { (void)ptcb; }
