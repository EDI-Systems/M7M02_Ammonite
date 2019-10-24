/******************************************************************************
Filename    : rvm_guest.c
Author      : pry
Date        : 11/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The functionality wrapper for guests.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"

#define __HDR_PRIVATE_MEMBERS__
#include "rvm_guest.h"
#undef __HDR_PRIVATE_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:RVM_Clear ***************************************************
Description : Memset a memory area to zero. This is not fast due to byte operations;
              this is not meant for large memory.
Input       : void* Addr - The address to clear.
              rvm_ptr_t Size - The size to clear.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Clear(void* Addr, rvm_ptr_t Size)
{
    rvm_cnt_t Count;

    for(Count=0;Count<Size;Count++)
        ((rvm_u8_t*)Addr)[Count]=0;
}
/* End Function:RVM_Clear ****************************************************/

/* Begin Function:RVM_List_Crt ************************************************
Description : Create a doubly-linked list.
Input       : volatile struct RVM_List* Head - The pointer to the list head.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_List_Crt(volatile struct RVM_List* Head)
{
    Head->Prev=(struct RVM_List*)Head;
    Head->Next=(struct RVM_List*)Head;
}
/* End Function:RVM_List_Crt *************************************************/

/* Begin Function:RVM_List_Del ************************************************
Description : Delete a node from the doubly-linked list.
Input       : volatile struct RVM_List* Prev - The prevoius node of the target node.
              volatile struct RVM_List* Next - The next node of the target node.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_List_Del(volatile struct RVM_List* Prev,volatile struct RVM_List* Next)
{
    Next->Prev=(struct RVM_List*)Prev;
    Prev->Next=(struct RVM_List*)Next;
}
/* End Function:RVM_List_Del *************************************************/

/* Begin Function:RVM_List_Ins ************************************************
Description : Insert a node to the doubly-linked list.
Input       : volatile struct RVM_List* New - The new node to insert.
              volatile struct RVM_List* Prev - The previous node.
              volatile struct RVM_List* Next - The next node.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_List_Ins(volatile struct RVM_List* New,
                  volatile struct RVM_List* Prev,
                  volatile struct RVM_List* Next)
{
    Next->Prev=(struct RVM_List*)New;
    New->Next=(struct RVM_List*)Next;
    New->Prev=(struct RVM_List*)Prev;
    Prev->Next=(struct RVM_List*)New;
}
/* End Function:RVM_List_Ins *************************************************/

/* Begin Function:RVM_Print_Int ***********************************************
Description : Print a signed integer on the debugging console. This integer is
              printed as decimal with sign.
Input       : rvm_cnt_t Int - The integer to print.
Output      : None.
Return      : rvm_cnt_t - The length of the string printed.
******************************************************************************/
rvm_cnt_t RVM_Print_Int(rvm_cnt_t Int)
{
    rvm_ptr_t Iter;
    rvm_cnt_t Count;
    rvm_cnt_t Num;
    rvm_ptr_t Div;
    
    /* how many digits are there? */
    if(Int==0)
    {
        RVM_Putchar('0');
        return 1;
    }
    else if(Int<0)
    {
        /* How many digits are there? */
        Count=0;
        Div=1;
        Iter=-Int;
        while(Iter!=0)
        {
            Iter/=10;
            Count++;
            Div*=10;
        }
        Div/=10;
        
        RVM_Putchar('-');
        Iter=-Int;
        Num=Count+1;
        
        while(Count>0)
        {
            Count--;
            RVM_Putchar(Iter/Div+'0');
            Iter=Iter%Div;
            Div/=10;
        }
    }
    else
    {
        /* How many digits are there? */
        Count=0;
        Div=1;
        Iter=Int;
        while(Iter!=0)
        {
            Iter/=10;
            Count++;
            Div*=10;
        }
        Div/=10;
        
        Iter=Int;
        Num=Count;
        
        while(Count>0)
        {
            Count--;
            RVM_Putchar(Iter/Div+'0');
            Iter=Iter%Div;
            Div/=10;
        }
    }
    
    return Num;
}
/* End Function:RVM_Print_Int ************************************************/

/* Begin Function:RVM_Print_Uint **********************************************
Description : Print a unsigned integer on the debugging console. This integer is
              printed as hexadecimal.
Input       : rvm_ptr_t Uint - The unsigned integer to print.
Output      : None.
Return      : rvm_cnt_t - The length of the string printed.
******************************************************************************/
rvm_cnt_t RVM_Print_Uint(rvm_ptr_t Uint)
{
    rvm_ptr_t Iter;
    rvm_cnt_t Count;
    rvm_cnt_t Num;
    
    /* how many digits are there? */
    if(Uint==0)
    {
        RVM_Putchar('0');
        return 1;
    }
    else
    {
        /* Filter out all the zeroes */
        Count=0;
        Iter=Uint;
        while((Iter>>((sizeof(rvm_ptr_t)*8)-4))==0)
        {
            Iter<<=4;
            Count++;
        }
        /* Count is the number of pts to print */
        Count=sizeof(rvm_ptr_t)*2-Count;
        Num=Count;
        while(Count>0)
        {
            Count--;
            Iter=(Uint>>(Count*4))&0x0F;
            if(Iter<10)
                RVM_Putchar('0'+Iter);
            else
                RVM_Putchar('A'+Iter-10);
        }
    }
    
    return Num;
}
/* End Function:RVM_Print_Uint ***********************************************/

/* Begin Function:RVM_Print_String ********************************************
Description : Print a string on the debugging console.
              This is only used for user-level debugging.
Input       : rvm_s8_t* String - The string to print.
Output      : None.
Return      : rvm_cnt_t - The length of the string printed, the '\0' is not included.
******************************************************************************/
rvm_cnt_t RVM_Print_String(rvm_s8_t* String)
{
    rvm_cnt_t Count;
    
    Count=0;
    while(Count<RVM_USER_DEBUG_MAX_STR)
    {
        if(String[Count]=='\0')
            break;
        
        RVM_Putchar(String[Count++]);
    }
    
    return Count;
}
/* End Function:RVM_Print_String *********************************************/

/* Begin Function:RVM_Captbl_Crt **********************************************
Description : Create a capability table.
Input       : rvm_cid_t Cap_Captbl_Crt - The capability table that contains the newly 
                                         created cap to captbl. 2-Level.
              rvm_cid_t Cap_Kmem - The kernel memory capability. 2-Level.
              rvm_cid_t Cap_Crt - The capability slot that you want this newly created
                                  capability table capability to be in. 1-Level.
              rvm_ptr_t Raddr - The relative virtual address to store the capability table.
              rvm_ptr_t Entry_Num - The number of entries in that capability table.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Captbl_Crt(rvm_cid_t Cap_Captbl_Crt, rvm_cid_t Cap_Kmem, 
                         rvm_cid_t Cap_Captbl, rvm_ptr_t Raddr, rvm_ptr_t Entry_Num)
{
    return RVM_CAP_OP(RVM_SVC_CAPTBL_CRT, Cap_Captbl_Crt, 
                      RVM_PARAM_D1(Cap_Kmem)|RVM_PARAM_D0(Cap_Captbl),
                      Raddr,
                      Entry_Num);
}
/* End Function:RVM_Captbl_Crt ***********************************************/

