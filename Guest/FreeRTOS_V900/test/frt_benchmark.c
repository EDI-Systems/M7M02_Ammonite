/*
    FreeRTOS V9.0.0 benchmark application for the RVM hypervisor.
    Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.

    This file is NOT part of the FreeRTOS distribution.  It belongs to the
    FreeRTOS-on-RVM test suite and is released under the Unlicense; refer to
    the LICENSE file for details.  The FreeRTOS kernel itself is copyright
    Real Time Engineers Ltd. and is not modified by this file.

    This file measures the performance of the FreeRTOS kernel when it runs
    as a guest of the RVM hypervisor.  All the hardware dependent code is
    held in the chip specific test header that is selected by the generated
    "frt_test.h" header, so this file does not have to be modified when the
    benchmark is ported to a new chip.
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* Benchmark includes. */
#include "frt_test.h"

/*-----------------------------------------------------------*/

/* The number of rounds each test is repeated for before its average,
maximum and minimum results are calculated. */
#ifndef benchTEST_ROUNDS
    #define benchTEST_ROUNDS                        ( ( uint32_t ) 10000U )
#endif

/* The number of counter overflows that are counted between two progress
reports that are printed once the benchmark itself has finished. */
#define benchOVERFLOW_REPORT_INTERVAL            ( ( uint32_t ) 10000U )

/* The size of the stack allocated to each benchmark task, measured in
words rather than bytes. */
#define benchTASK_STACK_SIZE                    ( ( uint16_t ) 128 )

/* Define benchFLOAT_CONTEXT to also exercise the floating point context of
the processor while the benchmark is running. */
/* #define benchFLOAT_CONTEXT */

/*-----------------------------------------------------------*/

/* Reset the statistics that are accumulated by the benchRECORD_STATISTIC()
macro. */
#define benchRESET_STATISTICS()                        \
do                                                    \
{                                                    \
    ulTotal = 0U;                                    \
    usMaximum = 0U;                                    \
    usMinimum = ( uint16_t ) 0xFFFFU;                \
}                                                    \
while( 0 )

/* Accumulate the result of a single round into the current statistics.  The
counter difference is calculated using 16 bit arithmetic so that a single
overflow of the counter between the two reads is handled transparently. */
#define benchRECORD_STATISTIC()                        \
do                                                    \
{                                                    \
    usDifference = ( uint16_t )( usEndCount - usStartCount );\
    ulTotal += ( uint32_t ) usDifference;            \
    if( usDifference > usMaximum )                    \
    {                                                \
        usMaximum = usDifference;                    \
    }                                                \
    if( usDifference < usMinimum )                    \
    {                                                \
        usMinimum = usDifference;                    \
    }                                                \
}                                                    \
while( 0 )

/* Print the name of a test followed by its average, maximum and minimum
results, all of them expressed in CPU cycles. */
#define benchREPORT_STATISTICS( pcTestName )        \
do                                                    \
{                                                    \
    benchPRINT_STRING( pcTestName );                        \
    benchPRINT_STRING( " : " );                            \
    benchPRINT_NUMBER( ulTotal / benchTEST_ROUNDS );        \
    benchPRINT_STRING( " / " );                            \
    benchPRINT_NUMBER( usMaximum );                        \
    benchPRINT_STRING( " / " );                            \
    benchPRINT_NUMBER( usMinimum );                        \
    benchPRINT_STRING( "\r\n" );                            \
}                                                    \
while( 0 )

/* Accumulate the RVM activation relay latency of a single interrupt into
the relay statistics.  The chip specific test header must provide the
kernel side interrupt timestamp for this to be available. */
#ifdef benchGET_KERNEL_TIMESTAMP
#define benchRECORD_RELAY_STATISTIC()                \
do                                                    \
{                                                    \
    uint16_t usRelayCount;                            \
    usRelayCount = ( uint16_t ) benchGET_RUN_TIME_COUNTER_VALUE();\
    usRelayCount = ( uint16_t ) ( usRelayCount - benchGET_KERNEL_TIMESTAMP() );\
    ulRelayFromIsrTotal += ( uint32_t ) usRelayCount;\
    if( usRelayCount > usRelayFromIsrMaximum )        \
    {                                                \
        usRelayFromIsrMaximum = usRelayCount;        \
    }                                                \
    if( usRelayCount < usRelayFromIsrMinimum )        \
    {                                                \
        usRelayFromIsrMinimum = usRelayCount;        \
    }                                                \
}                                                    \
while( 0 )
#endif

