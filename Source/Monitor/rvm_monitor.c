/******************************************************************************
Filename    : rvm_monitor.c
Author      : pry
Date        : 11/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : Virtual machine monitor implementation.
******************************************************************************/

/* Include *******************************************************************/
#include "rvm_boot.h"

#define __HDR_DEF__
#include "rvm_platform.h"
#include "Syslib/rvm_syslib.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_DEF__

#define __HDR_STRUCT__
#include "rvm_platform.h"
#include "Syslib/rvm_syslib.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_STRUCT__

/* Private include */
#include "Monitor/rvm_monitor.h"

#define __HDR_PUBLIC__
#include "rvm_platform.h"
#include "Syslib/rvm_syslib.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_PUBLIC__
/* End Include ***************************************************************/

/* Function:RVM_Putchar *******************************************************
Description : Output a character to console. This is for user-level debugging 
              only.
Input       : char Char - The character to print.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_DBGLOG_ENABLE!=0U)
void RVM_Putchar(char Char)
{
    RVM_PUTCHAR(Char);
}
#endif
/* End Function:RVM_Putchar **************************************************/

/* Function:_RVM_Boot_Main_Crt ************************************************
Description : Create all 'main' capability tables at boot-time. Main capability 
              tables are placed into the boot-time capability table, and they
              contain the real kernel objects.
Input       : const struct RVM_Meta_Main_Struct* Main - The metadata array.
              rvm_ptr_t Number - The number of entries in the array.
              rvm_ptr_t Addr - The kmem address to create those tables from.
Output      : None.
Return      : None.
******************************************************************************/
static rvm_ptr_t _RVM_Boot_Main_Crt(const struct RVM_Meta_Main_Struct* Meta,
                                    rvm_ptr_t Number,
                                    rvm_ptr_t Addr)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=Addr;
    
    /* Create all the capability table capability tables first */
    for(Count=0U;Count<Number;Count++)
    {
        RVM_ASSERT(RVM_Cpt_Crt(RVM_BOOT_INIT_CPT,
                               RVM_BOOT_INIT_KOM,
                               Meta[Count].Slot,
                               Cur_Addr,
                               Meta[Count].Size)==0);
        RVM_DBG_S("Init: Main capability table '");
        RVM_DBG_I(Meta[Count].Slot);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(Cur_Addr);
        RVM_DBG_S(".\r\n");
        Cur_Addr+=RVM_CPT_SIZE(Meta[Count].Size);
    }

    return Cur_Addr;
}
/* End Function:_RVM_Boot_Main_Crt *******************************************/

/* Function:_RVM_Boot_Vep_Crt *************************************************
Description : Create all VM endpoints at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_VEP_CRT_NUM!=0U)
static void _RVM_Boot_Vep_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_VEP_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Creating VM endpoints.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=_RVM_Boot_Main_Crt(RVM_Meta_Vep_Main,RVM_BOOT_VEP_MAIN_NUM,Cur_Addr);

    /* Then the virtual endpoints themselves */
    for(Count=0U;Count<RVM_BOOT_VEP_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Sig_Crt(RVM_Meta_Vep_Crt[Count].Main,
                               RVM_Meta_Vep_Crt[Count].Slot)==0U);
        
        RVM_DBG_S("Init: VM endpoint '");
        RVM_DBG_I(RVM_Meta_Vep_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Vep_Crt[Count].Slot);
        RVM_DBG_S("'.\r\n");
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_VEP_AFTER);
}
#endif
/* End Function:_RVM_Boot_Vep_Crt ********************************************/

/* Function:_RVM_Boot_Cpt_Crt *************************************************
Description : Create all capability tables at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Boot_Cpt_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_INIT_CPT_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Creating capability tables.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=_RVM_Boot_Main_Crt(RVM_Meta_Cpt_Main,RVM_BOOT_INIT_CPT_MAIN_NUM,Cur_Addr);

    /* Then the capability tables themselves */
    for(Count=0U;Count<RVM_BOOT_INIT_CPT_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Cpt_Crt(RVM_Meta_Cpt_Crt[Count].Main,
                               RVM_BOOT_INIT_KOM,
                               RVM_Meta_Cpt_Crt[Count].Slot,
                               Cur_Addr,
                               RVM_Meta_Cpt_Crt[Count].Size)==0U);
        
        RVM_DBG_S("Init: Capability table '");
        RVM_DBG_I(RVM_Meta_Cpt_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Cpt_Crt[Count].Slot);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(Cur_Addr);
        RVM_DBG_S(".\r\n");
        
        Cur_Addr+=RVM_CPT_SIZE(RVM_Meta_Cpt_Crt[Count].Size);
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_INIT_CPT_AFTER);
}
/* End Function:_RVM_Boot_Cpt_Crt ********************************************/

/* Function:_RVM_Boot_Pgt_Crt *************************************************
Description : Create all page tables at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_PGT_RAW_ENABLE==0U)
static void _RVM_Boot_Pgt_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_INIT_PGT_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Creating page tables.\r\n");
    

    /* Create the main capability tables first */
    Cur_Addr=_RVM_Boot_Main_Crt(RVM_Meta_Pgt_Main,RVM_BOOT_INIT_PGT_MAIN_NUM,Cur_Addr);

    /* Then the page tables themselves */
    for(Count=0U;Count<RVM_BOOT_INIT_PGT_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Pgt_Crt(RVM_Meta_Pgt_Crt[Count].Main,
                               RVM_BOOT_INIT_KOM,
                               RVM_Meta_Pgt_Crt[Count].Slot,
                               Cur_Addr,
                               RVM_Meta_Pgt_Crt[Count].Base,
                               RVM_Meta_Pgt_Crt[Count].Is_Top,
                               RVM_Meta_Pgt_Crt[Count].Size_Order,
                               RVM_Meta_Pgt_Crt[Count].Num_Order)==0);
        
        RVM_DBG_S("Init: Page table '");
        RVM_DBG_I(RVM_Meta_Pgt_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Pgt_Crt[Count].Slot);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(Cur_Addr);
        RVM_DBG_S(".\r\n");
        
        if(RVM_Meta_Pgt_Crt[Count].Is_Top!=0U)
        {
            RVM_COV_MARKER();
            
            Cur_Addr+=RVM_PGT_SIZE_TOP(RVM_Meta_Pgt_Crt[Count].Num_Order);
        }
        else
        {
            RVM_COV_MARKER();
            
            Cur_Addr+=RVM_PGT_SIZE_NOM(RVM_Meta_Pgt_Crt[Count].Num_Order);
        }
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_INIT_PGT_AFTER);
}
#endif
/* End Function:_RVM_Boot_Pgt_Crt ********************************************/

/* Function:_RVM_Boot_Prc_Crt *************************************************
Description : Create all processes at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Boot_Prc_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_PRC_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Creating processes.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=_RVM_Boot_Main_Crt(RVM_Meta_Prc_Main,RVM_BOOT_PRC_MAIN_NUM,Cur_Addr);

    /* Then the processes themselves */
    for(Count=0U;Count<RVM_BOOT_PRC_CRT_NUM;Count++)
    {
#if(RVM_PGT_RAW_ENABLE==0U)
        RVM_ASSERT(RVM_Prc_Crt(RVM_Meta_Prc_Crt[Count].Main,
                               RVM_Meta_Prc_Crt[Count].Slot,
                               RVM_Meta_Prc_Crt[Count].Cpt,
                               RVM_Meta_Prc_Crt[Count].Pgt)==0);
#else
        RVM_ASSERT(RVM_Prc_Crt(RVM_Meta_Prc_Crt[Count].Main,
                               RVM_Meta_Prc_Crt[Count].Slot,
                               RVM_Meta_Prc_Crt[Count].Cpt,
                               RVM_Meta_Prc_Crt[Count].Raw)==0);
#endif
        
        RVM_DBG_S("Init: Process '");
        RVM_DBG_I(RVM_Meta_Prc_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Prc_Crt[Count].Slot);
        RVM_DBG_S("'.\r\n");
    }
    
    RVM_ASSERT(Cur_Addr==RVM_BOOT_PRC_AFTER);
}
/* End Function:_RVM_Boot_Prc_Crt ********************************************/

/* Function:_RVM_Boot_Thd_Crt *************************************************
Description : Create all threads at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Boot_Thd_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_THD_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Creating threads.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=_RVM_Boot_Main_Crt(RVM_Meta_Thd_Main,RVM_BOOT_THD_MAIN_NUM,Cur_Addr);

    /* Then the threads themselves */
    for(Count=0U;Count<RVM_BOOT_THD_CRT_NUM;Count++)
    {
        if(RVM_Meta_Thd_Crt[Count].Is_Hyp==0U)
        {
            RVM_COV_MARKER();
            
            RVM_ASSERT(RVM_Thd_Crt(RVM_Meta_Thd_Crt[Count].Main,
                                   RVM_BOOT_INIT_KOM,
                                   RVM_Meta_Thd_Crt[Count].Slot,
                                   RVM_Meta_Thd_Crt[Count].Prc, 
                                   RVM_Meta_Thd_Crt[Count].Prio,
                                   Cur_Addr,
                                   RVM_Meta_Thd_Crt[Count].Attr)==0);
            
            RVM_DBG_S("Init: Thread '");
            RVM_DBG_I(RVM_Meta_Thd_Crt[Count].Main);
            RVM_DBG_S(",");
            RVM_DBG_I(RVM_Meta_Thd_Crt[Count].Slot);
            RVM_DBG_S("' @ 0x");
            RVM_DBG_H(Cur_Addr);
            RVM_DBG_S(" attr 0x");
            RVM_DBG_H(RVM_Meta_Thd_Crt[Count].Attr);
            RVM_DBG_S(".\r\n");
            
            Cur_Addr+=RVM_THD_SIZE(RVM_Meta_Thd_Crt[Count].Attr);
        }
        else
        {
            RVM_COV_MARKER();
            
            RVM_ASSERT(RVM_Hyp_Crt(RVM_Meta_Thd_Crt[Count].Main,
                                   RVM_BOOT_INIT_KOM,
                                   RVM_Meta_Thd_Crt[Count].Slot,
                                   RVM_Meta_Thd_Crt[Count].Prc, 
                                   RVM_Meta_Thd_Crt[Count].Prio,
                                   Cur_Addr,
                                   RVM_Meta_Thd_Crt[Count].Attr)==0);
            
            RVM_DBG_S("Init: VM thread '");
            RVM_DBG_I(RVM_Meta_Thd_Crt[Count].Main);
            RVM_DBG_S(",");
            RVM_DBG_I(RVM_Meta_Thd_Crt[Count].Slot);
            RVM_DBG_S("' @ 0x");
            RVM_DBG_H(Cur_Addr);
            RVM_DBG_S(" attr 0x");
            RVM_DBG_H(RVM_Meta_Thd_Crt[Count].Attr);
            RVM_DBG_S(".\r\n");
            
            Cur_Addr+=RVM_HYP_SIZE;
        }
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_THD_AFTER);
}
/* End Function:_RVM_Boot_Thd_Crt ********************************************/