/* Begin Function:RVM_Captbl_Del **********************************************
Description : Delete a layer of capability table. The table will be deleted regardless
              of whether there are still capabilities in it.
Input       : rvm_cid_t Cap_Captbl_Del - The capability table that contains the cap to 
                                         captbl. 2-Level.
              rvm_cid_t Cap_Del - The capability to the capability table to be deleted. 1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Captbl_Del(rvm_cid_t Cap_Captbl_Del, rvm_cid_t Cap_Del)
{
    return RVM_CAP_OP(RVM_SVC_CAPTBL_DEL, Cap_Captbl_Del,
                      Cap_Del,
                      0,
                      0);
}
/* End Function:RVM_Captbl_Del ***********************************************/

/* Begin Function:RVM_Captbl_Frz **********************************************
Description : Freeze a capability in the capability table.
Input       : rvm_cid_t Cap_Captbl_Frz  - The capability table containing the cap to
                                          captbl for this operation. 2-Level.
              rvm_cid_t Cap_Frz - The cap to the kernel object being freezed. 1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Captbl_Frz(rvm_cid_t Cap_Captbl_Frz, rvm_cid_t Cap_Frz)
{
        return RVM_CAP_OP(RVM_SVC_CAPTBL_FRZ, Cap_Captbl_Frz,
                          Cap_Frz,
                          0,
                          0);
}
/* End Function:RVM_Captbl_Frz ***********************************************/

/* Begin Function:RVM_Captbl_Add **********************************************
Description : Add one capability into the capability table. This is doing a 
              capability delegation. This function should only be used to delegate
              capabilities other than kernel memory, kernel function and page
              tables.
Input       : rvm_cid_t Cap_Captbl_Dst - The capability to the destination capability table.
                                         2-Level.
              rvm_cid_t Cap_Dst - The capability slot you want to add to. 1-Level.
              rvm_cid_t Cap_Captbl_Src - The capability to the source capability table.
                                         2-Level.
              rvm_cid_t Cap_Src - The capability in the source capability table to delegate.
                                  1-Level.
              rvm_ptr_t Flags - The flags for the capability.
Output      : None.
Return      : rvm_ret_t - If the mapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Captbl_Add(rvm_cid_t Cap_Captbl_Dst, rvm_cid_t Cap_Dst, 
                         rvm_cid_t Cap_Captbl_Src, rvm_cid_t Cap_Src, rvm_ptr_t Flags)
{
    return RVM_CAP_OP(RVM_SVC_CAPTBL_ADD, 0,
                      RVM_PARAM_D1(Cap_Captbl_Dst)|RVM_PARAM_D0(Cap_Dst),
                      RVM_PARAM_D1(Cap_Captbl_Src)|RVM_PARAM_D0(Cap_Src),Flags);
}
/* End Function:RVM_Captbl_Add ***********************************************/

/* Begin Function:RVM_Captbl_Pgtbl ********************************************
Description : Add one capability into the capability table. This is doing a 
              capability delegation. This function should only be used to delegate
              page tables.
Input       : rvm_cid_t Cap_Captbl_Dst - The capability to the destination capability table.
                                         2-Level.
              rvm_cid_t Cap_Dst - The capability slot you want to add to. 1-Level.
              rvm_cid_t Cap_Captbl_Src - The capability to the source capability table.
                                         2-Level.
              rvm_cid_t Cap_Src - The capability in the source capability table to delegate.
                                  1-Level.
              rvm_ptr_t Start - The start position of the page table entry.
              rvm_ptr_t End - The end position of the page table entry.
              rvm_ptr_t Flags - The flags for the page table capability.
Output      : None.
Return      : rvm_ret_t - If the mapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Captbl_Pgtbl(rvm_cid_t Cap_Captbl_Dst, rvm_cid_t Cap_Dst, 
                           rvm_cid_t Cap_Captbl_Src, rvm_cid_t Cap_Src,
                           rvm_ptr_t Start, rvm_ptr_t End, rvm_ptr_t Flags)
{
    return RVM_CAP_OP(RVM_SVC_CAPTBL_ADD, 0,
                      RVM_PARAM_D1(Cap_Captbl_Dst)|RVM_PARAM_D0(Cap_Dst),
                      RVM_PARAM_D1(Cap_Captbl_Src)|RVM_PARAM_D0(Cap_Src),
                      RVM_PGTBL_FLAG(End,Start,Flags));
}
/* End Function:RVM_Captbl_Pgtbl *********************************************/

/* Begin Function:RVM_Captbl_Kern *********************************************
Description : Add one capability into the capability table. This is doing a 
              capability delegation. This function should only be used to delegate
              kernel functions.
Input       : rvm_cid_t Cap_Captbl_Dst - The capability to the destination capability table.
                                         2-Level.
              rvm_cid_t Cap_Dst - The capability slot you want to add to. 1-Level.
              rvm_cid_t Cap_Captbl_Src - The capability to the source capability table.
                                         2-Level.
              rvm_cid_t Cap_Src - The capability in the source capability table to delegate.
                                  1-Level.
              rvm_ptr_t Start - The start of the kernel function ID.
              rvm_ptr_t End - The end of the kernel function ID.
Output      : None.
Return      : rvm_ret_t - If the mapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Captbl_Kern(rvm_cid_t Cap_Captbl_Dst, rvm_cid_t Cap_Dst, 
                          rvm_cid_t Cap_Captbl_Src, rvm_cid_t Cap_Src,
                          rvm_ptr_t Start, rvm_ptr_t End)
{
    return RVM_CAP_OP(RVM_SVC_CAPTBL_ADD, 0,
                      RVM_PARAM_D1(Cap_Captbl_Dst)|RVM_PARAM_D0(Cap_Dst),
                      RVM_PARAM_D1(Cap_Captbl_Src)|RVM_PARAM_D0(Cap_Src),
                      RVM_KERN_FLAG(End,Start));
}
/* End Function:RVM_Captbl_Kern **********************************************/

