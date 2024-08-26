/******************************************************************************
Filename    : rvm_monitor.c
Author      : pry
Date        : 11/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : The RVM virtual machine monitor.
******************************************************************************/

/* Include *******************************************************************/
#include "rvm.h"
#include "rvm_boot.h"

#define __HDR_DEF__
#include "rvm_platform.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_DEF__

#define __HDR_STRUCT__
#include "rvm_platform.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_STRUCT__

/* Private include */
#include "Monitor/rvm_monitor.h"

#define __HDR_PUBLIC__
#include "rvm_platform.h"
#include "Monitor/rvm_monitor.h"
#undef __HDR_PUBLIC__
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
              This is only used for user-level debugging.
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
    RVM_DBG_S("\r\n***\r\nMonitor panic - not syncing :\r\n"); \
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

/* Function:_RVM_MSB_Generic **************************************************
Description : Find the MSB's position. This is a portable solution for all
              processors; if your processor does not have fast built-in bit
              manipulation support, you can resort to this when porting.
Input       : rvm_ptr_t Value - The value to compute for.
Output      : None.
Return      : rvm_ptr_t - The result. 0 will be returned for 0.
******************************************************************************/
rvm_ptr_t _RVM_MSB_Generic(rvm_ptr_t Value)
{
    rvm_ptr_t Bit;
    static const rvm_u8_t Table[256U]=
    {
        0U,0U,1U,1U,2U,2U,2U,2U,3U,3U,3U,3U,3U,3U,3U,3U,
        4U,4U,4U,4U,4U,4U,4U,4U,4U,4U,4U,4U,4U,4U,4U,4U,
        5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,
        5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,5U,
        6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,
        6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,
        6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,
        6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,6U,
        7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,
        7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,
        7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,
        7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,
        7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,
        7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,
        7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,
        7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U,7U
    };

#if(RVM_WORD_ORDER==4U)
    /* 15-8 */
    if(Value>=RVM_POW2(8U))
    {
        RVM_COV_MARKER();
        
        Bit=8U;
    }
    /* 7-0 */
    else
    {
        RVM_COV_MARKER();
        
        Bit=0U;
    }
#elif(RVM_WORD_ORDER==5U)
    /* 31-16 */
    if(Value>=RVM_POW2(16U))
    {
        RVM_COV_MARKER();
        
        /* 31-24 */
        if(Value>=RVM_POW2(24U))
        {
            RVM_COV_MARKER();
            
            Bit=24U;
        }
        /* 24-16 */
        else
        {
            RVM_COV_MARKER();
            
            Bit=16U;
        }
    }
    /* 15-0 */
    else
    {
        RVM_COV_MARKER();
        
        /* 15-8 */
        if(Value>=RVM_POW2(8U))
        {
            RVM_COV_MARKER();
            
            Bit=8U;
        }
        /* 7-0 */
        else
        {
            RVM_COV_MARKER();
            
            Bit=0U;
        }
    }
#elif(RVM_WORD_ORDER==6U)
    /* 63-32 */
    if(Value>=RVM_POW2(32U))
    {
        RVM_COV_MARKER();
        
        /* 63-48 */
        if(Value>=RVM_POW2(48U))
        {
            RVM_COV_MARKER();
            
            /* 63-56 */
            if(Value>=RVM_POW2(56U))
            {
                RVM_COV_MARKER();
                
                Bit=56U;
            }
            /* 56-48 */
            else
            {
                RVM_COV_MARKER();
                
                Bit=48U;
            }
        }
        /* 47-32 */
        else
        {
            RVM_COV_MARKER();
            
            /* 47-40 */
            if(Value>=RVM_POW2(40U))
            {
                RVM_COV_MARKER();
                
                Bit=40U;
            }
            /* 39-32 */
            else
            {
                RVM_COV_MARKER();
                
                Bit=32U;
            }
        }
    }
    /* 31-0 */
    else
    {
        RVM_COV_MARKER();
        
        /* 31-16 */
        if(Value>=RVM_POW2(16U))
        {
            RVM_COV_MARKER();
            
            /* 31-24 */
            if(Value>=RVM_POW2(24U))
            {
                RVM_COV_MARKER();
                
                Bit=24U;
            }
            /* 24-16 */
            else
            {
                RVM_COV_MARKER();
                
                Bit=16U;
            }
        }
        /* 15-0 */
        else
        {
            RVM_COV_MARKER();
            
            /* 15-8 */
            if(Value>=RVM_POW2(8U))
            {
                RVM_COV_MARKER();
                
                Bit=8U;
            }
            /* 7-0 */
            else
            {
                RVM_COV_MARKER();
                
                Bit=0U;
            }
        }
    }
#else
#error Generic MSB for 128-bits & above are not implemented.
#endif

    return Table[Value>>Bit]+Bit;
}
/* End Function:_RVM_MSB_Generic *********************************************/

