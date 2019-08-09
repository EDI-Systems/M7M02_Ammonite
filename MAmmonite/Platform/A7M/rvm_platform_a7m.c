/******************************************************************************
Filename    : rvm_platform_a7m.c
Author      : pry
Date        : 26/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The Cortex-M system library platform specific header.
******************************************************************************/

/* Includes ******************************************************************/
#include "rme.h"

#define __HDR_DEFS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Init/rvm_syssvc.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Init/rvm_syssvc.h"
#undef __HDR_STRUCTS__

/* Private include */
#include "Platform/A7M/rvm_platform_a7m.h"

#define __HDR_PUBLIC_MEMBERS__
#include "Init/rvm_syssvc.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:RVM_Putchar *************************************************
Description : Output a character to console. This is for user-level debugging 
              only.
Input       : char Char - The character to print.
Output      : None.
Return      : rvm_ptr_t - Always 0.
******************************************************************************/
rvm_ptr_t RVM_Putchar(char Char)
{
    USART1->TDR=Char;
    
    while((USART1->ISR&0x40)==0)
    {
        
    }
    return 0;
}
/* End Function:RVM_Putchar **************************************************/

/* Begin Function:RVM_Stack_Init **********************************************
Description : Initialize a thread's stack for synchronous invocation or thread 
              creation.
Input       : rvm_ptr_t Stack - The start(lower) address of the stub.
              rvm_ptr_t Size  - The size of the stack.
Output      : None.
Return      : rvm_ptr_t - The actual stack address to use for system call.
******************************************************************************/
rvm_ptr_t RVM_Stack_Init(rvm_ptr_t Stack, rvm_ptr_t Size)
{
    struct RVM_A7M_Ret_Stack* Stack_Ptr;
    
    Stack_Ptr=(struct RVM_A7M_Ret_Stack*)(Stack+Size-RVM_STACK_SAFE_SIZE*sizeof(rvm_ptr_t)-sizeof(struct RVM_A7M_Ret_Stack));
    Stack_Ptr->R12=0;
    Stack_Ptr->LR=0;
    Stack_Ptr->PC=(rvm_ptr_t)_RVM_Jmp_Stub;
    /* Initialize the xPSR to avoid a transition to ARM state */
    Stack_Ptr->XPSR=0x01000200;
    
    return (rvm_ptr_t)Stack_Ptr;
}
/* End Function:RVM_Stack_Init ***********************************************/

/* Begin Function:__RVM_Pgtbl_Map *********************************************
Description : Actually map and construct the page table itself.
Input       : struct RVM_Hdr_Pgtbl* Pgtbl - The page table info stored in the header.
              rvm_ptr_t Pos - The position of the current page table in the page table info array.
              rvm_ptr_t Pgtbl_Cap - The capability to the current page table.
Output      : None.
Return      : rvm_ptr_t - The actual stack address to use for system call.
******************************************************************************/
void __RVM_Pgtbl_Map(struct RVM_Hdr_Pgtbl* Pgtbl, rvm_ptr_t Pos, rvm_cid_t Pgtbl_Cap)
{
    rvm_ptr_t Count;
    rvm_ptr_t Cur_Addr;
    rvm_ptr_t Parent_Slot;
    rvm_ptr_t Parent_Index;
    
    /* Map in all the pages, using the init process's page tables */
    if(RVM_PGTBL_NUM(Pgtbl[Pos].Order)==RME_PGTBL_NUM_8)
    {
        /* Check if there are any pages to map. If there are, check the flags
         * and map them in. */
        for(Count=0;Count<8;Count++)
        {
            if(Pgtbl[Pos].Flags[Count]!=0)
            {
                Cur_Addr=Pgtbl[Pos].Addr+RVM_POW2(RVM_PGTBL_SIZE(Pgtbl[Pos].Order))*Count;
                Parent_Slot=Cur_Addr/0x20000000;
                Parent_Index=(Cur_Addr%0x20000000)>>RVM_PGTBL_SIZE(Pgtbl[Pos].Order);
                RVM_ASSERT(RVM_Pgtbl_Add(RVM_CAPID(RVM_VIRT_TBL_PGTBL,Pgtbl_Cap), Count,
                           Pgtbl[Pos].Flags[Count], RVM_BOOT_PGTBL, Parent_Slot, Parent_Index)==0);
            }
        }
    }
    
    /* See if we are the parent ourself. If no, we do a construction */
    if(Pgtbl[Pos].Parent!=Pos)
    {
        RVM_ASSERT(Pgtbl[Pos].Parent<Pos);
        RVM_ASSERT(Pgtbl[Pos].Addr>=Pgtbl[Pgtbl[Pos].Parent].Addr);
        
        Parent_Slot=(Pgtbl[Pos].Addr-Pgtbl[Pgtbl[Pos].Parent].Addr)>>RVM_PGTBL_SIZE(Pgtbl[Pgtbl[Pos].Parent].Order);
        RVM_ASSERT(RVM_Pgtbl_Con(Pgtbl_Cap-Pos+Pgtbl[Pos].Parent, Parent_Slot, Pgtbl_Cap, RME_PGTBL_ALL_PERM)==0);
    }
    else
    {
        /* If they are equal, only the top-level can have this */
        RVM_ASSERT(Pos==0);
    }
}
/* End Function:__RVM_Pgtbl_Map **********************************************/