/* Begin Function:RVM_Captbl_Kmem *********************************************
Description : Add one capability into the capability table. This is doing a 
              capability delegation. This function should only be used to delegate
              kernel memory.
Input       : rvm_cid_t Cap_Captbl_Dst - The capability to the destination capability table.
                                         2-Level.
              rvm_cid_t Cap_Dst - The capability slot you want to add to. 1-Level.
              rvm_cid_t Cap_Captbl_Src - The capability to the source capability table.
                                         2-Level.
              rvm_cid_t Cap_Src - The capability in the source capability table to delegate.
                                  1-Level.
              rvm_ptr_t Start - The relative start virtual address of the kernel memory.
              rvm_ptr_t End - The relative end virtual address of the kernel memory.
              rvm_ptr_t Flags - The flags for the kernel memory capability.
Output      : None.
Return      : rvm_ret_t - If the mapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Captbl_Kmem(rvm_cid_t Cap_Captbl_Dst, rvm_cid_t Cap_Dst, 
                          rvm_cid_t Cap_Captbl_Src, rvm_cid_t Cap_Src,
                          rvm_ptr_t Start, rvm_ptr_t End, rvm_ptr_t Flags)
{
    return RVM_CAP_OP(RVM_KMEM_SVC(End,RVM_SVC_CAPTBL_ADD), RVM_KMEM_CAPID(Start,Flags),
                      RVM_PARAM_D1(Cap_Captbl_Dst)|RVM_PARAM_D0(Cap_Dst),
                      RVM_PARAM_D1(Cap_Captbl_Src)|RVM_PARAM_D0(Cap_Src),
                      RVM_KMEM_FLAG(End,Start));
}
/* End Function:RVM_Captbl_Kmem **********************************************/

/* Begin Function:RVM_Captbl_Rem **********************************************
Description : Remove one capability from the capability table. This function reverts
              the delegation.
Input       : rvm_cid_t Cap_Captbl_Rem - The capability to the capability table to remove from.
                                         2-Level.
              rvm_cid_t Cap_Rem - The capability slot you want to remove. 1-Level.
Output      : None.
Return      : rvm_ret_t - If the mapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Captbl_Rem(rvm_cid_t Cap_Captbl_Rem, rvm_cid_t Cap_Rem)
{
    return RVM_CAP_OP(RVM_SVC_CAPTBL_REM, Cap_Captbl_Rem,
                      Cap_Rem,
                      0,
                      0);
}
/* End Function:_RVM_Captbl_Rem **********************************************/

/* Begin Function:RVM_Kern_Act ************************************************
Description : Activate a kernel function.
Input       : rvm_cid_t Cap_Kern - The capability to the kernel capability. 2-Level.
              rvm_ptr_t Func_ID - The function ID to invoke.
              rvm_ptr_t Sub_ID - The subfunction ID to invoke.
              rvm_ptr_t Param1 - The first parameter.
              rvm_ptr_t Param2 - The second parameter
Output      : None.
Return      : rvm_ret_t - If the call is successful, it will return whatever the 
                          function returned(It is expected that these functions shall
                          never return an negative value); else error code. If the 
                          kernel function ever causes a context switch, it is responsible
                          for setting the return value. On failure, a context switch 
                          in the kernel fucntion is banned.
******************************************************************************/
rvm_ret_t RVM_Kern_Act(rvm_cid_t Cap_Kern, rvm_ptr_t Func_ID, rvm_ptr_t Sub_ID, rvm_ptr_t Param1, rvm_ptr_t Param2)
{
    return RVM_CAP_OP(RVM_SVC_KERN, Cap_Kern,
                      RVM_PARAM_D1(Sub_ID)|RVM_PARAM_D0(Func_ID),
                      Param1,
                      Param2);
}
/* End Function:RVM_Kern_Act *************************************************/

/* Begin Function:RVM_Pgtbl_Crt ***********************************************
Description : Create a layer of page table, and put that capability into a designated
              capability table. The function will check if the memory region is large
              enough, and how it has been typed; if the function found out that it is
              impossible to create a page table due to some error, it will return an
              error.
Input       : rvm_cid_t Cap_Captbl - The capability table that contains the newly created 
                                     cap to pgtbl. 2-Level.
              rvm_cid_t Cap_Kmem - The kernel memory capability. 2-Level.
              rvm_cid_t Cap_Pgtbl - The capability slot that you want this newly created
                                    page table capability to be in. 1-Level.
              rvm_ptr_t Raddr - The relative virtual address to store the page table kernel object.
              rvm_ptr_t Base_Addr - The virtual address to start mapping for this page table.  
                                    This address must be aligned to the total size of the table.
              rvm_ptr_t Top_Flag - Whether this page table is the top-level. If it is, we will
                                   map all the kernel page directories into this one.
              rvm_ptr_t Size_Order - The size order of the page table. The size refers to
                                     the size of each page in the page directory.
              rvm_ptr_t Num_Order - The number order of entries in the page table.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Pgtbl_Crt(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Kmem, rvm_cid_t Cap_Pgtbl, 
                        rvm_ptr_t Raddr, rvm_ptr_t Base_Addr, rvm_ptr_t Top_Flag,
                        rvm_ptr_t Size_Order, rvm_ptr_t Num_Order)
{
    
    return RVM_CAP_OP(RVM_PGTBL_SVC(Num_Order,RVM_SVC_PGTBL_CRT), Cap_Captbl,
                      RVM_PARAM_D1(Cap_Kmem)|RVM_PARAM_Q1(Cap_Pgtbl)|RVM_PARAM_Q0(Size_Order),
                      Raddr, 
                      Base_Addr|Top_Flag);
}
/* End Function:RVM_Pgtbl_Crt ************************************************/

/* Begin Function:RVM_Pgtbl_Del ***********************************************
Description : Delete a layer of page table. We do not care if the childs are all
              deleted. For MPU based environments, it is required that all the 
              mapped child page tables are deconstructed from the master table
              before we can destroy the master table.
Input       : rvm_cid_t Cap_Captbl - The capability to the captbl that may contain the cap
                                     to new captbl. 2-Level.
              rvm_cid_t Cap_Pgtbl - The capability slot that you want this newly created
                                    page table capability to be in. 1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Pgtbl_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Pgtbl)
{
    return RVM_CAP_OP(RVM_SVC_PGTBL_DEL, Cap_Captbl,
                      Cap_Pgtbl,
                      0,
                      0);
}
/* End Function:RVM_Pgtbl_Del ************************************************/

