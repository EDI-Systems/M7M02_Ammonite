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
#include "rvm_boot.h"

#define __HDR_DEFS__
#include "rvm_platform.h"
#include "Monitor/rvm_syssvc.h"
#include "Monitor/rvm_init.h"
#include "Monitor/rvm_hyper.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "rvm_platform.h"
#include "Monitor/rvm_syssvc.h"
#include "Monitor/rvm_init.h"
#include "Monitor/rvm_hyper.h"
#undef __HDR_STRUCTS__

/* Private include */
#include "Monitor/rvm_init.h"

#define __HDR_PUBLIC_MEMBERS__
#include "rvm_platform.h"
#include "Monitor/rvm_syssvc.h"
#include "Monitor/rvm_hyper.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:RVM_Clear ***************************************************
Description : Memset a memory area to zero.
Input       : volatile void* Addr - The address to clear.
              rvm_ptr_t Size - The size to clear.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Clear(volatile void* Addr,
               rvm_ptr_t Size)
{
    rvm_ptr_t Count;

    for(Count=0U;Count<Size;Count++)
        ((volatile rvm_u8_t*)Addr)[Count]=0U;
}
/* End Function:RVM_Clear ****************************************************/

/* Begin Function:RVM_Boot_Main_Crt *******************************************
Description : Create all 'main' capability tables at boot-time. Main capability 
              tables are placed into the boot-time capability table, and they
              contain the real kernel objects.
Input       : const struct RVM_Meta_Main_Struct* Main - The metadata array.
              rvm_ptr_t Number - The number of entries in the array.
              rvm_ptr_t Addr - The kmem address to create those tables from.
Output      : None.
Return      : None.
******************************************************************************/
rvm_ptr_t RVM_Boot_Main_Crt(const struct RVM_Meta_Main_Struct* Meta,
                            rvm_ptr_t Number,
                            rvm_ptr_t Addr)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=Addr;
    
    /* Create all the capability table capability tables first */
    for(Count=0U;Count<Number;Count++)
    {
        RVM_ASSERT(RVM_Cpt_Crt(RVM_BOOT_CPT,
                               RVM_BOOT_INIT_KOM,
                               Meta[Count].Slot,
                               Cur_Addr,
                               Meta[Count].Size)==0);
        RVM_DBG_S("Init:Main capability table '");
        RVM_DBG_I(Meta[Count].Slot);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(Cur_Addr);
        RVM_DBG_S(".\r\n");
        Cur_Addr+=RVM_CPT_SIZE(Meta[Count].Size);
    }

    return Cur_Addr;
}
/* End Function:RVM_Boot_Main_Crt ********************************************/

/* Begin Function:RVM_Boot_Vep_Crt ********************************************
Description : Create all VM endpoints at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_VEP_CRT_NUM!=0U)
void RVM_Boot_Vep_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_VEP_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Creating VM endpoints.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=RVM_Boot_Main_Crt(RVM_Meta_Vep_Main, RVM_BOOT_VEP_MAIN_NUM, Cur_Addr);

    /* Then the virtual endpoints themselves */
    for(Count=0U;Count<RVM_BOOT_VEP_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Sig_Crt(RVM_Meta_Vep_Crt[Count].Main,
                               RVM_Meta_Vep_Crt[Count].Slot)==0U);
        
        RVM_DBG_S("Init:VM endpoint '");
        RVM_DBG_I(RVM_Meta_Vep_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Vep_Crt[Count].Slot);
        RVM_DBG_S("'.\r\n");
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_VEP_AFTER);
}
#endif
/* End Function:RVM_Boot_Vep_Crt *********************************************/

