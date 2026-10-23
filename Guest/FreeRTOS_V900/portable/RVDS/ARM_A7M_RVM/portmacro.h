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

    This file implements the FreeRTOS porting layer for the ARMv7-M
    architecture under the RVM hypervisor.  It maps FreeRTOS primitives
    onto the RVM system call interface so that FreeRTOS runs as an
    unprivileged guest of the hypervisor.
*/

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *----------------------------------------------------------*/

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* Include *******************************************************************/
#include "rvm.h"
/* End Include ***************************************************************/

/* Define ********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define portCHAR                                char
#define portFLOAT                               float
#define portDOUBLE                              double
#define portLONG                                long
#define portSHORT                               short
#define portSTACK_TYPE                          uint32_t
#define portBASE_TYPE                           long

typedef portSTACK_TYPE                          StackType_t;
typedef long                                    BaseType_t;
typedef unsigned long                           UBaseType_t;

#if(configUSE_16_BIT_TICKS==1)
typedef uint16_t                                TickType_t;
#define portMAX_DELAY                           0xFFFFU
#else
typedef uint32_t                                TickType_t;
#define portMAX_DELAY                           0xFFFFFFFFUL
#define portTICK_TYPE_IS_ATOMIC                 1
#endif

#define portSTACK_GROWTH                        (-1)
#define portTICK_PERIOD_MS                      ((TickType_t)1000/configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT                      8

/* Scheduler utilities. */
#if( configUSE_RVM_OPTIMISED_YIELD != 0 )
    /* The fast-path yield in port.c - peeks the scheduler's next choice,
     * then either switches on the task stack or falls back to the
     * hypervisor yield for ICI/IT-dirty targets. */
    extern void vPortYield( void );
    #define portYIELD()                         vPortYield()
#else
    /* Yield through the hypervisor - simpler, but traps on every switch. */
    #define portYIELD()                         RVM_Virt_Yield()
#endif
#define portEND_SWITCHING_ISR(xSwitchRequired) \
do \
{ \
    if(xSwitchRequired!=pdFALSE ) \
        RVM_Virt_Yield(); \
} \
while(0)
#define portYIELD_FROM_ISR(x)                   portEND_SWITCHING_ISR(x)

/* Critical section management. */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
extern uint32_t ulPortRaiseBASEPRI( void );
extern void vPortSetBASEPRI( uint32_t ulNewMaskValue );

#define portSET_INTERRUPT_MASK_FROM_ISR()       ulPortRaiseBASEPRI()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )  vPortSetBASEPRI( x )
#define portDISABLE_INTERRUPTS()                RVM_Virt_Int_Mask()
#define portENABLE_INTERRUPTS()                 RVM_Virt_Int_Unmask()
#define portENTER_CRITICAL()                    vPortEnterCritical()
#define portEXIT_CRITICAL()                     vPortExitCritical()


#define portTASK_FUNCTION_PROTO(vFunction,pvParameters) \
                                                void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction,pvParameters) \
                                                void vFunction(void *pvParameters)

#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

/* Generic helper function. */
__attribute__( ( always_inline ) ) static inline uint8_t ucPortCountLeadingZeros( uint32_t ulBitmap )
{
uint8_t ucReturn;

    __asm volatile ( "clz %0, %1" : "=r" ( ucReturn ) : "r" ( ulBitmap ) );
    return ucReturn;
}

/* Check the configuration. */
#if( configMAX_PRIORITIES > 32 )
#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
#endif

/* Store/clear the ready priorities in a bit map. */
#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - ( uint32_t ) ucPortCountLeadingZeros( ( uxReadyPriorities ) ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/* The RVM guest cannot read the physical NVIC priority registers, so there
 * is nothing that the interrupt priority validation could check - define
 * the hook away. */
#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID()

#define portNOP()
#define portINLINE                              /* __inline */

#ifndef portFORCE_INLINE
#define portFORCE_INLINE                        /* inline __attribute__(( always_inline)) */
#endif

#ifdef __cplusplus
}
#endif
/* End Define ****************************************************************/

/* End Of File ***************************************************************/
#endif /* PORTMACRO_H */
/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/