/* Begin Function:RVM_Pgtbl_Add ***********************************************
Description : Delegate a page from one page table to another. This is the only way
              to add pages to new page tables after the system boots.
Input       : rvm_cid_t Cap_Pgtbl_Dst - The capability to the destination page directory. 2-Level.
              rvm_ptr_t Pos_Dst - The position to delegate to in the destination page directory.
              rvm_ptr_t Flags_Dst - The page access permission for the destination page. This is
                                    not to be confused with the flags for the capabilities for
                                    page tables!
              rvm_cid_t Cap_Pgtbl_Src - The capability to the source page directory. 2-Level.
              rvm_ptr_t Pos_Dst - The position to delegate from in the source page directory.
              rvm_ptr_t Index - The index of the physical address frame to delegate.
                                For example, if the destination directory's page size is 1/4
                                of that of the source directory, index=0 will delegate the first
                                1/4, index=1 will delegate the second 1/4, index=2 will delegate
                                the third 1/4, and index=3 will delegate the last 1/4.
                                All other index values are illegal.
Output      : None.
Return      : rvm_ret_t - If the unmapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Pgtbl_Add(rvm_cid_t Cap_Pgtbl_Dst, rvm_ptr_t Pos_Dst, rvm_ptr_t Flags_Dst,
                        rvm_cid_t Cap_Pgtbl_Src, rvm_ptr_t Pos_Src, rvm_ptr_t Index)
{
    return RVM_CAP_OP(RVM_SVC_PGTBL_ADD, RVM_PARAM_D0(Flags_Dst),
                      RVM_PARAM_D1(Cap_Pgtbl_Dst)|RVM_PARAM_D0(Pos_Dst),
                      RVM_PARAM_D1(Cap_Pgtbl_Src)|RVM_PARAM_D0(Pos_Src),
                      Index);
}
/* End Function:RVM_Pgtbl_Add ************************************************/

/* Begin Function:RVM_Pgtbl_Rem ***********************************************
Description : Remove a page from the page table. We are doing unmapping of a page.
Input       : rvm_cid_t Cap_Pgtbl - The capability to the page table. 2-Level.
              rvm_ptr_t Pos - The virtual address position to unmap from.
Output      : None.
Return      : rvm_ret_t - If the unmapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Pgtbl_Rem(rvm_cid_t Cap_Pgtbl, rvm_ptr_t Pos)
{
    return RVM_CAP_OP(RVM_SVC_PGTBL_REM, 0,
                      Cap_Pgtbl,
                      Pos,
                      0);
}
/* End Function:RVM_Pgtbl_Rem ************************************************/

/* Begin Function:RVM_Pgtbl_Con ***********************************************
Description : Map a child page table from the parent page table. Basically, we 
              are doing the construction of a page table.
Input       : rvm_cid_t Cap_Pgtbl_Parent - The capability to the parent page table. 2-Level.
              rvm_ptr_t Pos - The virtual address to position map the child page table to.
              rvm_cid_t Cap_Pgtbl_Child - The capability to the child page table. 2-Level.
              rvm_ptr_t Flags_Child - This have no effect on MPU-based architectures.
Output      : None.
Return      : rvm_ret_t - If the mapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Pgtbl_Con(rvm_cid_t Cap_Pgtbl_Parent, rvm_ptr_t Pos, rvm_cid_t Cap_Pgtbl_Child, rvm_ptr_t Flags_Child)
{
    return RVM_CAP_OP(RVM_SVC_PGTBL_CON, 0,
                      RVM_PARAM_D1(Cap_Pgtbl_Parent)|RVM_PARAM_D0(Cap_Pgtbl_Child),
                      Pos,
                      Flags_Child);
}
/* End Function:RVM_Pgtbl_Con ************************************************/

/* Begin Function:RVM_Pgtbl_Des ***********************************************
Description : Unmap a child page table from the parent page table. Basically, we 
              are doing the destruction of a page table.
Input       : rvm_cid_t Cap_Pgtbl - The capability to the page table. 2-Level.
              rvm_ptr_t Pos - The virtual address to position unmap the child page
                              table from.
Output      : None.
Return      : rvm_ret_t - If the mapping is successful, it will return 0; else error code.
******************************************************************************/
rvm_ret_t RVM_Pgtbl_Des(rvm_cid_t Cap_Pgtbl, rvm_ptr_t Pos)
{
    return RVM_CAP_OP(RVM_SVC_PGTBL_DES, 0,
                      Cap_Pgtbl,
                      Pos,
                      0);
}
/* End Function:RVM_Pgtbl_Des ************************************************/

/* Begin Function:RVM_Proc_Crt ************************************************
Description : Create a process. A process is in fact a protection domain associated
              with a set of capabilities.
Input       : rvm_cid_t Cap_Captbl_Crt - The capability to the capability table to put
                                         this process capability in. 2-Level.
              rvm_cid_t Cap_Kmem - The kernel memory capability. 2-Level.
              rvm_cid_t Cap_Proc - The capability slot that you want this newly created
                                   process capability to be in. 1-Level.
              rvm_cid_t Cap_Captbl - The capability to the capability table to use for
                                     this process. 2-Level.
              rvm_cid_t Cap_Pgtbl - The capability to the page table to use for this process.
                                    2-Level.
              rvm_ptr_t Raddr - The relative virtual address to store the process kernel object.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Proc_Crt(rvm_cid_t Cap_Captbl_Crt, rvm_cid_t Cap_Kmem, rvm_cid_t Cap_Proc,
                       rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Pgtbl, rvm_ptr_t Raddr)
{
    return RVM_CAP_OP(RVM_SVC_PROC_CRT, Cap_Captbl_Crt,
                      RVM_PARAM_D1(Cap_Kmem)|RVM_PARAM_D0(Cap_Proc),
                      RVM_PARAM_D1(Cap_Captbl)|RVM_PARAM_D0(Cap_Pgtbl),
                      Raddr);
}
/* End Function:RVM_Proc_Crt *************************************************/

/* Begin Function:RVM_Proc_Del ************************************************
Description : Delete a process.
Input       : rvm_cid_t Cap_Captbl - The capability to the capability table. 2-Level.
              rvm_cid_t Cap_Proc - The capability to the process. 1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Proc_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Proc)
{
    return RVM_CAP_OP(RVM_SVC_PROC_DEL, Cap_Captbl,
                      Cap_Proc,
                      0,
                      0);
}
/* End Function:RVM_Proc_Del *************************************************/

/* Begin Function:RVM_Proc_Cpt ***********************************************
Description : Assign or change a process's capability table.
Input       : rvm_cid_t Cap_Proc - The capability to the process that have been created
                               already. 2-Level.
              rvm_cid_t Cap_Captbl - The capability to the capability table to use for
                                 this process. 2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Proc_Cpt(rvm_cid_t Cap_Proc, rvm_cid_t Cap_Captbl)
{
    return RVM_CAP_OP(RVM_SVC_PROC_CPT, 0,
                      Cap_Proc,
                      Cap_Captbl,
                      0);
}
/* End Function:RVM_Proc_Cpt *************************************************/