/* Begin Function:RVM_Boot_Cpt_Crt *****************************************
Description : Create all capability tables at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Cpt_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_CPT_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Creating capability tables.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=RVM_Boot_Main_Crt(RVM_Meta_Cpt_Main, RVM_BOOT_CPT_MAIN_NUM, Cur_Addr);

    /* Then the capability tables themselves */
    for(Count=0U;Count<RVM_BOOT_CPT_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Cpt_Crt(RVM_Meta_Cpt_Crt[Count].Main,
                               RVM_BOOT_INIT_KOM,
                               RVM_Meta_Cpt_Crt[Count].Slot,
                               Cur_Addr,
                               RVM_Meta_Cpt_Crt[Count].Size)==0U);
        
        RVM_DBG_S("Init:Capability table '");
        RVM_DBG_I(RVM_Meta_Cpt_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Cpt_Crt[Count].Slot);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(Cur_Addr);
        RVM_DBG_S(".\r\n");
        
        Cur_Addr+=RVM_CPT_SIZE(RVM_Meta_Cpt_Crt[Count].Size);
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_CPT_AFTER);
}
/* End Function:RVM_Boot_Cpt_Crt ******************************************/

/* Begin Function:RVM_Boot_Pgt_Crt ******************************************
Description : Create all page tables at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Pgt_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_PGT_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Creating page tables.\r\n");
    

    /* Create the main capability tables first */
    Cur_Addr=RVM_Boot_Main_Crt(RVM_Meta_Pgt_Main, RVM_BOOT_PGT_MAIN_NUM, Cur_Addr);

    /* Then the page tables themselves */
    for(Count=0U;Count<RVM_BOOT_PGT_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Pgt_Crt(RVM_Meta_Pgt_Crt[Count].Main,
                               RVM_BOOT_INIT_KOM,
                               RVM_Meta_Pgt_Crt[Count].Slot,
                               Cur_Addr,
                               RVM_Meta_Pgt_Crt[Count].Base,
                               RVM_Meta_Pgt_Crt[Count].Is_Top,
                               RVM_Meta_Pgt_Crt[Count].Size_Order,
                               RVM_Meta_Pgt_Crt[Count].Num_Order)==0);
        
        RVM_DBG_S("Init:Page table '");
        RVM_DBG_I(RVM_Meta_Pgt_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Pgt_Crt[Count].Slot);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(Cur_Addr);
        RVM_DBG_S(".\r\n");
        
        if(RVM_Meta_Pgt_Crt[Count].Is_Top!=0)
            Cur_Addr+=RVM_PGT_SIZE_TOP(RVM_Meta_Pgt_Crt[Count].Num_Order);
        else
            Cur_Addr+=RVM_PGT_SIZE_NOM(RVM_Meta_Pgt_Crt[Count].Num_Order);
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_PGT_AFTER);
}
/* End Function:RVM_Boot_Pgt_Crt *******************************************/

/* Begin Function:RVM_Boot_Prc_Crt *******************************************
Description : Create all processes at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Prc_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_PRC_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Creating processes.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=RVM_Boot_Main_Crt(RVM_Meta_Prc_Main, RVM_BOOT_PRC_MAIN_NUM, Cur_Addr);

    /* Then the processes themselves */
    for(Count=0U;Count<RVM_BOOT_PRC_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Prc_Crt(RVM_Meta_Prc_Crt[Count].Main,
                               RVM_Meta_Prc_Crt[Count].Slot,
                               RVM_Meta_Prc_Crt[Count].Cpt,
                               RVM_Meta_Prc_Crt[Count].Pgt)==0);
        
        RVM_DBG_S("Init:Process '");
        RVM_DBG_I(RVM_Meta_Prc_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Prc_Crt[Count].Slot);
        RVM_DBG_S("'.\r\n");
    }
    
    RVM_ASSERT(Cur_Addr==RVM_BOOT_PRC_AFTER);
}
/* End Function:RVM_Boot_Prc_Crt ********************************************/

