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

    This file implements the FreeRTOS porting layer for the ARMv6-M
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

/* Type definitions. */
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

#if( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t                                TickType_t;
#define portMAX_DELAY                           ( TickType_t ) (0xFFFFU)
#else
typedef uint32_t                                TickType_t;
#define portMAX_DELAY                           ( TickType_t ) (0xFFFFFFFFUL)
#define portTICK_TYPE_IS_ATOMIC                 1
#endif

/* Architecture specifics. */
#define portSTACK_GROWTH                        (-1)
#define portTICK_PERIOD_MS                      ((TickType_t)1000/configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT                      8

/* Scheduler utilities. */
#if( configUSE_RVM_OPTIMISED_YIELD != 0 )
    /* The fast-path assembly routine - see portasm.s. */
    extern void vPortYield( void );
    #define portYIELD()                         vPortYield()
#else
    /* Yield through the hypervisor - simpler, but traps on every switch. */
    #define portYIELD()                         RVM_Virt_Yield()
#endif
#define portEND_SWITCHING_ISR(xSwitchRequired) \
do \
{ \
    if( xSwitchRequired != pdFALSE ) \
    { \
        RVM_Virt_Yield(); \
    } \
} \
while( 0 )
#define portYIELD_FROM_ISR( x )                 portEND_SWITCHING_ISR( x )

/* Critical section management. */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
extern uint32_t ulPortRaiseBASEPRI( void );

#define portSET_INTERRUPT_MASK_FROM_ISR()       ulPortRaiseBASEPRI()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )  ( ( void ) ( x ) )
#define portDISABLE_INTERRUPTS()                RVM_Virt_Int_Mask()
#define portENABLE_INTERRUPTS()                 RVM_Virt_Int_Unmask()
#define portENTER_CRITICAL()                    vPortEnterCritical()
#define portEXIT_CRITICAL()                     vPortExitCritical()

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO(vFunction,pvParameters) \
                                                void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction,pvParameters) \
                                                void vFunction(void *pvParameters)

#define portNOP()                               __asm volatile( "nop" )


#ifdef __cplusplus
}
#endif
/* End Define ****************************************************************/

/* End Of File ***************************************************************/
#endif /* PORTMACRO_H */
/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