/* Begin Function:RVM_Proc_Pgt ************************************************
Description : Assign or change a process's page table.
Input       : rvm_cid_t Cap_Proc - The capability slot that you want this newly created
                               process capability to be in. 2-Level.
              rvm_cid_t Cap_Pgtbl - The capability to the page table to use for this
                                process. 2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Proc_Pgt(rvm_cid_t Cap_Proc, rvm_cid_t Cap_Pgtbl)
{
    return RVM_CAP_OP(RVM_SVC_PROC_PGT, 0,
                      Cap_Proc,
                      Cap_Pgtbl,
                      0);
}
/* End Function:RVM_Proc_Pgt *************************************************/

/* Begin Function:RVM_Thd_Crt *************************************************
Description : Create a thread. A thread is the minimal kernel-level execution unit.
Input       : rvm_cid_t Cap_Captbl - The capability to the capability table. 2-Level.
              rvm_cid_t Cap_Kmem - The kernel memory capability. 2-Level.
              rvm_cid_t Cap_Thd - The capability slot that you want this newly created
                              thread capability to be in. 1-Level.
              rvm_cid_t Cap_Proc - The capability to the process that it is in. 2-Level.
              rvm_ptr_t Max_Prio - The maximum priority allowed for this thread. Once set,
                               this cannot be changed.
              rvm_ptr_t Raddr - The relative virtual address to store the thread kernel object.
Output      : None.
Return      : rvm_ret_t - If successful, the Thread ID; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Crt(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Kmem, rvm_cid_t Cap_Thd,
                  rvm_cid_t Cap_Proc, rvm_ptr_t Max_Prio, rvm_ptr_t Raddr)
{
    return RVM_CAP_OP(RVM_SVC_THD_CRT, Cap_Captbl, 
                      RVM_PARAM_D1(Cap_Kmem)|RVM_PARAM_D0(Cap_Thd),
                      RVM_PARAM_D1(Cap_Proc)|RVM_PARAM_D0(Max_Prio),
                      Raddr);
}
/* End Function:RVM_Thd_Crt **************************************************/

/* Begin Function:RVM_Thd_Del *************************************************
Description : Delete a thread.
Input       : rvm_cid_t Cap_Captbl - The capability to the capability table. 2-Level.
              rvm_cid_t Cap_Thd - The capability to the thread in the captbl. 1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Thd)
{
    return RVM_CAP_OP(RVM_SVC_THD_DEL, Cap_Captbl,
                      Cap_Thd,
                      0,
                      0);
}
/* End Function:RVM_Thd_Del **************************************************/

/* Begin Function:RVM_Thd_Exec_Set ********************************************
Description : Set a thread's entry point and stack. The registers will be initialized
              with these contents.
Input       : rvm_cid_t Cap_Thd - The capability to the thread. 2-Level.
              rvm_ptr_t Entry - The entry of the thread. An address.
              rvm_ptr_t Stack - The stack address to use for execution. An address.
              rvm_ptr_t Param - The parameter of the thread.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Exec_Set(rvm_cid_t Cap_Thd, rvm_ptr_t Entry, rvm_ptr_t Stack, rvm_ptr_t Param)
{
    return RVM_CAP_OP(RVM_SVC_THD_EXEC_SET, Cap_Thd,
                      Entry, 
                      Stack,
                      Param);
}
/* End Function:RVM_Thd_Exec_Set *********************************************/

/* Begin Function:RVM_Thd_Hyp_Set *********************************************
Description : Set a thread's entry point and stack. The registers will be initialized
              with these contents.
Input       : rvm_cid_t Cap_Thd - The capability to the thread. 2-Level.
              rvm_ptr_t Kaddr - The kernel-accessible virtual memory address for this
                            thread's register sets.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Hyp_Set(rvm_cid_t Cap_Thd, rvm_ptr_t Kaddr)
{
    return RVM_CAP_OP(RVM_SVC_THD_HYP_SET, 0,
                      Cap_Thd,
                      Kaddr,
                      0);
}
/* End Function:RVM_Thd_Hyp_Set **********************************************/

/* Begin Function:RVM_Thd_Sched_Bind ******************************************
Description : Set a thread's priority level, and its scheduler thread. When there
              is any state change on this thread, a notification will be sent to
              scheduler thread. If the state of the thread changes for multiple
              times, then only the most recent state will be in the scheduler's 
              receive queue.
              The scheduler and the threads that it schedule must be on the same
              core. When a thread wants to go from one core to another, its notification
              to the scheduler must all be processed, and it must have no scheduler
              notifications in itself. 
              This must be called on the same core with the Cap_Thd_Sched, and the
              Cap_Thd itself must be free.
              It is impossible to set a thread's priority beyond its maximum priority. 
Input       : rvm_cid_t Cap_Thd - The capability to the thread. 2-Level.
              rvm_cid_t Cap_Thd_Sched - The scheduler thread. 2-Level.
              rvm_cid_t Cap_Sig - The signal endpoint for scheduler notifications. This signal
                                  endpoint will be sent to whenever this thread has a fault, or
                                  timeouts. This is purely optional; if it is not needed, pass
                                  in RME_CAPID_NULL.
              rvm_tid_t TID - The thread ID. This is user-supplied, and the kernel will not
                              check whether there are two threads that have the same TID.
              rvm_ptr_t Prio - The priority level, higher is more critical.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Bind(rvm_cid_t Cap_Thd, rvm_cid_t Cap_Thd_Sched, rvm_cid_t Cap_Sig, rvm_tid_t TID, rvm_ptr_t Prio)
{
    return RVM_CAP_OP(RVM_SVC_THD_SCHED_BIND, Cap_Thd,
                      RVM_PARAM_D1(Cap_Thd_Sched)|RVM_PARAM_D0(Cap_Sig),
                      TID, 
                      Prio);
}
/* End Function:RVM_Thd_Sched_Bind *******************************************/

/* Begin Function:RVM_Thd_Sched_Prio ******************************************
Description : Change a thread's priority level. This can only be called from the
              core that have the thread bonded.
              This system call can cause a potential context switch.
              It is impossible to set a thread's priority beyond its maximum priority. 
Input       : rvm_cid_t Cap_Thd - The capability to the thread. 2-Level.
              rvm_ptr_t Prio - The priority level, higher is more critical.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Prio(rvm_cid_t Cap_Thd, rvm_ptr_t Prio)
{
    return RVM_CAP_OP(RVM_SVC_THD_SCHED_PRIO, 0,
                      Cap_Thd,
                      Prio, 
                      0);
}
/* End Function:RVM_Thd_Sched_Prio *******************************************/

/* Begin Function:RVM_Thd_Sched_Free ******************************************
Description : Free a thread from its current bonding. This function can only be
              executed from the same core on with the thread.
              This system call can cause a potential context switch.
Input       : rvm_cid_t Cap_Thd - The capability to the thread. 2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Sched_Free(rvm_cid_t Cap_Thd)
{
    return RVM_CAP_OP(RVM_SVC_THD_SCHED_FREE, 0,
                      Cap_Thd,
                      0, 
                      0);
}
/* End Function:RVM_Thd_Sched_Free *******************************************/