/* Begin Function:RVM_Boot_Thd_Crt ********************************************
Description : Create all threads at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Thd_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_THD_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Creating threads.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=RVM_Boot_Main_Crt(RVM_Meta_Thd_Main, RVM_BOOT_THD_MAIN_NUM, Cur_Addr);

    /* Then the threads themselves */
    for(Count=0U;Count<RVM_BOOT_THD_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Thd_Crt(RVM_Meta_Thd_Crt[Count].Main,
                               RVM_BOOT_INIT_KOM,
                               RVM_Meta_Thd_Crt[Count].Slot,
                               RVM_Meta_Thd_Crt[Count].Prc, 
                               RVM_Meta_Thd_Crt[Count].Prio,
                               Cur_Addr)==0);
        
        RVM_DBG_S("Init:Thread '");
        RVM_DBG_I(RVM_Meta_Thd_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Thd_Crt[Count].Slot);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(Cur_Addr);
        RVM_DBG_S(".\r\n");
        
        Cur_Addr+=RVM_THD_SIZE;
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_THD_AFTER);
}
/* End Function:RVM_Boot_Thd_Crt *********************************************/

/* Begin Function:RVM_Boot_Inv_Crt ********************************************
Description : Create all invocations at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_INV_CRT_NUM!=0U)
void RVM_Boot_Inv_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_INV_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Creating invocations.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=RVM_Boot_Main_Crt(RVM_Meta_Inv_Main, RVM_BOOT_INV_MAIN_NUM, Cur_Addr);

    /* Then the invocations themselves */
    for(Count=0U;Count<RVM_BOOT_INV_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Inv_Crt(RVM_Meta_Inv_Crt[Count].Main,
                               RVM_BOOT_INIT_KOM,
                               RVM_Meta_Inv_Crt[Count].Slot,
                               RVM_Meta_Inv_Crt[Count].Prc,
                               Cur_Addr)==0);
        
        RVM_DBG_S("Init:Invocation '");
        RVM_DBG_I(RVM_Meta_Inv_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Inv_Crt[Count].Slot);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(Cur_Addr);
        RVM_DBG_S(".\r\n");
        
        Cur_Addr+=RVM_INV_SIZE;
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_INV_AFTER);
}
#endif
/* End Function:RVM_Boot_Inv_Crt *********************************************/

/* Begin Function:RVM_Boot_Rcv_Crt ********************************************
Description : Create all invocations at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_RCV_CRT_NUM!=0U)
void RVM_Boot_Rcv_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_RCV_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Creating receive endpoints.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=RVM_Boot_Main_Crt(RVM_Meta_Rcv_Main, RVM_BOOT_RCV_MAIN_NUM, Cur_Addr);

    /* Then the receive endpoints themselves */
    for(Count=0U;Count<RVM_BOOT_RCV_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Sig_Crt(RVM_Meta_Rcv_Crt[Count].Main,
                               RVM_Meta_Rcv_Crt[Count].Slot)==0);
        
        RVM_DBG_S("Init:Receive endpoint '");
        RVM_DBG_I(RVM_Meta_Rcv_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Rcv_Crt[Count].Slot);
        RVM_DBG_S("'.\r\n");
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_RCV_AFTER);
}
#endif
/* End Function:RVM_Boot_Rcv_Crt *********************************************/

/* Begin Function:RVM_Boot_Vcap_Init ******************************************
Description : Initialize all VM capability table special contents.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_VCAP_INIT_NUM!=0U)
void RVM_Boot_Vcap_Init(void)
{
    rvm_ptr_t Count;

    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Initializing VM capability tables.\r\n");

    for(Count=0U;Count<RVM_BOOT_VCAP_INIT_NUM;Count++)
    {
        /* Setup system call send endpoint at capability table location 0 */
        RVM_ASSERT(RVM_Cpt_Add(RVM_Meta_Vcap_Init[Count].Cpt,
                               0U,
                               RVM_BOOT_CPT,
                               RVM_Vmmd_Sig_Cap,
                               RVM_SIG_FLAG_SND)==0);
        
        RVM_ASSERT(RVM_Cpt_Add(RVM_Meta_Vcap_Init[Count].Cpt,
                               1U,
                               RVM_Meta_Vcap_Init[Count].Vep_Main,
                               RVM_Meta_Vcap_Init[Count].Vep_Slot,
                               RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV)==0);
        
        RVM_DBG_S("Init:VM send endpoints to captbl '0x");
        RVM_DBG_H(RVM_Meta_Vcap_Init[Count].Cpt);
        RVM_DBG_S("' at position 0 and 1.\r\n"); 
    }
}
#endif
/* End Function:RVM_Boot_Vcap_Init *******************************************/