/* Function:_RVM_Boot_Inv_Crt *************************************************
Description : Create all invocations at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_INV_CRT_NUM!=0U)
static void _RVM_Boot_Inv_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_INV_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Creating invocations.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=_RVM_Boot_Main_Crt(RVM_Meta_Inv_Main,RVM_BOOT_INV_MAIN_NUM,Cur_Addr);

    /* Then the invocations themselves */
    for(Count=0U;Count<RVM_BOOT_INV_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Inv_Crt(RVM_Meta_Inv_Crt[Count].Main,
                               RVM_BOOT_INIT_KOM,
                               RVM_Meta_Inv_Crt[Count].Slot,
                               RVM_Meta_Inv_Crt[Count].Prc,
                               Cur_Addr)==0);
        
        RVM_DBG_S("Init: Invocation '");
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
/* End Function:_RVM_Boot_Inv_Crt ********************************************/

/* Function:_RVM_Boot_Rcv_Crt *************************************************
Description : Create all invocations at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_RCV_CRT_NUM!=0U)
static void _RVM_Boot_Rcv_Crt(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;

    Cur_Addr=RVM_BOOT_RCV_BEFORE;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Creating receive endpoints.\r\n");
    
    /* Create the main capability tables first */
    Cur_Addr=_RVM_Boot_Main_Crt(RVM_Meta_Rcv_Main,RVM_BOOT_RCV_MAIN_NUM,Cur_Addr);

    /* Then the receive endpoints themselves */
    for(Count=0U;Count<RVM_BOOT_RCV_CRT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Sig_Crt(RVM_Meta_Rcv_Crt[Count].Main,
                               RVM_Meta_Rcv_Crt[Count].Slot)==0);
        
        RVM_DBG_S("Init: Receive endpoint '");
        RVM_DBG_I(RVM_Meta_Rcv_Crt[Count].Main);
        RVM_DBG_S(",");
        RVM_DBG_I(RVM_Meta_Rcv_Crt[Count].Slot);
        RVM_DBG_S("'.\r\n");
    }

    RVM_ASSERT(Cur_Addr==RVM_BOOT_RCV_AFTER);
}
#endif
/* End Function:_RVM_Boot_Rcv_Crt ********************************************/

/* Function:_RVM_Boot_Kobj_Crt ************************************************
Description : Create kernel objects at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Boot_Kobj_Crt(void)
{
    /* Check macro code validity */
#if(RVM_ASSERT_ENABLE!=0U)
    RVM_ASSERT(RVM_VIRT_NUM==RVM_BOOT_VEP_CRT_NUM);
    RVM_ASSERT(RVM_VIRT_NUM==RVM_BOOT_VCAP_INIT_NUM);
    RVM_ASSERT(RVM_BOOT_INIT_CPT_CRT_NUM!=0U);
    RVM_ASSERT(RVM_BOOT_INIT_CPT_CRT_NUM!=0U);
#if(RVM_PGT_RAW_ENABLE==0U)
    RVM_ASSERT(RVM_BOOT_INIT_PGT_CRT_NUM!=0U);
    RVM_ASSERT(RVM_BOOT_INIT_PGT_ADD_NUM!=0U);
#endif
    RVM_ASSERT(RVM_BOOT_PRC_CRT_NUM!=0U);
    RVM_ASSERT(RVM_BOOT_THD_CRT_NUM!=0U);
    RVM_ASSERT(RVM_BOOT_THD_CRT_NUM==RVM_BOOT_THD_INIT_NUM);
    RVM_ASSERT(RVM_BOOT_INV_CRT_NUM==RVM_BOOT_INV_INIT_NUM);
#endif
    
    /* Create kernel objects */
#if(RVM_BOOT_VEP_CRT_NUM!=0U)
    _RVM_Boot_Vep_Crt();
#endif
    _RVM_Boot_Cpt_Crt();
#if(RVM_PGT_RAW_ENABLE==0U)
    _RVM_Boot_Pgt_Crt();
#endif
    _RVM_Boot_Prc_Crt();
    _RVM_Boot_Thd_Crt();
#if(RVM_BOOT_INV_CRT_NUM!=0U)
    _RVM_Boot_Inv_Crt();
#endif
#if(RVM_BOOT_RCV_CRT_NUM!=0U)
    _RVM_Boot_Rcv_Crt();
#endif
}
/* End Function:_RVM_Boot_Kobj_Crt *******************************************/

/* Function:_RVM_Boot_Vcap_Init ***********************************************
Description : Initialize all VM capability table special contents.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_VCAP_INIT_NUM!=0U)
static void _RVM_Boot_Vcap_Init(void)
{
    rvm_ptr_t Count;

    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Initializing VM capability tables.\r\n");

    for(Count=0U;Count<RVM_BOOT_VCAP_INIT_NUM;Count++)
    {
        /* Setup system call send endpoint at captbl location 0 */
        RVM_ASSERT(RVM_Cpt_Add(RVM_Meta_Vcap_Init[Count].Cpt,
                               0U,
                               RVM_BOOT_INIT_CPT,
                               RVM_BOOT_INIT_VCT,
                               RVM_SIG_FLAG_SND)==0);
        
        /* Setup software interrupt send/receive endpoint at captbl location 1 */
        RVM_ASSERT(RVM_Cpt_Add(RVM_Meta_Vcap_Init[Count].Cpt,
                               1U,
                               RVM_Meta_Vcap_Init[Count].Vep_Main,
                               RVM_Meta_Vcap_Init[Count].Vep_Slot,
                               RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV)==0);
        
        RVM_DBG_S("Init: VM send endpoints to captbl '0x");
        RVM_DBG_H(RVM_Meta_Vcap_Init[Count].Cpt);
        RVM_DBG_S("' at position 0 and 1.\r\n"); 
    }
}
#endif
/* End Function:_RVM_Boot_Vcap_Init ******************************************/

/* Function:_RVM_Boot_Cpt_Init ************************************************
Description : Initialize the capability tables of all processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Boot_Cpt_Init(void)
{
    rvm_ptr_t Count;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Initializing process capability tables.\r\n");

#if(RVM_BOOT_INIT_CPT_INIT_NUM!=0U)
    for(Count=0U;Count<RVM_BOOT_INIT_CPT_INIT_NUM;Count++)
    {
        RVM_ASSERT(RVM_Cpt_Add(RVM_Meta_Cpt_Init[Count].Dst,
                               RVM_Meta_Cpt_Init[Count].Pos_Dst,
                               RVM_Meta_Cpt_Init[Count].Src,
                               RVM_Meta_Cpt_Init[Count].Pos_Src,
                               RVM_Meta_Cpt_Init[Count].Flag)==0);
        
        RVM_DBG_S("Init: Cap to '0x");
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

#if(RVM_BOOT_INIT_CPT_KFN_NUM)
    for(Count=0U;Count<RVM_BOOT_INIT_CPT_KFN_NUM;Count++)
    {
        RVM_ASSERT(RVM_Cpt_Kfn(RVM_Meta_Cpt_Kfn[Count].Dst,
                               RVM_Meta_Cpt_Kfn[Count].Pos_Dst,
                               RVM_BOOT_INIT_CPT,
                               RVM_BOOT_INIT_KFN,
                               RVM_Meta_Cpt_Kfn[Count].Start,
                               RVM_Meta_Cpt_Kfn[Count].End)==0);
        
        RVM_DBG_S("Init: Kfunc to '0x");
        RVM_DBG_H(RVM_Meta_Cpt_Kfn[Count].Dst);
        RVM_DBG_S("' position '");
        RVM_DBG_I(RVM_Meta_Cpt_Kfn[Count].Pos_Dst);
        RVM_DBG_S("'.\r\n");
    }
#endif
}
/* End Function:_RVM_Boot_Cpt_Init *******************************************/

/* Function:_RVM_Boot_Pgt_Init ************************************************
Description : Initialize the page tables of all processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_PGT_RAW_ENABLE==0U)
static void _RVM_Boot_Pgt_Init(void)
{
    rvm_ptr_t Count;

    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Initializing page tables.\r\n");

#if(RVM_BOOT_INIT_PGT_CON_NUM!=0U)
    /* Construct tree */
    for(Count=0U;Count<RVM_BOOT_INIT_PGT_CON_NUM;Count++)
    {
        RVM_ASSERT(RVM_Pgt_Con(RVM_Meta_Pgt_Con[Count].Parent,
                               RVM_Meta_Pgt_Con[Count].Position,
                               RVM_Meta_Pgt_Con[Count].Child,
                               RVM_PGT_ALL_PERM)==0);
        
        RVM_DBG_S("Init: Constructed child '0x");
        RVM_DBG_H(RVM_Meta_Pgt_Con[Count].Child);
        RVM_DBG_S("' into parent '0x");
        RVM_DBG_H(RVM_Meta_Pgt_Con[Count].Parent);
        RVM_DBG_S("' @ 0x");
        RVM_DBG_H(RVM_Meta_Pgt_Con[Count].Position);
        RVM_DBG_S(".\r\n");
    }
