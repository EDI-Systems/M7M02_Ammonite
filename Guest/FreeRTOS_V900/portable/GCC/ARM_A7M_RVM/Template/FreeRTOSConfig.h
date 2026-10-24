/*
    FreeRTOS V9.0.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
    All rights reserved.

    This file is NOT part of the FreeRTOS distribution.  It belongs to the
    FreeRTOS-on-RVM test suite and is released under the Unlicense; refer
    to the LICENSE file for details.  The FreeRTOS kernel itself is
    copyright Real Time Engineers Ltd. and is not modified by this file.

    ***************************************************************************

    This file provides the FreeRTOS kernel configuration for the ARMv7-M
    RVM port.  It contains all the kernel settings used by the
    FreeRTOS-on-RVM test suite and should not require alteration in
    normal use.
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      ( 180000000U )
#define configTICK_RATE_HZ                      ( ( TickType_t ) 100 )
#define configMAX_PRIORITIES                    ( 5 )
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 256 )
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 40 * 1024 ) )
#define configMAX_TASK_NAME_LEN                 ( 9 )
#define configUSE_TRACE_FACILITY                1
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               8
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configGENERATE_RUN_TIME_STATS           0

/* The generic task selection is required: the scheduler lookahead in
 * tasksrvm.c mirrors exactly this selection mode, and the ARMv7-M fast
 * yield depends on that lookahead to route the ICI/IT-dirty frames
 * through the slow path. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Route portYIELD() through the port's fast-path routine in port.c, which
 * peeks the scheduler's next choice and switches tasks directly on the
 * task stack, without a trap into the hypervisor - unless the target
 * holds interrupted IT block or LDM/STM state, which only a real
 * exception return can restore, in which case the slow hypervisor yield
 * is taken for that switch.  Set this to 0 to take the RVM yield
 * hypercall on every switch instead, which is simpler but slower. */
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
standard names - or at least those used in the unmodified vector table.
None of these are used here: the port registers its handlers with the
hypervisor through RVM_Virt_Tim_Reg()/RVM_Virt_Ctx_Reg() instead. */
//#define vPortSVCHandler SVC_Handler
//#define xPortPendSVHandler PendSV_Handler
//#define xPortSysTickHandler SysTick_Handler

/* Close time slice schedule. */
#define configUSE_TIME_SLICING                  0
/* Disable stack overflow detection. */
#define configCHECK_FOR_STACK_OVERFLOW          0
/* Disable hook function when Malloc fails. */
#define configUSE_MALLOC_FAILED_HOOK            0

#endif /* FREERTOS_CONFIG_H */
