/******************************************************************************
Filename    : rvm_guest.c
Author      : pry
Date        : 11/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The functionality wrapper for guests.
******************************************************************************/

/* Include *******************************************************************/
#include "rvm.h"

#define __HDR_PRIVATE__
#include "rvm_guest.h"
#undef __HDR_PRIVATE__
/* End Include ***************************************************************/

/* Function:RVM_Int_Print *****************************************************
Description : Print a signed integer on the debugging console. This integer is
              printed as decimal with sign.
Input       : rvm_cnt_t Int - The integer to print.
Output      : None.
Return      : rvm_cnt_t - The length of the string printed.
******************************************************************************/
#if(RVM_DBGLOG_ENABLE!=0U)
rvm_cnt_t RVM_Int_Print(rvm_cnt_t Int)
{
    rvm_cnt_t Num;
    rvm_cnt_t Abs;
    rvm_cnt_t Iter;
    rvm_cnt_t Count;
    rvm_cnt_t Div;
    
    /* Exit on zero */
    if(Int==0)
    {
        RVM_COV_MARKER();
        
        RVM_Putchar('0');
        return 1;
    }
    else
    {
        RVM_COV_MARKER();
        /* No action required */
    }

    /* Correct all negatives into positives */
    if(Int<0)
    {
        RVM_COV_MARKER();
        
        RVM_Putchar('-');
        Abs=-Int;
        Num=1;
    }
    else
    {
        RVM_COV_MARKER();
        
        Abs=Int;
        Num=0;
    }

    /* How many digits are there? */
    Count=0;
    Div=1;
    Iter=Abs;
    while(1)
    {
        Iter/=10;
        Count++;
        if(Iter!=0)
        {
            RVM_COV_MARKER();
            
            Div*=10;
        }
        else
        {
            RVM_COV_MARKER();
            
            break;
        }
    }
    Num+=Count;

    /* Print the integer */
    Iter=Abs;
    while(Count>0)
    {
        Count--;
        RVM_Putchar((rvm_s8_t)(Iter/Div)+'0');
        Iter=Iter%Div;
        Div/=10;
    }
    
    return Num;
}
#endif
/* End Function:RVM_Int_Print ************************************************/

/* Function:RVM_Hex_Print *****************************************************
Description : Print a unsigned integer on the debugging console. This integer is
              printed as hexadecimal.
Input       : rvm_ptr_t Uint - The unsigned integer to print.
Output      : None.
Return      : rvm_cnt_t - The length of the string printed.
******************************************************************************/
#if(RVM_DBGLOG_ENABLE!=0U)
rvm_cnt_t RVM_Hex_Print(rvm_ptr_t Uint)
{
    rvm_ptr_t Iter;
    rvm_ptr_t Count;
    rvm_ptr_t Num;

    /* Exit on zero */
    if(Uint==0U)
    {
        RVM_COV_MARKER();
        
        RVM_Putchar('0');
        return 1;
    }
    else
    {
        RVM_COV_MARKER();
        /* No action required */
    }

    /* Filter out all the leading zeroes */
    Count=0U;
    Iter=Uint;
    while((Iter>>(RVM_WORD_BIT-4U))==0U)
    {
        Iter<<=4U;
        Count++;
    }

    /* Count is the number of pts to print */
    Count=RVM_POW2(RVM_WORD_ORDER-2U)-Count;
    Num=Count;
    while(Count>0U)
    {
        Count--;
        Iter=(Uint>>(Count<<2U))&0x0FU;
        if(Iter<10U)
        {
            RVM_COV_MARKER();
            
            RVM_Putchar((rvm_s8_t)Iter+'0');
        }
        else
        {
            RVM_COV_MARKER();
            
            RVM_Putchar((rvm_s8_t)Iter+'A'-10);
        }
    }
    
    return (rvm_cnt_t)Num;
}
#endif
/* End Function:RVM_Hex_Print ************************************************/

/* Function:RVM_Str_Print *****************************************************
Description : Print a string on the debugging console.
Input       : const rvm_s8_t* String - The string to print.
Output      : None.
Return      : rvm_cnt_t - The length of the string printed, the '\0' is not included.
******************************************************************************/
#if(RVM_DBGLOG_ENABLE!=0U)
rvm_cnt_t RVM_Str_Print(const rvm_s8_t* String)
{
    rvm_ptr_t Count;
    
    for(Count=0U;Count<RVM_DBGLOG_MAX;Count++)
    {
        if(String[Count]==(rvm_s8_t)'\0')
        {
            RVM_COV_MARKER();
            
            break;
        }
        else
        {
            RVM_COV_MARKER();
            /* No action required */
        }
        
        RVM_Putchar(String[Count]);
    }
    
    return (rvm_cnt_t)Count;
}
#endif
/* End Function:RVM_Str_Print ************************************************/

/* Function:RVM_Log ***********************************************************
Description : Default logging function, will be used when the user does not 
              supply one. This will only be called when the kernel panics.
Input       : const char* File - The filename.
              long Line - The line number.
              const char* Date - The compilation date.
              const char* Time - The compilation time.
Output      : None.
Return      : None.
******************************************************************************/
#ifndef RVM_LOG
void RVM_Log(const char* File,
             long Line,
             const char* Date,
             const char* Time)
{
    RVM_DBG_S("\r\n***\r\nProcess panic - not syncing :\r\n"); \
    RVM_DBG_S(File); \
    RVM_DBG_S(" , Line "); \
    RVM_DBG_I(Line); \
    RVM_DBG_S("\r\n"); \
    RVM_DBG_S(Date); \
    RVM_DBG_S(" , "); \
    RVM_DBG_S(Time); \
    RVM_DBG_S("\r\n"); \
}
#endif
/* End Function:RVM_Log ******************************************************/

/* Function:RVM_Cov_Print *****************************************************
Description : The coverage data printer. Should always be disabled for all cases
              except where a kernel coverage test is needed. This should never
              be called any any user application; for coverage testing only.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef RVM_COV_LINE_NUM
void RVM_Cov_Print(void)
{
    rmp_ptr_t Count;
    rmp_ptr_t Next;
    
    Next=0U;
    for(Count=0U;Count<RVM_COV_LINE_NUM;Count++)
    {
        if(RVM_BITMAP_IS_SET(RVM_Cov,Count))
        {
            RVM_COV_MARKER();
            
            RVM_DBG_I(Count);
            RVM_DBG_S(",");
            /* We put 12 markers on a single line */
            Next++;
            if(Next>11U)
            {
                RVM_COV_MARKER();
                
                Next=0U;
                RVM_DBG_S("\r\n");
            }
            else
            {
                RVM_COV_MARKER();
                /* No action needed */
            }
        }
        else
        {
            RVM_COV_MARKER();
            /* No action needed */
        }
    }
}
#endif
/* End Function:RVM_Cov_Print ************************************************/

/* Function:RVM_Clear *********************************************************
Description : Memset a memory area to zero. This is not fast due to byte operations;
              this is not meant for large memory.
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
    {
        ((volatile rvm_u8_t*)Addr)[Count]=0U;
    }
}
/* End Function:RVM_Clear ****************************************************/

/* Function:RVM_List_Crt ******************************************************
Description : Create a doubly-linked list.
Input       : struct RVM_List* Head - The pointer to the list head.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_List_Crt(struct RVM_List* Head)
{
    Head->Prev=Head;
    Head->Next=Head;
}
/* End Function:RVM_List_Crt *************************************************/

/* Function:RVM_List_Del ******************************************************
Description : Delete a node from the doubly-linked list.
Input       : struct RVM_List* Prev - The prevoius node of the target node.
              struct RVM_List* Next - The next node of the target node.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_List_Del(struct RVM_List* Prev,
                  struct RVM_List* Next)
{
    Next->Prev=Prev;
    Prev->Next=Next;
}
/* End Function:RVM_List_Del *************************************************/

/* Function:RVM_List_Ins ******************************************************
Description : Insert a node to the doubly-linked list.
Input       : struct RVM_List* New - The new node to insert.
              struct RVM_List* Prev - The previous node.
              struct RVM_List* Next - The next node.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_List_Ins(struct RVM_List* New,
                  struct RVM_List* Prev,
                  struct RVM_List* Next)
{
    Next->Prev=New;
    New->Next=Next;
    New->Prev=Prev;
    Prev->Next=New;
}
/* End Function:RVM_List_Ins *************************************************/

