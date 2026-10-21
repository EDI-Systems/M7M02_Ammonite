/*
    FreeRTOS V9.0.0 default application for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the FreeRTOS distribution.  It belongs to the
    FreeRTOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.  The FreeRTOS kernel itself is copyright
    Real Time Engineers Ltd. and is not modified by this file.

    The RVM project generator copies this file into a virtual machine's
    source folder as "main.c" whenever a project is generated without the
    benchmark option.  Use it as the starting point for your own
    application.
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* RVM hypervisor interface includes. */
#include "rvm.h"

/*-----------------------------------------------------------*/

/*
 * The first user task of the virtual machine.  Replace the body of this
 * function with your own application code.
 */
static void prvMainTask( void *pvParameters );

/*-----------------------------------------------------------*/

int main( void )
{
    /* Create the first user task of the virtual machine. */
    xTaskCreate( prvMainTask, "Main", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1U ), NULL );

    /* Start the FreeRTOS scheduler.  The scheduler takes over execution of
    all the created tasks, so the following lines are never reached when
    sufficient heap memory is available. */
    vTaskStartScheduler();

    for( ;; );
}
/*-----------------------------------------------------------*/

static void prvMainTask( void *pvParameters )
{
    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

    for( ;; )
    {
        vTaskDelay( pdMS_TO_TICKS( 100U ) );
    }
}
/*-----------------------------------------------------------*/