/* Function:_RVM_LSB_Generic **************************************************
Description : Find the LSB's position. This is a portable solution for all
              processors; if your processor does not have fast built-in bit
              manipulation support, you can resort to this when porting.
Input       : rvm_ptr_t Value - The value to count.
Output      : None.
Return      : rvm_ptr_t - The result. 0 will be returned for 0.
******************************************************************************/
rvm_ptr_t _RVM_LSB_Generic(rvm_ptr_t Value)
{
    rvm_ptr_t Bit;
    static const rvm_u8_t Table[256U]=
    {
        0U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        4U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        5U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        4U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        6U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        4U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        5U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        4U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        7U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        4U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        5U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        4U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        6U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        4U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        5U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U,
        4U,0U,1U,0U,2U,0U,1U,0U,3U,0U,1U,0U,2U,0U,1U,0U
    };
    
#if(RVM_WORD_ORDER==4U)
    /* 16-8 */
    if((Value<<8U)==0U)
    {
        RVM_COV_MARKER();
        
        Bit=8U;
    }
    /* 7-0 */
    else
    {
        RVM_COV_MARKER();
        
        Bit=0U;
    }
#elif(RVM_WORD_ORDER==5U)
    /* 31-16 */
    if((Value<<16U)==0U)
    {
        RVM_COV_MARKER();
        
        /* 31-24 */
        if((Value<<8U)==0U)
        {
            RVM_COV_MARKER();
            
            Bit=24U;
        }
        /* 24-16 */
        else
        {
            RVM_COV_MARKER();
            
            Bit=16U;
        }
    }
    /* 15-0 */
    else
    {
        RVM_COV_MARKER();
        
        /* 15-8 */
        if((Value<<24U)==0U)
        {
            RVM_COV_MARKER();
            
            Bit=8U;
        }
        /* 7-0 */
        else
        {
            RVM_COV_MARKER();
            
            Bit=0U;
        }
    }
#elif(RVM_WORD_ORDER==6U)
    /* 63-32 */
    if((Value<<32U)==0U)
    {
        RVM_COV_MARKER();
        
        /* 63-48 */
        if((Value<<16U)==0U)
        {
            RVM_COV_MARKER();
            
            /* 63-56 */
            if((Value<<8U)==0U)
            {
                RVM_COV_MARKER();
                
                Bit=56U;
            }
            /* 56-48 */
            else
            {
                RVM_COV_MARKER();
                
                Bit=48U;
            }
        }
        /* 47-32 */
        else
        {
            RVM_COV_MARKER();
            
            /* 47-40 */
            if((Value<<24U)==0U)
            {
                RVM_COV_MARKER();
                
                Bit=40U;
            }
            /* 39-32 */
            else
            {
                RVM_COV_MARKER();
                
                Bit=32U;
            }
        }
    }
    /* 31-0 */
    else
    {
        RVM_COV_MARKER();
        
        /* 31-16 */
        if((Value<<48U)==0U)
        {
            RVM_COV_MARKER();
            
            /* 31-24 */
            if((Value<<40U)==0U)
            {
                RVM_COV_MARKER();
                
                Bit=24U;
            }
            /* 24-16 */
            else
            {
                RVM_COV_MARKER();
                
                Bit=16U;
            }
        }
        /* 15-0 */
        else
        {
            RVM_COV_MARKER();
            
            /* 15-8 */
            if((Value<<56U)==0U)
            {
                RVM_COV_MARKER();
                
                Bit=8U;
            }
            /* 7-0 */
            else
            {
                RVM_COV_MARKER();
                
                Bit=0U;
            }
        }
    }
#else
#error Generic LSB for 128-bits & above are not implemented.
#endif

    return Table[(rvm_u8_t)(Value>>Bit)]+Bit;
}
/* End Function:_RVM_LSB_Generic *********************************************/