/* Function:RVM_Cpt_Crt *******************************************************
Description : Create a capability table.
Input       : rvm_cid_t Cap_Cpt_Crt - The capability table that contains the 
                                      newly created cap to captbl.
                                      2-Level.
              rvm_cid_t Cap_Kom - The kernel memory capability.
                                  2-Level.
              rvm_cid_t Cap_Crt - The capability slot that you want this newly
                                  created capability table capability to be in.
                                  1-Level.
              rvm_ptr_t Raddr - The relative virtual address to store the 
                                capability table.
              rvm_ptr_t Entry_Num - The number of entries in that capability
                                    table.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_LIB_ENABLE==0U)
rvm_ret_t RVM_Cpt_Crt(rvm_cid_t Cap_Cpt_Crt,
                      rvm_cid_t Cap_Kom, 
                      rvm_cid_t Cap_Cpt,
                      rvm_ptr_t Raddr,
                      rvm_ptr_t Entry_Num)
{
    return RVM_SVC(RVM_SVC_CPT_CRT,
                   Cap_Cpt_Crt, 
                   RVM_PARAM_D1(Cap_Kom)|RVM_PARAM_D0(Cap_Cpt),
                   Raddr,
                   Entry_Num);
}
/* End Function:RVM_Cpt_Crt **************************************************/

/* Function:RVM_Cpt_Del *******************************************************
Description : Delete a layer of capability table. The table will be deleted
              regardless of whether there are still capabilities in it.
Input       : rvm_cid_t Cap_Cpt_Del - The capability table that contains the
                                      cap to captbl.
                                      2-Level.
              rvm_cid_t Cap_Del - The capability to the capability table to be
                                  deleted.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Cpt_Del(rvm_cid_t Cap_Cpt_Del,
                      rvm_cid_t Cap_Del)
{
    return RVM_SVC(RVM_SVC_CPT_DEL,
                   Cap_Cpt_Del,
                   Cap_Del,
                   0U,
                   0U);
}
/* End Function:RVM_Cpt_Del **************************************************/

/* Function:RVM_Cpt_Frz *******************************************************
Description : Freeze a capability in the capability table.
Input       : rvm_cid_t Cap_Cpt_Frz  - The capability table containing the cap
                                       to captbl for this operation.
                                       2-Level.
              rvm_cid_t Cap_Frz - The cap to the kernel object being freezed.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Cpt_Frz(rvm_cid_t Cap_Cpt_Frz,
                      rvm_cid_t Cap_Frz)
{
    return RVM_SVC(RVM_SVC_CPT_FRZ,
                   Cap_Cpt_Frz,
                   Cap_Frz,
                   0U,
                   0U);
}
/* End Function:RVM_Cpt_Frz **************************************************/

/* Function:RVM_Cpt_Add *******************************************************
Description : Delegate capability from one capability table to another. This
              function should only be used to delegate capabilities other than
              kernel memory, kernel function and page tables.
Input       : rvm_cid_t Cap_Cpt_Dst - The capability to the destination 
                                      capability table.
                                      2-Level.
              rvm_cid_t Cap_Dst - The capability slot you want to add to.
                                  1-Level.
              rvm_cid_t Cap_Cpt_Src - The capability to the source capability
                                      table.
                                      2-Level.
              rvm_cid_t Cap_Src - The capability in the source capability table
                                  to delegate.
                                  1-Level.
              rvm_ptr_t Flag - The flags for the capability.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Cpt_Add(rvm_cid_t Cap_Cpt_Dst,
                      rvm_cid_t Cap_Dst, 
                      rvm_cid_t Cap_Cpt_Src,
                      rvm_cid_t Cap_Src,
                      rvm_ptr_t Flag)
{
    return RVM_SVC(RVM_SVC_CPT_ADD,
                   0U,
                   RVM_PARAM_D1(Cap_Cpt_Dst)|RVM_PARAM_D0(Cap_Dst),
                   RVM_PARAM_D1(Cap_Cpt_Src)|RVM_PARAM_D0(Cap_Src),
                   Flag);
}
/* End Function:RVM_Cpt_Add **************************************************/

/* Function:RVM_Cpt_Pgt *******************************************************
Description : Delegate capability from one capability table to another. This
              function should only be used to delegate page tables.
Input       : rvm_cid_t Cap_Cpt_Dst - The capability to the destination
                                      capability table.
                                      2-Level.
              rvm_cid_t Cap_Dst - The capability slot you want to add to.
                                  1-Level.
              rvm_cid_t Cap_Cpt_Src - The capability to the source capability
                                      table.
                                      2-Level.
              rvm_cid_t Cap_Src - The capability in the source capability table
                                  to delegate.
                                  1-Level.
              rvm_ptr_t Begin - The begin position of the page table entry.
              rvm_ptr_t End - The end position of the page table entry, inclusive.
              rvm_ptr_t Flag - The flags for the page table capability.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Cpt_Pgt(rvm_cid_t Cap_Cpt_Dst,
                      rvm_cid_t Cap_Dst, 
                      rvm_cid_t Cap_Cpt_Src,
                      rvm_cid_t Cap_Src,
                      rvm_ptr_t Begin,
                      rvm_ptr_t End,
                      rvm_ptr_t Flag)
{
    return RVM_SVC(RVM_SVC_CPT_ADD,
                   0U,
                   RVM_PARAM_D1(Cap_Cpt_Dst)|RVM_PARAM_D0(Cap_Dst),
                   RVM_PARAM_D1(Cap_Cpt_Src)|RVM_PARAM_D0(Cap_Src),
                   RVM_PGT_FLAG(End,Begin,Flag));
}
/* End Function:RVM_Cpt_Pgt **************************************************/

/* Function:RVM_Cpt_Kfn *******************************************************
Description : Delegate capability from one capability table to another. This
              function should only be used to delegate kernel functions.
Input       : rvm_cid_t Cap_Cpt_Dst - The capability to the destination 
                                      capability table.
                                      2-Level.
              rvm_cid_t Cap_Dst - The capability slot you want to add to.
                                  1-Level.
              rvm_cid_t Cap_Cpt_Src - The capability to the source capability
                                      table.
                                      2-Level.
              rvm_cid_t Cap_Src - The capability in the source capability table
                                  to delegate.
                                  1-Level.
              rvm_ptr_t Begin - The begin of the kernel function ID, inclusive.
              rvm_ptr_t End - The end of the kernel function ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Cpt_Kfn(rvm_cid_t Cap_Cpt_Dst,
                      rvm_cid_t Cap_Dst, 
                      rvm_cid_t Cap_Cpt_Src,
                      rvm_cid_t Cap_Src,
                      rvm_ptr_t Begin,
                      rvm_ptr_t End)
{
    return RVM_SVC(RVM_SVC_CPT_ADD,
                   0U,
                   RVM_PARAM_D1(Cap_Cpt_Dst)|RVM_PARAM_D0(Cap_Dst),
                   RVM_PARAM_D1(Cap_Cpt_Src)|RVM_PARAM_D0(Cap_Src),
                   RVM_KFN_FLAG(End,Begin));
}
/* End Function:RVM_Cpt_Kfn **************************************************/

/* Function:RVM_Cpt_Kom *******************************************************
Description : Delegate capability from one capability table to another. This
              function should only be used to delegate kernel object memories.
Input       : rvm_cid_t Cap_Cpt_Dst - The capability to the destination 
                                      capability table.
                                      2-Level.
              rvm_cid_t Cap_Dst - The capability slot you want to add to.
                                  1-Level.
              rvm_cid_t Cap_Cpt_Src - The capability to the source capability
                                      table.
                                      2-Level.
              rvm_cid_t Cap_Src - The capability in the source capability table
                                  to delegate.
                                  1-Level.
              rvm_ptr_t Begin - The relative begin virtual address of the
                                kernel memory.
              rvm_ptr_t End - The relative end virtual address of the kernel
                              memory, exclusive.
              rvm_ptr_t Flag - The flags for the kernel memory capability.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Cpt_Kom(rvm_cid_t Cap_Cpt_Dst,
                      rvm_cid_t Cap_Dst, 
                      rvm_cid_t Cap_Cpt_Src,
                      rvm_cid_t Cap_Src,
                      rvm_ptr_t Begin,
                      rvm_ptr_t End,
                      rvm_ptr_t Flag)
{
    return RVM_SVC(RVM_KOM_SVC(End,RVM_SVC_CPT_ADD),
                   RVM_KOM_CID(Begin,Flag),
                   RVM_PARAM_D1(Cap_Cpt_Dst)|RVM_PARAM_D0(Cap_Dst),
                   RVM_PARAM_D1(Cap_Cpt_Src)|RVM_PARAM_D0(Cap_Src),
                   RVM_KOM_FLAG(End,Begin));
}
/* End Function:RVM_Cpt_Kom **************************************************/

