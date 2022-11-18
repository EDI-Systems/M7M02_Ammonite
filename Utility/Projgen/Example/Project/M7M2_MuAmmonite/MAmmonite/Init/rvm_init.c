/******************************************************************************
Filename    : rvm_init.c
Author      : pry
Date        : 11/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The init process of MPU-based RVM systems. This process just sets
              up all other parts of the system then go idle.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"

#define __HDR_DEFS__
#include "Platform/rvm_platform.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#include "Init/rvm_hyper.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/rvm_platform.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#include "Init/rvm_hyper.h"
#undef __HDR_STRUCTS__

/* Private include */
#include "Init/rvm_init.h"

#define __HDR_PUBLIC_MEMBERS__
#include "Platform/rvm_platform.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_hyper.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:RVM_Clear ***************************************************
Description : Memset a memory area to zero.
Input       : void* Addr - The address to clear.
              rvm_ptr_t Size - The size to clear.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Clear(void* Addr, rvm_ptr_t Size)
{
    rvm_ptr_t* Word_Inc;
    rvm_u8_t* Byte_Inc;
    rvm_ptr_t Words;
    rvm_ptr_t Bytes;
    
    /* On processors that are not that fast, copy by word is really important */
    Word_Inc=(rvm_ptr_t*)Addr;
    for(Words=Size/sizeof(rvm_ptr_t);Words>0;Words--)
    {
        *Word_Inc=0;
        Word_Inc++;
    }
    
    /* Get the final bytes */
    Byte_Inc=(rvm_u8_t*)Word_Inc;
    for(Bytes=Size%sizeof(rvm_ptr_t);Bytes>0;Bytes--)
    {
        *Byte_Inc=0;
        Byte_Inc++;
    }
}
/* End Function:RVM_Clear ****************************************************/

/* Begin Function:RVM_Proc_Init ***********************************************
Description : Initialize processes.
Input       : rvm_ptr_t Kmem_Bump - The kernel memory frontier before allocations.
Output      : None.
Return      : rvm_ptr_t - The kernel memory frontier after allocations.
******************************************************************************/
rvm_ptr_t RVM_Proc_Init(rvm_ptr_t Kmem_Bump)
{
    extern void RVM_Boot_Kobj_Crt(void);
    extern void RVM_Boot_Kobj_Init(void);
    
    RVM_Boot_Kobj_Crt();
    RVM_Boot_Kobj_Init();
    
    return 0;
}
/* End Function:RVM_Proc_Init ************************************************/

/* Begin Function:RVM_Daemon_Init *********************************************
Description : Initialize the daemons. These daemons include four ones:
              1. SFTD, the daemon for the startup and process recovery.
              2. TIMD, the daemon for time accounting and sending timer interrupts to VMs.
              3. VMMD, the daemon for real-time scheduling & hypercall handling.
              4. INTD, the daemon for interrupt handling to VMs and .
              The TID of all these daemons are zero.
Input       : rvm_ptr_t Cap_Front - The capability frontier for allocation.
              rvm_ptr_t Kmem_Front - The kernel memory pointer for allocation.
Output      : None.
Return      : rvm_ptr_t - The kernel memory pointer's new location.
******************************************************************************/
rvm_ptr_t RVM_Daemon_Init(rvm_ptr_t Cap_Front, rvm_ptr_t Kmem_Front)
{
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    
    /* Safety daemon initialization - highest priority as always */
    RVM_Sftd_Sig_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CAPTBL, RVM_Sftd_Sig_Cap)==0);
    RVM_LOG_SIS("Init:Created safety daemon fault endpoint CID ",RVM_Sftd_Sig_Cap,".\r\n");
    
    RVM_Sftd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_Sftd_Thd_Cap,
                           RVM_BOOT_INIT_PROC, RVM_SFTD_PRIO, Kmem_Front)>=0);
    RVM_LOG_SISUS("Init:Created safety daemon CID ",RVM_Sftd_Thd_Cap," @ address 0x",Kmem_Front,".\r\n");
    Kmem_Front+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Sftd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_Sftd_Sig_Cap, RVM_Sftd_Thd_Cap, RVM_MAX_PREEMPT_PRIO-1)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Sftd_Thd_Cap, (rvm_ptr_t)RVM_Sftd, 
                                RVM_Stack_Init(RVM_SFTD_STACK_BASE, RVM_SFTD_STACK_SIZE,
                                               (rvm_ptr_t)RVM_Sftd, (rvm_ptr_t)_RVM_Jmp_Stub),0)==0);
    RVM_LOG_S("Init:Safety daemon initialization complete.\r\n");

    /* Don't bother boot these daemons if we do not have virtual machines installed at all */