/* Begin Function:RVM_Boot_Cpt_Init ****************************************
Description : Initialize the capability tables of all processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Cpt_Init(void)
{
    rvm_ptr_t Count;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Initializing process capability tables.\r\n");

#if(RVM_BOOT_CPT_INIT_NUM!=0U)
    for(Count=0U;Count<RVM_BOOT_CPT_INIT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Cpt_Add(RVM_Meta_Cpt_Init[Count].Dst,
                               RVM_Meta_Cpt_Init[Count].Pos_Dst,
                               RVM_Meta_Cpt_Init[Count].Src,
                               RVM_Meta_Cpt_Init[Count].Pos_Src,
                               RVM_Meta_Cpt_Init[Count].Flag)==0);
        
        RVM_DBG_S("Init:Cap to '0x");
        RVM_DBG_H(RVM_Meta_Cpt_Init[Count].Dst);
        RVM_DBG_S("' position '");
        RVM_DBG_I(RVM_Meta_Cpt_Init[Count].Pos_Dst);
        RVM_DBG_S("' from '0x");
        RVM_DBG_H(RVM_Meta_Cpt_Init[Count].Src);
        RVM_DBG_S("' position '");
        RVM_DBG_I(RVM_Meta_Cpt_Init[Count].Pos_Src);
        RVM_DBG_S("'.\r\n");
    }
#endif

#if(RVM_BOOT_CPT_KFUNC_NUM)
    for(Count=0U;Count<RVM_BOOT_CPT_KFUNC_NUM;Count++)
    {
        RVM_ASSERT(RVM_Cpt_Kfn(RVM_Meta_Cpt_Kfunc[Count].Dst,
                               RVM_Meta_Cpt_Kfunc[Count].Pos_Dst,
                               RVM_BOOT_CPT,
                               RVM_BOOT_INIT_KFN,
                               RVM_Meta_Cpt_Kfunc[Count].Start,
                               RVM_Meta_Cpt_Kfunc[Count].End)==0);
        
        RVM_DBG_S("Init:Kfunc to '0x");
        RVM_DBG_H(RVM_Meta_Cpt_Kfunc[Count].Dst);
        RVM_DBG_S("' position '");
        RVM_DBG_I(RVM_Meta_Cpt_Kfunc[Count].Pos_Dst);
        RVM_DBG_S("'.\r\n");
    }
#endif
}
/* End Function:RVM_Boot_Cpt_Init *****************************************/

/* Begin Function:RVM_Boot_Pgt_Init *****************************************
Description : Initialize the page tables of all processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Pgt_Init(void)
{
    rvm_ptr_t Count;

    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init:Initializing page tables.\r\n");

#if(RVM_BOOT_PGT_CON_NUM!=0U)
    /* Construct tree */
    for(Count=0U;Count<RVM_BOOT_PGT_CON_NUM;Count++)
    {
        RVM_ASSERT(RVM_Pgt_Con(RVM_Meta_Pgt_Con[Count].Parent,
                               RVM_Meta_Pgt_Con[Count].Position,
                               RVM_Meta_Pgt_Con[Count].Child,
                               RVM_PGT_ALL_PERM)==0);
        
        RVM_DBG_S("Init:Constructed child '0x");
        RVM_DBG_H(RVM_Meta_Pgt_Con[Count].Child);
        RVM_DBG_S("' into parent '0x");
        RVM_DBG_H(RVM_Meta_Pgt_Con[Count].Parent);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(RVM_Meta_Pgt_Con[Count].Position);
        RVM_DBG_S(".\r\n");
    }