/* Function:RVM_Cpt_Rem *******************************************************
Description : Remove one capability from the capability table. This function
              reverts the delegation.
Input       : rvm_cid_t Cap_Cpt_Rem - The capability to the capability table to
                                      remove from.
                                      2-Level.
              rvm_cid_t Cap_Rem - The capability slot you want to remove.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Cpt_Rem(rvm_cid_t Cap_Cpt_Rem,
                      rvm_cid_t Cap_Rem)
{
    return RVM_SVC(RVM_SVC_CPT_REM,
                   Cap_Cpt_Rem,
                   Cap_Rem,
                   0U,
                   0U);
}
/* End Function:_RVM_Cpt_Rem *************************************************/

/* Function:RVM_Pgt_Crt *******************************************************
Description : Create a page table.
Input       : rvm_cid_t Cap_Cpt - The capability table that contains the newly
                                  created cap to pgtbl.
                                  2-Level.
              rvm_cid_t Cap_Kom - The kernel memory capability.
                                  2-Level.
              rvm_cid_t Cap_Pgt - The capability slot that you want this newly
                                  created page table capability to be in.
                                  1-Level.
              rvm_ptr_t Raddr - The relative virtual address to store the page
                                table kernel object.
              rvm_ptr_t Base - The virtual address to start mapping for this
                               page table. This address must be aligned to the
                               total size of the table.
              rvm_ptr_t Is_Top - Whether this page table is the top-level. If
                                 it is, we will map all the kernel page 
                                 directories into this one.
              rvm_ptr_t Size_Order - The size order of the page table. The size
                                     refers to the size of each page in the 
                                     page directory.
              rvm_ptr_t Num_Order - The number order of entries in the table.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Pgt_Crt(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Kom,
                      rvm_cid_t Cap_Pgt, 
                      rvm_ptr_t Raddr,
                      rvm_ptr_t Base,
                      rvm_ptr_t Is_Top,
                      rvm_ptr_t Size_Order,
                      rvm_ptr_t Num_Order)
{
    
    return RVM_SVC(RVM_PGT_SVC(Num_Order, RVM_SVC_PGT_CRT),
                   Cap_Cpt,
                   RVM_PARAM_D1(Cap_Kom)|RVM_PARAM_Q1(Cap_Pgt)|RVM_PARAM_Q0(Size_Order),
                   Raddr, 
                   Base|Is_Top);
}
/* End Function:RVM_Pgt_Crt **************************************************/

/* Function:RVM_Pgt_Del *******************************************************
Description : Delete a page table.
Input       : rvm_cid_t Cap_Cpt - The capability to the captbl that may contain
                                  the cap to new captbl.
                                  2-Level.
              rvm_cid_t Cap_Pgt - The capability slot that you want this newly
                                  created page table capability to be in.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Pgt_Del(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Pgt)
{
    return RVM_SVC(RVM_SVC_PGT_DEL,
                   Cap_Cpt,
                   Cap_Pgt,
                   0U,
                   0U);
}
/* End Function:RVM_Pgt_Del **************************************************/

/* Function:RVM_Pgt_Add *******************************************************
Description : Delegate a page from one page table to another. This is the only
              way to add pages to new page tables after the system boots.
Input       : rvm_cid_t Cap_Pgt_Dst - The capability to the destination page
                                      directory.
                                      2-Level.
              rvm_ptr_t Pos_Dst - The position to delegate to in the
                                  destination page directory.
              rvm_ptr_t Flag_Dst - The page access permission for the
                                   destination page.
              rvm_cid_t Cap_Pgt_Src - The capability to the source page 
                                      directory.
                                      2-Level.
              rvm_ptr_t Pos_Dst - The position to delegate from in the source
                                  page directory.
              rvm_ptr_t Index - The index of the physical address frame to
                                delegate. For example, if the destination
                                directory's page size is 1/4 of that of the
                                source directory, index=0 will delegate the
                                first 1/4, index=1 will delegate the second
                                1/4, index=2 will delegate the third 1/4, and
                                index=3 will delegate the last 1/4.
                                All other index values are illegal.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Pgt_Add(rvm_cid_t Cap_Pgt_Dst,
                      rvm_ptr_t Pos_Dst,
                      rvm_ptr_t Flag_Dst,
                      rvm_cid_t Cap_Pgt_Src,
                      rvm_ptr_t Pos_Src,
                      rvm_ptr_t Index)
{
    return RVM_SVC(RVM_SVC_PGT_ADD,
                   RVM_PARAM_D0(Flag_Dst),
                   RVM_PARAM_D1(Cap_Pgt_Dst)|RVM_PARAM_D0(Pos_Dst),
                   RVM_PARAM_D1(Cap_Pgt_Src)|RVM_PARAM_D0(Pos_Src),
                   Index);
}
/* End Function:RVM_Pgt_Add **************************************************/

/* Function:RVM_Pgt_Rem *******************************************************
Description : Remove a page from the page table. We are doing unmapping of a
              page.
Input       : rvm_cid_t Cap_Pgt - The capability to the page table.
                                  2-Level.
              rvm_ptr_t Pos - The page table position to unmap from.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Pgt_Rem(rvm_cid_t Cap_Pgt,
                      rvm_ptr_t Pos)
{
    return RVM_SVC(RVM_SVC_PGT_REM,
                   0U,
                   Cap_Pgt,
                   Pos,
                   0U);
}
/* End Function:RVM_Pgt_Rem **************************************************/

/* Function:RVM_Pgt_Con *******************************************************
Description : Map a child page table into the parent page table, to construct
              an address space tree.
Input       : rvm_cid_t Cap_Pgt_Parent - The capability to the parent page 
                                         table.
                                         2-Level.
              rvm_ptr_t Pos - The parent page table position to map the child
                              page table to.
              rvm_cid_t Cap_Pgt_Child - The capability to the child page table.
                                        2-Level.
              rvm_ptr_t Flag_Child - Mapping flags.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Pgt_Con(rvm_cid_t Cap_Pgt_Parent,
                      rvm_ptr_t Pos,
                      rvm_cid_t Cap_Pgt_Child,
                      rvm_ptr_t Flag_Child)
{
    return RVM_SVC(RVM_SVC_PGT_CON,
                   0U,
                   RVM_PARAM_D1(Cap_Pgt_Parent)|RVM_PARAM_D0(Cap_Pgt_Child),
                   Pos,
                   Flag_Child);
}
/* End Function:RVM_Pgt_Con **************************************************/

/* Function:RVM_Pgt_Des *******************************************************
Description : Unmap a child page table from the parent page table, destructing
              the address space tree.
Input       : rvm_cid_t Cap_Pgt_Parent - The capability to the parent page
                                         table.
                                         2-Level.
              rvm_ptr_t Pos - The parent page table position to position unmap
                              the child page table from. The child page table
                              must be there for the destruction to succeed.
              rvm_cid_t Cap_Pgt_Child - The capability to the child page table.
                                        2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Pgt_Des(rvm_cid_t Cap_Pgt_Parent,
                      rvm_ptr_t Pos,
                      rvm_cid_t Cap_Pgt_Child)
{
    return RVM_SVC(RVM_SVC_PGT_DES,
                   0U,
                   Cap_Pgt_Parent,
                   Pos,
                   Cap_Pgt_Child);
}
/* End Function:RVM_Pgt_Des **************************************************/