#if(RVM_MAX_PREEMPT_VPRIO!=0)
    /* Timer daemon initialization - main priority */
    RVM_Timd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_Timd_Thd_Cap,
                           RVM_BOOT_INIT_PROC, RVM_TIMD_PRIO, Kmem_Front)>=0);
    RVM_LOG_SISUS("Init:Created timer daemon CID ",RVM_Timd_Thd_Cap," @ address 0x",Kmem_Front,".\r\n");
    Kmem_Front+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Timd_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_Timd_Thd_Cap, RVM_TIMD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Timd_Thd_Cap, (rvm_ptr_t)RVM_Timd, 
                                RVM_Stack_Init(RVM_TIMD_STACK_BASE, RVM_TIMD_STACK_SIZE,
                                               (rvm_ptr_t)RVM_Timd, (rvm_ptr_t)_RVM_Jmp_Stub),0)==0);
    RVM_LOG_S("Init:Timer daemon initialization complete.\r\n");

    /* VMM daemon initialization - main priority */
    RVM_Vmmd_Sig_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CAPTBL, RVM_Vmmd_Sig_Cap)==0);
    RVM_LOG_SIS("Init:Created virtual machine monitor endpoint CID ",RVM_Vmmd_Sig_Cap,".\r\n");
    
    RVM_Vmmd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_Vmmd_Thd_Cap,
                           RVM_BOOT_INIT_PROC, RVM_VMMD_PRIO, Kmem_Front)>=0);
    RVM_LOG_SISUS("Init:Created virtual machine monitor daemon CID ",RVM_Vmmd_Thd_Cap," @ address 0x",Kmem_Front,".\r\n");
    Kmem_Front+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Vmmd_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_Vmmd_Thd_Cap, RVM_VMMD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Vmmd_Thd_Cap, (rvm_ptr_t)RVM_Vmmd, 
                                RVM_Stack_Init(RVM_VMMD_STACK_BASE, RVM_VMMD_STACK_SIZE,
                                               (rvm_ptr_t)RVM_Vmmd, (rvm_ptr_t)_RVM_Jmp_Stub),0)==0);
    RVM_LOG_S("Init:Virtual machine monitor daemon initialization complete.\r\n");

    /* Interrupt relaying daemon initialization - main priority */
    RVM_Vctd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_Vctd_Thd_Cap,
                           RVM_BOOT_INIT_PROC, RVM_VCTD_PRIO, Kmem_Front)>=0);
    RVM_LOG_SISUS("Init:Created vector handling daemon CID ",RVM_Vctd_Thd_Cap," @ address 0x",Kmem_Front,".\r\n");
    Kmem_Front+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Vctd_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_Vctd_Thd_Cap, RVM_VCTD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Vctd_Thd_Cap, (rvm_ptr_t)RVM_Vctd, 
                                RVM_Stack_Init(RVM_VCTD_STACK_BASE, RVM_VCTD_STACK_SIZE,
                                               (rvm_ptr_t)RVM_Vctd, (rvm_ptr_t)_RVM_Jmp_Stub),0)==0);
    RVM_LOG_S("Init:Vector handling daemon initialization complete.\r\n");
#endif

    return Kmem_Front;
}
/* End Function:RVM_Daemon_Init **********************************************/

