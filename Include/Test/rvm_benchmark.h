/******************************************************************************
Filename    : rvm_benchmark.h
Author      : ryp
Date        : 27/07/2024
License     : Unlicense; see COPYING for details.
Description : The benchmark for RME/RVM on microcontrollers.
******************************************************************************/

/* Native Include ************************************************************/
#ifdef RVM_BENCHMARK_NATIVE_INC
typedef rvm_u16_t rvm_tim_t;
/* Number of rounds to run */
#define ROUND_NUM       10000

/* Field definitions - TARGET and START defined in chip-specific header */
#define END             (End)
#define TOTAL           (Total)
#define MIN             (Min)
#define MAX             (Max)
#define DIFF            (Diff)

/* Data definitions */
#define RVM_TIME        ((rvm_tim_t)(END-START))
#define RVM_TOTAL()     (TOTAL+=(DIFF))
#define RVM_MAX()       (MAX=((DIFF)>(MAX)?(DIFF):(MAX)))
#define RVM_MIN()       (MIN=((DIFF)<(MIN)?(DIFF):(MIN)))

/* Data initialization */
#define RVM_INIT() \
do \
{ \
    TOTAL=0U; \
    MAX=0U; \
    MIN=((rvm_tim_t)-1U); \
} \
while(0)

/* Data extraction */
#define RVM_DATA() \
do \
{ \
    DIFF=RVM_TIME; \
    RVM_TOTAL(); \
    RVM_MAX(); \
    RVM_MIN(); \
} \
while(0)
    
/* Data printing */
#define RVM_LIST(X) \
do \
{ \
    RVM_DBG_S(X); \
    RVM_DBG_S(" : "); \
    RVM_DBG_I(TOTAL/ROUND_NUM); \
    RVM_DBG_S(" / "); \
    RVM_DBG_I(MAX); \
    RVM_DBG_S(" / "); \
    RVM_DBG_I(MIN); \
    RVM_DBG_S("\r\n"); \
} \
while(0)
#endif
/* End Native Include ********************************************************/

/* Virtual Include ***********************************************************/
#ifdef RVM_BENCHMARK_VIRTUAL_INC
typedef rvm_u16_t rvm_tim_t;

#define RVM_DATA() \
do \
{ \
    Guest_Diff=(rvm_tim_t)RMP_CNT_READ(); \
    Guest_Diff-=(rvm_tim_t)START; \
    Guest_ISR_Total+=Guest_Diff; \
    Guest_ISR_Max=(Guest_Diff>Guest_ISR_Max)?Guest_Diff:Guest_ISR_Max; \
    Guest_ISR_Min=(Guest_Diff<Guest_ISR_Min)?Guest_Diff:Guest_ISR_Min; \
} \
while(0)
#endif
/* End Virtual Include *******************************************************/
        
/* Process 1 Thread 1 ********************************************************/
#ifdef RVM_BENCHMARK_PRC_PROC1_THD_THD1
    rvm_cnt_t Count;
    rvm_ptr_t Total;
    rvm_tim_t End;
    rvm_tim_t Min;
    rvm_tim_t Max;
    rvm_tim_t Diff;
    
    /* Switch a few times so that the process 2 will fully boot */
    RVM_ASSERT(RVM_Thd_Swt(RVM_CID_NULL,0U)==0);
    RVM_ASSERT(RVM_Thd_Swt(RVM_CID_NULL,0U)==0);
    RVM_ASSERT(RVM_Thd_Swt(RVM_CID_NULL,0U)==0);
    
    /* Print table header */
    RVM_DBG_S("\r\n");
    RVM_DBG_S("    ___  _   __ __  ___\r\n");
    RVM_DBG_S("   / _ \\| | / //  |/  /    Feather-weight hypervisor\r\n");
    RVM_DBG_S("  / , _/| |/ // /|_/ /      Standard benchmark test\r\n");
    RVM_DBG_S(" /_/|_| |___//_/  /_/\r\n");
    RVM_DBG_S("====================================================\r\n");
    RVM_DBG_S("Test (number in CPU cycles)        : AVG / MAX / MIN\r\n");
    
    /* Initialize timer */
    RVM_TIM_INIT();
    
    /* Yield intra/self-test */
    RVM_INIT();
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        START=RVM_CNT_READ();
        RVM_ASSERT(RVM_Thd_Swt(RVM_CID_NULL,0U)==0);
        END=RVM_CNT_READ();
        RVM_DATA();
    }
    RVM_LIST("Yield (self, one-way)             ");
    
    /* Signal self-test */
    RVM_INIT();
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        START=RVM_CNT_READ();
        RVM_ASSERT(RVM_Sig_Snd(SND_RECV1_PRC_PROC1,1U)==0);
        RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV1,RVM_RCV_BM)>=0);
        END=RVM_CNT_READ();
        RVM_DATA();
    }
    RVM_LIST("Signal (self)                     ");
    
    /* Invocation test */
    RVM_INIT();
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        START=RVM_CNT_READ();
        RVM_ASSERT(RVM_Inv_Act(PRT_INV1_PRC_PROC2,0x1234U,RVM_NULL)==0);
        END=RVM_CNT_READ();
        RVM_DATA();
    }
    RVM_LIST("Invocation (round-trip)           ");
    
    /* Interrupt targeting native process */
    TARGET=0U;
    
    /* Tell the process 1 thread 2 that it should run, this is to avoid having
     * nothing running in the process so that the virtual machine don't run */
    RVM_ASSERT(RVM_Sig_Snd(SND_RECV2_PRC_PROC1,1U)==0);
    
    /* Interrupt test */
    RVM_INIT();
    RVM_INT_INIT();
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        RVM_ASSERT(RVM_Sig_Rcv(SIG_VCT,RVM_RCV_BM)>=0);
        END=RVM_CNT_READ();
        RVM_DATA();
    }
    RVM_INT_DISABLE();
    RVM_LIST("Vector                            ");
    
    /* Interrupt target the virtual machine monitor */
    TARGET=1U;
    
    /* Signal intra test */
    RVM_INIT();
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV1,RVM_RCV_BM)>=0);
        END=RVM_CNT_READ();
        RVM_DATA();
    }
    RVM_LIST("Signal (intra)                    ");
    
    /* Tell the process 2 thread 2 that it should run */
    RVM_ASSERT(RVM_Sig_Snd(SND_RECV2_PRC_PROC2,1U)==0);
    
    /* Signal inter test */
    RVM_INIT();
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV1,RVM_RCV_BM)>=0);
        END=RVM_CNT_READ();
        RVM_DATA();
    }
    RVM_LIST("Signal (inter)                    ");
    
    /* Tell the process 2 thread 1 that it should run */
    RVM_ASSERT(RVM_Sig_Snd(SND_RECV1_PRC_PROC2,1U)==0);
    
    /* Yield inter */
    RVM_INIT();
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        RVM_ASSERT(RVM_Thd_Swt(RVM_CID_NULL,0U)==0);
        END=RVM_CNT_READ();
        RVM_DATA();
    }
    RVM_LIST("Yield (inter, one-way)            ");
    
    /* Test done, block on the signal forever */
    RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV1,RVM_RCV_BM)>=0);
    
    /* Should never reach here */
    while(1U);