/* Function:RVM_Clear *********************************************************
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
    {
        ((volatile rvm_u8_t*)Addr)[Count]=0U;
    }
}
/* End Function:RVM_Clear ****************************************************/

/* Function:RVM_List_Crt ******************************************************
Description : Create a doubly linkled list.
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
              rvm_ptr_t Begin - The begin of the kernel function ID.
              rvm_ptr_t End - The end of the kernel function ID, inclusive.
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
#if(RVM_PGT_RAW_ENABLE==0U)
rvm_ret_t RVM_Pgt_Crt(rvm_cid_t Cap_Cpt,
                      rvm_cid_t Cap_Kom,
                      rvm_cid_t Cap_Pgt, 
                      rvm_ptr_t Raddr,
                      rvm_ptr_t Base,
                      rvm_ptr_t Is_Top,
                      rvm_ptr_t Size_Order,
                      rvm_ptr_t Num_Order)
{
    
    return RVM_SVC(RVM_PGT_SVC(Num_Order,RVM_SVC_PGT_CRT),
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
#endif
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
              rme_ptr_t Raw_Pgt - Alternate user-supplied page table physical
                                  address.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_PGT_RAW_ENABLE==0U)
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
#else
rvm_ret_t RVM_Prc_Crt(rvm_cid_t Cap_Cpt_Crt,
                      rvm_cid_t Cap_Prc,
                      rvm_cid_t Cap_Cpt,
                      rvm_ptr_t Raw_Pgt)
{
    return RVM_SVC(RVM_SVC_PRC_CRT,
                   Cap_Cpt_Crt,
                   Cap_Prc,
                   Cap_Cpt,
                   Raw_Pgt);
}
#endif
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
              rme_ptr_t Raw_Pgt - Alternate user-supplied page table physical
                                  address.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_PGT_RAW_ENABLE==0U)
rvm_ret_t RVM_Prc_Pgt(rvm_cid_t Cap_Prc,
                      rvm_cid_t Cap_Pgt)
{
    return RVM_SVC(RVM_SVC_PRC_PGT,
                   0U,
                   Cap_Prc,
                   Cap_Pgt,
                   0U);
}
#else
rvm_ret_t RVM_Prc_Pgt(rvm_cid_t Cap_Prc,
                      rvm_ptr_t Raw_Pgt)
{
    return RVM_SVC(RVM_SVC_PRC_PGT,
                   0U,
                   Cap_Prc,
                   Raw_Pgt,
                   0U);
}
#endif
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
              void* Entry - The entry address of the thread.
              void* Stack - The stack address to use for execution.
              void* Param - The parameter to pass to the thread.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Thd_Exec_Set(rvm_cid_t Cap_Thd,
                           void* Entry,
                           void* Stack,
                           void* Param)
{
    return RVM_SVC(RVM_SVC_THD_EXEC_SET,
                   Cap_Thd,
                   (rvm_ptr_t)Entry, 
                   (rvm_ptr_t)Stack,
                   (rvm_ptr_t)Param);
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
/* End Function:RVM_Sig_Del **************************************************/

