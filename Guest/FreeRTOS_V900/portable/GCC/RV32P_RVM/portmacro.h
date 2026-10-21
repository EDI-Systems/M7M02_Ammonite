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

    This file implements the FreeRTOS porting layer for the RV32
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

/*-----------------------------------------------------------
 * Coprocessor selection, copied from the RVM project generator's
 * per-machine "prc_*.h" header.
 *----------------------------------------------------------*/
#define configUSE_PORT_RVF                        RVM_RV32P_COP_RVF
#define configUSE_PORT_RVD                        RVM_RV32P_COP_RVD

#if( ( configUSE_PORT_RVF != 0 ) || ( configUSE_PORT_RVD != 0 ) ) && ( RVM_COP_NUM == 0U )
    #error The hypervisor must have coprocessor support enabled (RVM_COP_NUM) for the FPU ports.
#endif
/*-----------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#if __riscv_xlen == 64
    #define portSTACK_TYPE                      uint64_t
    #define portBASE_TYPE                       int64_t
    #define portUBASE_TYPE                      uint64_t
    #define portMAX_DELAY                       ( TickType_t ) 0xffffffffffffffffUL
    #define portPOINTER_SIZE_TYPE               uint64_t
#elif __riscv_xlen == 32
    #define portSTACK_TYPE                      uint32_t
    #define portBASE_TYPE                       int32_t
    #define portUBASE_TYPE                      uint32_t
    #define portMAX_DELAY                       ( TickType_t ) 0xffffffffUL
#else
    #error Assembler did not define __riscv_xlen
#endif


typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE BaseType_t;
typedef portUBASE_TYPE UBaseType_t;
typedef portUBASE_TYPE TickType_t;

/* Legacy type definitions. */
#define portCHAR                                char
#define portFLOAT                               float
#define portDOUBLE                              double
#define portLONG                                long
#define portSHORT                               short

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC                 1
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH                        ( -1 )
#define portTICK_PERIOD_MS                      ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#ifdef __riscv64
    #error This is the RV32 port that has not yet been adapted for 64.
    #define portBYTE_ALIGNMENT                  16
#else
    #define portBYTE_ALIGNMENT                  16
#endif
/*-----------------------------------------------------------*/


/* Scheduler utilities. */
#if( configUSE_RVM_OPTIMISED_YIELD != 0 )
    /* The fast-path assembly routines - see portasm.s. */
    #if( configUSE_PORT_RVD != 0 )
        extern void vPortYieldRVD( void );
        #define portYIELD() \
        do \
        { \
            vPortYieldRVD(); \
            __asm__ __volatile__("fence iorw,iorw":::"memory"); \
        } \
        while(0)
    #elif( configUSE_PORT_RVF != 0 )
        extern void vPortYieldRVF( void );
        #define portYIELD() \
        do \
        { \
            vPortYieldRVF(); \
            __asm__ __volatile__("fence iorw,iorw":::"memory"); \
        } \
        while(0)
    #else
        extern void vPortYield( void );
        #define portYIELD() \
        do \
        { \
            vPortYield(); \
            __asm__ __volatile__("fence iorw,iorw":::"memory"); \
        } \
        while(0)
    #endif
#else
    /* Yield through the hypervisor - simpler, but traps on every switch. */
    #define portYIELD() \
    do \
    { \
        RVM_Virt_Yield(); \
        __asm__ __volatile__("fence iorw,iorw":::"memory"); \
    } \
    while(0)
#endif

#define portEND_SWITCHING_ISR( xSwitchRequired ) do { if( xSwitchRequired ) RVM_Virt_Yield(); } while( 0 )
#define portYIELD_FROM_ISR( x )                 portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/


/* Critical section management. */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
extern portUBASE_TYPE xPortSetInterruptMask(void);

#define portSET_INTERRUPT_MASK_FROM_ISR()       xPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedStatusValue )
#define portDISABLE_INTERRUPTS()                RVM_Virt_Int_Mask()
#define portENABLE_INTERRUPTS()                 RVM_Virt_Int_Unmask()
#define portENTER_CRITICAL()                    vPortEnterCritical()
#define portEXIT_CRITICAL()                     vPortExitCritical()
/*-----------------------------------------------------------*/


/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
    #define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if( configUSE_PORT_OPTIMISED_TASK_SELECTION == 1 )

    /* Check the configuration. */
    #if( configMAX_PRIORITIES > 32 )
        #error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
    #endif

    /* Store/clear the ready priorities in a bit map. */
    #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
    #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

    /*-----------------------------------------------------------*/

    #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - __builtin_clz( uxReadyPriorities ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */


/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not necessary for to use this port.  They are defined so the common demo files
(which build with all the ports) will build. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/*-----------------------------------------------------------*/

#define portNOP()                               __asm volatile( " nop " )

#define portINLINE                              __inline

#ifndef portFORCE_INLINE
    #define portFORCE_INLINE                    inline __attribute__(( always_inline))
#endif

#define portMEMORY_BARRIER()                    __asm volatile( "" ::: "memory" )
/*-----------------------------------------------------------*/


/* configCLINT_BASE_ADDRESS is a legacy definition that was replaced by the
configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS definitions.  For
backward compatibility derive the newer definitions from the old if the old
definition is found. */
#if defined( configCLINT_BASE_ADDRESS ) && !defined( configMTIME_BASE_ADDRESS ) && ( configCLINT_BASE_ADDRESS == 0 )
    /* Legacy case where configCLINT_BASE_ADDRESS was defined as 0 to indicate
    there was no CLINT.  Equivalent now is to set the MTIME and MTIMECMP
    addresses to 0. */
    #define configMTIME_BASE_ADDRESS            ( 0 )
    #define configMTIMECMP_BASE_ADDRESS         ( 0 )
#elif defined( configCLINT_BASE_ADDRESS ) && !defined( configMTIME_BASE_ADDRESS )
    /* Legacy case where configCLINT_BASE_ADDRESS was set to the base address of
    the CLINT.  Equivalent now is to derive the MTIME and MTIMECMP addresses
    from the CLINT address. */
    #define configMTIME_BASE_ADDRESS            ( ( configCLINT_BASE_ADDRESS ) + 0xBFF8UL )
    #define configMTIMECMP_BASE_ADDRESS         ( ( configCLINT_BASE_ADDRESS ) + 0x4000UL )
#elif !defined( configMTIME_BASE_ADDRESS ) || !defined( configMTIMECMP_BASE_ADDRESS )
    #error configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  Set them to zero if there is no MTIME (machine time) clock.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif



#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