/* Begin Function:RVM_Thd_Time_Xfer *******************************************
Description : Transfer time from one thread to another. This can only be called
              from the core that the thread is on, and the the two threads involved
              in the time transfer must be on the same core.
              If the time transfered is more than or equal to what the source have,
              the source will be out of time or blocked. If the source is both out of
              time and blocked, we do not send the notification; Instead, we send the
              notification when the receive endpoint actually receive something.
              It is possible to transfer time to threads have a lower priority, and it
              is also possible to transfer time to threads that have a higher priority.
              In the latter case, and if the source is currently running, a preemption
              will directly occur.
Input       : rvm_cid_t Cap_Thd_Dst - The destination thread. 2-Level.
              rvm_cid_t Cap_Thd_Src - The source thread. 2-Level.
              rvm_ptr_t Time - The time to transfer, in slices. A slice is the minimal
                               amount of time transfered in the system usually on the
                               order of 100us or 1ms.
Output      : None.
Return      : rvm_ret_t - If successful, the destination time amount; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Time_Xfer(rvm_cid_t Cap_Thd_Dst, rvm_cid_t Cap_Thd_Src, rvm_ptr_t Time)
{
    return RVM_CAP_OP(RVM_SVC_THD_TIME_XFER, 0,
                      Cap_Thd_Dst,
                      Cap_Thd_Src, 
                      Time);
}
/* End Function:RVM_Thd_Time_Xfer ********************************************/

/* Begin Function:RVM_Thd_Swt *************************************************
Description : Switch to another thread. The thread to switch to must have the same
              preemptive priority as this thread, and have time, and not blocked.
              If trying to switch to a higher priority thread, this is impossible
              because whenever a thread with higher priority exists in the system,
              the kernel will let it preempt the current thread. 
              If trying to switch to a lower priority thread, this is impossible
              because the current thread just preempts it after the thread switch.
Input       : rvm_cid_t Cap_Thd - The capability to the thread. 2-Level. If this is
                                  -1, the kernel will pickup whatever thread that have
                                  the highest priority and have time to run. 
              rvm_ptr_t Full_Yield - This is a flag to indicate whether this is a full
                                     yield. If it is, the kernel will kill all the time
                                     alloted on this thread.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Swt(rvm_cid_t Cap_Thd, rvm_ptr_t Full_Yield)
{
    return RVM_CAP_OP(RVM_SVC_THD_SWT, 0,
                      Cap_Thd,
                      Full_Yield, 
                      0);
}
/* End Function:RVM_Thd_Swt **************************************************/

/* Begin Function:RVM_Sig_Crt *************************************************
Description : Create a signal capability.
Input       : rvm_cid_t Cap_Captbl - The capability to the capability table to use
                                     for this signal. 2-Level.
              rvm_cid_t Cap_Kmem - The kernel memory capability. 2-Level.
              rvm_cid_t Cap_Inv - The capability slot that you want this newly
                                  created signal capability to be in. 1-Level.
              rvm_ptr_t Raddr - The relative virtual address to store the signal
                                endpoint kernel object.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Sig_Crt(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Kmem, rvm_cid_t Cap_Sig, rvm_ptr_t Raddr)
{
    return RVM_CAP_OP(RVM_SVC_SIG_CRT, Cap_Captbl,
                      Cap_Kmem,
                      Cap_Sig, 
                      Raddr);
}
/* End Function:RVM_Sig_Crt **************************************************/

/* Begin Function:RVM_Sig_Del *************************************************
Description : Delete a signal capability.
Input       : rvm_cid_t Cap_Captbl - The capability to the capability table to delete from.
                                     2-Level.
              rvm_cid_t Cap_Sig - The capability to the signal. 1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Sig_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Sig)
{
    return RVM_CAP_OP(RVM_SVC_SIG_DEL, Cap_Captbl,
                      Cap_Sig,
                      0, 
                      0);
}
/* End Function:RVM_Sig_Del **************************************************/

/* Begin Function:RVM_Sig_Snd ************************************************
Description : Try to send a signal from user level. This system call can cause
              a potential context switch.
Input       : rvm_cid_t Cap_Sig - The capability to the signal. 2-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0, or an error code.
******************************************************************************/
rvm_ret_t RVM_Sig_Snd(rvm_cid_t Cap_Sig)
{
    return RVM_CAP_OP(RVM_SVC_SIG_SND, 0,
                      Cap_Sig,
                      0, 
                      0);
}
/* End Function:RVM_Sig_Snd **************************************************/

/* Begin Function:RVM_Sig_Rcv *************************************************
Description : Try to receive a signal capability. The rules for the signal capability
              is:
              1.If a receive endpoint have many send endpoints, everyone can send to it,
                and sending to it will increase the signal count by 1.
              2.If some thread blocks on a receive endpoint, the wakeup is only possible
                from the same core that thread is on.
              3.It is not recommended to let 2 cores operate on the rcv endpoint simutaneously.
              This system call can potentially trigger a context switch.
Input       : rvm_cid_t Cap_Sig - The capability to the signal. 2-Level.
              rvm_ptr_t Option - The receive option.
Output      : None.
Return      : rvm_ret_t - If successful, a non-negative number containing the number of
                          signals received will be returned; else an error code.
******************************************************************************/
rvm_ret_t RVM_Sig_Rcv(rvm_cid_t Cap_Sig, rvm_ptr_t Option)
{
    return RVM_CAP_OP(RVM_SVC_SIG_RCV, 0,
                      Cap_Sig,
                      Option, 
                      0);
}
/* End Function:RVM_Sig_Rcv **************************************************/

/* Begin Function:RVM_Inv_Crt *************************************************
Description : Create a invocation capability.
Input       : rvm_cid_t Cap_Captbl - The capability to the capability table to use
                                     for this process. 2-Level.
              rvm_cid_t Cap_Inv - The capability slot that you want this newly created
                                  invocation capability to be in. 1-Level.
              rvm_cid_t Cap_Proc - The capability to the process that it is in. 2-Level.
              rvm_ptr_t Raddr - The relative virtual address to store the invocation port
                                kernel object.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Inv_Crt(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Kmem, 
                      rvm_cid_t Cap_Inv, rvm_cid_t Cap_Proc, rvm_ptr_t Raddr)
{
    return RVM_CAP_OP(RVM_SVC_INV_CRT, Cap_Captbl,
                      RVM_PARAM_D1(Cap_Kmem)|RVM_PARAM_D0(Cap_Inv),
                      Cap_Proc, 
                      Raddr);
}
/* End Function:RVM_Inv_Crt **************************************************/