#endif

    /* Map pages */
    for(Count=0U;Count<RVM_BOOT_INIT_PGT_ADD_NUM;Count++)
    {
        RVM_ASSERT(RVM_Pgt_Add(RVM_Meta_Pgt_Add[Count].Pgt_Dst,
                               RVM_Meta_Pgt_Add[Count].Pos_Dst,
                               RVM_Meta_Pgt_Add[Count].Flag,
                               RVM_BOOT_INIT_PGT,
                               RVM_Meta_Pgt_Add[Count].Pos_Src,
                               RVM_Meta_Pgt_Add[Count].Index)==0);
        
        RVM_DBG_S("Init: Added page to table '0x");
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
#endif
/* End Function:_RVM_Boot_Pgt_Init *******************************************/

/* Function:_RVM_Boot_Thd_Init ************************************************
Description : Initialize the all threads.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Boot_Thd_Init(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Init_Entry;
    rvm_ptr_t Init_Stub;
    rvm_ptr_t Init_Stack;

    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Initializing threads.\r\n");
    
    for(Count=0U;Count<RVM_BOOT_THD_INIT_NUM;Count++)
    {
        /* Bind thread to safety daemon - if this is a VM thread, set its address accordingly */
        if(RVM_Meta_Thd_Init[Count].Flag==0U)
        {
            RVM_COV_MARKER();
            
            RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Meta_Thd_Init[Count].Thd,
                                          RVM_Sftd_Thd_Cap,
                                          RVM_Sftd_Sig_Cap,
                                          RVM_Meta_Thd_Init[Count].Thd,
                                          RVM_Meta_Thd_Init[Count].Prio)==0);
        }
        else
        {
            RVM_COV_MARKER();
            
            RVM_ASSERT(RVM_Hyp_Sched_Bind(RVM_Meta_Thd_Init[Count].Thd,
                                          RVM_Sftd_Thd_Cap,
                                          RVM_Sftd_Sig_Cap,
                                          RVM_VIRT_TID(RVM_Meta_Thd_Init[Count].Thd),
                                          RVM_Meta_Thd_Init[Count].Prio,
                                          RVM_Meta_Thd_Init[Count].Reg_Base)==0);
        }
        
        /* Initialize stack with whatever we have to initialize */
        Init_Entry=RVM_DESC_ENTRY(RVM_Meta_Thd_Init[Count].Code_Base,
                                  RVM_Meta_Thd_Init[Count].Desc_Slot);
        Init_Stub=RVM_DESC_STUB(RVM_Meta_Thd_Init[Count].Code_Base);
        Init_Stack=RVM_Stack_Init(RVM_Meta_Thd_Init[Count].Stack_Base,
                                  RVM_Meta_Thd_Init[Count].Stack_Size,
                                  &Init_Entry,Init_Stub);
        
        /* Set execution info and transfer time */
        RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Meta_Thd_Init[Count].Thd,
                                    (void*)Init_Entry,(void*)Init_Stack,
                                    (void*)RVM_Meta_Thd_Init[Count].Param)==0);
        
        RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Meta_Thd_Init[Count].Thd,
                                     RVM_BOOT_INIT_THD,
                                     RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
        
        if(RVM_Meta_Thd_Init[Count].Flag!=0U)
        {
            RVM_COV_MARKER();
            
            if(RVM_Meta_Thd_Init[Count].Reg_Base!=0U)
            {
                RVM_COV_MARKER();
                
                RVM_DBG_S("Init: VM user '0x");
            }
            else
            {
                RVM_COV_MARKER();
                
                RVM_DBG_S("Init: VM vector '0x");
            }
        }
        else
        {
            RVM_COV_MARKER();
            
            RVM_DBG_S("Init: Thread '0x");
        }
        
        RVM_DBG_H(RVM_Meta_Thd_Init[Count].Thd);
        RVM_DBG_S("' desc 0x");
        RVM_DBG_H(RVM_Meta_Thd_Init[Count].Desc_Slot);
        RVM_DBG_S(" stack 0x");
        RVM_DBG_H(RVM_Meta_Thd_Init[Count].Stack_Base);
        RVM_DBG_S(" size 0x");
        RVM_DBG_H(RVM_Meta_Thd_Init[Count].Stack_Size);
        RVM_DBG_S(" param 0x");
        RVM_DBG_H(RVM_Meta_Thd_Init[Count].Param);
        if(RVM_Meta_Thd_Init[Count].Reg_Base!=0U)
        {
            RVM_COV_MARKER();
                
            RVM_DBG_S(" haddr 0x");
            RVM_DBG_H(RVM_Meta_Thd_Init[Count].Reg_Base);
        }
        else
        {
            RVM_COV_MARKER();
            /* No action required */
        }
        RVM_DBG_S(".\r\n");
    }
}
/* End Function:_RVM_Boot_Thd_Init *******************************************/

/* Function:_RVM_Boot_Inv_Init ************************************************
Description : Initialize the all invocations.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_BOOT_INV_INIT_NUM!=0U)
static void _RVM_Boot_Inv_Init(void)
{
    rvm_ptr_t Count;
    rvm_ptr_t Init_Entry;
    rvm_ptr_t Init_Stub;
    rvm_ptr_t Init_Stack;

    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_DBG_S("Init: Initializing invocations.\r\n");
    
    for(Count=0U;Count<RVM_BOOT_INV_INIT_NUM;Count++)
    {
        Init_Entry=RVM_DESC_ENTRY(RVM_Meta_Inv_Init[Count].Code_Base,
                                  RVM_Meta_Inv_Init[Count].Desc_Slot);
        Init_Stub=RVM_DESC_STUB(RVM_Meta_Inv_Init[Count].Code_Base);
        Init_Stack=RVM_Stack_Init(RVM_Meta_Inv_Init[Count].Stack_Base,
                                  RVM_Meta_Inv_Init[Count].Stack_Size,
                                  &Init_Entry,Init_Stub);
                                       
        /* These invocations always return on error */
        RVM_ASSERT(RVM_Inv_Set(RVM_Meta_Inv_Init[Count].Inv,
                               Init_Entry,Init_Stack,1U)==0);

        RVM_DBG_S("Init: Invocation '0x");
        RVM_DBG_H(RVM_Meta_Inv_Init[Count].Inv);
        RVM_DBG_S("' desc 0x");
        RVM_DBG_H(RVM_Meta_Inv_Init[Count].Desc_Slot);
        RVM_DBG_S(" stack base 0x");
        RVM_DBG_H(RVM_Meta_Inv_Init[Count].Stack_Base);
        RVM_DBG_S(" size 0x");
        RVM_DBG_H(RVM_Meta_Inv_Init[Count].Stack_Size);
        RVM_DBG_S(".\r\n");
    }
}
#endif
/* End Function:_RVM_Boot_Inv_Init *******************************************/

/* Function:_RVM_Boot_Kobj_Init ***********************************************
Description : Initialize kernel objects at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Boot_Kobj_Init(void)
{
    /* Initialize kernel objects */
#if(RVM_BOOT_VCAP_INIT_NUM!=0U)
    _RVM_Boot_Vcap_Init();
#endif
    _RVM_Boot_Cpt_Init();
#if(RVM_PGT_RAW_ENABLE==0U)
    _RVM_Boot_Pgt_Init();
#endif
    _RVM_Boot_Thd_Init();
#if(RVM_BOOT_INV_INIT_NUM!=0U)
    _RVM_Boot_Inv_Init();
#endif
}
/* End Function:_RVM_Boot_Kobj_Init ******************************************/

/* Function:_RVM_Virt_Crt ******************************************************
Description : Set up all virtual machines according to the database.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine database.
              const struct RVM_Vmap_Struct* Vmap - The virtual machine static memory map.
              rvm_ptr_t Virt_Num - The number of virtual machines in the system.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
static void _RVM_Virt_Crt(struct RVM_Virt_Struct* Virt,
                          const struct RVM_Vmap_Struct* Vmap,
                          rvm_ptr_t Virt_Num)
{
    rvm_ptr_t Count;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    
    /* Initialize basic data structure one by one */
    for(Count=0U;Count<Virt_Num;Count++)
    {
        /* State set to running, but interrupts are disabled, and VM uninitailized */
        Virt[Count].Sched.State=RVM_VM_RUNNING;
        Virt[Count].Sched.Slice_Left=Vmap[Count].Slice;
        RVM_List_Crt(&Virt[Count].Vct_Head);
        Virt[Count].Map=&Vmap[Count];
        
        /* Insert into timer wheel - +Count here to make sure that they distribute
         * on the timer wheel as evenly as possible */
        _RVM_Wheel_Ins(&Virt[Count],Vmap[Count].Period+Count);
        
        /* Set all these virtual machines as ready, but all the threads' priority at idle */
        _RVM_Run_Ins(&Virt[Count]);
        RVM_ASSERT(RVM_Thd_Sched_Prio(Vmap[Count].Usr_Thd_Cap,RVM_WAIT_PRIO)==0U);
        RVM_ASSERT(RVM_Thd_Sched_Prio(Vmap[Count].Vct_Thd_Cap,RVM_WAIT_PRIO)==0U);
        
        /* Clean up all virtual interrupt flags/parameters for this virtual machine */
        RVM_Clear(Vmap[Count].State_Base,Vmap[Count].State_Size);
        
        /* Clean up all event send capabilities for this virtual machine */
        RVM_Clear(Virt[Count].Evt_Cap,RVM_EVTCAP_BITMAP*RVM_WORD_BYTE);

        /* Print log */
        RVM_DBG_S("Init: Created VM ");
        RVM_DBG_S(Vmap[Count].Name);
        RVM_DBG_S(" control block 0x");
        RVM_DBG_H((rvm_ptr_t)&Virt[Count]);
        RVM_DBG_S(" database 0x");
        RVM_DBG_H((rvm_ptr_t)&Vmap[Count]);
        RVM_DBG_S(".\r\n");
    }
    
    /* Now set up the scheduler for the first runnable VM, and switch to it */
    RVM_Virt_Cur=_RVM_Run_High();
    _RVM_Virt_Switch(RVM_NULL,RVM_Virt_Cur);
}
#endif
/* End Function:_RVM_Virt_Crt ************************************************/

/* Function:_RVM_Prc_Init *****************************************************
Description : Initialize processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Prc_Init(void)
{
    /* Create kernel objects */
    _RVM_Boot_Kobj_Crt();
    
    /* Initialize kernel objects */
    _RVM_Boot_Kobj_Init();
    
    /* Initialize virtual machines */
#if(RVM_VIRT_NUM!=0U)
    _RVM_Virt_Crt(RVM_Virt,RVM_Vmap,RVM_VIRT_NUM);
#endif
}
/* End Function:_RVM_Prc_Init ************************************************/

/* Function:_RVM_Daemon_Init **************************************************
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
static void _RVM_Daemon_Init(rvm_cid_t Cap_Base,
                             rvm_ptr_t Kom_Base)
{
    rvm_cid_t Cap_Front;
    rvm_ptr_t Kom_Front;
    rvm_ptr_t Init_Stack;
    rvm_ptr_t Init_Entry;
    
    Cap_Front=Cap_Base;
    Kom_Front=Kom_Base;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    
    /* Safety daemon initialization - highest priority as always */
    RVM_Sftd_Sig_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_INIT_CPT,RVM_Sftd_Sig_Cap)==0);
    RVM_DBG_SIS("Init: Created safety daemon fault endpoint '",RVM_Sftd_Sig_Cap,"'.\r\n");
    
    RVM_Sftd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_INIT_CPT,RVM_BOOT_INIT_KOM,RVM_Sftd_Thd_Cap,
                           RVM_BOOT_INIT_PRC,RVM_SFTD_PRIO,Kom_Front,0U)>=0);
    RVM_DBG_SISHS("Init: Created safety daemon '",RVM_Sftd_Thd_Cap,"' @ 0x",Kom_Front,".\r\n");
    Kom_Front+=RVM_THD_SIZE(0U);
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Sftd_Thd_Cap,RVM_BOOT_INIT_THD,RVM_Sftd_Sig_Cap,RVM_Sftd_Thd_Cap,RVM_PREEMPT_PRIO_NUM-1U)==0);
    Init_Entry=(rvm_ptr_t)RVM_Sftd;
    Init_Stack=RVM_Stack_Init(RVM_SFTD_STACK_BASE,RVM_SFTD_STACK_SIZE,&Init_Entry,(rvm_ptr_t)__RVM_Stub);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Sftd_Thd_Cap,(void*)Init_Entry,(void*)Init_Stack,RVM_NULL)==0);
    RVM_DBG_S("Init: Safety daemon initialization complete.\r\n");