/* Function:RVM_Prc_Crt *******************************************************
Description : Create a process. A process is in fact a protection domain
              associated with a set of capabilities.
Input       : rvm_cid_t Cap_Cpt_Crt - The capability to the capability table to
                                      put this process capability in.
                                      2-Level.
              rvm_cid_t Cap_Prc - The capability slot that you want this newly
                                  created process capability to be in.
                                  1-Level.
              rvm_cid_t Cap_Cpt - The capability to the capability table to use
                                  for this process.
                                  2-Level.
              rvm_cid_t Cap_Pgt - The capability to the page table to use for
                                  this process.
                                  2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Prc_Crt(rvm_cid_t Cap_Cpt_Crt,
                      rvm_cid_t Cap_Prc,
                      rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Pgt)
{
    return RVM_SVC(RVM_SVC_PRC_CRT,
                   Cap_Cpt_Crt,
                   Cap_Prc,
                   Cap_Cpt,
                   Cap_Pgt);
}
/* End Function:RVM_Prc_Crt **************************************************/

/* Function:RVM_Prc_Del *******************************************************
Description : Delete a process.
Input       : rvm_cid_t Cap_Cpt - The capability to the capability table.
                                  2-Level.
              rvm_cid_t Cap_Prc - The capability to the process.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Prc_Del(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Prc)
{
    return RVM_SVC(RVM_SVC_PRC_DEL,
                   Cap_Cpt,
                   Cap_Prc,
                   0U,
                   0U);
}
/* End Function:RVM_Prc_Del **************************************************/

/* Function:RVM_Prc_Cpt *******************************************************
Description : Change a process's capability table.
Input       : rvm_cid_t Cap_Prc - The capability to the process that have been
                                  created already.
                                  2-Level.
              rvm_cid_t Cap_Cpt - The capability to the capability table to use
                                  for this process.
                                  2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Prc_Cpt(rvm_cid_t Cap_Prc,
                      rvm_cid_t Cap_Cpt)
{
    return RVM_SVC(RVM_SVC_PRC_CPT,
                   0U,
                   Cap_Prc,
                   Cap_Cpt,
                   0U);
}
/* End Function:RVM_Prc_Cpt **************************************************/

/* Function:RVM_Prc_Pgt *******************************************************
Description : Change a process's page table.
Input       : rvm_cid_t Cap_Prc - The capability to the process that have been
                                  created already.
                                  2-Level.
              rvm_cid_t Cap_Pgt - The capability to the page table to use for
                                  this process.
                                  2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Prc_Pgt(rvm_cid_t Cap_Prc,
                      rvm_cid_t Cap_Pgt)
{
    return RVM_SVC(RVM_SVC_PRC_PGT,
                   0U,
                   Cap_Prc,
                   Cap_Pgt,
                   0U);
}
/* End Function:RVM_Prc_Pgt **************************************************/

/* Function:RVM_Thd_Crt *******************************************************
Description : Create a thread. A thread is the minimal kernel-level execution
              unit.
Input       : rvm_cid_t Cap_Cpt - The capability to the capability table.
                                  2-Level.
              rvm_cid_t Cap_Kom - The kernel memory capability.
                                  2-Level.
              rvm_cid_t Cap_Thd - The capability slot that you want this newly
                                  created thread capability to be in.
                                  1-Level.
              rvm_cid_t Cap_Prc - The capability to the process that it is in.
                                  2-Level.
              rvm_ptr_t Prio_Max - The maximum priority allowed for this
                                   thread. Once set, this cannot be changed.
              rvm_ptr_t Raddr - The relative virtual address to store the
                                thread kernel object.
              rvm_ptr_t Attr - The context attributes.
Output      : None.
Return      : rvm_ret_t - If successful, the Thread ID; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Crt(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Kom,
                      rvm_cid_t Cap_Thd,
                      rvm_cid_t Cap_Prc,
                      rvm_ptr_t Prio_Max,
                      rvm_ptr_t Raddr,
                      rvm_ptr_t Attr)
{
    return RVM_SVC(RVM_THD_SVC(Attr,0U,RVM_SVC_THD_CRT),
                   Cap_Cpt, 
                   RVM_PARAM_D1(Cap_Kom)|RVM_PARAM_D0(Cap_Thd),
                   RVM_PARAM_D1(Cap_Prc)|RVM_PARAM_D0(Prio_Max),
                   Raddr);
}
/* End Function:RVM_Thd_Crt **************************************************/

/* Function:RVM_Hyp_Crt *******************************************************
Description : Create a hypervisor-managed thread. A thread is the minimal
              kernel-level execution unit.
Input       : rvm_cid_t Cap_Cpt - The capability to the capability table.
                                  2-Level.
              rvm_cid_t Cap_Kom - The kernel memory capability.
                                  2-Level.
              rvm_cid_t Cap_Thd - The capability slot that you want this newly
                                  created thread capability to be in.
                                  1-Level.
              rvm_cid_t Cap_Prc - The capability to the process that it is in.
                                  2-Level.
              rvm_ptr_t Prio_Max - The maximum priority allowed for this
                                   thread. Once set, this cannot be changed.
              rvm_ptr_t Raddr - The relative virtual address to store the
                                thread kernel object.
              rvm_ptr_t Attr - The context attributes.
Output      : None.
Return      : rvm_ret_t - If successful, the Thread ID; or an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Crt(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Kom,
                      rvm_cid_t Cap_Thd,
                      rvm_cid_t Cap_Prc,
                      rvm_ptr_t Prio_Max,
                      rvm_ptr_t Raddr,
                      rvm_ptr_t Attr)
{
    return RVM_SVC(RVM_THD_SVC(Attr,1U,RVM_SVC_THD_CRT),
                   Cap_Cpt, 
                   RVM_PARAM_D1(Cap_Kom)|RVM_PARAM_D0(Cap_Thd),
                   RVM_PARAM_D1(Cap_Prc)|RVM_PARAM_D0(Prio_Max),
                   Raddr);
}
/* End Function:RVM_Hyp_Crt **************************************************/

/* Function:RVM_Thd_Del *******************************************************
Description : Delete a thread.
Input       : rvm_cid_t Cap_Cpt - The capability to the capability table.
                                  2-Level.
              rvm_cid_t Cap_Thd - The capability to the thread in the captbl.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Del(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Thd)
{
    return RVM_SVC(RVM_SVC_THD_DEL,
                   Cap_Cpt,
                   Cap_Thd,
                   0U,
                   0U);
}
/* End Function:RVM_Thd_Del **************************************************/

/* Function:RVM_Thd_Exec_Set **************************************************
Description : Set a thread's entry point and stack. The registers will be
              initialized with these contents.
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
                                  2-Level.
              rvm_ptr_t Entry - The entry address of the thread.
              rvm_ptr_t Stack - The stack address to use for execution.
              rvm_ptr_t Param - The parameter to pass to the thread.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Exec_Set(rvm_cid_t Cap_Thd,
                           rvm_ptr_t Entry,
                           rvm_ptr_t Stack,
                           rvm_ptr_t Param)
{
    return RVM_SVC(RVM_SVC_THD_EXEC_SET,
                   Cap_Thd,
                   Entry, 
                   Stack,
                   Param);
}
/* End Function:RVM_Thd_Exec_Set *********************************************/

/* Function:RVM_Thd_Sched_Bind ************************************************
Description : Set a thread's priority level, and its scheduler thread. When
              there are any state changes on this thread, a notification will
              be sent to its scheduler thread. If the state of the thread
              changes for multiple times, then only the most recent state will
              be reflected in the scheduler's receive queue.
              The scheduler and the threads that it schedule must be on the
              same core. When a thread wants to go from one core to another,
              its notification to the scheduler must all be processed, and it
              must have no scheduler notifications in itself. 
              This must be called on the same core with the Cap_Thd_Sched, and
              the Cap_Thd itself must be free.
              It is impossible to set a thread's priority beyond its maximum
              priority. 
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
                                  2-Level.
              rvm_cid_t Cap_Thd_Sched - The scheduler thread.
                                        2-Level.
              rvm_cid_t Cap_Sig - The signal endpoint for scheduler
                                  notifications. This signal endpoint will be
                                  sent to whenever this thread has a fault, or
                                  timeouts. This is purely optional; if it is
                                  not needed, pass in RME_CID_NULL.
              rvm_tid_t TID - The thread ID. This is user-supplied, and the
                              kernel will not check whether there are two
                              threads that have the same TID.
              rvm_ptr_t Prio - The priority level, higher is more critical.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Bind(rvm_cid_t Cap_Thd,
                             rvm_cid_t Cap_Thd_Sched,
                             rvm_cid_t Cap_Sig,
                             rvm_tid_t TID,
                             rvm_ptr_t Prio)
{
    return RVM_SVC(RVM_SVC_THD_SCHED_BIND,
                   Cap_Thd,
                   RVM_PARAM_D1(Cap_Thd_Sched)|RVM_PARAM_D0(Cap_Sig),
                   RVM_PARAM_D1(TID)|RVM_PARAM_D0(Prio), 
                   RVM_NULL);
}
/* End Function:RVM_Thd_Sched_Bind *******************************************/