/* Begin Function:RVM_Inv_Del *************************************************
Description : Delete an invocation capability.
Input       : rvm_cid_t Cap_Captbl - The capability to the capability table to delete from.
                                     2-Level.
              rvm_cid_t Cap_Inv - The capability to the invocation stub. 1-Level.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Inv_Del(rvm_cid_t Cap_Captbl, rvm_cid_t Cap_Inv)
{
    return RVM_CAP_OP(RVM_SVC_INV_DEL, Cap_Captbl,
                      Cap_Inv,
                      0, 
                      0);
}
/* End Function:RVM_Inv_Del **************************************************/

/* Begin Function:RVM_Inv_Set *************************************************
Description : Set an invocation stub's entry point and stack. The registers will
              be initialized with these contents.
Input       : rvm_cid_t Cap_Inv - The capability to the invocation stub. 2-Level.
              rvm_ptr_t Entry - The entry of the thread.
              rvm_ptr_t Stack - The stack address to use for execution.
              rvm_ptr_t Fault_Ret_Flag - If there is an error in this invocation, we
                                         return immediately, or we wait for fault handling?
                                         If 1, we return directly on fault.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Inv_Set(rvm_cid_t Cap_Inv, rvm_ptr_t Entry, rvm_ptr_t Stack, rvm_ptr_t Fault_Ret_Flag)
{
    return RVM_CAP_OP(RVM_SVC_INV_SET, 0,
                      RVM_PARAM_D1(Fault_Ret_Flag)|RVM_PARAM_D0(Cap_Inv),
                      Entry, 
                      Stack);
}
/* End Function:RVM_Inv_Set **************************************************/

/* Begin Function:RVM_Proc_Send_Evt *******************************************
Description : Send to an event source from a process.
Input       : rvm_ptr_t Evt_Num - The number to send to.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Proc_Send_Evt(rvm_ptr_t Evt_Num)
{
    return RVM_Kern_Act(0, RVM_KERN_EVT_LOCAL_TRIG, 0, Evt_Num, 0);
}
/* End Function:RVM_Proc_Send_Evt ********************************************/

/* Begin Function:RVM_Virt_Init ***********************************************
Description : Initialize virtual machine related RVM library.
Input       : None
Output      : None.
Return      : None.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
void RVM_Virt_Init(void)
{
    /* Clear all VM-related flags and registration tables */
    RVM_Int_Enable=0;
    RVM_Vect_Pend=0;
    RVM_Vect_Active=0;

    /* Clean up all global variables */
    RVM_Clear((void*)RVM_VIRT_VCTF_BASE, RVM_VIRT_VCTF_SIZE);
    RVM_Clear(&RVM_Vect, sizeof(struct RVM_Vect_Handler));
}
#endif
/* End Function:RVM_Virt_Init ************************************************/

/* Begin Function:RVM_Virt_Reg_Vect *******************************************
Description : Initialize an interrupt handler.
Input       : rvm_ptr_t Num - The vector number to register for.
              void* Vect - The actual vector handler, should take no arguments
                           and return nothing.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Virt_Reg_Vect(rvm_ptr_t Vect_Num, void* Vect)
{
    if(Vect_Num>=RVM_VIRT_VECT_NUM)
        return -1;
    
    RVM_Vect.Vect[Vect_Num]=Vect;
    return 0;
}
#endif
/* End Function:RVM_Virt_Reg_Vect ********************************************/

/* Begin Function:RVM_Virt_Reg_Timer ******************************************
Description : Register special timer interrupt handler.
Input       : None
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Virt_Reg_Timer(void* Timer)
{
    RVM_Vect.Timer=Timer;
    return 0;
}
#endif
/* End Function:RVM_Virt_Reg_Timer *******************************************/

/* Begin Function:RVM_Virt_Reg_Ctxsw ******************************************
Description : Register special context switch interrupt handler.
Input       : None
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Virt_Reg_Ctxsw(void* Ctxsw)
{
    RVM_Vect.Ctxsw=Ctxsw;
    return 0;
}
#endif
/* End Function:RVM_Virt_Reg_Ctxsw *******************************************/

/* Begin Function:RVM_Virt_Mask_Int *******************************************
Description : Mask interrupts; this does not stop the interrupt delivery, but will
              postpone the processing until we unmask them.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
void RVM_Virt_Mask_Int(void)
{
    RVM_Int_Enable=0;
}
#endif
/* End Function:RVM_Virt_Mask_Int ********************************************/

/* Begin Function:RVM_Virt_Unmask_Int *****************************************
Description : Unmask pending interrupts so we can process them.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
void RVM_Virt_Unmask_Int(void)
{
    RVM_Int_Enable=1;
    /* Trigger interrupt processing if there are pending ones */
    if(RVM_Vect_Pend!=0)
    {
        RVM_Vect_Pend=0;
        RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VECT)==0);
    }
}
#endif
/* End Function:RVM_Virt_Unmask_Int ******************************************/

/* Begin Function:RVM_Virt_Yield **********************************************
Description : Yield the current thread by triggering the context switch endpoint.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
void RVM_Virt_Yield(void)
{
    /* Set the context switch flag */
    RVM_VECT_FLAG->Ctxsw=1;
    
    /* We send a new trigger if the interrupt is not masked and we are not in interrupt */
    if(RVM_Vect_Active==0)
    {
        if(RVM_Int_Enable!=0)
            RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VECT)==0);
        else
            RVM_Vect_Pend=1;
    }
}
#endif
/* End Function:RVM_Virt_Yield ***********************************************/

/* Begin Function:RVM_Hyp *****************************************************
Description : Do a hypercall to the virtual machine.
Input       : rvm_ptr_t Number - The hypercall number.
              rvm_ptr_t Param1 - The first parameter.
              rvm_ptr_t Param2 - The second parameter.
              rvm_ptr_t Param3 - The third parameter.
Output      : None.
Return      : rvm_ret_t - The return value position.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Hyp(rvm_ptr_t Number, rvm_ptr_t Param1, rvm_ptr_t Param2, rvm_ptr_t Param3, rvm_ptr_t Param4)
{
    volatile struct RVM_Param* Args;

    if(RVM_Vect_Active!=0)
    {
        RVM_PARAM->Vect_Active=1;
        Args=&(RVM_PARAM->Vect);
    }
    else
    {
        RVM_PARAM->Vect_Active=0;
        Args=&(RVM_PARAM->User);
    }

    /* Pass the parameters */
    Args->Number=Number;
    Args->Param[0]=Param1;
    Args->Param[1]=Param2;
    Args->Param[2]=Param3;
    Args->Param[3]=Param4;
    
    /* Do the hypercall */
    RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_HYP)==0);
    
    /* Return the result */
    return Args->Param[0];
}
#endif
/* End Function:RVM_Hyp ******************************************************/