#if(RVM_VIRT_NUM!=0U)
    /* VMM daemon initialization - main priority - don't boot this if we have no VM at all */
    RVM_Vmmd_Thd_Cap=Cap_Front++;
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_INIT_CPT,RVM_BOOT_INIT_KOM,RVM_Vmmd_Thd_Cap,
                           RVM_BOOT_INIT_PRC,RVM_VMMD_PRIO,Kom_Front,0U)>=0);
    RVM_DBG_SISHS("Init: Created VMM daemon '",RVM_Vmmd_Thd_Cap,"' @ 0x",Kom_Front,".\r\n");
    Kom_Front+=RVM_THD_SIZE(0U);
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Vmmd_Thd_Cap,RVM_Sftd_Thd_Cap,RVM_Sftd_Sig_Cap,RVM_Vmmd_Thd_Cap,RVM_VMMD_PRIO)==0);
    Init_Entry=(rvm_ptr_t)RVM_Vmmd;
    Init_Stack=RVM_Stack_Init(RVM_VMMD_STACK_BASE,RVM_VMMD_STACK_SIZE,&Init_Entry,(rvm_ptr_t)__RVM_Stub);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Vmmd_Thd_Cap,(void*)Init_Entry,(void*)Init_Stack,RVM_NULL)==0);
    RVM_DBG_S("Init: Main daemon initialization complete.\r\n");
#endif
}
/* End Function:_RVM_Daemon_Init *********************************************/

/* Function:_RVM_Virt_Init ****************************************************
Description : Initialize necessary virtual machine monitor database.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
static void _RVM_Virt_Init(void)
{
    rvm_ptr_t Count;
    
    RVM_Switch=0U;
    
    RVM_Tick=0U;
    for(Count=0U;Count<RVM_WHEEL_SIZE;Count++)
    {
        RVM_List_Crt(&RVM_Wheel[Count]);
    }
    
    RVM_Virt_Cur=0U;
    for(Count=0U;Count<RVM_VPRIO_BITMAP;Count++)
    {
        RVM_Bitmap[Count]=0U;
    }
    for(Count=0U;Count<RVM_PREEMPT_VPRIO_NUM;Count++)
    {
        RVM_List_Crt(&RVM_Run[Count]);
    }
    
    for(Count=0U;Count<RVM_VIRT_EVT_NUM;Count++)
    {
        RVM_List_Crt(&RVM_Evt[Count]);
    }
    for(Count=0U;Count<RVM_PHYS_VCT_NUM;Count++)
    {
        RVM_List_Crt(&RVM_Phys[Count]);
    }
    
    RVM_List_Crt(&RVM_Map_Free);
    for(Count=0U;Count<RVM_VIRT_MAP_NUM;Count++)
    {
        RVM_List_Ins(&RVM_Map[Count].Dst_Head,RVM_Map_Free.Prev,&RVM_Map_Free);
    }
    
    /* Clean up physical vector flags and event flags */
    RVM_Clear((void*)RVM_PHYS_VCTF_BASE,RVM_PHYS_VCTF_SIZE);
    RVM_Clear((void*)RVM_VIRT_EVTF_BASE,RVM_VIRT_EVTF_SIZE);
}
#endif
/* End Function:_RVM_Virt_Init ***********************************************/

/* Function:RVM_Init **********************************************************
Description : The VMM's init thread, which boots the VMM from here.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void RVM_Init(void)
{
    RVM_Boot_Pre_Init();

    /* Print boot logo */
    RVM_DBG_S("\r\n");
    RVM_DBG_S("            ___  _   __ __  ___\r\n");
    RVM_DBG_S("           / _ \\| | / //  |/  /    Feather-weight hypervisor\r\n");
    RVM_DBG_S("          / , _/| |/ // /|_/ /      Starting boot sequence...\r\n");
    RVM_DBG_S("         /_/|_| |___//_/  /_/\r\n");
    RVM_DBG_S("===============================================================================\r\n");

    /* Raise our own priority to the top of the system */
    RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_BOOT_INIT_THD,RVM_PREEMPT_PRIO_NUM-1U)==0);
    RVM_DBG_S("Init: Preparation - priority raised.\r\n");
    
    /* Initialize RVM database */
#if(RVM_VIRT_NUM!=0U)
    _RVM_Virt_Init();
#endif
    RVM_DBG_SHS("Init: Kernel object memory base @ 0x",RVM_KOM_VA_BASE,".\r\n");
    RVM_DBG_SHS("Init: Start creating kernel objects @ offset 0x",RVM_KOM_BOOT_FRONT, ".\r\n");
    
    /* Create the startup thread in the init process, because the init thread
     * cannot block. Bind that to the processor, and let it have infinite budget.
     * after this the task will be handled by this task, and we will never return
     * to init unless there is nothing to run */
    _RVM_Daemon_Init(RVM_CPT_BOOT_FRONT,RVM_KOM_BOOT_FRONT);
    RVM_DBG_S("Init: Daemon initialization done.\r\n");
    
    /* Initialize the virtual machine databases, and create whatever is needed */
    _RVM_Prc_Init();
    RVM_DBG_S("Init: Process initialization done.\r\n");
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    
    /* Delegate timeslice to safety daemon and VMM daemon */
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Sftd_Thd_Cap,RVM_BOOT_INIT_THD,RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
#if(RVM_VIRT_NUM!=0U)
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Vmmd_Thd_Cap,RVM_BOOT_INIT_THD,RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
#endif
    RVM_DBG_S("Init: Daemon time budget initialization complete.\r\n");
    
    /* Put our priority to the lowest in system - this should get everything else running */
    RVM_DBG_S("Init: Job finished - lowering priority.\r\n");
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_BOOT_INIT_THD,RVM_INIT_PRIO)==0);

    RVM_Boot_Post_Init();
		
    /* Wait for next interrupt */
    while(1U)
    {
        /* RVM_DBG_S("I"); */
#if(RVM_IDLE_SLEEP_ENABLE!=0U)
        RVM_Kfn_Act(RVM_BOOT_INIT_KFN,RVM_KFN_IDLE_SLEEP,0U,0U,0U);
#endif
    }
}
/* End Function:RVM_Init *****************************************************/

/* Function:main **************************************************************
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

/* Function:_RVM_Run_Ins ******************************************************
Description : Set the virtual machine as ready to schedule.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to put into 
                                             the runqueue.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
static void _RVM_Run_Ins(struct RVM_Virt_Struct* Virt)
{
    /* Insert this into the corresponding runqueue */
    RVM_List_Ins(&(Virt->Head),RVM_Run[Virt->Map->Prio].Prev,&(RVM_Run[Virt->Map->Prio]));
    /* Set this runlevel as active */
    RVM_BITMAP_SET(RVM_Bitmap,Virt->Map->Prio);
}
/* End Function:_RVM_Run_Ins *************************************************/

/* Function:_RVM_Run_Del ******************************************************
Description : Clear the virtual machine from the runqueue.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to delete from
                                             the runqueue.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Run_Del(struct RVM_Virt_Struct* Virt)
{
    /* See if it is the last thread on the priority level */
    if(Virt->Head.Prev==Virt->Head.Next)
    {
        RVM_COV_MARKER();
        
        /* Clear the runlevel */
        RVM_BITMAP_CLR(RVM_Bitmap,Virt->Map->Prio);
    }
    else
    {
        RVM_COV_MARKER();
        /* No action required */
    }
    
    /* Delete this from the corresponding runqueue */
    RVM_List_Del(Virt->Head.Prev,Virt->Head.Next);
}
/* End Function:_RVM_Run_Del *************************************************/

/* Function:_RVM_Run_High *****************************************************
Description : Get the highest priority ready virtual machine available.
Input       : None.
Output      : None.
Return      : struct RVM_Virt_Struct* Virt - The virtual machine. If all virtual
                                             machine have gone asleep, this will
                                             be NULL.
******************************************************************************/
static struct RVM_Virt_Struct* _RVM_Run_High(void)
{
    rvm_cnt_t Count;
    
    /* See which one is ready, and pick it */
    for(Count=RVM_VPRIO_BITMAP-1;Count>=0;Count--)
    {
        if(RVM_Bitmap[Count]==0U)
        {
            RVM_COV_MARKER();
        
            continue;
        }
        else
        {
            RVM_COV_MARKER();
            /* No action required */
        }
        
        Count=(rvm_cnt_t)RVM_MSB_GET(RVM_Bitmap[Count])+(Count<<RVM_WORD_ORDER);
        return (struct RVM_Virt_Struct*)(RVM_Run[Count].Next);
    }
    
    return RVM_NULL;
}
/* End Function:_RVM_Run_High ************************************************/

/* Function:_RVM_Virt_Switch **************************************************
Description : Switch between two virtual machines.
Input       : struct RVM_Virt_Struct* From - The source VM.
              struct RVM_Virt_Struct* To - The destination VM.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Virt_Switch(struct RVM_Virt_Struct* From, 
                             struct RVM_Virt_Struct* To)
{
    if(From==To)
    {
        RVM_COV_MARKER();
        
        return;
    }
    else
    {
        RVM_COV_MARKER();
        /* No action required */
    }
    
    /* Just change the thread's priorities - use 2x version to minimize delay */
    if(From!=RVM_NULL)
    {
        RVM_COV_MARKER();
        
        RVM_ASSERT(RVM_Thd_Sched_Prio2(From->Map->Usr_Thd_Cap, RVM_WAIT_PRIO,
                                       From->Map->Vct_Thd_Cap, RVM_WAIT_PRIO)==0);
    }
    else
    {
        RVM_COV_MARKER();
        /* No action required */
    }
    
    if(To!=RVM_NULL)
    {
        RVM_COV_MARKER();
        
        RVM_ASSERT(RVM_Thd_Sched_Prio2(To->Map->Usr_Thd_Cap, RVM_VUSR_PRIO,
                                       To->Map->Vct_Thd_Cap, RVM_VVCT_PRIO)==0);
    }
    else
    {
        RVM_COV_MARKER();
        /* No action required */
    }
}
/* End Function:_RVM_Virt_Switch *********************************************/