/*-----------------------------------------------------------*/

/*
 * The two tasks that run the benchmark.  prvTestTask1 starts the tests and
 * generates the workload while prvTestTask2 receives the workload and
 * prints the results as they become available.
 */
static void prvTestTask1( void *pvParameters );
static void prvTestTask2( void *pvParameters );

/*
 * One function per benchmark.  The functions that end in a "1" are called
 * from prvTestTask1 and generate the workload, their counterparts that end
 * in a "2" are called from prvTestTask2 and receive the workload.
 */
static void prvYieldTest1( void );
static void prvYieldTest2( void );
static void prvNotifyTest1( void );
static void prvNotifyTest2( void );
static void prvSemaphoreTest1( void );
static void prvSemaphoreTest2( void );
static void prvQueueTest1( void );
static void prvQueueTest2( void );
static void prvNotifyFromIsrTest( void );
static void prvSemaphoreFromIsrTest( void );
static void prvQueueFromIsrTest( void );

/*
 * The heap allocation and deallocation pair test, together with the two
 * helpers it uses.  These are only compiled when the chip specific test
 * header defines benchMEM_POOL_SIZE.
 */
#ifdef benchMEM_POOL_SIZE
    static void prvMemoryTest( void );
    static uint32_t prvRand( void );
    static void prvSwap( uint32_t *pulFirst, uint32_t *pulSecond );
#endif
/*-----------------------------------------------------------*/

/* The flag that is used to verify that execution ping-pongs between the
two test tasks, and between the tasks and the interrupt handler, exactly
as expected. */
static volatile BaseType_t xFlip = pdFALSE;

/* The counter values that bracket the operation under test, and the
statistics that are accumulated from the difference between them. */
static volatile uint16_t usStartCount = 0U;
static volatile uint16_t usEndCount = 0U;
static volatile uint16_t usDifference = 0U;
static volatile uint16_t usMinimum = 0U;
static volatile uint16_t usMaximum = 0U;
static volatile uint32_t ulTotal = 0U;
static volatile uint32_t ulOverflowCount = 0U;

/* The statistics of the interrupt driven tests are latched into these
variables because the counter is reused by the test that follows before
they are printed. */
static volatile uint32_t ulNotifyFromIsrTotal = 0U;
static volatile uint32_t ulNotifyFromIsrMaximum = 0U;
static volatile uint32_t ulNotifyFromIsrMinimum = 0U;
static volatile uint32_t ulSemaphoreFromIsrTotal = 0U;
static volatile uint32_t ulSemaphoreFromIsrMaximum = 0U;
static volatile uint32_t ulSemaphoreFromIsrMinimum = 0U;
static volatile uint32_t ulQueueFromIsrTotal = 0U;
static volatile uint32_t ulQueueFromIsrMaximum = 0U;
static volatile uint32_t ulQueueFromIsrMinimum = 0U;

/* The statistics of the RVM activation relay are accumulated by the
interrupt handler itself, because the counter is reused by the test that
follows before they are printed.  These are only available when the chip
specific test header provides the kernel side interrupt timestamp. */
#ifdef benchGET_KERNEL_TIMESTAMP
    static volatile uint32_t ulRelayFromIsrTotal = 0U;
    static volatile uint16_t usRelayFromIsrMaximum = 0U;
    static volatile uint16_t usRelayFromIsrMinimum = 0U;
#endif

/* Only referenced when benchFLOAT_CONTEXT is defined. */
#ifdef benchFLOAT_CONTEXT
    static volatile float fSum = 0.0f;
#endif

/* The kernel objects that are used by the benchmark. */
static TaskHandle_t xTask1Handle = NULL;
static TaskHandle_t xTask2Handle = NULL;
static SemaphoreHandle_t xTestSemaphore = NULL;
static QueueHandle_t xTestQueue = NULL;
static uint32_t ulQueuedValue = 1U;

/*-----------------------------------------------------------*/

