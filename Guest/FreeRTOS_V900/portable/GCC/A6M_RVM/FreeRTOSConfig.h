/*
    FreeRTOS V9.0.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
    All rights reserved

    This file is NOT part of the FreeRTOS distribution.  It belongs to the
    FreeRTOS-on-RVM test suite and is released under the Unlicense; refer
    to the LICENSE file for details.  The FreeRTOS kernel itself is
    copyright Real Time Engineers Ltd. and is not modified by this file.

    FreeRTOS is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License (version 2) as
    published by the Free Software Foundation >>>> AND MODIFIED BY <<<<
    the FreeRTOS exception.

    ***************************************************************************

    This file provides the FreeRTOS kernel configuration for the ARMv6-M
    RVM port.  It contains all the kernel settings used by the
    FreeRTOS-on-RVM test suite and should not require alteration in
    normal use.
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */
#ifdef __ICCARM__
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
#endif

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      ( 64000000U )
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                    ( 5 )
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 60 )
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 8 * 1024 ) )
#define configMAX_TASK_NAME_LEN                 ( 5 )
#define configUSE_TRACE_FACILITY                1
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               8
//#define configCHECK_FOR_STACK_OVERFLOW    2
#define configUSE_RECURSIVE_MUTEXES             1
//#define configUSE_MALLOC_FAILED_HOOK  1
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_COUNTING_SEMAPHORES           1
#define configGENERATE_RUN_TIME_STATS           0

/* Route portYIELD() through the port's fast-path assembly routine, which
 * switches tasks directly on the task stack, without a trap into the
 * hypervisor.  Set this to 0 to take the RVM yield hypercall on every
 * switch instead, which is simpler but slower. */
#define configUSE_RVM_OPTIMISED_YIELD           1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS                        0
#define configTIMER_TASK_PRIORITY               ( 2 )
#define configTIMER_QUEUE_LENGTH                5
#define configTIMER_TASK_STACK_DEPTH            ( 80 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) \
do \
{ \
    if( ( x ) == 0 ) \
    { \
        taskDISABLE_INTERRUPTS(); \
        for( ;; ); \
    } \
}while(0)

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names - or at least those used in the unmodified vector table. */
//#define vPortSVCHandler SVC_Handler
//#define xPortPendSVHandler PendSV_Handler
//#define xPortSysTickHandler SysTick_Handler

/* Close time slice schedule */
#define configUSE_TIME_SLICING                  0
/* Disable stack overflow detection */
#define configCHECK_FOR_STACK_OVERFLOW          0
/* Disable hook function when Malloc fails */
#define configUSE_MALLOC_FAILED_HOOK            0

#endif /* FREERTOS_CONFIG_H */