/* Function:_RVM_Virt_Pend_Check **********************************************
Description : Check if there is one pending vector for the virtual machine.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to check.
Output      : None.
Return      : If there is one interrupt pending, return 1; else 0.
******************************************************************************/
static rvm_ret_t _RVM_Virt_Pend_Check(struct RVM_Virt_Struct* Virt)
{
    rvm_ptr_t Count;
    rvm_ptr_t Limit;
    volatile rvm_ptr_t* Vct;
    volatile struct RVM_Vctf* Flag;
    
    Flag=&Virt->Map->State_Base->Flag;
    
    /* Check timer and context vector */
    if((Flag->Tim!=0U)||(Flag->Ctx!=0U))
    {
        RVM_COV_MARKER();
        
        return 1;
    }
    else
    {
        RVM_COV_MARKER();
        /* No action required */
    }
    
    /* Check other vectors - we know that the structure must be always word-aligned,
     * and are using some casting tricks to avoid possible compiler warning here. */
    Vct=(volatile rvm_ptr_t*)((rvm_ptr_t)Flag->Vct);
    Limit=RVM_ROUND_UP(Virt->Map->Vct_Num,RVM_WORD_ORDER-3U);
    for(Count=0U;Count<Limit;Count++)
    {
        if(Vct[Count]!=0U)
        {
            RVM_COV_MARKER();
        
            return 1;
        }
        else
        {
            RVM_COV_MARKER();
            /* No action required */
        }
    }
    
    return 0;
}
/* End Function:_RVM_Virt_Pend_Check *****************************************/

/* Function:_RVM_Virt_Vct_Snd *************************************************
Description : Send to all virtual machine vectors registered on this physical 
              physical interrupt channel or event.
Input       : struct RVM_List* Array - The array containing lists of registered
                                       virtual vectors.
              rvm_ptr_t Num - The physical vector ID or event ID.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Virt_Vct_Snd(struct RVM_List* Array,
                              rvm_ptr_t Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    struct RVM_Virt_Struct* Virt;
    
    /* Send to every receipient VM on the list */
    Trav=Array[Num].Next;
    while(Trav!=&Array[Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        Virt=Map->Virt;
        
        /* Set the corresponding virtual vector activation flag */
        Virt->Map->State_Base->Flag.Vct[Map->Vct_Num]=1U;
        RVM_ASSERT(RVM_Sig_Snd(Virt->Map->Vct_Sig_Cap)==0);
        
        /* If it is waiting, notify it of new interrupts */
        if(RVM_VM_STATE(Virt->Sched.State)==RVM_VM_WAITING)
        {
            RVM_COV_MARKER();
            
            RVM_VM_STATE_SET(RVM_Virt_Cur->Sched.State,RVM_VM_RUNNING);
            _RVM_Run_Ins(Virt);
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
        
        Trav=Trav->Next;
    }

    /* May need a context switch, someone received an interrupt */
    RVM_Switch=1U;
}
/* End Function:_RVM_Virt_Vct_Snd ********************************************/

/* Function:_RVM_Virt_Recover *************************************************
Description : Print the registers, then recover the current virtual machine.
Input       : rvm_s8_t* Daemon - The daemon that called this.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Virt_Recover(rvm_s8_t* Daemon)
{
    struct RVM_Map_Struct* Map;
    rvm_ptr_t Init_Entry;
    rvm_ptr_t Init_Stack;

#if(RVM_DBGLOG_ENABLE!=0U)
    /* Print thread registers */
    RVM_DBG_S(Daemon);
    RVM_DBG_S(": Vector handling thread register set:\r\n");
    RVM_ASSERT(RVM_Thd_Print_Reg(RVM_Virt_Cur->Map->Vct_Thd_Cap)==0);
    RVM_DBG_S(Daemon);
    RVM_DBG_S(": User program thread register set:\r\n");
    RVM_ASSERT(RVM_Thd_Print_Reg(RVM_Virt_Cur->Map->Usr_Thd_Cap)==0);
#endif

    /* Unbind the threads from the core then rebind in case they are still blocked. 
     * This can break the assumption that the vector thread shall run first. */
    RVM_ASSERT(RVM_Thd_Sched_Free(RVM_Virt_Cur->Map->Vct_Thd_Cap)==0);
    RVM_ASSERT(RVM_Thd_Sched_Free(RVM_Virt_Cur->Map->Usr_Thd_Cap)==0);
    
    /* Rebind them */
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Virt_Cur->Map->Vct_Thd_Cap,RVM_Sftd_Thd_Cap,RVM_Sftd_Sig_Cap, 
                                  RVM_VIRT_TID(RVM_Virt_Cur->Map->Vct_Thd_Cap),RVM_VVCT_PRIO)==0);
    RVM_ASSERT(RVM_Hyp_Sched_Bind(RVM_Virt_Cur->Map->Usr_Thd_Cap,RVM_Sftd_Thd_Cap,RVM_Sftd_Sig_Cap, 
                                  RVM_VIRT_TID(RVM_Virt_Cur->Map->Usr_Thd_Cap),RVM_VUSR_PRIO, 
                                  (rvm_ptr_t)(RVM_Virt_Cur->Map->Reg_Base))==0);
    
    /* Set the execution properties for virt @ position 0 */
    Init_Entry=RVM_DESC_ENTRY(RVM_Virt_Cur->Map->Desc_Base,0U);
    Init_Stack=RVM_Stack_Init(RVM_Virt_Cur->Map->Vct_Stack_Base,RVM_Virt_Cur->Map->Vct_Stack_Size,
                              &Init_Entry,RVM_DESC_STUB(RVM_Virt_Cur->Map->Desc_Base));
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Virt_Cur->Map->Vct_Thd_Cap,
                                (void*)Init_Entry,(void*)Init_Stack,RVM_NULL)==0);
    
    /* Set the execution properties for user @ position 1 */
    Init_Entry=RVM_DESC_ENTRY(RVM_Virt_Cur->Map->Desc_Base,1U);
    Init_Stack=RVM_Stack_Init(RVM_Virt_Cur->Map->Usr_Stack_Base, RVM_Virt_Cur->Map->Usr_Stack_Size,
                              &Init_Entry,RVM_DESC_STUB(RVM_Virt_Cur->Map->Desc_Base));
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Virt_Cur->Map->Usr_Thd_Cap,
                                (void*)Init_Entry,(void*)Init_Stack,RVM_NULL)==0);
    
    /* Delegate infinite time to both threads - still needed because we have unbound them */
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Virt_Cur->Map->Vct_Thd_Cap,
                                 RVM_BOOT_INIT_THD,RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Virt_Cur->Map->Usr_Thd_Cap,
                                 RVM_BOOT_INIT_THD,RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    
    /* Clear all of its register state and interrupt flags */
    RVM_Clear(RVM_Virt_Cur->Map->State_Base,RVM_Virt_Cur->Map->State_Size);
    
    /* Clear all of its event capabilities */
    RVM_Clear(RVM_Virt_Cur->Evt_Cap,RVM_EVTCAP_BITMAP*RVM_WORD_BYTE);

    /* Set the state to running, interrupt disabled, vector unlocked, and watchdog disabled */
    RVM_Virt_Cur->Sched.State=RVM_VM_RUNNING;
    
    /* If it registered for anything, delete them and return them to database */
    while(RVM_Virt_Cur->Vct_Head.Next!=&(RVM_Virt_Cur->Vct_Head))
    {
        Map=RVM_DST2MAP(RVM_Virt_Cur->Vct_Head.Next);
        RVM_List_Del(Map->Src_Head.Prev,Map->Src_Head.Next);
        RVM_List_Del(Map->Dst_Head.Prev,Map->Dst_Head.Next);
        RVM_List_Ins(&(Map->Src_Head),&RVM_Map_Free,RVM_Map_Free.Next);
    }
    
    /* Reinsert this into the wheel */
    RVM_List_Del(RVM_Virt_Cur->Delay.Prev,RVM_Virt_Cur->Delay.Next);
    _RVM_Wheel_Ins(RVM_Virt_Cur,RVM_Virt_Cur->Map->Period);

    /* Context switch needed, someone rebooted */
    RVM_Switch=1U;
}
/* End Function:_RVM_Virt_Recover ********************************************/

/* Function:_RVM_Wheel_Ins ****************************************************
Description : Insert one VM into the timer wheel.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to insert.
              rvm_ptr_t Period - The period of the timer interrupt.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Wheel_Ins(struct RVM_Virt_Struct* Virt,
                           rvm_ptr_t Period)
{
    struct RVM_List* Slot;
    struct RVM_List* Trav_Ptr;
    struct RVM_Virt_Struct* Trav_Virt;
    
    Slot=&(RVM_Wheel[(RVM_Tick+Period)%RVM_WHEEL_SIZE]);
    
    Trav_Ptr=Slot->Next;
    while(Trav_Ptr!=Slot)
    {
        Trav_Virt=RVM_DLY2VM(Trav_Ptr);
        if((Trav_Virt->Sched.Period_Timeout-RVM_Tick)>Period)
        {
            RVM_COV_MARKER();
        
            break;
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
        
        Trav_Ptr=Trav_Ptr->Next;
    }

    /* Insert this into the list */
    Virt->Sched.Period_Timeout=RVM_Tick+Period;
    RVM_List_Ins(&(Virt->Delay),Trav_Ptr->Prev,Trav_Ptr);
}
/* End Function:_RVM_Wheel_Ins ***********************************************/

/* Function:_RVM_Tim_Snd ******************************************************
Description : Send an timer interrupt to a virtual machine.
Input       : struct RVM_Virt_Struct* Virt - The pointer to the virtual machine.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Tim_Snd(struct RVM_Virt_Struct* Virt)
{
    if(RVM_VM_STATE(Virt->Sched.State)==RVM_VM_WAITING)
    {
        RVM_COV_MARKER();
        
        _RVM_Run_Ins(Virt);
        RVM_VM_STATE_SET(Virt->Sched.State,RVM_VM_RUNNING);
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Set the timer bit */
    Virt->Map->State_Base->Flag.Tim=1U;
    
    /* Send interrupt to it, if its interrupts are enabled */
    if((Virt->Sched.State&RVM_VM_INTENA)!=0U)
    {
        RVM_COV_MARKER();
        
        RVM_ASSERT(RVM_Sig_Snd(Virt->Map->Vct_Sig_Cap)==0);
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* Context switch needed, someone received a timer interrupt */
    RVM_Switch=1U;
}
/* End Function:_RVM_Tim_Snd *************************************************/

