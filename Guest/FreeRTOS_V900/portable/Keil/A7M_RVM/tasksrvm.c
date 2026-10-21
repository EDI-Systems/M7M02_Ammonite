/*
 * FreeRTOS V9.0.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved.
 *
 * This file is NOT part of the FreeRTOS distribution.  It belongs to the
 * FreeRTOS-on-RVM test suite and is released under the Unlicense; refer
 * to the LICENSE file for details.  The FreeRTOS kernel itself is
 * copyright Real Time Engineers Ltd. and is not modified by this file.
 *
 * ***************************************************************************

 * This file provides the scheduler lookahead used by the ARMv7-M RVM port
 * fast yield path, WITHOUT modifying any FreeRTOS kernel source file.
 *
 * The ARMv7-M EPSR carries the IT block and the interruptible-continuable
 * LDM/STM state, which cannot be read nor restored by thread mode
 * software - only a real exception return can put it back.  The fast path
 * therefore has to know, BEFORE switching, whether the target task holds
 * such a dirty frame, so it can fall back to the hypervisor's slow path
 * (a real exception return) for that task.  Answering this requires a
 * non-committing peek of the scheduler's next choice, which needs access
 * to the ready lists held as file statics in tasks.c.
 *
 * Compiling this file INSTEAD OF tasks.c exposes those statics to the
 * peek function appended below, while the kernel source itself stays
 * untouched.  Do not add tasks.c to the build alongside this file - every
 * kernel symbol would be defined twice.
 *
 * The peek mirrors the generic (configUSE_PORT_OPTIMISED_TASK_SELECTION
 * == 0) taskSELECT_HIGHEST_PRIORITY_TASK() of FreeRTOS V9.0.0: the same
 * top-priority scan, but reading pxIndex->pxNext->pvOwner instead of
 * advancing the round-robin cursor and committing pxCurrentTCB.  It must
 * be called with the scheduler running (not suspended) and with the RVM
 * virtual interrupts masked, so that the scheduler state cannot change
 * between the peek and the real vTaskSwitchContext() that follows - the
 * two are then guaranteed to select the very same task.
 */

/*-----------------------------------------------------------*/

/* Pull in the whole unmodified kernel through the back door. */
#include "../../../tasks.c"

/*-----------------------------------------------------------*/

/*
 * Peeks the task that the next vTaskSwitchContext() would select, without
 * committing any scheduler state: neither the round-robin list cursor is
 * advanced, nor pxCurrentTCB is written.
 */
TaskHandle_t xTaskPeekNextToRun( void )
{
const List_t * pxList;
const ListItem_t * pxNextItem;
UBaseType_t uxTopPriority = uxTopReadyPriority;

    /* Mirror the selection mode this peek was written for. */
    configASSERT( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 );

    /* Find the highest priority list that holds ready tasks, exactly like
     * taskSELECT_HIGHEST_PRIORITY_TASK() does. */
    while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopPriority ] ) ) != pdFALSE )
    {
        configASSERT( uxTopPriority );
        uxTopPriority--;
    }

    pxList = &( pxReadyTasksLists[ uxTopPriority ] );

    /* The read-only equivalent of listGET_OWNER_OF_NEXT_ENTRY(): the next
     * owner is the item the cursor would advance to, INCLUDING the V9
     * wrap-around check - when that lands on the list end marker, it takes
     * one more step, because the marker's pvOwner slot is just padding. */
    pxNextItem = pxList->pxIndex->pxNext;
    if( pxNextItem == ( const ListItem_t * ) &( pxList->xListEnd ) )
    {
        pxNextItem = pxNextItem->pxNext;
    }

    return ( TaskHandle_t ) ( pxNextItem->pvOwner );
}
/*-----------------------------------------------------------*/