static void prvYieldTest1( void )
{
uint32_t ulCount;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        /* Verify that execution returned from prvTestTask2 as expected. */
        if( xFlip != pdFALSE )
        {
            benchPRINT_STRING( "Yield error in task 1 @ round " );
            benchPRINT_NUMBER( ulCount );
            benchPRINT_STRING( ".\r\n" );
        }

        xFlip = pdTRUE;

        /* Read the counter that opens the bracket around the yield. */
        usStartCount = benchGET_RUN_TIME_COUNTER_VALUE();

        taskYIELD();
    }
}
/*-----------------------------------------------------------*/

static void prvYieldTest2( void )
{
uint32_t ulCount;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        taskYIELD();

        /* Read the counter that closes the bracket around the yield, then
        record the result of this round. */
        usEndCount = benchGET_RUN_TIME_COUNTER_VALUE();
        benchRECORD_STATISTIC();

        /* Verify that execution came from prvTestTask1 as expected. */
        if( xFlip == pdFALSE )
        {
            benchPRINT_STRING( "Yield error in task 2 @ round " );
            benchPRINT_NUMBER( ulCount );
            benchPRINT_STRING( ", " );
            benchPRINT_NUMBER( usStartCount );
            benchPRINT_STRING( ", " );
            benchPRINT_NUMBER( usEndCount );
            benchPRINT_STRING( ".\r\n" );
        }

        xFlip = pdFALSE;
    }
}
/*-----------------------------------------------------------*/

static void prvNotifyTest1( void )
{
uint32_t ulCount;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        usStartCount = benchGET_RUN_TIME_COUNTER_VALUE();
        xTaskNotify( xTask2Handle, 1U, eSetValueWithOverwrite );
    }
}
/*-----------------------------------------------------------*/

static void prvNotifyTest2( void )
{
uint32_t ulCount;
uint32_t ulNotifiedValue;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        xTaskNotifyWait( 0x00U, 0xFFFFFFFFU, &ulNotifiedValue, portMAX_DELAY );

        usEndCount = benchGET_RUN_TIME_COUNTER_VALUE();
        benchRECORD_STATISTIC();
    }
}
/*-----------------------------------------------------------*/

static void prvSemaphoreTest1( void )
{
uint32_t ulCount;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        usStartCount = benchGET_RUN_TIME_COUNTER_VALUE();
        xSemaphoreGive( xTestSemaphore );
    }
}
/*-----------------------------------------------------------*/

static void prvSemaphoreTest2( void )
{
uint32_t ulCount;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        xSemaphoreTake( xTestSemaphore, portMAX_DELAY );

        usEndCount = benchGET_RUN_TIME_COUNTER_VALUE();
        benchRECORD_STATISTIC();
    }
}
/*-----------------------------------------------------------*/

static void prvQueueTest1( void )
{
uint32_t ulCount;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        usStartCount = benchGET_RUN_TIME_COUNTER_VALUE();
        xQueueSend( xTestQueue, &ulQueuedValue, portMAX_DELAY );
    }
}
/*-----------------------------------------------------------*/

static void prvQueueTest2( void )
{
uint32_t ulCount;
uint32_t ulReceivedValue;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        xQueueReceive( xTestQueue, &ulReceivedValue, portMAX_DELAY );

        usEndCount = benchGET_RUN_TIME_COUNTER_VALUE();
        benchRECORD_STATISTIC();
    }
}
/*-----------------------------------------------------------*/

static void prvNotifyFromIsrTest( void )
{
uint32_t ulCount;
uint32_t ulNotifiedValue;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        xTaskNotifyWait( 0x00U, 0xFFFFFFFFU, &ulNotifiedValue, portMAX_DELAY );

        usEndCount = benchGET_RUN_TIME_COUNTER_VALUE();
        benchRECORD_STATISTIC();

        xFlip = pdFALSE;
    }
}
/*-----------------------------------------------------------*/

static void prvSemaphoreFromIsrTest( void )
{
uint32_t ulCount;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        xSemaphoreTake( xTestSemaphore, portMAX_DELAY );

        usEndCount = benchGET_RUN_TIME_COUNTER_VALUE();
        benchRECORD_STATISTIC();

        xFlip = pdFALSE;
    }
}
/*-----------------------------------------------------------*/

static void prvQueueFromIsrTest( void )
{
uint32_t ulCount;
uint32_t ulReceivedValue;

    for( ulCount = 0U; ulCount < benchTEST_ROUNDS; ulCount++ )
    {
        xQueueReceive( xTestQueue, &ulReceivedValue, portMAX_DELAY );

        usEndCount = benchGET_RUN_TIME_COUNTER_VALUE();
        benchRECORD_STATISTIC();

        xFlip = pdFALSE;
    }
}
/*-----------------------------------------------------------*/

