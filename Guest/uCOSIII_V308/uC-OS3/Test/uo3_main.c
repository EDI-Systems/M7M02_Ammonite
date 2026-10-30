/*
*********************************************************************************************************
*                                               uC/OS-III
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-III distribution.  It belongs
*                to the uC/OS-III-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-III and uC/CPU distributions are copyright Silicon
*                Laboratories Inc. and are not modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           MAIN FILE TEMPLATE
*                                          RVM Hypervisor Guest
*
* File      : uo3_main.c
* Version   : V1.00.00
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Note(s)  : (1) This file is copied into a generated virtual machine as its "main.c" when the
*                project is generated without the benchmark mode.  It boots the kernel and starts
*                a single application task that does nothing but delay.
*********************************************************************************************************
*/

#include  "os.h"
#include  "rvm.h"

/*-----------------------------------------------------------*/

static  OS_TCB   AppTaskTCB;
static  CPU_STK  AppTaskStk[128];


/*
*********************************************************************************************************
*                                               APPLICATION TASK
*
* Description: A minimal application task that does nothing but delay.
*
* Arguments  : p_arg    The task argument, unused.
*
* Returns    : none.
*********************************************************************************************************
*/

static  void  AppTask (void *p_arg)
{
    OS_ERR  os_err;


    (void)p_arg;

    while (DEF_ON) {
        OSTimeDly(1u, OS_OPT_TIME_DLY, &os_err);
    }
}


/*
*********************************************************************************************************
*                                               IDLE HOOK
*
* Description: Called by OSIdleTaskHook() from the idle task.  The RVM user manual requires the guest's
*              idle thread to park in the hypervisor so that the core is not spun away while there is
*              nothing to run.  RVM_Hyp_Vct_Wait() blocks the user thread until the next virtual
*              interrupt is delivered and resumes it right here; it is transparent to the kernel and
*              never blocks in the uC/OS-III sense.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

static  void  AppIdleHook (void)
{
    RVM_Hyp_Vct_Wait();
}


/*
*********************************************************************************************************
*                                                   MAIN
*
* Description: Boot the kernel and start a single application task.
*
* Arguments  : none.
*
* Returns    : none.
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  os_err;


    OSInit(&os_err);

#if (OS_CFG_APP_HOOKS_EN > 0u)
    /* The idle policy belongs to the application: the port only forwards
     * OSIdleTaskHook() to this pointer. */
    OS_AppIdleTaskHookPtr = AppIdleHook;
#endif

    OSTaskCreate(&AppTaskTCB, "App", AppTask, (void *)0u, 5u,
                 &AppTaskStk[0], 12u, 128u,
                 (OS_MSG_QTY)0u, (OS_TICK)0u, (void *)0u, OS_OPT_TASK_NONE, &os_err);

    OSStart(&os_err);

    while (DEF_ON);
}