/* Begin Function:main ********************************************************
Description : The entry of the VMM's init thread. 
Input       : None.
Output      : None.
Return      : int - This function shall never return.
******************************************************************************/
int main(void)
{
    /* The kernel heap allocation bump pointer */
    rvm_ptr_t Kmem_Bump;
    extern void RVM_Boot_Pre_Init(void);
    extern void RVM_Boot_Post_Init(void);
    
    RVM_Boot_Pre_Init();

    RVM_LOG_S("\r\n\r\n-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("                               Welcome to the\r\n");
    RVM_LOG_S("\r\n");
    RVM_LOG_S("                        ######    ###  ###  ########\r\n");
    RVM_LOG_S("                        #######   ###  ###  ########\r\n");
    RVM_LOG_S("                        ##    ##  ###  ###  ##\r\n");
    RVM_LOG_S("                        ##    ##  ###  ###  ##\r\n");
    RVM_LOG_S("                        ##    ##  ## ## ##  ##\r\n");
    RVM_LOG_S("                        #######   ## ## ##  #######\r\n");
    RVM_LOG_S("                        ######    ## ## ##  #######\r\n");
    RVM_LOG_S("                        ##   ##   ## ## ##  ##\r\n");
    RVM_LOG_S("                        ##   ##   ##    ##  ##\r\n");
    RVM_LOG_S("                        ##    ##  ##    ##  ##\r\n");
    RVM_LOG_S("                        ##    ##  ##    ##  ########\r\n");
    RVM_LOG_S("                        ##    ### ##    ##  ########\r\n");
    RVM_LOG_S("\r\n");
    RVM_LOG_S("                    Microcontroller User-level Platform!\r\n");
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");

    /* Raise our own priority to the top of the system */
    RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_BOOT_INIT_THD, RVM_MAX_PREEMPT_PRIO-1)==0);
    RVM_LOG_S("Init:Preparation - priority raised.\r\n");
    
    /* Initialize RVM database */
#if(RVM_MAX_PREEMPT_VPRIO!=0)
    RVM_Virt_Init();
#endif
    RVM_LOG_SUS("Init:Kernel virtual address base is 0x",RVM_KMEM_VA_START,".\r\n");
    RVM_LOG_SUS("Init:Start creating kernel objects @ frontier 0x",RVM_KMEM_BOOT_FRONTIER,".\r\n");
    Kmem_Bump=RVM_KMEM_BOOT_FRONTIER;
    
    /* Create the startup thread in the init process, because the init thread
     * cannot block. Bind that to the processor, and let it have infinite budget.
     * after this the task will be handled by this task, and we will never return
     * to init unless there is nothing to run */
    Kmem_Bump=RVM_Daemon_Init(RVM_CAP_BOOT_FRONTIER,Kmem_Bump);
    RVM_LOG_S("Init:Daemon initialization done.\r\n");
    
    /* Initialize the virtual machine databases, and create whatever is needed */
    Kmem_Bump=RVM_Proc_Init(Kmem_Bump);
    RVM_LOG_S("Init:Process initialization done.\r\n");
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    
    /* Delegate timeslice to safety daemon, and make it work - we should stop here
     * actually, and we have done operating the database, now all the work is the safety
     * daemon's. From now on, no further kernel objects will be created anymore. */
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Sftd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
#if(RVM_MAX_PREEMPT_VPRIO!=0)
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Timd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Vmmd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Vctd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
#endif
    RVM_LOG_S("Init:Daemon time budget initialization complete.\r\n");
    
    /* Put our priority to the lowest in system - this should get everything else running */
    RVM_LOG_S("Init:Job finished - lowering priority.\r\n");
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_BOOT_INIT_THD, RVM_INIT_PRIO)==0);

    RVM_Boot_Post_Init();
		
    while(1)
    {
        /* Receive the scheduler notifications for the interrupt threads. If any of
         * them failed, then we just panic, because none of them are supposed to fail. */
        /* RVM_LOG_S("I"); */
        RVM_Idle();
    }
}
/* End Function:main *********************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