#endif

    /* Map pages */
    for(Count=0U;Count<RVM_BOOT_PGT_ADD_NUM;Count++)
    {
        RVM_ASSERT(RVM_Pgt_Add(RVM_Meta_Pgt_Add[Count].Pgt_Dst,
                               RVM_Meta_Pgt_Add[Count].Pos_Dst,
                               RVM_Meta_Pgt_Add[Count].Flags,
                               RVM_BOOT_PGT,
                               RVM_Meta_Pgt_Add[Count].Pos_Src,
                               RVM_Meta_Pgt_Add[Count].Index)==0);
        
        RVM_DBG_S("Init:Added page to table '0x");
        RVM_DBG_H(RVM_Meta_Pgt_Add[Count].Pgt_Dst);
        RVM_DBG_S("' position '0x");
        RVM_DBG_H(RVM_Meta_Pgt_Add[Count].Pos_Dst);
        RVM_DBG_S("' from '0x");
        RVM_DBG_H(RVM_Meta_Pgt_Add[Count].Pos_Src);
        RVM_DBG_S("[0x");
        RVM_DBG_H(RVM_Meta_Pgt_Add[Count].Index);
        RVM_DBG_S("].\r\n");
    }
}
/* End Function:RVM_Boot_Pgt_Init ******************************************/

/* Begin Function:RVM_Prc_Init ***********************************************
Description : Initialize processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Prc_Init(void)
{
    /* Create kernel objects */
#if(RVM_BOOT_VEP_CRT_NUM!=0U)
    RVM_Boot_Vep_Crt();
#endif
    RVM_Boot_Cpt_Crt();
    RVM_Boot_Pgt_Crt();
    RVM_Boot_Prc_Crt();
    RVM_Boot_Thd_Crt();
#if(RVM_BOOT_INV_CRT_NUM!=0U)
    RVM_Boot_Inv_Crt();
#endif
#if(RVM_BOOT_RCV_CRT_NUM!=0U)
    RVM_Boot_Rcv_Crt();
#endif

    /* Initialize kernel objects */
#if(RVM_BOOT_VCAP_INIT_NUM!=0U)
    RVM_Boot_Vcap_Init();
#endif
    RVM_Boot_Cpt_Init();
    RVM_Boot_Pgt_Init();
    RVM_Boot_Thd_Init();
    RVM_Boot_Inv_Init();
    
    /* Initialize virtual machines */
    RVM_Virt_Crt(RVM_Virt, RVM_Vmap, RVM_VIRT_NUM);
}
/* End Function:RVM_Prc_Init ************************************************/

/* Begin Function:RVM_Daemon_Init *********************************************
Description : Initialize the daemons. These daemons include four ones:
              1. SFTD, the daemon for the startup and process recovery.
              2. TIMD, the daemon for time accounting and sending timer interrupts to VMs.
              3. VMMD, the daemon for real-time scheduling & hypercall handling.
              4. VCTD, the daemon for interrupt vector handling to VMs.
              The TID of all these daemons are zero.
Input       : rvm_cid_t Cap_Base - The capability frontier for allocation.
              rvm_ptr_t Kom_Base - The kernel memory pointer for allocation.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Daemon_Init(rvm_cid_t Cap_Base,
                     rvm_ptr_t Kom_Base)
{
    rvm_cid_t Cap_Front;
    rvm_ptr_t Kom_Front;
    
    Cap_Front=Cap_Base;
    Kom_Front=Kom_Base;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    
    /* Safety daemon initialization - highest priority as always */
    RVM_Sftd_Sig_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CPT, RVM_Sftd_Sig_Cap)==0);
    RVM_DBG_SIS("Init:Created safety daemon fault endpoint '",RVM_Sftd_Sig_Cap,"'.\r\n");
    
    RVM_Sftd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CPT, RVM_BOOT_INIT_KOM, RVM_Sftd_Thd_Cap,
                           RVM_BOOT_INIT_PRC, RVM_SFTD_PRIO, Kom_Front)>=0);
    RVM_DBG_SISHS("Init:Created safety daemon '",RVM_Sftd_Thd_Cap,"' @ 0x",Kom_Front,".\r\n");
    Kom_Front+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Sftd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_Sftd_Sig_Cap, RVM_Sftd_Thd_Cap, RVM_PREEMPT_PRIO_NUM-1U)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Sftd_Thd_Cap, (rvm_ptr_t)RVM_Sftd, 
                                RVM_Stack_Init(RVM_SFTD_STACK_BASE, RVM_SFTD_STACK_SIZE,
                                               (rvm_ptr_t)RVM_Sftd, (rvm_ptr_t)_RVM_Jmp_Stub),0U)==0);
    RVM_DBG_S("Init:Safety daemon initialization complete.\r\n");

    /* Don't bother boot these daemons if we do not have virtual machines installed at all */