/* Function:RVM_Hyp_Sched_Bind ************************************************
Description : Set a thread's priority level, and its scheduler thread. When
              there are any state changes on this thread, a notification will
              be sent to its scheduler thread. If the state of the thread
              changes for multiple times, then only the most recent state will
              be reflected in the scheduler's receive queue.
              The scheduler and the threads that it schedule must be on the
              same core. When a thread wants to go from one core to another,
              its notification to the scheduler must all be processed, and it
              must have no scheduler notifications in itself. 
              This must be called on the same core with the Cap_Thd_Sched, and
              the Cap_Thd itself must be free.
              It is impossible to set a thread's priority beyond its maximum
              priority. 
              This is for hypervisor-managed threads, and hence need to provide
              a Haddr mapping address.
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
                                  2-Level.
              rvm_cid_t Cap_Thd_Sched - The scheduler thread.
                                        2-Level.
              rvm_cid_t Cap_Sig - The signal endpoint for scheduler
                                  notifications. This signal endpoint will be
                                  sent to whenever this thread has a fault, or
                                  timeouts. This is purely optional; if it is
                                  not needed, pass in RME_CID_NULL.
              rvm_tid_t TID - The thread ID. This is user-supplied, and the
                              kernel will not check whether there are two
                              threads that have the same TID.
              rvm_ptr_t Prio - The priority level, higher is more critical.
              rvm_ptr_t Haddr - The kernel-accessible virtual memory address
                                for this thread's register sets, only used by
                                hypervisor-managed threads. For other threads,
                                please pass in NULL instead.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Sched_Bind(rvm_cid_t Cap_Thd,
                             rvm_cid_t Cap_Thd_Sched,
                             rvm_cid_t Cap_Sig,
                             rvm_tid_t TID,
                             rvm_ptr_t Prio,
                             rvm_ptr_t Haddr)
{
    return RVM_SVC(RVM_SVC_THD_SCHED_BIND,
                   Cap_Thd,
                   RVM_PARAM_D1(Cap_Thd_Sched)|RVM_PARAM_D0(Cap_Sig),
                   RVM_PARAM_D1(TID)|RVM_PARAM_D0(Prio), 
                   Haddr);
}
/* End Function:RVM_Hyp_Sched_Bind *******************************************/

/* Function:RVM_Thd_Sched_Prio ************************************************
Description : Change a thread's priority level. This can only be called from
              the core that has the thread binded. Single thread version.
              This system call can cause a potential context switch.
              It is impossible to set a thread's priority beyond its maximum 
              priority. 
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
                                  2-Level.
              rvm_ptr_t Prio - The priority level, higher is more critical.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Prio(rvm_cid_t Cap_Thd,
                             rvm_ptr_t Prio)
{
    return RVM_SVC(RVM_SVC_THD_SCHED_PRIO,
                   1U,
                   RVM_PARAM_D1(Prio)|RVM_PARAM_D0(Cap_Thd),
                   0U, 
                   0U);
}
/* End Function:RVM_Thd_Sched_Prio *******************************************/

/* Function:RVM_Thd_Sched_Prio2 ***********************************************
Description : Change a thread's priority level. This can only be called from
              the core that has the thread binded. Double thread version.
              This system call can cause a potential context switch.
              It is impossible to set a thread's priority beyond its maximum 
              priority. 
Input       : rvm_cid_t Cap_Thd0 - The capability to the first thread.
                                   2-Level.
              rvm_ptr_t Prio0 - The priority level, higher is more critical.
              rvm_cid_t Cap_Thd1 - The capability to the second thread.
                                   2-Level.
              rvm_ptr_t Prio1 - The priority level, higher is more critical.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Prio2(rvm_cid_t Cap_Thd0,
                              rvm_ptr_t Prio0,
                              rvm_cid_t Cap_Thd1,
                              rvm_ptr_t Prio1)
{
    return RVM_SVC(RVM_SVC_THD_SCHED_PRIO,
                   2U,
                   RVM_PARAM_D1(Prio0)|RVM_PARAM_D0(Cap_Thd0),
                   RVM_PARAM_D1(Prio1)|RVM_PARAM_D0(Cap_Thd1), 
                   0U);
}
/* End Function:RVM_Thd_Sched_Prio2 ******************************************/

/* Function:RVM_Thd_Sched_Prio3 ***********************************************
Description : Change a thread's priority level. This can only be called from
              the core that has the thread binded. Triple thread version.
              This system call can cause a potential context switch.
              It is impossible to set a thread's priority beyond its maximum 
              priority. 
Input       : rvm_cid_t Cap_Thd0 - The capability to the first thread.
                                   2-Level.
              rvm_ptr_t Prio0 - The priority level, higher is more critical.
              rvm_cid_t Cap_Thd1 - The capability to the second thread.
                                   2-Level.
              rvm_ptr_t Prio1 - The priority level, higher is more critical.
              rvm_cid_t Cap_Thd2 - The capability to the third thread.
                                   2-Level.
              rvm_ptr_t Prio2 - The priority level, higher is more critical.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Prio3(rvm_cid_t Cap_Thd0,
                              rvm_ptr_t Prio0,
                              rvm_cid_t Cap_Thd1,
                              rvm_ptr_t Prio1,
                              rvm_cid_t Cap_Thd2,
                              rvm_ptr_t Prio2)
{
    return RVM_SVC(RVM_SVC_THD_SCHED_PRIO,
                   3U,
                   RVM_PARAM_D1(Prio0)|RVM_PARAM_D0(Cap_Thd0),
                   RVM_PARAM_D1(Prio1)|RVM_PARAM_D0(Cap_Thd1), 
                   RVM_PARAM_D1(Prio2)|RVM_PARAM_D0(Cap_Thd2));
}
/* End Function:RVM_Thd_Sched_Prio3 ******************************************/

/* Function:RVM_Thd_Sched_Free ************************************************
Description : Free a thread from its current binding. This function can only be
              executed from the same core on with the thread.
              This system call can cause a potential context switch.
Input       : rvm_cid_t Cap_Thd - The capability to the thread.
                                  2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Free(rvm_cid_t Cap_Thd)
{
    return RVM_SVC(RVM_SVC_THD_SCHED_FREE,
                   0U,
                   Cap_Thd,
                   0U, 
                   0U);
}
/* End Function:RVM_Thd_Sched_Free *******************************************/

/* Function:_RME_Thd_Sched_Rcv ************************************************
Description : Try to receive a notification from the scheduler queue. This
              can only be called from the same core the thread is on, and is
              always non-blocking.
Input       : rme_cid_t Cap_Thd - The capability to the scheduler thread. We
                                  are going to get timeout or exception
                                  notifications for the threads that it is
                                  responsible for scheduling. This capability
                                  must point to a thread on the same core.
                                  2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, the thread ID; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Rcv(rvm_cid_t Cap_Thd)
{
    return RVM_SVC(RVM_SVC_THD_SCHED_RCV,
                   0U,
                   Cap_Thd,
                   0U, 
                   0U);
}
/* End Function:_RME_Thd_Sched_Rcv *******************************************/

/* Function:RVM_Thd_Time_Xfer *************************************************
Description : Transfer time from one thread to another. This can only be called
              from the core that the thread is on, and the the two threads
              involved in the time transfer must be on the same core.
              If the time transfered is more than or equal to what the source
              have, the source will be out of time or blocked. If the source is
              both out of time and blocked, we do not send the notification;
              Instead, we send the notification when the receive endpoint
              actually receive something.
              It is possible to transfer time to threads have a lower priority,
              and it is also possible to transfer time to threads that have a
              higher priority. In the latter case, and if the source is
              currently running, a preemption will occur.
Input       : rvm_cid_t Cap_Thd_Dst - The destination thread.
                                      2-Level.
              rvm_cid_t Cap_Thd_Src - The source thread.
                                      2-Level.
              rvm_ptr_t Time - The time to transfer, in slices. A slice is the
                               minimal amount of time transfered in the system
                               usually on the order of 100us or 1ms.
                               Use RVM_THD_INIT_TIME for revoking transfer.
                               Use RVM_THD_INF_TIME for infinite trasnfer.
Output      : None.
Return      : rvm_ret_t - If successful, the destination time amount; or an
                          error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Time_Xfer(rvm_cid_t Cap_Thd_Dst,
                            rvm_cid_t Cap_Thd_Src,
                            rvm_ptr_t Time)
{
    return RVM_SVC(RVM_SVC_THD_TIME_XFER,
                   0U,
                   Cap_Thd_Dst,
                   Cap_Thd_Src, 
                   Time);
}
/* End Function:RVM_Thd_Time_Xfer ********************************************/