/* Function:RVM_Sig_Snd ******************************************************
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
		
    while(1U)
    {
        /* Receive the scheduler notifications for the interrupt threads. If any of
         * them failed, then we just panic, because none of them are supposed to fail. */
        /* RVM_DBG_S("I"); */
        RVM_Idle();
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
Description : Recover the currently running virtual machine.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
static void _RVM_Virt_Recover(void)
{
    struct RVM_Map_Struct* Map;
    rvm_ptr_t Init_Entry;
    rvm_ptr_t Init_Stack;
    
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
            if(Number!=RVM_HYP_INVALID)
            {
                RVM_COV_MARKER();
                
                Param=Arg->Param;
                Arg->Number=RVM_HYP_INVALID;
                
                /* RVM_DBG_SIS("Hypd: Hypercall ",Number," called.\r\n"); */
            
                switch(Number)
                {
#if(RVM_DBGLOG_ENABLE!=0U)
                    /* Debug hypercalls */
                    case RVM_HYP_PUTCHAR:
                    {
                        RVM_COV_MARKER();
                        
                        Param[0]=(rvm_ptr_t)_RVM_Hyp_Putchar(Param[0]);     /* rvm_ptr_t Char */
                        break;
                    }
#endif
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
                    /* Virtual vector hypercalls */
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
                    /* Event send hypercalls */
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
                        
                        /* Also print registers so that the user can debug. This is abnormal anyway */
                        RVM_DBG_S("Timd: Vector handling thread register set:\r\n");
                        RVM_ASSERT(RVM_Thd_Print_Reg(RVM_Virt_Cur->Map->Vct_Thd_Cap)==0);
                        RVM_DBG_S("Timd: User program thread register set:\r\n");
                        RVM_ASSERT(RVM_Thd_Print_Reg(RVM_Virt_Cur->Map->Usr_Thd_Cap)==0);

                        /* Actually reboot the virtual machine */
                        _RVM_Virt_Recover();
                        
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
    rvm_tid_t Thd_Pure;
    
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
        Thd_Pure=(rvm_tid_t)(Thd_Flag&(~RVM_THD_EXCPEND_FLAG)&(~RVM_VIRT_TID_FLAG));
        RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
        RVM_Thd_Print_Exc(Thd_Pure);

        /* We're done - this is a monitor fault, have to reboot; monitor CID/TIDs are one-level */
        if(Thd_Pure<(rvm_tid_t)RVM_CID_2L)
        {
            RVM_COV_MARKER();
            
            RVM_DBG_S("Sftd: Critical fault on RVM ");
            
            /* Safety daemon crashed - how is this ever possible, we're handling our own faults */
            if(Thd_Pure==RVM_Sftd_Thd_Cap)
            {
                RVM_COV_MARKER();
            
                RVM_DBG_S("Sftd");
            }
#if(RVM_VIRT_NUM!=0U)
            /* Virtual machine daemon crashed */
            else if(Thd_Pure==RVM_Vmmd_Thd_Cap)
            {
                RVM_COV_MARKER();
                
                RVM_DBG_S("Vmmd");
            }
#endif
            /* Some user-supplied daemon crashed */
            else
            {
                RVM_COV_MARKER();
                
                RVM_DBG_SHS("unrecognized daemon TID 0x", Thd_Pure,"");
            }
            
            RVM_DBG_S(". Rebooting system...\r\n");
            
            /* Print registers & reboot */
            RVM_ASSERT(RVM_Thd_Print_Reg(Thd_Pure)==0);
            RVM_ASSERT(0);
        }
        /* We're done - process fault; processes are considered critical */
        else if((Thd_Flag&RVM_VIRT_TID_FLAG)==0U)
        {
            RVM_COV_MARKER();
            
            RVM_DBG_SHS("Sftd: Critical fault on process thread TID 0x",Thd_Pure,". Panicking system...\r\n");
            
            /* Print registers & reboot */
            RVM_ASSERT(RVM_Thd_Print_Reg(Thd_Pure)==0);
            RVM_ASSERT(0);
        }
        /* Can recover - virtual machine fault */
        else
        {
            RVM_COV_MARKER();
            
#if(RVM_VIRT_NUM!=0U)
            RVM_DBG_S("Sftd: Recoverable fault in virtual machine ");
            RVM_DBG_S(RVM_Virt_Cur->Map->Name);

            if(Thd_Pure==RVM_Virt_Cur->Map->Vct_Thd_Cap)
            {
                RVM_COV_MARKER();
            
                RVM_DBG_S(" vector handling thread");
            }
            else if(Thd_Pure==RVM_Virt_Cur->Map->Usr_Thd_Cap)
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
            
            /* Also print registers so that the user can debug - this is abnormal anyway */
            RVM_DBG_S("Sftd: Vector handling thread register set:\r\n");
            RVM_ASSERT(RVM_Thd_Print_Reg(RVM_Virt_Cur->Map->Vct_Thd_Cap)==0);
            RVM_DBG_S("Sftd: User program thread register set:\r\n");
            RVM_ASSERT(RVM_Thd_Print_Reg(RVM_Virt_Cur->Map->Usr_Thd_Cap)==0);

            /* Reboot the current virtual machine immediately */
            _RVM_Virt_Recover();
            
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