/* Begin Function:RVM_Hyp_Ena_Int *********************************************
Description : Enable interrupts. This must be successful so it does not have
              a return value.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
void RVM_Hyp_Ena_Int(void)
{
    /* Must be successful */
    RVM_Int_Enable=1;
    RVM_Hyp(RVM_HYP_ENAINT,0,0,0,0);
}
#endif
/* End Function:RVM_Hyp_Ena_Int **********************************************/

/* Begin Function:RVM_Hyp_Dis_Int *********************************************
Description : Disable interrupts. This must be successful so it does not have
              a return value.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
void RVM_Hyp_Dis_Int(void)
{
    /* Must be successful */
    RVM_Hyp(RVM_HYP_DISINT,0,0,0,0);
    RVM_Int_Enable=0;
}
#endif
/* End Function:RVM_Hyp_Dis_Int **********************************************/

/* Begin Function:RVM_Hyp_Reg_Phys ********************************************
Description : Register the virtual machine's virtual vector with a physical vector.
Input       : rvm_ptr_t Phys_Num - The physical vector number.
              rvm_ptr_t Vect_Num - The virtual vector number.
Output      : None.
Return      : rvm_ret_t - If successful, the interrupt registration ID; else an error code.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Hyp_Reg_Phys(rvm_ptr_t Phys_Num, rvm_ptr_t Vect_Num)
{   
    return RVM_Hyp(RVM_HYP_REGPHYS,Phys_Num,Vect_Num,0,0);
}
#endif
/* End Function:RVM_Hyp_Reg_Phys *********************************************/

/* Begin Function:RVM_Hyp_Reg_Evt *********************************************
Description : Register the virtual machine's virtual vector with an event channel.
Input       : rvm_ptr_t Evt_Num - The event number.
              rvm_ptr_t Vect_Num - The virtual vector number.
Output      : None.
Return      : rvm_ret_t - If successful, the event channel ID; else an error code.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Hyp_Reg_Evt(rvm_ptr_t Evt_Num, rvm_ptr_t Vect_Num)
{
    return RVM_Hyp(RVM_HYP_REGEVT,Evt_Num,Vect_Num,0,0);
}
#endif
/* End Function:RVM_Hyp_Reg_Evt **********************************************/

/* Begin Function:RVM_Hyp_Del_Vect ********************************************
Description : Deregister the vector of an virtual machine.
Input       : rvm_ptr_t Vect_Num - The virtual vector to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Hyp_Del_Vect(rvm_ptr_t Vect_Num)
{
    return RVM_Hyp(RVM_HYP_DELVECT,Vect_Num,0,0,0);
}
#endif
/* End Function:RVM_Hyp_Del_Vect *********************************************/

/* Begin Function:RVM_Hyp_Wait_Vect *******************************************
Description : Set the virtual machine to sleep until a vector comes in.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Hyp_Wait_Vect(void)
{
    return RVM_Hyp(RVM_HYP_WAITVECT,0,0,0,0);
}
#endif
/* End Function:RVM_Hyp_Wait_Vect ********************************************/

/* Begin Function:RVM_Hyp_Send_Evt ********************************************
Description : Send an event to the event channel.
Input       : rvm_ptr_t Evt_Num - The event channel ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Hyp_Send_Evt(rvm_ptr_t Evt_Num)
{
    return RVM_Hyp(RVM_HYP_SENDEVT,Evt_Num,0,0,0);
}
#endif
/* End Function:RVM_Hyp_Send_Evt *********************************************/

/* Begin Function:RVM_Get_Vect ************************************************
Description : Get the interrupt number to handle. After returning the vector, clean
              up the corresponding bit.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If there is interrupt pending, the interrupt number; else -1.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
rvm_ret_t RVM_Get_Vect(void)
{
    rvm_cnt_t Count;
    rvm_cnt_t Pos;
    
    /* See if interrupt enabled */
    if(RVM_Int_Enable==0)
        return -1;
    
    /* See which one is ready, and pick it */
    Pos=-1;
    for(Count=RVM_VIRT_VCTF_WORDS-1;Count>=0;Count--)
    {
        if(RVM_VECT_FLAG->Vect[Count]==0)
            continue;
        
        Pos=_RVM_MSB_Get(RVM_VECT_FLAG->Vect[Count]);
        Pos+=(Count<<RVM_WORD_ORDER);
        break;
    }

    /* Now kill the bit */
    if(Pos>=0)
        RVM_Fetch_And((rvm_ptr_t*)&(RVM_VECT_FLAG->Vect[Count]),~RVM_POW2(Pos));
    
    return Pos;
}
#endif
/* End Function:RVM_Get_Vect *************************************************/

/* Begin Function:RVM_Vect_Loop ***********************************************
Description : The interrupt handling thread's routine. This should be called by 
              the interrupt handling thread upon entering the system.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef RVM_VIRT_VECT_NUM
void RVM_Vect_Loop(void)
{
    rvm_cnt_t Vect_Num;
    
    while(1)
    {
        /* Block on the receive endpoint */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_SIG_VECT, RVM_RCV_BM)>=0);

        /* Only try to get interrupts if we didn't mask it */
        if(RVM_Int_Enable!=0)
        {
            /* Clear the pending flag */
            RVM_Vect_Pend=0;

            /* Look for interrupts to handle from the first */
            Vect_Num=RVM_Get_Vect();
            /* Handle the vector here - the vectors are tail-chained */
            while(Vect_Num>=0)
            {
                if(RVM_Vect.Vect[Vect_Num]!=0)
                {
                    RVM_Vect_Active=1;
                    ((void(*)(void))RVM_Vect.Vect[Vect_Num])();
                    RVM_Vect_Active=0;
                }
                Vect_Num=RVM_Get_Vect();
            }
            
            /* We have handled all vectors. Now handle timer vectors */
            if(RVM_VECT_FLAG->Timer!=0)
            {
                RVM_VECT_FLAG->Timer=0;
                if(RVM_Vect.Timer!=0)
                {
                    RVM_Vect_Active=1;
                    ((void(*)(void))RVM_Vect.Timer)();
                    RVM_Vect_Active=0;
                }
            }

            /* Then finally handle the context switch vector, conduct context switch */
            if(RVM_VECT_FLAG->Ctxsw!=0)
            {
                RVM_VECT_FLAG->Ctxsw=0;
                if(RVM_Vect.Ctxsw!=0)
                {
                    RVM_Vect_Active=1;
                    ((void(*)(void))RVM_Vect.Ctxsw)();
                    RVM_Vect_Active=0;
                }
            }
        }
        else
            RVM_Vect_Pend=1;
    }
}
#endif
/* End Function:RVM_Vect_Loop ************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