/* Function:RVM_Thd_Swt *******************************************************
Description : Switch to another thread. The thread to switch to must have the
              same preemptive priority as this thread, and have time, and not
              blocked.
Input       : rvm_cid_t Cap_Thd - The capability to the thread. If this is -1,
                                  the kernel will pickup whatever thread that
                                  has the highest priority and time to run. 
                                  2-Level. 
              rvm_ptr_t Is_Yield - This is a flag to indicate whether this
                                   is a full yield. If it is, the kernel will
                                   discard all the time alloted on this
                                   thread.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Swt(rvm_cid_t Cap_Thd,
                      rvm_ptr_t Is_Yield)
{
    return RVM_SVC(RVM_SVC_THD_SWT,
                   0U,
                   Cap_Thd,
                   Is_Yield, 
                   0U);
}
/* End Function:RVM_Thd_Swt **************************************************/

/* Function:RVM_Sig_Crt *******************************************************
Description : Create a signal endpoint.
Input       : rvm_cid_t Cap_Cpt - The capability to the capability table to use
                                  for this signal.
                                  2-Level.
              rvm_cid_t Cap_Sig - The capability slot that you want this newly
                                  created signal capability to be in.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Sig_Crt(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Sig)
{
    return RVM_SVC(RVM_SVC_SIG_CRT,
                   Cap_Cpt,
                   Cap_Sig,
                   0U,
                   0U);
}
/* End Function:RVM_Sig_Crt **************************************************/

/* Function:RVM_Sig_Del *******************************************************
Description : Delete a signal endpoint.
Input       : rvm_cid_t Cap_Cpt - The capability to the capability table to
                                  delete from.
                                  2-Level.
              rvm_cid_t Cap_Sig - The capability to the signal.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Sig_Del(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Sig)
{
    return RVM_SVC(RVM_SVC_SIG_DEL,
                   Cap_Cpt,
                   Cap_Sig,
                   0U,
                   0U);
}
#endif
/* End Function:RVM_Sig_Del **************************************************/

/* Function:RVM_Sig_Snd *******************************************************
Description : Try to send to a signal endpoint. This system call can cause
              a potential context switch.
Input       : rvm_cid_t Cap_Sig - The capability to the signal.
                                  2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0, or an error code.
******************************************************************************/
rvm_ret_t RVM_Sig_Snd(rvm_cid_t Cap_Sig)
{
    return RVM_SVC(RVM_SVC_SIG_SND,
                   0U,
                   Cap_Sig,
                   0U, 
                   0U);
}
/* End Function:RVM_Sig_Snd **************************************************/

/* Function:RVM_Sig_Rcv *******************************************************
Description : Try to receive from a signal endpoint. The rules for signal
              endpoint receive is:
              1.If a receive endpoint have many send endpoints, everyone can
                send to it, and sending to it will increase the count by 1.
              2.If some thread blocks on a receive endpoint, the wakeup is only
                possible from the same core that thread is on.
              3.It is not recommended to let 2 cores operate on the rcv 
                endpoint simutaneously.
              This system call can potentially trigger a context switch.
Input       : rvm_cid_t Cap_Sig - The capability to the signal.
                                  2-Level.
              rvm_ptr_t Option - The receive option.
Output      : None.
Return      : rvm_ret_t - If successful, a non-negative number containing the
                          number of signals received; or an error code.
******************************************************************************/
rvm_ret_t RVM_Sig_Rcv(rvm_cid_t Cap_Sig,
                      rvm_ptr_t Option)
{
    return RVM_SVC(RVM_SVC_SIG_RCV,
                   0U,
                   Cap_Sig,
                   Option, 
                   0U);
}
/* End Function:RVM_Sig_Rcv **************************************************/

/* Function:RVM_Inv_Crt *******************************************************
Description : Create a invocation stub.
Input       : rvm_cid_t Cap_Cpt - The capability to the capability table to use
                                  for this process.
                                  2-Level.
              rvm_cid_t Cap_Inv - The capability slot that you want this newly
                                  created invocation capability to be in.
                                  1-Level.
              rvm_cid_t Cap_Prc - The capability to the process that it is in.
                                  2-Level.
              rvm_ptr_t Raddr - The relative virtual address to store the
                                invocation port kernel object.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_LIB_ENABLE==0U)
rvm_ret_t RVM_Inv_Crt(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Kom, 
                      rvm_cid_t Cap_Inv,
                      rvm_cid_t Cap_Prc,
                      rvm_ptr_t Raddr)
{
    return RVM_SVC(RVM_SVC_INV_CRT,
                   Cap_Cpt,
                   RVM_PARAM_D1(Cap_Kom)|RVM_PARAM_D0(Cap_Inv),
                   Cap_Prc, 
                   Raddr);
}
/* End Function:RVM_Inv_Crt **************************************************/

/* Function:RVM_Inv_Del *******************************************************
Description : Delete an invocation stub.
Input       : rvm_cid_t Cap_Cpt - The capability to the capability table to
                                  delete from.
                                  2-Level.
              rvm_cid_t Cap_Inv - The capability to the invocation stub.
                                  1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Inv_Del(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Inv)
{
    return RVM_SVC(RVM_SVC_INV_DEL,
                   Cap_Cpt,
                   Cap_Inv,
                   0U, 
                   0U);
}
/* End Function:RVM_Inv_Del **************************************************/

/* Function:RVM_Inv_Set *******************************************************
Description : Set an invocation stub's entry point and stack. The registers
              will be initialized with these contents.
Input       : rvm_cid_t Cap_Inv - The capability to the invocation stub.
                                  2-Level.
              rvm_ptr_t Entry - The entry of the thread.
              rvm_ptr_t Stack - The stack address to use for execution.
              rvm_ptr_t Is_Exc_Ret - If there is an exception in this
                                     invocation, return immediately, or wait
                                     for fault handling?
                                     If 1, we return directly on fault.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Inv_Set(rvm_cid_t Cap_Inv,
                      rvm_ptr_t Entry,
                      rvm_ptr_t Stack,
                      rvm_ptr_t Is_Exc_Ret)
{
    return RVM_SVC(RVM_SVC_INV_SET,
                   0U,
                   RVM_PARAM_D1(Is_Exc_Ret)|RVM_PARAM_D0(Cap_Inv),
                   Entry, 
                   Stack);
}
#endif
/* End Function:RVM_Inv_Set **************************************************/

/* Function:RVM_Kfn_Act *******************************************************
Description : Activate a kernel function.
Input       : rvm_cid_t Cap_Kfn - The capability to the kernel capability.
                                  2-Level.
              rvm_ptr_t Func_ID - The function ID to invoke.
              rvm_ptr_t Sub_ID - The subfunction ID to invoke.
              rvm_ptr_t Param1 - The first parameter.
              rvm_ptr_t Param2 - The second parameter.
Output      : None.
Return      : rvm_ret_t - If the call is successful, it will return whatever
                          the 
                          function returned (It is expected that they shall
                          never return an negative value); or an error code.
                          If the kernel function ever causes a context switch,
                          it is responsible for setting the return value. On 
                          failure, no context switch is allowed.
******************************************************************************/
rvm_ret_t RVM_Kfn_Act(rvm_cid_t Cap_Kfn,
                      rvm_ptr_t Func_ID,
                      rvm_ptr_t Sub_ID,
                      rvm_ptr_t Param1,
                      rvm_ptr_t Param2)
{
    return RVM_SVC(RVM_SVC_KFN,
                   Cap_Kfn,
                   RVM_PARAM_D1(Sub_ID)|RVM_PARAM_D0(Func_ID),
                   Param1,
                   Param2);
}
/* End Function:RVM_Kfn_Act **************************************************/