/* Function:_RVM_Flagset_Get **************************************************
Description : Get the physical vector source or event source from the interrupt
              set. When this is called, there must be at least one vector or
              event pending in this set.
Input       : volatile struct RVM_Flag* Set - The flagset data.
Output      : volatile struct RVM_Flag* Set - The flagset data.
Return      : rvm_ptr_t - The interrupt number.
******************************************************************************/
static rvm_ptr_t _RVM_Flagset_Get(volatile struct RVM_Flag* Set)
{
    rvm_ptr_t Group;
    rvm_ptr_t Member;
    
    /* See which source group could possibly have any interrupts */
    Group=RVM_MSB_GET(Set->Group);
    Member=RVM_MSB_GET(Set->Flag[Group]);
    
    /* Clean up the slot now */
    Set->Flag[Group]&=~RVM_POW2(Member);
    if(Set->Flag[Group]==0U)
    {
        RVM_COV_MARKER();
        
        Set->Group&=~RVM_POW2(Group);
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    return (Group<<RVM_WORD_ORDER)|Member;
}
/* End Function:_RVM_Flagset_Get *********************************************/

/* Function:_RVM_Vctset_Handle ************************************************
Description : Process a single vector flagset.
Input       : volatile struct RVM_Flag* Set - The vector flagset data.
Output      : volatile struct RVM_Flag* Set - The vector flagset data.
Return      : rvm_ptr_t - The timer tick status.
******************************************************************************/
static rvm_ptr_t _RVM_Vctset_Handle(volatile struct RVM_Flag* Set)
{
    rvm_ptr_t Tick;
    rvm_ptr_t Number;
    
    Tick=0U;
    
    /* Lock set */
    Set->Lock=1U;
    
    /* Detect timer interrupt */
    if(Set->Fast!=0U)
    {
        RVM_COV_MARKER();
        
        Set->Fast=0U;
        Tick=1U;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Detect the rest */
    while(Set->Group!=0U)
    {
        /* Process interrupts until we ran out */
        Number=_RVM_Flagset_Get(Set);
        
        /* Only send if smaller than the number of physical vectors declared */
        if((Number<RVM_PHYS_VCT_NUM)&&(RVM_Vct_Filter(Number)==RVM_FILTER_PASS))
        {
            RVM_COV_MARKER();
            
            _RVM_Virt_Vct_Snd(RVM_Phys,Number);
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
    }
    
    /* Unlock set */
    Set->Lock=0U;
        
    return Tick;
}
/* End Function:_RVM_Vctset_Handle *******************************************/

/* Function:_RVM_Evtset_Handle ************************************************
Description : Process a single event flag set.
Input       : volatile struct RVM_Flag* Set - The event flagset data.
Output      : volatile struct RVM_Flag* Set - The event flagset data.
Return      : None.
******************************************************************************/
static void _RVM_Evtset_Handle(volatile struct RVM_Flag* Set)
{
    rvm_ptr_t Number;
    
    /* Lock set */
    Set->Lock=1U;
    
    while(Set->Group!=0U)
    {
        /* Process interrupts until we ran out */
        Number=_RVM_Flagset_Get(Set);
        
        /* Only send if smaller than the number of events declared */
        if((Number<RVM_VIRT_EVT_NUM)&&(RVM_Evt_Filter(Number)==RVM_FILTER_PASS))
        {
            RVM_COV_MARKER();
            
            _RVM_Virt_Vct_Snd(RVM_Evt,Number);
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
    }
    
    /* Unlock set */
    Set->Lock=0U;
}
/* End Function:_RVM_Evtset_Handle *******************************************/

/* Function:_RVM_Hyp_Reboot ***************************************************
Description : Reboot the current virtual machine.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Reboot(void)
{
    /* Watchdog timeout - seek to reboot the VM */
    RVM_DBG_S("Hypd: Reboot request from virtual machine ");
    RVM_DBG_S(RVM_Virt_Cur->Map->Name);
    RVM_DBG_S(". Rebooting...\r\n");

    /* Print registers & reboot */
    _RVM_Virt_Recover((rvm_s8_t*)"Hypd");
    
    RVM_DBG_S("Hypd: Rebooted.\r\n");
    return 0;
}
/* End Function:_RVM_Hyp_Reboot **********************************************/

/* Function:_RVM_Hyp_Putchar **************************************************
Description : Print one character to the RVM debug console.
Input       : rvm_ptr_t Char - The character.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_DBGLOG_ENABLE!=0U)
static rvm_ret_t _RVM_Hyp_Putchar(rvm_ptr_t Char)
{
    RVM_Putchar((char)Char);
    
    return 0;
}
#endif
/* End Function:_RVM_Hyp_Putchar *********************************************/

/* Function:_RVM_Hyp_Int_Ena **************************************************
Description : Enable interrupts for a virtual machine. Need to call this when
              the virtual machine fave finished all its initialization routines
              or it wouldn't be able to receive interrupts.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Int_Ena(void)
{
    if((RVM_Virt_Cur->Sched.State&RVM_VM_INTENA)!=0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
        
    /* If the virtual machine ever calls this, then it means that it have done booting */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_BOOTED)==0U)
    {
        RVM_COV_MARKER();
    
        RVM_Virt_Cur->Sched.State|=RVM_VM_BOOTED;
        RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_Virt_Cur->Map->Vct_Thd_Cap, RVM_VVCT_PRIO)==0);
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Enable interrupt */
    RVM_Virt_Cur->Sched.State|=RVM_VM_INTENA;
    
    /* Trigger virtual interrupt processing if there are pending ones */
    if(_RVM_Virt_Pend_Check(RVM_Virt_Cur)!=0)
    {
        RVM_COV_MARKER();
        
        RVM_ASSERT(RVM_Sig_Snd(RVM_Virt_Cur->Map->Vct_Sig_Cap)==0);
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    return 0;
}
/* End Function:_RVM_Hyp_Int_Ena *********************************************/

/* Function:_RVM_Hyp_Int_Dis **************************************************
Description : Disable the interrupt for the virtual machine. All interrupts for a
              virtual machine, including the tick timer interrupt,is disabled on
              startup.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Int_Dis(void)
{
    if((RVM_Virt_Cur->Sched.State&RVM_VM_INTENA)==0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    RVM_Virt_Cur->Sched.State&=~RVM_VM_INTENA;

    return 0;
}
/* End Function:_RVM_Hyp_Int_Dis *********************************************/

/* Function:_RVM_Hyp_Vct_Phys *************************************************
Description : Register the virtual machine's virtual vector with a physical vector.
Input       : rvm_ptr_t Phys_Num - The physical vector number.
              rvm_ptr_t Vct_Num - The virtual vector number.
Output      : None.
Return      : rvm_ret_t - If successful, the interrupt registration ID; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Vct_Phys(rvm_ptr_t Phys_Num,
                                   rvm_ptr_t Vct_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if both numbers are overrange */
    if((Phys_Num>=RVM_PHYS_VCT_NUM)||(Vct_Num>=(RVM_Virt_Cur->Map->Vct_Num)))
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_RANGE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* Check if the VM have registered for this physical vector before */
    Trav=RVM_Phys[Phys_Num].Next;
    while(Trav!=&RVM_Phys[Phys_Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        
        if(Map->Virt==RVM_Virt_Cur)
        {
            RVM_COV_MARKER();
        
            return RVM_ERR_PHYS;
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
        
        Trav=Trav->Next;
    }
    
    /* Check if the VM's vector already have something mapped */
    Trav=RVM_Virt_Cur->Vct_Head.Next;
    while(Trav!=&(RVM_Virt_Cur->Vct_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vct_Num==Vct_Num)
        {
            RVM_COV_MARKER();
            
            return RVM_ERR_VIRT;
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }

        Trav=Trav->Next;
    }
    
    /* See if there are empty interrupt registration blocks available */
    if(RVM_Map_Free.Next==&RVM_Map_Free)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_MAP;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Register it */
    Map=(struct RVM_Map_Struct*)(RVM_Map_Free.Next);
    
    RVM_List_Del(Map->Src_Head.Prev,Map->Src_Head.Next);
    RVM_List_Ins(&(Map->Src_Head),RVM_Phys[Phys_Num].Prev, &RVM_Phys[Phys_Num]);
    RVM_List_Ins(&(Map->Dst_Head),RVM_Virt_Cur->Vct_Head.Prev, &(RVM_Virt_Cur->Vct_Head));
    
    Map->Virt=RVM_Virt_Cur;
    Map->Vct_Num=Vct_Num;
    
    return 0;
}
/* End Function:_RVM_Hyp_Vct_Phys ********************************************/

/* Function:_RVM_Hyp_Vct_Evt **************************************************
Description : Register the virtual machine's virtual vector with an event channel.
Input       : rvm_ptr_t Evt_Num - The event number.
              rvm_ptr_t Vct_Num - The virtual vector number.
Output      : None.
Return      : rvm_ret_t - If successful, the event channel ID; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Vct_Evt(rvm_ptr_t Evt_Num,
                                  rvm_ptr_t Vct_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if both numbers are overrange */
    if((Evt_Num>=RVM_VIRT_EVT_NUM)||(Vct_Num>=(RVM_Virt_Cur->Map->Vct_Num)))
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_RANGE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* Check if the VM have registered for this event channel before */
    Trav=RVM_Evt[Evt_Num].Next;
    while(Trav!=&RVM_Evt[Evt_Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        
        if(Map->Virt==RVM_Virt_Cur)
        {
            RVM_COV_MARKER();
            
            return RVM_ERR_EVT;
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
        
        Trav=Trav->Next;
    }
    
    /* Check if the VM's vector already have something mapped */
    Trav=RVM_Virt_Cur->Vct_Head.Next;
    while(Trav!=&(RVM_Virt_Cur->Vct_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vct_Num==Vct_Num)
        {
            RVM_COV_MARKER();
            
            return RVM_ERR_VIRT;
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }

        Trav=Trav->Next;
    }
    
    /* See if there are empty interrupt registration blocks available */
    if(RVM_Map_Free.Next==&RVM_Map_Free)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_MAP;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Register it */
    Map=(struct RVM_Map_Struct*)(RVM_Map_Free.Next);
    
    RVM_List_Del(Map->Src_Head.Prev,Map->Src_Head.Next);
    RVM_List_Ins(&(Map->Src_Head),RVM_Evt[Evt_Num].Prev,&RVM_Evt[Evt_Num]);
    RVM_List_Ins(&(Map->Dst_Head),RVM_Virt_Cur->Vct_Head.Prev,&(RVM_Virt_Cur->Vct_Head));
    
    Map->Virt=RVM_Virt_Cur;
    Map->Vct_Num=Vct_Num;
    
    return 0;
}
/* End Function:_RVM_Hyp_Vct_Evt *********************************************/

/* Function:_RVM_Hyp_Vct_Del **************************************************
Description : Deregister the vector of an virtual machine.
Input       : rvm_ptr_t Vct_Num - The virtual vector to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Vct_Del(rvm_ptr_t Vct_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if the number is overrange */
    if(Vct_Num>=RVM_Virt_Cur->Map->Vct_Num)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_RANGE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* Is it registered to anything? */
    Trav=RVM_Virt_Cur->Vct_Head.Next;
    while(Trav!=&(RVM_Virt_Cur->Vct_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vct_Num==Vct_Num)
        {
            RVM_COV_MARKER();
            
            /* Found it. Deregister this from the database and put it back to free */
            RVM_List_Del(Map->Src_Head.Prev,Map->Src_Head.Next);
            RVM_List_Del(Map->Dst_Head.Prev,Map->Dst_Head.Next);
            RVM_List_Ins(&(Map->Src_Head),RVM_Map_Free.Prev,&RVM_Map_Free);
            return 0;
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }

        Trav=Trav->Next;
    }
    
    /* Nothing found */
    return RVM_ERR_VIRT;
}
/* End Function:_RVM_Hyp_Vct_Del *********************************************/

/* Function:_RVM_Hyp_Vct_Lck **************************************************
Description : Lockdown the vector mapping of an virtual machine.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Vct_Lck(void)
{
    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Lock down mappings */
    RVM_Virt_Cur->Sched.State|=RVM_VM_VCTLCK;
    
    return 0;
}
/* End Function:_RVM_Hyp_Vct_Lck *********************************************/