#ifdef benchMEM_POOL_SIZE

/*
 * A small maximal length 16 bit LFSR that is used to randomise the order
 * in which the heap blocks are allocated and freed, and the size of each
 * block that is requested.
 */
static uint32_t prvRand( void )
{
static uint32_t ulState = 0xACE1U;

    if( ( ulState & 0x01U ) != 0U )
    {
        ulState >>= 1;
        ulState ^= 0xB400U;
    }
    else
    {
        ulState >>= 1;
    }

    return ulState;
}
/*-----------------------------------------------------------*/

static void prvSwap( uint32_t *pulFirst, uint32_t *pulSecond )
{
uint32_t ulTemp;

    ulTemp = *pulFirst;
    *pulFirst = *pulSecond;
    *pulSecond = ulTemp;
}
/*-----------------------------------------------------------*/
static void prvMemoryTest( void )
{
static void *pvMemory[ 8 ];
static uint32_t ulAllocationOrder[ 8 ];
static uint32_t ulFreeOrder[ 8 ];
static uint32_t ulSizeOrder[ 8 ];
static uint32_t ulRequestedSize[ 8 ];
uint32_t ulCaseCount;
uint32_t ulTestCount;

    /* The block sizes the heap is carved into, which are randomised for
    every round of the test. */
    ulRequestedSize[ 0 ] = ( benchMEM_POOL_SIZE / 32U ) * sizeof( uint32_t );
    ulRequestedSize[ 1 ] = ( benchMEM_POOL_SIZE / 64U + 16U ) * sizeof( uint32_t );
    ulRequestedSize[ 2 ] = ( benchMEM_POOL_SIZE / 4U ) * sizeof( uint32_t );
    ulRequestedSize[ 3 ] = ( benchMEM_POOL_SIZE / 128U + 32U ) * sizeof( uint32_t );
    ulRequestedSize[ 4 ] = ( benchMEM_POOL_SIZE / 16U ) * sizeof( uint32_t );
    ulRequestedSize[ 5 ] = ( benchMEM_POOL_SIZE / 8U + 16U ) * sizeof( uint32_t );
    ulRequestedSize[ 6 ] = ( benchMEM_POOL_SIZE / 128U + 64U ) * sizeof( uint32_t );
    ulRequestedSize[ 7 ] = ( benchMEM_POOL_SIZE / 2U - 256U ) * sizeof( uint32_t );

    for( ulTestCount = 0U; ulTestCount < benchTEST_ROUNDS; ulTestCount++ )
    {
        /* Generate a random allocation order, deallocation order and block
        size selection. */
        for( ulCaseCount = 0U; ulCaseCount < 8U; ulCaseCount++ )
        {
            ulAllocationOrder[ ulCaseCount ] = ulCaseCount;
            ulFreeOrder[ ulCaseCount ] = ulCaseCount;
            ulSizeOrder[ ulCaseCount ] = ulCaseCount;
        }

        for( ulCaseCount = 7U; ulCaseCount > 0U; ulCaseCount-- )
        {
            prvSwap( &ulAllocationOrder[ ulCaseCount ], &ulAllocationOrder[ prvRand() % ( ulCaseCount + 1U ) ] );
            prvSwap( &ulFreeOrder[ ulCaseCount ], &ulFreeOrder[ prvRand() % ( ulCaseCount + 1U ) ] );
            prvSwap( &ulSizeOrder[ ulCaseCount ], &ulSizeOrder[ prvRand() % ( ulCaseCount + 1U ) ] );
        }

        usStartCount = benchGET_RUN_TIME_COUNTER_VALUE();

        /* Allocation tests.  One of the allocations may fail because the
        heap management structures consume some of the pool, however the
        first four allocations must always succeed. */
        pvMemory[ ulAllocationOrder[ 0 ] ] = pvPortMalloc( ulRequestedSize[ ulSizeOrder[ 0 ] ] );
        if( pvMemory[ ulAllocationOrder[ 0 ] ] == NULL )
        {
            benchPRINT_STRING( "Failure.\r\n" );
        }
        pvMemory[ ulAllocationOrder[ 1 ] ] = pvPortMalloc( ulRequestedSize[ ulSizeOrder[ 1 ] ] );
        if( pvMemory[ ulAllocationOrder[ 1 ] ] == NULL )
        {
            benchPRINT_STRING( "Failure.\r\n" );
        }
        pvMemory[ ulAllocationOrder[ 2 ] ] = pvPortMalloc( ulRequestedSize[ ulSizeOrder[ 2 ] ] );
        if( pvMemory[ ulAllocationOrder[ 2 ] ] == NULL )
        {
            benchPRINT_STRING( "Failure.\r\n" );
        }
        pvMemory[ ulAllocationOrder[ 3 ] ] = pvPortMalloc( ulRequestedSize[ ulSizeOrder[ 3 ] ] );
        if( pvMemory[ ulAllocationOrder[ 3 ] ] == NULL )
        {
            benchPRINT_STRING( "Failure.\r\n" );
        }
        pvMemory[ ulAllocationOrder[ 4 ] ] = pvPortMalloc( ulRequestedSize[ ulSizeOrder[ 4 ] ] );
        if( pvMemory[ ulAllocationOrder[ 4 ] ] == NULL )
        {
            benchPRINT_STRING( "Failure.\r\n" );
        }
        pvMemory[ ulAllocationOrder[ 5 ] ] = pvPortMalloc( ulRequestedSize[ ulSizeOrder[ 5 ] ] );
        if( pvMemory[ ulAllocationOrder[ 5 ] ] == NULL )
        {
            benchPRINT_STRING( "Failure.\r\n" );
        }
        pvMemory[ ulAllocationOrder[ 6 ] ] = pvPortMalloc( ulRequestedSize[ ulSizeOrder[ 6 ] ] );
        if( pvMemory[ ulAllocationOrder[ 6 ] ] == NULL )
        {
            benchPRINT_STRING( "Failure.\r\n" );
        }
        pvMemory[ ulAllocationOrder[ 7 ] ] = pvPortMalloc( ulRequestedSize[ ulSizeOrder[ 7 ] ] );
        if( pvMemory[ ulAllocationOrder[ 7 ] ] == NULL )
        {
            benchPRINT_STRING( "Failure.\r\n" );
        }

        /* Deallocation tests. */
        vPortFree( pvMemory[ ulFreeOrder[ 0 ] ] );
        vPortFree( pvMemory[ ulFreeOrder[ 1 ] ] );
        vPortFree( pvMemory[ ulFreeOrder[ 2 ] ] );
        vPortFree( pvMemory[ ulFreeOrder[ 3 ] ] );
        vPortFree( pvMemory[ ulFreeOrder[ 4 ] ] );
        vPortFree( pvMemory[ ulFreeOrder[ 5 ] ] );
        vPortFree( pvMemory[ ulFreeOrder[ 6 ] ] );
        vPortFree( pvMemory[ ulFreeOrder[ 7 ] ] );

        usEndCount = benchGET_RUN_TIME_COUNTER_VALUE();
        benchRECORD_STATISTIC();

        /* This allocation must always succeed because every block was
        freed above, and the heap management structures never consume more
        than an eighth of the pool. */
        pvMemory[ 0 ] = pvPortMalloc( benchMEM_POOL_SIZE * sizeof( uint32_t ) * 7U / 8U );
        if( pvMemory[ 0 ] == NULL )
        {
            benchPRINT_STRING( "Memory test failure: " );
            benchPRINT_NUMBER( ulTestCount );
            benchPRINT_STRING( " runs.\r\n" );
            for( ;; );
        }
        vPortFree( pvMemory[ 0 ] );
    }

    /* The statistics that were accumulated above cover eight allocation
    and deallocation pairs per round, so scale them down accordingly. */
    ulTotal /= 8U;
    usMaximum /= 8U;
    usMinimum /= 8U;
}