/* Function:RVM_Prc_Evt_Snd ***************************************************
Description : Send to an event source from a process.
Input       : rvm_ptr_t Evt_Num - The number to send to.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_LIB_ENABLE==0U)
rvm_ret_t RVM_Prc_Evt_Snd(rvm_ptr_t Evt_Num)
{
    return RVM_Kfn_Act(0U,RVM_KFN_EVT_LOCAL_TRIG,0U,Evt_Num,0U);
}
#endif
/* End Function:RVM_Prc_Evt_Snd **********************************************/

/* Function:RVM_Virt_Init *****************************************************
Description : Initialize virtual machine related RVM library.
Input       : None
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_LIB_ENABLE!=0U)
void RVM_Virt_Init(void)
{
    /* Interrupt masked on boot, no vector pending */
    RVM_Int_Mask=1U;
    RVM_Vct_Pend=0U;
    
    /* Clean up all shared global variables - the RVM would clean it for us on
     * first boot and reboots, but we're doing it here againt just in case. */
    RVM_Clear((void*)RVM_VIRT_STATE_BASE,RVM_VIRT_STATE_SIZE);
    RVM_Clear(&RVM_Handler,sizeof(struct RVM_Handler_Struct));
}
/* End Function:RVM_Virt_Init ************************************************/

/* Function:RVM_Virt_Vct_Reg **************************************************
Description : Initialize an interrupt handler.
Input       : rvm_ptr_t Vct_Num - The vector number to register for.
              void (*Vct)(void) - The actual vector handler, should take no
                                  arguments and return nothing.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Virt_Vct_Reg(rvm_ptr_t Vct_Num,
                           void (*Vct)(void))
{
    if(Vct_Num>=RVM_VIRT_VCT_NUM)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_RANGE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    RVM_Handler.Vct[Vct_Num]=Vct;
    return 0;
}
/* End Function:RVM_Virt_Vct_Reg *********************************************/

/* Function:RVM_Virt_Vct_Trig *************************************************
Description : Trigger a certain designated virtual vector with the VM.
Input       : rvm_ptr_t Vct_Num - The vector number to trigger.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Virt_Vct_Trig(rvm_ptr_t Vct_Num)
{
    if(Vct_Num>=RVM_VIRT_VCT_NUM)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_RANGE;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
        
    /* Set corresponding interrupt flag */
    RVM_VCTF->Vct[Vct_Num]=1U;
    
    /* Trigger if the interrupt is not masked and we are not in interrupt */
    if(RVM_STATE->Vct_Act==0U)
    {
        RVM_COV_MARKER();
        
        if(RVM_Int_Mask==0U)
        {
            RVM_COV_MARKER();
            
            RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VCT)==0);
        }
        else
        {
            RVM_COV_MARKER();
            
            RVM_Vct_Pend=1U;
        }
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    return 0;
}
/* End Function:RVM_Virt_Vct_Trig ********************************************/

/* Function:RVM_Virt_Tim_Reg **************************************************
Description : Register special timer interrupt handler.
Input       : void (*Tim)(void) - The timer interrupt vector handler.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Virt_Tim_Reg(void (*Tim)(void))
{
    RVM_Handler.Tim=Tim;
}
/* End Function:RVM_Virt_Tim_Reg *********************************************/

/* Function:RVM_Virt_Ctx_Reg **************************************************
Description : Register special context switch interrupt handler.
Input       : void (*Ctx)(void) - The context switch interrupt vector handler.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Virt_Ctx_Reg(void (*Ctx)(void))
{
    RVM_Handler.Ctx=Ctx;
}
/* End Function:RVM_Virt_Ctx_Reg *********************************************/

/* Function:RVM_Virt_Int_Mask *************************************************
Description : Mask interrupts; this does not stop the interrupt delivery, but
              will postpone the processing until we unmask them. Masking is much
              lighter and faster than the hypercall disabling interrupts, which
              prevents virtual interrupt delivery altogether.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Virt_Int_Mask(void)
{
    RVM_Int_Mask=1U;
}
/* End Function:RVM_Virt_Int_Mask ********************************************/

/* Function:RVM_Virt_Int_Unmask ***********************************************
Description : Unmask pending interrupts so we can process them.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Virt_Int_Unmask(void)
{
    RVM_Int_Mask=0U;
    
    /* Trigger interrupt processing if there are pending ones */
    if(RVM_Vct_Pend!=0U)
    {
        RVM_COV_MARKER();
        
        RVM_Vct_Pend=0U;
        RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VCT)==0);
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
}
/* End Function:RVM_Virt_Int_Unmask ******************************************/

/* Function:RVM_Virt_Yield ****************************************************
Description : Yield the current thread by triggering the context switch endpoint.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Virt_Yield(void)
{
    /* Set the context switch flag */
    RVM_VCTF->Ctx=1U;
    
    /* Trigger if the interrupt is not masked and we are not in interrupt */
    if(RVM_STATE->Vct_Act==0U)
    {
        RVM_COV_MARKER();
        
        if(RVM_Int_Mask==0U)
        {
            RVM_COV_MARKER();
            
            RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VCT)==0);
        }
        else
        {
            RVM_COV_MARKER();
            
            RVM_Vct_Pend=1U;
        }
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
}
/* End Function:RVM_Virt_Yield ***********************************************/

/* Function:RVM_Hyp ***********************************************************
Description : Do a hypercall to the virtual machine.
Input       : rvm_ptr_t Number - The hypercall number.
              rvm_ptr_t Param1 - The first parameter.
              rvm_ptr_t Param2 - The second parameter.
              rvm_ptr_t Param3 - The third parameter.
Output      : None.
Return      : rvm_ret_t - The return value position.
******************************************************************************/
rvm_ret_t RVM_Hyp(rvm_ptr_t Number,
                  rvm_ptr_t Param1,
                  rvm_ptr_t Param2,
                  rvm_ptr_t Param3,
                  rvm_ptr_t Param4)
{
    /* Might be changed by the hypervisor */
    volatile struct RVM_Param* Arg;

    if(RVM_STATE->Vct_Act!=0U)
    {
        RVM_COV_MARKER();
        
        Arg=&(RVM_STATE->Vct);
    }
    else
    {
        RVM_COV_MARKER();
        
        Arg=&(RVM_STATE->Usr);
    }

    /* Pass the parameters */
    Arg->Param[0]=Param1;
    Arg->Param[1]=Param2;
    Arg->Param[2]=Param3;
    Arg->Param[3]=Param4;
    
    /* Set the number, do the hypercall */
    Arg->Number=Number;
    RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_HYP)==0);
    
    /* Make sure the call is indeed processed */
    RVM_ASSERT(Arg->Number==RVM_HYP_INVALID);
    
    return (rvm_ret_t)Arg->Param[0];
}
/* End Function:RVM_Hyp ******************************************************/

/* Function:RVM_Hyp_Putchar ***************************************************
Description : Print one character to the RVM debug console.
Input       : char Char - The character.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Hyp_Putchar(char Char)
{
#if(RVM_DBGLOG_ENABLE!=0U)
    /* Must be successful */
    RVM_Hyp(RVM_HYP_PUTCHAR,(rvm_ptr_t)Char,0U,0U,0U);
#endif
}
/* End Function:RVM_Hyp_Putchar **********************************************/

/* Function:RVM_Hyp_Int_Ena ***************************************************
Description : Enable interrupts. This must be successful so it does not have
              a return value.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Hyp_Int_Ena(void)
{
    /* Must be successful */
    RVM_Int_Mask=0U;
    RVM_Hyp(RVM_HYP_INT_ENA,0U,0U,0U,0U);
}
/* End Function:RVM_Hyp_Int_Ena **********************************************/

/* Function:RVM_Hyp_Int_Dis ***************************************************
Description : Disable interrupts. This must be successful so it does not have
              a return value.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Hyp_Int_Dis(void)
{
    /* Must be successful */
    RVM_Hyp(RVM_HYP_INT_DIS,0U,0U,0U,0U);
    RVM_Int_Mask=1U;
}
/* End Function:RVM_Hyp_Int_Dis **********************************************/