#if(RVM_VIRT_NUM!=0U)
    /* Timer daemon initialization - main priority */
    RVM_Timd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CPT, RVM_BOOT_INIT_KOM, RVM_Timd_Thd_Cap,
                           RVM_BOOT_INIT_PRC, RVM_TIMD_PRIO, Kom_Front)>=0);
    RVM_DBG_SISHS("Init:Created timer daemon '",RVM_Timd_Thd_Cap,"' @ 0x",Kom_Front,".\r\n");
    Kom_Front+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Timd_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_Timd_Thd_Cap, RVM_TIMD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Timd_Thd_Cap, (rvm_ptr_t)RVM_Timd, 
                                RVM_Stack_Init(RVM_TIMD_STACK_BASE, RVM_TIMD_STACK_SIZE,
                                               (rvm_ptr_t)RVM_Timd, (rvm_ptr_t)_RVM_Jmp_Stub),0)==0);
    RVM_DBG_S("Init:Timer daemon initialization complete.\r\n");

    /* VMM daemon initialization - main priority */
    RVM_Vmmd_Sig_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CPT, RVM_Vmmd_Sig_Cap)==0);
    RVM_DBG_SIS("Init:Created virtual machine monitor endpoint '",RVM_Vmmd_Sig_Cap,"'.\r\n");
    
    RVM_Vmmd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CPT, RVM_BOOT_INIT_KOM, RVM_Vmmd_Thd_Cap,
                           RVM_BOOT_INIT_PRC, RVM_VMMD_PRIO, Kom_Front)>=0);
    RVM_DBG_SISHS("Init:Created virtual machine monitor daemon '",RVM_Vmmd_Thd_Cap,"' @ 0x",Kom_Front,".\r\n");
    Kom_Front+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Vmmd_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_Vmmd_Thd_Cap, RVM_VMMD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Vmmd_Thd_Cap, (rvm_ptr_t)RVM_Vmmd, 
                                RVM_Stack_Init(RVM_VMMD_STACK_BASE, RVM_VMMD_STACK_SIZE,
                                               (rvm_ptr_t)RVM_Vmmd, (rvm_ptr_t)_RVM_Jmp_Stub),0)==0);
    RVM_DBG_S("Init:Virtual machine monitor daemon initialization complete.\r\n");

    /* Interrupt relaying daemon initialization - main priority */
    RVM_Vctd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CPT, RVM_BOOT_INIT_KOM, RVM_Vctd_Thd_Cap,
                           RVM_BOOT_INIT_PRC, RVM_VCTD_PRIO, Kom_Front)>=0);
    RVM_DBG_SISHS("Init:Created vector handling daemon '",RVM_Vctd_Thd_Cap,"' @ 0x",Kom_Front,".\r\n");
    Kom_Front+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Vctd_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_Vctd_Thd_Cap, RVM_VCTD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Vctd_Thd_Cap, (rvm_ptr_t)RVM_Vctd, 
                                RVM_Stack_Init(RVM_VCTD_STACK_BASE, RVM_VCTD_STACK_SIZE,
                                               (rvm_ptr_t)RVM_Vctd, (rvm_ptr_t)_RVM_Jmp_Stub),0)==0);
    RVM_DBG_S("Init:Vector handling daemon initialization complete.\r\n");