/* Function:_RVM_Hyp_Vct_Wait *************************************************
Description : Set the virtual machine to sleep until a vector comes in.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Vct_Wait(void)
{
    /* See if it have interrupt disabled */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_INTENA)==0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* Now suspend the virtual machine and switch to somebody else */
    RVM_VM_STATE_SET(RVM_Virt_Cur->Sched.State,RVM_VM_WAITING);
    _RVM_Run_Del(RVM_Virt_Cur);
    
    /* Need a context switch, someone went to sleep */
    RVM_Switch=1U;
    
    return 0;
}
/* End Function:_RVM_Hyp_Vct_Wait ********************************************/

/* Function:_RVM_Hyp_Evt_Add **************************************************
Description : Add a event source's send capability to virtual machine.
Input       : rvm_ptr_t Evt_Num - The event souce to register.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Evt_Add(rvm_ptr_t Evt_Num)
{
    rvm_ptr_t* Slot;
    rvm_ptr_t Value;
    
    /* See if both numbers are overrange */
    if(Evt_Num>=RVM_VIRT_EVT_NUM)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_RANGE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Is this virtual machine already registered on this? */
    Slot=&(RVM_Virt_Cur->Evt_Cap[Evt_Num>>RVM_WORD_ORDER]);
    Value=RVM_POW2(Evt_Num&RVM_MASK_END(RVM_WORD_ORDER-1U));
    
    /* We have registered, no need to register again */
    if((Slot[0]&Value)!=0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_EVT;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Register this */
    Slot[0]|=Value;
    
    return 0;
}
/* End Function:_RVM_Hyp_Evt_Add *********************************************/

/* Function:_RVM_Hyp_Evt_Del **************************************************
Description : Delete a event source's send capability from virtual machine.
Input       : rvm_ptr_t Evt_Num - The event souce to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Evt_Del(rvm_ptr_t Evt_Num)
{
    rvm_ptr_t* Slot;
    rvm_ptr_t Value;
    
    /* See if both numbers are overrange */
    if(Evt_Num>=RVM_VIRT_EVT_NUM)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_RANGE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }

    /* Is this virtual machine already registered on this? */
    Slot=&(RVM_Virt_Cur->Evt_Cap[Evt_Num>>RVM_WORD_ORDER]);
    Value=RVM_POW2(Evt_Num&RVM_MASK_END(RVM_WORD_ORDER-1U));
    
    /* We have deregistered, no need to deregister again */
    if((Slot[0]&Value)==0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_EVT;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Deregister this */
    Slot[0]&=~Value;
    
    return 0;
}
/* End Function:_RVM_Hyp_Evt_Del *********************************************/

/* Function:_RVM_Hyp_Evt_Snd **************************************************
Description : Send an event to the event channel.
Input       : rvm_ptr_t Evt_Num - The event channel ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Evt_Snd(rvm_ptr_t Evt_Num)
{
    rvm_ptr_t Value;
    rvm_ptr_t* Slot;
    
    /* See if the number is overrange */
    if(Evt_Num>=RVM_VIRT_EVT_NUM)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_RANGE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Are we allowed to send to this at all? */
    Slot=&(RVM_Virt_Cur->Evt_Cap[Evt_Num>>RVM_WORD_ORDER]);
    Value=RVM_POW2(Evt_Num&RVM_MASK_END(RVM_WORD_ORDER-1U));
    
    /* We are not allowed to send to this at all */
    if((Slot[0]&Value)==0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_EVT;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Send to that event */
    _RVM_Virt_Vct_Snd(RVM_Evt,Evt_Num);
    
    return 0;
}
/* End Function:_RVM_Hyp_Evt_Snd *********************************************/

/* Function:_RVM_Hyp_Wdg_Clr **************************************************
Description : Start and feed the watchdog for the current virtual machine.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
static rvm_ret_t _RVM_Hyp_Wdg_Clr(void)
{   
    /* See if this virtual machine have watchdog enabled */
    if(RVM_Virt_Cur->Map->Watchdog==0U)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_STATE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* Set state and reset watchdog counter */
    RVM_Virt_Cur->Sched.State|=RVM_VM_WDGENA;
    RVM_Virt_Cur->Sched.Watchdog_Left=RVM_Virt_Cur->Map->Watchdog;
    
    return 0;
}
/* End Function:_RVM_Hyp_Wdg_Clr *********************************************/

/* Function:RVM_Vmmd **********************************************************
Description : The system daemon for timer tick processing, interrupt handling 
              and hypercalls. This only exists when there are virtual machines
              installed.
              It contains three logical daemons to handle different events, which
              are Hypd for hypercalls, Vctd for interrupts, and Timd for timer
              ticks respectively.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void RVM_Vmmd(void)
{
    rvm_ptr_t Tick;
    rvm_ptr_t Number;
    struct RVM_List* Slot;
    struct RVM_List* Trav;
    struct RVM_Virt_Struct* Virt;
    /* Might be changed by the user at any time */
    volatile rvm_ptr_t* Param;
    volatile struct RVM_State* State;
    volatile struct RVM_Param* Arg;
    /* Might be changed by the kernel at any time */
    volatile struct RVM_Flag* Vctf_Set0;
    volatile struct RVM_Flag* Vctf_Set1;
    volatile struct RVM_Flag* Evtf_Set0;
    volatile struct RVM_Flag* Evtf_Set1;
    
    RVM_DBG_S("Vmmd: Monitor daemon initialization complete.\r\n");

    /* Compiler will optimize these out */
    Vctf_Set0=RVM_FLAG_SET(RVM_PHYS_VCTF_BASE,RVM_PHYS_VCTF_SIZE,0U);
    Vctf_Set1=RVM_FLAG_SET(RVM_PHYS_VCTF_BASE,RVM_PHYS_VCTF_SIZE,1U);
    Evtf_Set0=RVM_FLAG_SET(RVM_VIRT_EVTF_BASE,RVM_VIRT_EVTF_SIZE,0U);
    Evtf_Set1=RVM_FLAG_SET(RVM_VIRT_EVTF_BASE,RVM_VIRT_EVTF_SIZE,1U);

    while(1U)
    {
        RVM_COV_MARKER();
        
        /* Blocking multi receive, wait for activation signal */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_BOOT_INIT_VCT,RVM_RCV_BM)>=0);
        
/* Hypd daemon for hypercall handling ****************************************/
        /* See if there is an active VM to handle hypercalls */
        if(RVM_Virt_Cur!=RVM_NULL)
        {
            RVM_COV_MARKER();
            
            /* See if the vector is active */
            State=RVM_Virt_Cur->Map->State_Base;
            if(State->Vct_Act!=0U)
            {
                RVM_COV_MARKER();
                
                Arg=&(State->Vct);
            }
            else
            {
                RVM_COV_MARKER();
                
                Arg=&(State->Usr);
            }
            /* Extract number and parameters - and see if such call is valid */
            Number=Arg->Number;
            if(Number!=RVM_HYP_SPURIOUS)
            {
                RVM_COV_MARKER();
                
                Param=Arg->Param;
                /* Indicate that we've responded to this hypercall */
                Arg->Number=RVM_HYP_SPURIOUS;
                
                /* RVM_DBG_SIS("Hypd: Hypercall ",Number," called.\r\n"); */
            
                switch(Number)
                {
#if(RVM_DBGLOG_ENABLE!=0U)
                    /* Debug hypercall */
                    case RVM_HYP_PUTCHAR:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Putchar(Param[0]);     /* rvm_ptr_t Char */
                        break;
                    }
#endif
                    /* Reboot hypercall */
                    case RVM_HYP_REBOOT:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Reboot();
                        break;
                    }
                    /* Interrupt hypercalls */
                    case RVM_HYP_INT_ENA:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Int_Ena();
                        break;
                    }
                    case RVM_HYP_INT_DIS:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Int_Dis();
                        break;
                    }
                    /* Virtual vector hypercall */
                    case RVM_HYP_VCT_PHYS:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Vct_Phys(Param[0],     /* rvm_ptr_t Phys_Num */
                                                              Param[1]);    /* rvm_ptr_t Vct_Num */
                        break;
                    }
                    case RVM_HYP_VCT_EVT:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Vct_Evt(Param[0],      /* rvm_ptr_t Evt_Num */
                                                             Param[1]);     /* rvm_ptr_t Vct_Num */
                        break;
                    }
                    case RVM_HYP_VCT_DEL:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Vct_Del(Param[0]);     /* rvm_ptr_t Vct_Num */
                        break;
                    }
                    case RVM_HYP_VCT_LCK:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Vct_Lck();
                        break;
                    }
                    case RVM_HYP_VCT_WAIT:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Vct_Wait();
                        break;
                    }
                    /* Event send hypercall */
                    case RVM_HYP_EVT_ADD:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Evt_Add(Param[0]);     /* rvm_ptr_t Evt_Num */
                        break;
                    }
                    case RVM_HYP_EVT_DEL:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Evt_Del(Param[0]);     /* rvm_ptr_t Evt_Num */
                        break;
                    }
                    case RVM_HYP_EVT_SND:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Evt_Snd(Param[0]);     /* rvm_ptr_t Evt_Num */
                        break;
                    }
                    /* Watchdog hypercall */
                    case RVM_HYP_WDG_CLR:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Wdg_Clr();
                        break;
                    }

                    default:break;
                }
                    
                /* RVM_DBG_SIS("Hypd: Hypercall ",Number," processed.\r\n"); */
            }
            else
            {
                RVM_COV_MARKER();
                /* No operation required */
            }
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
        