/* Function:RVM_Hyp_Vct_Phys **************************************************
Description : Register the virtual machine's virtual vector with a physical vector.
Input       : rvm_ptr_t Phys_Num - The physical vector number.
              rvm_ptr_t Vct_Num - The virtual vector number.
Output      : None.
Return      : rvm_ret_t - If successful, the interrupt registration ID; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Vct_Phys(rvm_ptr_t Phys_Num,
                           rvm_ptr_t Vct_Num)
{   
    return RVM_Hyp(RVM_HYP_VCT_PHYS,Phys_Num,Vct_Num,0U,0U);
}
/* End Function:RVM_Hyp_Vct_Phys *********************************************/

/* Function:RVM_Hyp_Vct_Evt ***************************************************
Description : Register the virtual machine's virtual vector with an event channel.
Input       : rvm_ptr_t Evt_Num - The event number.
              rvm_ptr_t Vct_Num - The virtual vector number.
Output      : None.
Return      : rvm_ret_t - If successful, the event channel ID; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Vct_Evt(rvm_ptr_t Evt_Num,
                          rvm_ptr_t Vct_Num)
{
    return RVM_Hyp(RVM_HYP_VCT_EVT,Evt_Num,Vct_Num,0U,0U);
}
/* End Function:RVM_Hyp_Vct_Evt **********************************************/

/* Function:RVM_Hyp_Vct_Del ***************************************************
Description : Deregister the vector of an virtual machine.
Input       : rvm_ptr_t Vct_Num - The virtual vector to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Vct_Del(rvm_ptr_t Vct_Num)
{
    return RVM_Hyp(RVM_HYP_VCT_DEL,Vct_Num,0U,0U,0U);
}
/* End Function:RVM_Hyp_Vct_Del **********************************************/

/* Function:RVM_Hyp_Evt_Add ***************************************************
Description : Add a event source's send capability to virtual machine.
Input       : rvm_ptr_t Evt_Num - The event souce to register.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Evt_Add(rvm_ptr_t Evt_Num)
{
    return RVM_Hyp(RVM_HYP_EVT_ADD,Evt_Num,0U,0U,0U);
}
/* End Function:RVM_Hyp_Evt_Add **********************************************/

/* Function:RVM_Hyp_Evt_Del ***************************************************
Description : Delete a event source's send capability from virtual machine.
Input       : rvm_ptr_t Evt_Num - The event souce to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Evt_Del(rvm_ptr_t Evt_Num)
{
    return RVM_Hyp(RVM_HYP_EVT_DEL,Evt_Num,0U,0U,0U);
}
/* End Function:RVM_Hyp_Evt_Del **********************************************/

/* Function:RVM_Hyp_Vct_Lck ***************************************************
Description : Lockdown the vector mappings in the virtual machine so that it
              cannot be edited in the future.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Vct_Lck(void)
{
    return RVM_Hyp(RVM_HYP_VCT_LCK,0U,0U,0U,0U);
}
/* End Function:RVM_Hyp_Vct_Lck **********************************************/

/* Function:RVM_Hyp_Vct_Wait **************************************************
Description : Set the virtual machine to sleep until a vector comes in.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Vct_Wait(void)
{
    return RVM_Hyp(RVM_HYP_VCT_WAIT,0U,0U,0U,0U);
}
/* End Function:RVM_Hyp_Vct_Wait *********************************************/

/* Function:RVM_Hyp_Evt_Snd ***************************************************
Description : Send an event to the event channel.
Input       : rvm_ptr_t Evt_Num - The event channel ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Evt_Snd(rvm_ptr_t Evt_Num)
{
    return RVM_Hyp(RVM_HYP_EVT_SND,Evt_Num,0U,0U,0U);
}
/* End Function:RVM_Hyp_Evt_Snd **********************************************/

/* Function:RVM_Hyp_Wdg_Clr ***************************************************
Description : Start and feed the watchdog.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
rvm_ret_t RVM_Hyp_Wdg_Clr(void)
{
    return RVM_Hyp(RVM_HYP_WDG_CLR,0U,0U,0U,0U);
}
/* End Function:RVM_Hyp_Wdg_Clr **********************************************/

/* Function:RVM_Vct_Get *******************************************************
Description : Get the interrupt number to handle. After returning the vector, 
              clean up the corresponding bit, then we run the corresponding
              interrupt vector. Note that we won't lose any pending interrupts
              because the interrupt vector is called after the interrupt flag
              is reset; were there any incoming interrupts while the interrupt
              is being processed, the flag will become set again.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If there is interrupt pending, the number; else -1.
******************************************************************************/
rvm_ret_t RVM_Vct_Get(void)
{
    rvm_ptr_t Pos;
    rvm_ptr_t Word_Cnt;
    rvm_ptr_t Byte_Cnt;
    rvm_ptr_t Word;
    rvm_u8_t* Flag;
    
    /* See if interrupt enabled */
    if(RVM_Int_Mask!=0U)
    {
        RVM_COV_MARKER();
        
        return -1;
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
    
    /* See which word is ready, and pick it - must be word aligned */
    for(Word_Cnt=0U;Word_Cnt<RVM_VCTF_WORD_SIZE;Word_Cnt++)
    {
        Word=RVM_VCTF->Vct[Word_Cnt];
        
        /* A word contains the activated byte */
        if(Word!=0U)
        {
            RVM_COV_MARKER();
            
            Flag=(rvm_u8_t*)&Word;
            
            /* Find the exact byte */
            for(Byte_Cnt=0U;Byte_Cnt<RVM_WORD_BYTE;Byte_Cnt++)
            {
                if(Flag[Byte_Cnt]!=0U)
                {
                    RVM_COV_MARKER();
                    
                    Pos=(Word_Cnt<<3)+Byte_Cnt;
                    /* Clear flag then return byte position */
                    ((volatile rvm_u8_t*)RVM_VCTF->Vct)[Pos]=0U;
                    return (rvm_ret_t)Pos;
                }
                else
                {
                    RVM_COV_MARKER();
                    /* No operation required */
                }
            }
            
            /* There must be a non-zero byte or we have an error */
            RVM_ASSERT(0);
        }
        else
        {
            RVM_COV_MARKER();
            /* No operation required */
        }
    }
    
    return -1;
}
/* End Function:RVM_Vct_Get **************************************************/

/* Function:RVM_Vct_Loop ******************************************************
Description : The interrupt handling thread's routine. This should be called by 
              the interrupt handling thread upon entering the system.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Vct_Loop(void)
{
    rvm_cnt_t Vct_Num;
    
    while(1)
    {
        /* Block on the receive endpoint */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_SIG_VCT,RVM_RCV_BM)>=0);
        
        /* Only try to get interrupts if we didn't mask it */
        if(RVM_Int_Mask==0U)
        {
            RVM_COV_MARKER();
            
            /* Indicate vector execution mode active */
            RVM_STATE->Vct_Act=1U;
            /* Clear the pending flag */
            RVM_Vct_Pend=0U;
            /* Look for interrupts to handle from the first */
            Vct_Num=RVM_Vct_Get();
            
            /* Handle the vectors here - all vectors tail-chained */
            while(Vct_Num>=0)
            {
                if(RVM_Handler.Vct[Vct_Num]!=RVM_NULL)
                {
                    RVM_COV_MARKER();
                    
                    RVM_Handler.Vct[Vct_Num]();
                }
                else
                {
                    RVM_COV_MARKER();
                    /* No operation required */
                }
                
                Vct_Num=RVM_Vct_Get();
            }
            
            /* Handle timer interrupts */
            if(RVM_VCTF->Tim!=0U)
            {
                RVM_COV_MARKER();
                
                RVM_VCTF->Tim=0U;
                if(RVM_Handler.Tim!=RVM_NULL)
                {
                    RVM_COV_MARKER();
                    
                    RVM_Handler.Tim();
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

            /* Handle the context switch vector if needed */
            if(RVM_VCTF->Ctx!=0U)
            {
                RVM_VCTF->Ctx=0U;
                if(RVM_Handler.Ctx!=RVM_NULL)
                {
                    RVM_COV_MARKER();
                    
                    RVM_Handler.Ctx();
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
            
            /* Deactivate once we finish */
            RVM_STATE->Vct_Act=0U;
        }
        else
        {
            RVM_COV_MARKER();
            
            /* Vector pending, will try to process when we enable interrupts */
            RVM_Vct_Pend=1U;
        }
    }
}
#endif
/* End Function:RVM_Vct_Loop *************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