#endif
/* End Process 1 Thread 1 ****************************************************/

/* Process 1 Thread 2 ********************************************************/
#ifdef RVM_BENCHMARK_PRC_PROC1_THD_THD2
    rvm_cnt_t Count;
    
    /* Stuck this before we measure anything */
    RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV2,RVM_RCV_BM)>=0);
    
    /* Will not start testing until we finish the vector */
    while(TARGET==0U);
    
    RVM_DBG_S("P1T2-S\r\n");
    
    /* Signal intra test */
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        START=RVM_CNT_READ();
        RVM_ASSERT(RVM_Sig_Snd(SND_RECV1_PRC_PROC1,1U)>=0);
    }
    
    /* Test done, block on the signal forever */
    RVM_DBG_S("P1T2-E\r\n");
    RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV2,RVM_RCV_BM)>=0);
    
    /* Should never reach here */
    while(1U);
#endif
/* End Process 1 Thread 2 ****************************************************/

/* Process 2 Thread 1 ********************************************************/
#ifdef RVM_BENCHMARK_PRC_PROC2_THD_THD1
    rvm_cnt_t Count;
    
    /* Stuck this before we measure anything */
    RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV1,RVM_RCV_BM)>=0);
    
    RVM_DBG_S("P2T1-S\r\n");
    
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        START=RVM_CNT_READ();
        RVM_ASSERT(RVM_Thd_Swt(RVM_CID_NULL,0U)==0);
    }
    
    /* Test done, block on the signal forever */
    RVM_DBG_S("P2T1-E\r\n");
    RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV1,RVM_RCV_BM)>=0);
    
    /* Should never reach here */
    while(1U);
#endif
/* End Process 2 Thread 1 ****************************************************/

/* Process 2 Thread 2 ********************************************************/
#ifdef RVM_BENCHMARK_PRC_PROC2_THD_THD2
    rvm_cnt_t Count;
    
    /* Stuck this before we measure anything */
    RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV2,RVM_RCV_BM)>=0);
    
    RVM_DBG_S("P2T2-S\r\n");
    
    /* Signal intra test */
    for(Count=0;Count<ROUND_NUM;Count++)
    {
        START=RVM_CNT_READ();
        RVM_ASSERT(RVM_Sig_Snd(SND_RECV1_PRC_PROC1,1U)>=0);
    }
    
    /* Test done, block on the signal forever */
    RVM_DBG_S("P2T2-E\r\n");
    RVM_ASSERT(RVM_Sig_Rcv(RCV_RECV2,RVM_RCV_BM)>=0);
    
    /* Should never reach here */
    while(1U);
#endif
/* End Process 2 Thread 2 ****************************************************/

/* Process 2 Invocation 1 ****************************************************/
#ifdef RVM_BENCHMARK_PRC_PROC2_INV_INV1
    /* Return directly */
    RVM_ASSERT(RVM_Inv_Ret(0x1234U)>=0);
    
    /* Should never reach here */
    while(1U);
#endif
/* End Process 2 Invocation 1 ************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