#endif
}
/* End Function:RVM_Daemon_Init **********************************************/

/* Begin Function:RVM_Init ****************************************************
Description : The VMM's init thread, which boots the VMM from here.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Init(void)
{
    RVM_Boot_Pre_Init();

    RVM_DBG_S("\r\n\r\n-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("                               Welcome to the\r\n");
    RVM_DBG_S("\r\n");
    RVM_DBG_S("                        ######    ###  ###  ########\r\n");
    RVM_DBG_S("                        #######   ###  ###  ########\r\n");
    RVM_DBG_S("                        ##    ##  ###  ###  ##\r\n");
    RVM_DBG_S("                        ##    ##  ###  ###  ##\r\n");
    RVM_DBG_S("                        ##    ##  ## ## ##  ##\r\n");
    RVM_DBG_S("                        #######   ## ## ##  #######\r\n");
    RVM_DBG_S("                        ######    ## ## ##  #######\r\n");
    RVM_DBG_S("                        ##   ##   ## ## ##  ##\r\n");
    RVM_DBG_S("                        ##   ##   ##    ##  ##\r\n");
    RVM_DBG_S("                        ##    ##  ##    ##  ##\r\n");
    RVM_DBG_S("                        ##    ##  ##    ##  ########\r\n");
    RVM_DBG_S("                        ##    ### ##    ##  ########\r\n");
    RVM_DBG_S("\r\n");
    RVM_DBG_S("                    Microcontroller User-level Platform!\r\n");
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");

    /* Raise our own priority to the top of the system */
    RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_BOOT_INIT_THD, RVM_PREEMPT_PRIO_NUM-1U)==0);
    RVM_DBG_S("Init:Preparation - priority raised.\r\n");
    
    /* Initialize RVM database */
#if(RVM_VIRT_NUM!=0U)
    RVM_Virt_Init();
#endif
    RVM_DBG_SHS("Init:Kernel object memory base @ 0x", RVM_KOM_VA_BASE, ".\r\n");
    RVM_DBG_SHS("Init:Start creating kernel objects @ offset 0x", RVM_KOM_BOOT_FRONTIER, ".\r\n");
    
    /* Create the startup thread in the init process, because the init thread
     * cannot block. Bind that to the processor, and let it have infinite budget.
     * after this the task will be handled by this task, and we will never return
     * to init unless there is nothing to run */
    RVM_Daemon_Init(RVM_CAP_BOOT_FRONTIER, RVM_KOM_BOOT_FRONTIER);
    RVM_DBG_S("Init:Daemon initialization done.\r\n");
    
    /* Initialize the virtual machine databases, and create whatever is needed */
    RVM_Prc_Init();
    RVM_DBG_S("Init:Process initialization done.\r\n");
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    
    /* Delegate timeslice to safety daemon, and make it work - we should stop here
     * actually, and we have done operating the database, now all the work is the safety
     * daemon's. From now on, no further kernel objects will be created anymore. */
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Sftd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
#if(RVM_VIRT_NUM!=0U)
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Timd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Vmmd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Vctd_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
#endif
    RVM_DBG_S("Init:Daemon time budget initialization complete.\r\n");
    
    /* Put our priority to the lowest in system - this should get everything else running */
    RVM_DBG_S("Init:Job finished - lowering priority.\r\n");
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_BOOT_INIT_THD, RVM_INIT_PRIO)==0);

    RVM_Boot_Post_Init();
		
    while(1U)
    {
        /* Receive the scheduler notifications for the interrupt threads. If any of
         * them failed, then we just panic, because none of them are supposed to fail. */
        /* RVM_DBG_S("I"); */
        RVM_Idle();
    }
}
/* End Function:RVM_Init *****************************************************/

/* Begin Function:main ********************************************************
Description : The entry of the VMM's init thread.
Input       : None.
Output      : None.
Return      : int - This function shall never return.
******************************************************************************/
int main(void)
{
    RVM_Init();
}
/* End Function:main *********************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