#endif /* benchMEM_POOL_SIZE */
/*-----------------------------------------------------------*/

static void prvTestTask1( void *pvParameters )
{
    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

#ifdef benchFLOAT_CONTEXT
    fSum += 1.0f;
#endif

    benchPRINT_STRING( "Test (number in CPU cycles)        : AVG / MAX / MIN\r\n" );

    /* Drop the priority of this task so that both tasks run at the same
    priority while the yield test is executing. */
    vTaskPrioritySet( xTask1Handle, ( tskIDLE_PRIORITY + 1U ) );

    prvYieldTest1();

    /* Raise the priority of task 2 so that it takes over and prints the
    results of each test as it completes. */
    vTaskPrioritySet( xTask2Handle, ( tskIDLE_PRIORITY + 2U ) );

    prvNotifyTest1();
    prvSemaphoreTest1();
    prvQueueTest1();

    for( ;; );
}
/*-----------------------------------------------------------*/

static void prvTestTask2( void *pvParameters )
{
    /* Prevent the compiler warning about the unused parameter. */
    ( void ) pvParameters;

#ifdef benchFLOAT_CONTEXT
    fSum += 1.0f;
#endif

    /* Yield tests. */
    benchRESET_STATISTICS();
    prvYieldTest2();
    benchREPORT_STATISTICS( "Yield                             " );

    /* The remaining tests run with this task at the higher priority. */
    vTaskPrioritySet( xTask2Handle, ( tskIDLE_PRIORITY + 2U ) );

    /* Task notification tests. */
    benchRESET_STATISTICS();
    prvNotifyTest2();
    benchREPORT_STATISTICS( "Notification                      " );

    /* Semaphore tests. */
    benchRESET_STATISTICS();
    prvSemaphoreTest2();
    benchREPORT_STATISTICS( "Semaphore                         " );

    /* Blocking message queue tests. */
    benchRESET_STATISTICS();
    prvQueueTest2();
    benchREPORT_STATISTICS( "Message queue                     " );

#ifdef benchMEM_POOL_SIZE
    /* Heap allocation and deallocation pair tests. */
    benchRESET_STATISTICS();
    prvMemoryTest();
    benchREPORT_STATISTICS( "Memory allocation/free pair       " );
#endif

    /* Reset the relay statistics that are accumulated by the interrupt
    handler itself. */
#ifdef benchGET_KERNEL_TIMESTAMP
    ulRelayFromIsrTotal = 0U;
    usRelayFromIsrMaximum = 0U;
    usRelayFromIsrMinimum = 0xFFFFU;
#endif

    /* Prepare the interrupt driven tests. */
    vSetupTestInterrupt();

    /* Task notification from interrupt tests. */
    benchRESET_STATISTICS();
    prvNotifyFromIsrTest();
    ulNotifyFromIsrTotal = ulTotal;
    ulNotifyFromIsrMaximum = usMaximum;
    ulNotifyFromIsrMinimum = usMinimum;

    /* Semaphore from interrupt tests. */
    benchRESET_STATISTICS();
    prvSemaphoreFromIsrTest();
    ulSemaphoreFromIsrTotal = ulTotal;
    ulSemaphoreFromIsrMaximum = usMaximum;
    ulSemaphoreFromIsrMinimum = usMinimum;

    /* Blocking message queue from interrupt tests. */
    benchRESET_STATISTICS();
    prvQueueFromIsrTest();
    ulQueueFromIsrTotal = ulTotal;
    ulQueueFromIsrMaximum = usMaximum;
    ulQueueFromIsrMinimum = usMinimum;

    /* Print the results of the interrupt driven tests. */
    ulTotal = ulNotifyFromIsrTotal;
    usMaximum = ( uint16_t ) ulNotifyFromIsrMaximum;
    usMinimum = ( uint16_t ) ulNotifyFromIsrMinimum;
    benchREPORT_STATISTICS( "ISR Notification                  " );

    ulTotal = ulSemaphoreFromIsrTotal;
    usMaximum = ( uint16_t ) ulSemaphoreFromIsrMaximum;
    usMinimum = ( uint16_t ) ulSemaphoreFromIsrMinimum;
    benchREPORT_STATISTICS( "ISR Semaphore                     " );

    ulTotal = ulQueueFromIsrTotal;
    usMaximum = ( uint16_t ) ulQueueFromIsrMaximum;
    usMinimum = ( uint16_t ) ulQueueFromIsrMinimum;
    benchREPORT_STATISTICS( "ISR Message queue                 " );

#ifdef benchGET_KERNEL_TIMESTAMP
    ulTotal = ulRelayFromIsrTotal;
    usMaximum = usRelayFromIsrMaximum;
    usMinimum = usRelayFromIsrMinimum;
    benchREPORT_STATISTICS( "ISR RVM activation relay          " );
#endif

    /* The benchmark itself is done.  Keep dumping the counter value so
    that an incorrectly configured counter clock rate can be detected. */
#ifdef benchEXIT
    /* benchEXIT is only defined when the user also provides the vExitTest()
    function that terminates the virtual machine. */
    vExitTest();
#else
    for( ;; )
    {
        usStartCount = usEndCount;
        usEndCount = ( uint16_t ) benchGET_RUN_TIME_COUNTER_VALUE();
        if( usStartCount > usEndCount )
        {
            ulOverflowCount++;
            if( ( ulOverflowCount % benchOVERFLOW_REPORT_INTERVAL ) == 0U )
            {
                benchPRINT_NUMBER( benchOVERFLOW_REPORT_INTERVAL );
                benchPRINT_STRING( " overflows\r\n" );
            }
        }
    }
#endif
}
/*-----------------------------------------------------------*/