/* Begin Function:_RVM_Pgtbl_Setup ********************************************
Description : Setup all the page tables according to the header.
              Different microcontroller's architecture might be different, so this
              function is architecture-specific.
Input       : struct RVM_Hdr_Pgtbl* Pgtbl - The page table info stored in the header.
              rvm_cid_t Cap_Captbl - The capability to the capability table containing the page tables.
              rvm_ptr_t* Cap_Bump - The pgtbl capability table bump pointer, before allocations.
              rvm_ptr_t Cap_Kmem - The kernel memory capability.
              rvm_ptr_t* Kmem_Bump - The bump pointer for memory allocation.
              rvm_ptr_t Pos - The position of the page table in the page table info array.
Output      : rvm_ptr_t* Cap_Bump - The pgtbl capability table bump pointer, after allocations.
Return      : None.
******************************************************************************/
void _RVM_Pgtbl_Setup(struct RVM_Hdr_Pgtbl* Pgtbl, rvm_ptr_t Pos, 
                      rvm_cid_t Cap_Captbl, rvm_ptr_t* Cap_Bump,
                      rvm_cid_t Cap_Kmem, rvm_ptr_t* Kmem_Bump)
{
    /* Create the page table - 0 is always the top level */
    if(Pos==0)
    {
        RVM_ASSERT(RVM_Pgtbl_Crt(Cap_Captbl, Cap_Kmem, *Cap_Bump, 
                                 *Kmem_Bump, Pgtbl[Pos].Addr, 1, RVM_PGTBL_SIZE(Pgtbl[Pos].Order),
                                 RVM_PGTBL_NUM(Pgtbl[Pos].Order))==0);
        RVM_LOG_SISUS("Init:A7M:Created top-level page table number ",Pos," @ address ",*Kmem_Bump,".\r\n");
        *Kmem_Bump+=RVM_PGTBL_SIZE_TOP(Pgtbl[Pos].Order);
    }
    else
    {
        RVM_ASSERT(RVM_Pgtbl_Crt(Cap_Captbl, Cap_Kmem, *Cap_Bump, 
                                 *Kmem_Bump, Pgtbl[Pos].Addr, 0, RVM_PGTBL_SIZE(Pgtbl[Pos].Order),
                                 RVM_PGTBL_NUM(Pgtbl[Pos].Order))==0);
        RVM_LOG_SISUS("Init:A7M:Created normal page table number ",Pos," @ address ",*Kmem_Bump,".\r\n");
        *Kmem_Bump+=RVM_PGTBL_SIZE_NOM(Pgtbl[Pos].Order);
    }
    
    /* Do the mapping work */
    __RVM_Pgtbl_Map(Pgtbl,Pos,*Cap_Bump);
    (*Cap_Bump)++;
}
/* End Function:_RVM_Pgtbl_Setup *********************************************/

/* Begin Function:_RVM_Pgtbl_Check ********************************************
Description : Setup all the page tables according to the header.
              Different microcontroller's architecture might be different, so this
              function is architecture-specific.
Input       : const struct RVM_Hdr_Pgtbl* Pgtbl - The page table info stored in the header.
              rvm_ptr_t Pgtbl_Num - The number of page tables in the image.
              void* Addr - The start address to check.
              rvm_ptr_t Size - The size of the memory segment to check.
Output      : None.
Return      : rvm_ret_t - If the address is accessible, 0; else -1.
******************************************************************************/
rvm_ret_t _RVM_Pgtbl_Check(const struct RVM_Hdr_Pgtbl* Pgtbl, rvm_ptr_t Pgtbl_Num, void* Addr, rvm_ptr_t Size)
{
    /* Currently we perform no check because the system is under test */
//    rvm_cnt_t Count;
//    /* Is the address in SRAM or Flash? */
//    if(Addr<0x20000000)
//    {
//        /* Flash - just check if the address itself is readable */
//    }
//    else
//    {
//        /* RAM - need to check the address itself and 256 bytes before it */
//    }
    
    return 0;
}
/* End Function:_RVM_Pgtbl_Check *********************************************/

/* Begin Function:RVM_Idle ****************************************************
Description : Put the processor into idle state.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Idle(void)
{
    /* Do nothing. In the future we may call a kernel function to put us to sleep */
}
/* End Function:RVM_Idle *****************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
