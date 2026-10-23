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

    OSTaskCreate(&AppTaskTCB, "App", AppTask, (void *)0u, 5u,
                 &AppTaskStk[0], 12u, 128u,
                 (OS_MSG_QTY)0u, (OS_TICK)0u, (void *)0u, OS_OPT_TASK_NONE, &os_err);

    OSStart(&os_err);

    while (DEF_ON);
}