/* Vctd daemon for interrupt relaying ****************************************/
        /* Handle vector flags - Tick is for Timd, not Vctd */
        Tick=_RVM_Vctset_Handle(Vctf_Set0);
        Tick|=_RVM_Vctset_Handle(Vctf_Set1);
        
        /* Handle event flags */
        _RVM_Evtset_Handle(Evtf_Set0);
        _RVM_Evtset_Handle(Evtf_Set1);
        
/* Timd daemon for timer tick tracking ***************************************/
        /* See if there is a timer tick */
        if(Tick!=0U)
        {
            RVM_Tick++;
            
            /* Notify daemon passes if the debugging output is enabled. This
             * should not pop-up when doing ISR-related coverage or performance
             * tests; 100k ticks are long enough for a complete run. */
#if(RVM_DBGLOG_ENABLE!=0U)
            if((RVM_Tick%100000U)==0U)
            {
                RVM_COV_MARKER();
                
                RVM_DBG_S("Timd: 100k ticks passed.\r\n");
            }
            else
            {
                RVM_COV_MARKER();
                /* No operation required */
            }
#endif
            
            /* Process the timer wheel to deliver timer interrupts to VMs */
            Slot=&(RVM_Wheel[RVM_Tick%RVM_WHEEL_SIZE]);
            Trav=Slot->Next;
            while(Trav!=Slot)
            {
                Virt=RVM_DLY2VM(Trav);
                Trav=Trav->Next;
                /* If the value is more than this, then it means that the time
                 * have already passed and we have to process this */
                if((RVM_Tick-Virt->Sched.Period_Timeout)>=(RVM_MASK_FULL>>1))
                {
                    RVM_COV_MARKER();
                
                    break;
                }
                else
                {
                    RVM_COV_MARKER();
                    /* No operation required */
                }
                
                /* This VM should be processed, place it at the new position */
                RVM_List_Del(Virt->Delay.Prev,Virt->Delay.Next);
                _RVM_Wheel_Ins(Virt,Virt->Map->Period);
                
                /* Send special timer interrupt to it */
                _RVM_Tim_Snd(Virt);
            }
            
            /* If there is at least one virtual machine working, check slices and watchdog */
            if(RVM_Virt_Cur!=RVM_NULL)
            {
                RVM_COV_MARKER();
                
                /* Is the timeslices exhausted? */
                if(RVM_Virt_Cur->Sched.Slice_Left==0U)
                {
                    RVM_COV_MARKER();
                    
                    RVM_Virt_Cur->Sched.Slice_Left=RVM_Virt_Cur->Map->Slice;
                    /* Place it at the end of the run queue */
                    RVM_List_Del(RVM_Virt_Cur->Head.Prev,RVM_Virt_Cur->Head.Next);
                    RVM_List_Ins(&(RVM_Virt_Cur->Head),
                                 RVM_Run[RVM_Virt_Cur->Map->Prio].Prev,
                                 &(RVM_Run[RVM_Virt_Cur->Map->Prio]));
                    
                    /* Context switch needed, someone exhausted a timeslice */
                    RVM_Switch=1U;
                }
                else
                {
                    RVM_COV_MARKER();
                    
                    RVM_Virt_Cur->Sched.Slice_Left--;
                }
                
                /* Is the watchdog enabled for this virtual machine? */
                if((RVM_Virt_Cur->Sched.State&RVM_VM_WDGENA)!=0U)
                {
                    RVM_COV_MARKER();
                    
                    /* Is the watchdog already timeout? */
                    if(RVM_Virt_Cur->Sched.Watchdog_Left==0U)   
                    {
                        RVM_COV_MARKER();
                        
                        /* Watchdog timeout - seek to reboot the VM */
                        RVM_DBG_S("Timd: Watchdog overflow in virtual machine ");
                        RVM_DBG_S(RVM_Virt_Cur->Map->Name);
                        RVM_DBG_S(". Recovering...\r\n");

                        /* Print registers & reboot */
                        _RVM_Virt_Recover((rvm_s8_t*)"Timd");
                        
                        RVM_DBG_S("Timd: Recovered.\r\n");
                    }
                    else
                    {
                        RVM_COV_MARKER();
                        
                        RVM_Virt_Cur->Sched.Watchdog_Left--;
                    }
                }
                else
                {
                    RVM_COV_MARKER();
                    /* No operation required */
                }
            }
            else
            {
                RVM_COV_MARKER();
                /* No operation required */
            }
        }
    
        /* Check whether a VM reschedule is necessary */
        if(RVM_Switch!=0U)
        {
            RVM_COV_MARKER();
            
            Virt=_RVM_Run_High();
            
            /* It doesn't matter here whether the current or the old virtual machine
             * is NULL. If one of them is NULL, it won't equal, and if both are NULL,
             * the _RVM_Virt_Switch won't be doing anything anyway. */
            if(Virt!=RVM_Virt_Cur)
            {
                RVM_COV_MARKER();
                
                _RVM_Virt_Switch(RVM_Virt_Cur,Virt);
                RVM_Virt_Cur=Virt;
            }
            else
            {
                RVM_COV_MARKER();
                /* No operation required */
            }
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
    }
}
#endif
/* End Function:RVM_Vmmd *****************************************************/

/* Function:RVM_Sftd **********************************************************
Description : The safety daemon to guard against system failures.
              This always runs as a separate thread, and will always be present
              regardless whether there are virtual machines.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void RVM_Sftd(void)
{
    rvm_tid_t Retval;
    rvm_ptr_t Thd_Flag;
    rvm_cid_t Cap_Thd;
    
    RVM_DBG_S("Sftd: Safety guard daemon initialization complete.\r\n");

    /* Main cycle - recover faults if possible */
    while(1)
    {
        /* Blocking single receive */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_Sftd_Sig_Cap,RVM_RCV_BS)>=0);
        
        /* Receive scheduler notifications if there are any */
        Retval=RVM_Thd_Sched_Rcv(RVM_Sftd_Thd_Cap);
        Thd_Flag=(rvm_ptr_t)Retval;
        
        if(((Thd_Flag&RVM_THD_EXCPEND_FLAG)==0U)||(Retval==RVM_ERR_PTH_NOTIF))
        {
            RVM_COV_MARKER();
            
            RVM_DBG_SHS("Sftd: Ignoring spurious notification 0x",Retval,".\r\n");
            continue;
        }
        else
        {
            RVM_COV_MARKER();
            /* No action required */
        }
        
        /* This must be an exception or we hang - we'd never have timeout notifications
         * in RVM because we always perform EXEC_SET and TIME_XFER back-to-back. */
        if(((Thd_Flag&RVM_THD_TIMEOUT_FLAG)!=0U)||(Retval<0))
        {
            RVM_COV_MARKER();
            
            /* Hang the machine because this error is unrecoverable */
            RVM_DBG_SHS("Sftd: Critical fault with return value 0x",Retval,". Panicking system...\r\n");
            RVM_ASSERT(0);
        }
        else
        {
            RVM_COV_MARKER();
            /* No action required */
        }
        
        /* Print the reason of the fault */
        Cap_Thd=(rvm_cid_t)(Thd_Flag&(~RVM_THD_EXCPEND_FLAG)&(~RVM_VIRT_TID_FLAG));
        RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
        RVM_Thd_Print_Exc(Cap_Thd);

        /* We're done - this is a monitor fault, have to reboot; monitor CID/TIDs are one-level */
        if(Cap_Thd<(rvm_cid_t)RVM_CID_2L)
        {
            RVM_COV_MARKER();
            
            RVM_DBG_S("Sftd: Critical fault on RVM ");
            
            /* Safety daemon crashed - how is this ever possible, we're handling our own faults */
            if(Cap_Thd==RVM_Sftd_Thd_Cap)
            {
                RVM_COV_MARKER();
            
                RVM_DBG_S("Sftd");
            }
#if(RVM_VIRT_NUM!=0U)
            /* Virtual machine daemon crashed */
            else if(Cap_Thd==RVM_Vmmd_Thd_Cap)
            {
                RVM_COV_MARKER();
                
                RVM_DBG_S("Vmmd");
            }
#endif
            /* Some user-supplied daemon crashed */
            else
            {
                RVM_COV_MARKER();
                
                RVM_DBG_SHS("unrecognized daemon TID 0x", Cap_Thd,"");
            }
            
            RVM_DBG_S(". Rebooting system...\r\n");
            
            /* Print registers & reboot */
            RVM_ASSERT(RVM_Thd_Print_Reg(Cap_Thd)==0);
            RVM_ASSERT(0);
        }
        /* We're done - process fault; processes are considered critical */
        else if((Thd_Flag&RVM_VIRT_TID_FLAG)==0U)
        {
            RVM_COV_MARKER();
            
            RVM_DBG_SHS("Sftd: Critical fault on process thread TID 0x",Cap_Thd,". Panicking system...\r\n");
            
            /* Print registers & reboot */
            RVM_ASSERT(RVM_Thd_Print_Reg(Cap_Thd)==0);
            RVM_ASSERT(0);
        }
        /* Can recover - virtual machine fault */
        else
        {
            RVM_COV_MARKER();
            
#if(RVM_VIRT_NUM!=0U)
            RVM_DBG_S("Sftd: Recoverable fault in virtual machine ");
            RVM_DBG_S(RVM_Virt_Cur->Map->Name);

            if(Cap_Thd==RVM_Virt_Cur->Map->Vct_Thd_Cap)
            {
                RVM_COV_MARKER();
            
                RVM_DBG_S(" vector handling thread");
            }
            else if(Cap_Thd==RVM_Virt_Cur->Map->Usr_Thd_Cap)
            {
                RVM_COV_MARKER();
            
                RVM_DBG_S(" user program thread");
            }
            else
            {
                RVM_COV_MARKER();
            
                RVM_DBG_S(" unknown thread");
            }

            RVM_DBG_S(". Recovering...\r\n");

            /* Print registers & reboot */
            _RVM_Virt_Recover((rvm_s8_t*)"Sftd");
            
            /* Trigger the context switch ASAP */
            RVM_ASSERT(RVM_Sig_Snd(RVM_BOOT_INIT_VCT)==0);
            
            RVM_DBG_S("Sftd: Recovered.\r\n");
#else
            RVM_DBG_S("Sftd: Fault on virtual machine but no virtual machine exists. Panicking system...\r\n");
            RVM_ASSERT(0);
#endif
        }
    }
}
/* End Function:RVM_Sftd *****************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