/*
 * The interrupt handler that generates the workload for the interrupt
 * driven tests.  The chip specific test header registers this handler with
 * the RVM hypervisor, and the RVM kernel calls it once the physical
 * interrupt has been delivered to this virtual machine.
 */
void xTestIntHandler( void )
{
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
BaseType_t xResult;
static uint32_t ulCount = 0U;
uint32_t ulValueToSend = 1U;

    /* Measure the RVM activation relay latency while the first test phase
    is running.  The chip specific test header provides the timestamp that
    the kernel latched when it took this very interrupt. */
#ifdef benchGET_KERNEL_TIMESTAMP
    if( ulCount < benchTEST_ROUNDS )
    {
        benchRECORD_RELAY_STATISTIC();
    }
#endif

    /* Detect interrupt reentry. */
    if( xFlip != pdFALSE )
    {
        benchPRINT_STRING( "Interrupt reentered.\r\n" );
    }

    xFlip = pdTRUE;

    if( ulCount < benchTEST_ROUNDS )
    {
        ulCount++;
        usStartCount = benchGET_RUN_TIME_COUNTER_VALUE();
        xTaskNotifyFromISR( xTask2Handle, 1U, eSetValueWithOverwrite, &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
    else if( ulCount < ( benchTEST_ROUNDS * 2U ) )
    {
        ulCount++;
        usStartCount = benchGET_RUN_TIME_COUNTER_VALUE();
        xResult = xSemaphoreGiveFromISR( xTestSemaphore, &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        if( xResult != pdTRUE )
        {
            benchPRINT_STRING( "ISR semaphore give failed.\r\n" );
        }
    }
    else if( ulCount < ( benchTEST_ROUNDS * 3U ) )
    {
        ulCount++;
        usStartCount = benchGET_RUN_TIME_COUNTER_VALUE();
        xResult = xQueueSendFromISR( xTestQueue, &ulValueToSend, &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        if( xResult != pdTRUE )
        {
            benchPRINT_STRING( "ISR queue send failed.\r\n" );
        }
    }
    else
    {
        vDisableTestInterrupt();
    }
}
/*-----------------------------------------------------------*/

int main( void )
{
    /* Set up the counter that is used to time the tests. */
    vSetupBenchmarkTimer();

    /* Create the kernel objects that are used by the benchmark. */
    xTestSemaphore = xSemaphoreCreateCounting( benchMAX_SEMAPHORE_COUNT, 0U );
    xTestQueue = xQueueCreate( benchQUEUE_LENGTH, benchQUEUE_ITEM_SIZE );

    benchPRINT_STRING( "\r\n" );
    benchPRINT_STRING( "FreeRTOS V9.0.0 on RVM - Standard benchmark test\r\n" );
    benchPRINT_STRING( "====================================================\r\n" );

    /* Create the two test tasks.  Task 1 starts at the higher priority so
    that it prints the test header and kicks off the yield test. */
    xTaskCreate( prvTestTask2, "Test2", benchTASK_STACK_SIZE, ( void * ) 0x4321U, ( tskIDLE_PRIORITY + 1U ), &xTask2Handle );
    xTaskCreate( prvTestTask1, "Test1", benchTASK_STACK_SIZE, ( void * ) 0x1234U, ( tskIDLE_PRIORITY + 2U ), &xTask1Handle );

    /* Start the FreeRTOS scheduler. */
    vTaskStartScheduler();

    for( ;; );
}
/*-----------------------------------------------------------*/