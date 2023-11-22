/******************************************************************************
Filename    : rvm_hyper.c
Author      : pry
Date        : 02/10/2018
Licence     : The Unlicense; see LICENSE for details.
Description : The hypercall implementation file.
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
#include "Monitor/rvm_hyper.h"

#define __HDR_PUBLIC_MEMBERS__
#include "rvm_platform.h"
#include "Monitor/rvm_syssvc.h"
#include "Monitor/rvm_init.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:_RVM_Run_Ins ************************************************
Description : Set the virtual machine as ready to schedule.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to put into 
                                             the runqueue.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void _RVM_Run_Ins(struct RVM_Virt_Struct* Virt)
{
    /* Insert this into the corresponding runqueue */
    RVM_List_Ins(&(Virt->Head), RVM_Run[Virt->Map->Prio].Prev, &(RVM_Run[Virt->Map->Prio]));
    /* Set this runlevel as active */
    RVM_Bitmap[Virt->Map->Prio>>RVM_WORD_ORDER]|=RVM_POW2(Virt->Map->Prio&RVM_WORD_MASK);
}
#endif
/* End Function:_RVM_Run_Ins *************************************************/

/* Begin Function:_RVM_Run_Del ************************************************
Description : Clear the virtual machine from the runqueue.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to delete from
                                             the runqueue.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void _RVM_Run_Del(struct RVM_Virt_Struct* Virt)
{
    /* See if it is the last thread on the priority level */
    if(Virt->Head.Prev==Virt->Head.Next)
        RVM_Bitmap[Virt->Map->Prio>>RVM_WORD_ORDER]&=~RVM_POW2(Virt->Map->Prio&RVM_WORD_MASK);
    
    /* Delete this from the corresponding runqueue */
    RVM_List_Del(Virt->Head.Prev, Virt->Head.Next);
}
#endif
/* End Function:_RVM_Run_Del *************************************************/

/* Begin Function:_RVM_Run_High ***********************************************
Description : Get the highest priority ready virtual machine available.
Input       : None.
Output      : None.
Return      : struct RVM_Virt_Struct* Virt - The virtual machine. If all virtual
                                             machine have gone asleep, this will
                                             be NULL.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
struct RVM_Virt_Struct* _RVM_Run_High(void)
{
    rvm_cnt_t Count;
    
    /* See which one is ready, and pick it */
    for(Count=RVM_VPRIO_BITMAP-1;Count>=0;Count--)
    {
        if(RVM_Bitmap[Count]==0U)
            continue;
        
        Count=(rvm_cnt_t)RVM_MSB_GET(RVM_Bitmap[Count])+(Count<<RVM_WORD_ORDER);
        return (struct RVM_Virt_Struct*)(RVM_Run[Count].Next);
    }
    
    return RVM_NULL;
}
#endif
/* End Function:_RVM_Run_High ************************************************/

/* Begin Function:_RVM_Virt_Switch ********************************************
Description : Switch between two virtual machines.
Input       : struct RVM_Virt_Struct* From - The source VM.
              struct RVM_Virt_Struct* To - The destination VM.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void _RVM_Virt_Switch(struct RVM_Virt_Struct* From, 
                      struct RVM_Virt_Struct* To)
{
    if(From==To)
        return;
    
    /* Just change the thread's priorities - use 2x version to minimize delay */
    if(From!=RVM_NULL)
    {
        RVM_ASSERT(RVM_Thd_Sched_Prio2(From->Map->Usr_Thd_Cap, RVM_WAIT_PRIO,
                                       From->Map->Vct_Thd_Cap, RVM_WAIT_PRIO)==0);
    }
    
    if(To!=RVM_NULL)
    {
        RVM_ASSERT(RVM_Thd_Sched_Prio2(To->Map->Usr_Thd_Cap, RVM_VUSR_PRIO,
                                       To->Map->Vct_Thd_Cap, RVM_VVCT_PRIO)==0);
    }
}
#endif
/* End Function:_RVM_Virt_Switch *********************************************/

/* Begin Function:_RVM_Vct_Pend_Check *****************************************
Description : Check if there is one pending vector for the virtual machine.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to check.
Output      : None.
Return      : If there is one interrupt pending, return 1; else 0.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t _RVM_Vct_Pend_Check(struct RVM_Virt_Struct* Virt)
{
    rvm_ptr_t Count;
    volatile struct RVM_Vctf* Flag;
    
    Flag=&Virt->Map->State_Base->Flag;
    
    if(Flag->Tim!=0U)
        return 1;
    
    if(Flag->Ctx!=0U)
        return 1;
    
    for(Count=0U;Count<Virt->Map->Vct_Num;Count++)
    {
        if(Flag->Vct[Count]!=0U)
            return 1;
    }
    
    return 0;
}
#endif
/* End Function:_RVM_Vct_Pend_Check ******************************************/

/* Begin Function:_RVM_Vct_Flag_Set *******************************************
Description : Set an interrupt's flag for the virtual machine.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to set flag for.
              rvm_ptr_t Vct_Num - The vector number to set flag for.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void _RVM_Vct_Flag_Set(struct RVM_Virt_Struct* Virt,
                       rvm_ptr_t Vct_Num)
{
    Virt->Map->State_Base->Flag.Vct[Vct_Num]=1U;
}
#endif
/* End Function:_RVM_Vct_Flag_Set *******************************************/

/* Begin Function:RVM_Hyp_Putchar *********************************************
Description : Print one character to the RVM debug console.
Input       : rvm_ptr_t Char - The character.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if((RVM_PREEMPT_VPRIO_NUM!=0U)&&(RVM_DEBUG_PRINT==1U))
rvm_ret_t RVM_Hyp_Putchar(rvm_ptr_t Char)
{
    RVM_Putchar((char)Char);
    return 0;
}
#endif
/* End Function:RVM_Hyp_Putchar **********************************************/

/* Begin Function:RVM_Hyp_Int_Ena *********************************************
Description : Enable interrupts for a virtual machine. Need to call this when
              the virtual machine fave finished all its initialization routines
              or it wouldn't be able to receive interrupts.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Int_Ena(void)
{
    if((RVM_Virt_Cur->Sched.State&RVM_VM_INTENA)!=0U)
        return RVM_ERR_STATE;
    else
    {
        /* If the virtual machine ever calls this, then it means that it have done booting */
        if((RVM_Virt_Cur->Sched.State&RVM_VM_BOOTED)==0U)
        {
            RVM_Virt_Cur->Sched.State|=RVM_VM_BOOTED;
            RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_Virt_Cur->Map->Vct_Thd_Cap, RVM_VVCT_PRIO)==0);
        }
        RVM_Virt_Cur->Sched.State|=RVM_VM_INTENA;
        /* See if we do have excess interrupt to process. If yes, send to the endpoint now */
        if(_RVM_Vct_Pend_Check(RVM_Virt_Cur)!=0)
            RVM_ASSERT(RVM_Sig_Snd(RVM_Virt_Cur->Map->Vct_Sig_Cap)==0);
    }

    return 0;
}
#endif
/* End Function:RVM_Hyp_Int_Ena **********************************************/

/* Begin Function:RVM_Hyp_Int_Dis *********************************************
Description : Disable the interrupt for the virtual machine. All interrupts for a
              virtual machine, including the tick timer interrupt,is disabled on
              startup.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Int_Dis(void)
{
    if((RVM_Virt_Cur->Sched.State&RVM_VM_INTENA)==0)
        return RVM_ERR_STATE;
    else
        RVM_Virt_Cur->Sched.State&=~RVM_VM_INTENA;

    return 0;
}
#endif
/* End Function:RVM_Hyp_Int_Dis **********************************************/

/* Begin Function:RVM_Hyp_Vct_Phys ********************************************
Description : Register the virtual machine's virtual vector with a physical vector.
Input       : rvm_ptr_t Phys_Num - The physical vector number.
              rvm_ptr_t Vct_Num - The virtual vector number.
Output      : None.
Return      : rvm_ret_t - If successful, the interrupt registration ID; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Vct_Phys(rvm_ptr_t Phys_Num,
                           rvm_ptr_t Vct_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if both numbers are overrange */
    if((Phys_Num>=RVM_PHYS_VCT_NUM)||(Vct_Num>=(RVM_Virt_Cur->Map->Vct_Num)))
        return RVM_ERR_RANGE;

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0)
        return RVM_ERR_STATE;

    /* Check if the VM have registered for this physical vector before */
    Trav=RVM_Phys[Phys_Num].Next;
    while(Trav!=&RVM_Phys[Phys_Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        
        if(Map->Virt==RVM_Virt_Cur)
            return RVM_ERR_PHYS;
        
        Trav=Trav->Next;
    }
    
    /* Check if the VM's vector already have something mapped */
    Trav=RVM_Virt_Cur->Vct_Head.Next;
    while(Trav!=&(RVM_Virt_Cur->Vct_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vct_Num==Vct_Num)
            return RVM_ERR_VIRT;

        Trav=Trav->Next;
    }
    
    /* See if there are empty interrupt registration blocks available */
    if(RVM_Map_Free.Next==&RVM_Map_Free)
        return RVM_ERR_MAP;
    
    /* Register it */
    Map=(struct RVM_Map_Struct*)(RVM_Map_Free.Next);
    
    RVM_List_Del(Map->Src_Head.Prev,Map->Src_Head.Next);
    RVM_List_Ins(&(Map->Src_Head),RVM_Phys[Phys_Num].Prev, &RVM_Phys[Phys_Num]);
    RVM_List_Ins(&(Map->Dst_Head),RVM_Virt_Cur->Vct_Head.Prev, &(RVM_Virt_Cur->Vct_Head));
    
    Map->Virt=RVM_Virt_Cur;
    Map->Vct_Num=Vct_Num;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Vct_Phys *********************************************/

/* Begin Function:RVM_Hyp_Vct_Evt *********************************************
Description : Register the virtual machine's virtual vector with an event channel.
Input       : rvm_ptr_t Evt_Num - The event number.
              rvm_ptr_t Vct_Num - The virtual vector number.
Output      : None.
Return      : rvm_ret_t - If successful, the event channel ID; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Vct_Evt(rvm_ptr_t Evt_Num,
                          rvm_ptr_t Vct_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if both numbers are overrange */
    if((Evt_Num>=RVM_VIRT_EVT_NUM)||(Vct_Num>=(RVM_Virt_Cur->Map->Vct_Num)))
        return RVM_ERR_RANGE;

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0)
        return RVM_ERR_STATE;

    /* Check if the VM have registered for this event channel before */
    Trav=RVM_Evt[Evt_Num].Next;
    while(Trav!=&RVM_Evt[Evt_Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        
        if(Map->Virt==RVM_Virt_Cur)
            return RVM_ERR_EVT;
        
        Trav=Trav->Next;
    }
    
    /* Check if the VM's vector already have something mapped */
    Trav=RVM_Virt_Cur->Vct_Head.Next;
    while(Trav!=&(RVM_Virt_Cur->Vct_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vct_Num==Vct_Num)
            return RVM_ERR_VIRT;

        Trav=Trav->Next;
    }
    
    /* See if there are empty interrupt registration blocks available */
    if(RVM_Map_Free.Next==&RVM_Map_Free)
        return RVM_ERR_MAP;
    
    /* Register it */
    Map=(struct RVM_Map_Struct*)(RVM_Map_Free.Next);
    
    RVM_List_Del(Map->Src_Head.Prev,Map->Src_Head.Next);
    RVM_List_Ins(&(Map->Src_Head),RVM_Evt[Evt_Num].Prev, &RVM_Evt[Evt_Num]);
    RVM_List_Ins(&(Map->Dst_Head),RVM_Virt_Cur->Vct_Head.Prev, &(RVM_Virt_Cur->Vct_Head));
    
    Map->Virt=RVM_Virt_Cur;
    Map->Vct_Num=Vct_Num;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Vct_Evt **********************************************/

/* Begin Function:RVM_Hyp_Vct_Del *********************************************
Description : Deregister the vector of an virtual machine.
Input       : rvm_ptr_t Vct_Num - The virtual vector to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Vct_Del(rvm_ptr_t Vct_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if the number is overrange */
    if(Vct_Num>=RVM_Virt_Cur->Map->Vct_Num)
        return RVM_ERR_RANGE;

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0)
        return RVM_ERR_STATE;

    /* Is it registered to anything? */
    Trav=RVM_Virt_Cur->Vct_Head.Next;
    while(Trav!=&(RVM_Virt_Cur->Vct_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vct_Num==Vct_Num)
        {
            /* Found it. Deregister this from the database and put it back to free */
            RVM_List_Del(Map->Src_Head.Prev,Map->Src_Head.Next);
            RVM_List_Del(Map->Dst_Head.Prev,Map->Dst_Head.Next);
            RVM_List_Ins(&(Map->Src_Head),RVM_Map_Free.Prev, &RVM_Map_Free);
            return 0;
        }

        Trav=Trav->Next;
    }
    
    /* Nothing found */
    return RVM_ERR_VIRT;
}
#endif
/* End Function:RVM_Hyp_Vct_Del **********************************************/

/* Begin Function:RVM_Hyp_Vct_Lck *********************************************
Description : Lockdown the vector mapping of an virtual machine.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Vct_Lck(void)
{
    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
        return RVM_ERR_STATE;
    
    /* Lock down mappings */
    RVM_Virt_Cur->Sched.State|=RVM_VM_VCTLCK;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Vct_Lck **********************************************/

/* Begin Function:RVM_Hyp_Vct_Wait ********************************************
Description : Set the virtual machine to sleep until a vector comes in.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Vct_Wait(void)
{
    /* See if it have interrupt disabled */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_INTENA)==0U)
        return RVM_ERR_STATE;

    /* Now suspend the virtual machine and switch to somebody else */
    RVM_VM_STATE_SET(RVM_Virt_Cur->Sched.State,RVM_VM_WAITING);
    _RVM_Run_Del(RVM_Virt_Cur);
    
    /* Need a context switch, someone went to sleep */
    RVM_Switch=1U;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Vct_Wait *********************************************/

/* Begin Function:RVM_Hyp_Evt_Add *********************************************
Description : Add a event source's send capability to virtual machine.
Input       : rvm_ptr_t Evt_Num - The event souce to register.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Evt_Add(rvm_ptr_t Evt_Num)
{
    rvm_ptr_t* Slot;
    rvm_ptr_t Value;
    
    /* See if both numbers are overrange */
    if(Evt_Num>=RVM_VIRT_EVT_NUM)
        return RVM_ERR_RANGE;

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
        return RVM_ERR_STATE;
    
    /* Is this virtual machine already registered on this? */
    Slot=&(RVM_Virt_Cur->Evt_Cap[Evt_Num>>RVM_WORD_ORDER]);
    Value=RVM_POW2(Evt_Num&RVM_MASK_END(RVM_WORD_ORDER-1U));
    
    /* We have registered, no need to register again */
    if((Slot[0]&Value)!=0U)
        return RVM_ERR_EVT;
    
    /* Register this */
    Slot[0]|=Value;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Evt_Add **********************************************/

/* Begin Function:RVM_Hyp_Evt_Del *********************************************
Description : Delete a event source's send capability from virtual machine.
Input       : rvm_ptr_t Evt_Num - The event souce to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Evt_Del(rvm_ptr_t Evt_Num)
{
    rvm_ptr_t* Slot;
    rvm_ptr_t Value;
    
    /* See if both numbers are overrange */
    if(Evt_Num>=RVM_VIRT_EVT_NUM)
        return RVM_ERR_RANGE;

    /* See if we have locked ourselves down */
    if((RVM_Virt_Cur->Sched.State&RVM_VM_VCTLCK)!=0U)
        return RVM_ERR_STATE;

    /* Is this virtual machine already registered on this? */
    Slot=&(RVM_Virt_Cur->Evt_Cap[Evt_Num>>RVM_WORD_ORDER]);
    Value=RVM_POW2(Evt_Num&RVM_MASK_END(RVM_WORD_ORDER-1U));
    
    /* We have deregistered, no need to deregister again */
    if((Slot[0]&Value)==0U)
        return RVM_ERR_EVT;
    
    /* Deregister this */
    Slot[0]&=~Value;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Evt_Del **********************************************/

/* Begin Function:_RVM_Virt_Vct_Snd *******************************************
Description : Send to all virtual machine vectors registered on this physical 
              physical interrupt channel or event.
Input       : struct RVM_List* Array - The array containing lists of registered
                                       virtual vectors.
              rvm_ptr_t Num - The physical vector ID or event ID.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void _RVM_Virt_Vct_Snd(struct RVM_List* Array,
                       rvm_ptr_t Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    struct RVM_Virt_Struct* Virt;
    
    /* Is it registered to anything? */
    Trav=Array[Num].Next;
    while(Trav!=&Array[Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        Virt=Map->Virt;
        
        /* Send to everyone on that list */
        _RVM_Vct_Flag_Set(Virt, Map->Vct_Num);
        RVM_ASSERT(RVM_Sig_Snd(Virt->Map->Vct_Sig_Cap)==0);
        /* If it is waiting, notify it of new interrupts */
        if(RVM_VM_STATE(Virt->Sched.State)==RVM_VM_WAITING)
        {
            RVM_VM_STATE_SET(RVM_Virt_Cur->Sched.State,RVM_VM_RUNNING);
            _RVM_Run_Ins(Virt);
        }
        
        Trav=Trav->Next;
    }

    /* May need a context switch, someone received an interrupt */
    RVM_Switch=1U;
}
#endif
/* End Function:_RVM_Virt_Vct_Snd ********************************************/

/* Begin Function:RVM_Hyp_Evt_Snd *********************************************
Description : Send an event to the event channel.
Input       : rvm_ptr_t Evt_Num - The event channel ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Evt_Snd(rvm_ptr_t Evt_Num)
{
    rvm_ptr_t Value;
    rvm_ptr_t* Slot;
    
    /* See if the number is overrange */
    if(Evt_Num>=RVM_VIRT_EVT_NUM)
        return RVM_ERR_RANGE;
    
    /* Are we allowed to send to this at all? */
    Slot=&(RVM_Virt_Cur->Evt_Cap[Evt_Num>>RVM_WORD_ORDER]);
    Value=RVM_POW2(Evt_Num&RVM_MASK_END(RVM_WORD_ORDER-1));
    
    /* We are not allowed to send to this at all */
    if((Slot[0]&Value)==0U)
        return RVM_ERR_EVT;
    
    /* Send to that event */
    _RVM_Virt_Vct_Snd(RVM_Evt, Evt_Num);
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Evt_Snd **********************************************/

/* Begin Function:RVM_Hyp_Wdg_Clr *********************************************
Description : Start and feed the watchdog for the current virtual machine.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ret_t RVM_Hyp_Wdg_Clr(void)
{   
    /* See if this virtual machine have watchdog enabled */
    if(RVM_Virt_Cur->Map->Watchdog==0)
        return RVM_ERR_STATE;
    
    /* Set state and reset watchdog counter */
    RVM_Virt_Cur->Sched.State|=RVM_VM_WDGENA;
    RVM_Virt_Cur->Sched.Watchdog_Left=RVM_Virt_Cur->Map->Watchdog;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Wdg_Clr **********************************************/

/* Begin Function:_RVM_Wheel_Ins **********************************************
Description : Insert one VM into the timer wheel.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine to insert.
              rvm_ptr_t Period - The period of the timer interrupt.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void _RVM_Wheel_Ins(struct RVM_Virt_Struct* Virt,
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
            break;
        
        Trav_Ptr=Trav_Ptr->Next;
    }

    /* Insert this into the list */
    Virt->Sched.Period_Timeout=RVM_Tick+Period;
    RVM_List_Ins(&(Virt->Delay),Trav_Ptr->Prev,Trav_Ptr);
}
#endif
/* End Function:_RVM_Wheel_Ins ***********************************************/

/* Begin Function:_RVM_Tim_Snd ************************************************
Description : Send an timer interrupt to a virtual machine.
Input       : struct RVM_Virt_Struct* Virt - The pointer to the virtual machine.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void _RVM_Tim_Snd(struct RVM_Virt_Struct* Virt)
{
    if(RVM_VM_STATE(Virt->Sched.State)==RVM_VM_WAITING)
    {
        _RVM_Run_Ins(Virt);
        RVM_VM_STATE_SET(Virt->Sched.State,RVM_VM_RUNNING);
    }
    
    /* Set the timer bit */
    Virt->Map->State_Base->Flag.Tim=1U;
    
    /* Send interrupt to it, if its interrupts are enabled */
    if((Virt->Sched.State&RVM_VM_INTENA)!=0)
        RVM_ASSERT(RVM_Sig_Snd(Virt->Map->Vct_Sig_Cap)==0);

    /* Context switch needed, someone received a timer interrupt */
    RVM_Switch=1U;
}
#endif
/* End Function:_RVM_Tim_Snd *************************************************/

/* Begin Function:_RVM_Virt_Cur_Recover ***************************************
Description : Recover the currently running virtual machine.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void _RVM_Virt_Cur_Recover(void)
{
    struct RVM_Map_Struct* Map;
    rvm_ptr_t Init_Stack_Addr;
    
    /* Unbind the threads from the core then rebind in case they are still blocked. 
     * This can break the assumption that the vector thread shall run first */
    RVM_ASSERT(RVM_Thd_Sched_Free(RVM_Virt_Cur->Map->Vct_Thd_Cap)==0);
    RVM_ASSERT(RVM_Thd_Sched_Free(RVM_Virt_Cur->Map->Usr_Thd_Cap)==0);
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Virt_Cur->Map->Vct_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, 
                                  RVM_Virt_Cur->Map->Vct_Thd_Cap|RVM_VIRT_TID_MARKER, RVM_VVCT_PRIO)==0);
    RVM_ASSERT(RVM_Hyp_Sched_Bind(RVM_Virt_Cur->Map->Usr_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, 
                                  RVM_Virt_Cur->Map->Usr_Thd_Cap|RVM_VIRT_TID_MARKER, RVM_VUSR_PRIO, 
                                  (rvm_ptr_t)(RVM_Virt_Cur->Map->Reg_Base))==0);
    
    /* Set the execution properties for virt @ position 0 */
    Init_Stack_Addr=RVM_Stack_Init(RVM_Virt_Cur->Map->Vct_Stack_Base, RVM_Virt_Cur->Map->Vct_Stack_Size,
                                   RVM_DESC_ENTRY(RVM_Virt_Cur->Map->Desc_Base,0U),
                                   RVM_DESC_STUB(RVM_Virt_Cur->Map->Desc_Base));
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Virt_Cur->Map->Vct_Thd_Cap, 
                                RVM_DESC_ENTRY(RVM_Virt_Cur->Map->Desc_Base,0U), Init_Stack_Addr, 0)==0);
    
    /* Set the execution properties for user @ position 1 */
    Init_Stack_Addr=RVM_Stack_Init(RVM_Virt_Cur->Map->Usr_Stack_Base, RVM_Virt_Cur->Map->Usr_Stack_Size,
                                   RVM_DESC_ENTRY(RVM_Virt_Cur->Map->Desc_Base,1U),
                                   RVM_DESC_STUB(RVM_Virt_Cur->Map->Desc_Base));
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Virt_Cur->Map->Usr_Thd_Cap, 
                                RVM_DESC_ENTRY(RVM_Virt_Cur->Map->Desc_Base,1U), Init_Stack_Addr ,0)==0);
    
    /* Delegate infinite time to both threads */
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Virt_Cur->Map->Vct_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Virt_Cur->Map->Usr_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    
    /* Clear all its interrupt flags */
    RVM_Clear(RVM_Virt_Cur->Map->State_Base, RVM_Virt_Cur->Map->State_Size);
    
    /* Clear all event capabilities */
    RVM_Clear(RVM_Virt_Cur->Evt_Cap, RVM_EVTCAP_BITMAP*sizeof(rvm_ptr_t));

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
    RVM_List_Del(RVM_Virt_Cur->Delay.Prev, RVM_Virt_Cur->Delay.Next);
    _RVM_Wheel_Ins(RVM_Virt_Cur,RVM_Virt_Cur->Map->Period);

    /* Context switch needed, someone is rebooted */
    RVM_Switch=1U;
}
#endif
/* End Function:_RVM_Virt_Cur_Recover ****************************************/

/* Begin Function:RVM_Sftd ****************************************************
Description : The safety daemon against system partial or total failures.
              This runs as a separate thread.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Sftd(void)
{
    rvm_tid_t Thd;
    
    RVM_DBG_S("Sftd: Safety guard daemon initialization complete.\r\n");

    /* Main cycle - recover faults if possible */
    while(1)
    {
        /* Blocking single receive */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_Sftd_Sig_Cap, RVM_RCV_BS)>=0);
        
        Thd=RVM_Thd_Sched_Rcv(RVM_Sftd_Thd_Cap);
        
        /* Is an error a fault? If not, then we must have done something wrong - This is really bad */
        if(((Thd&RVM_THD_FAULT_FLAG)==0)||(Thd<0))
        {
            /* Hang the machine because this error is unrecoverable */
            RVM_DBG_SHS("Sftd: Intangible fault on return value 0x", (rvm_ptr_t)Thd, ". Shutting down system...\r\n");
            RVM_ASSERT(0);
        }
        
        /* See what thread faulted. If it is any of the process threads, then the system is done */
        Thd&=~RVM_THD_FAULT_FLAG;
        
        /* Print the reason of the fault */
        RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
        RVM_Thd_Print_Exc(Thd&~RVM_VIRT_TID_MARKER);

        /* Whatever thread faults, we will need to print their registers, so we can figure out
         * what the fault is. There should be a print somewhere, but shouldn't be in the kernel. */
        if(Thd<RVM_CID_2L)
        {
            /* We know that this happened within RVM itself. Sad, cannot recover */
            RVM_DBG_S("Sftd: Irrecoverable fault on RVM ");
            if(Thd==RVM_Sftd_Thd_Cap)
                RVM_DBG_S("Sftd");
#if(RVM_VIRT_NUM!=0U)
            else if(Thd==RVM_Vmmd_Thd_Cap)
                RVM_DBG_S("Vmmd");
#endif
            else
                RVM_DBG_SHS("unrecognized thread TID 0x", Thd,"");
            
            RVM_DBG_S(". Rebooting system...\r\n");
            
            /* Print registers so we know how it crashed */
            RVM_ASSERT(RVM_Thd_Print_Reg(Thd)==0);
            /* Reboot */
            RVM_ASSERT(0);
        }
        else if((Thd&RVM_VIRT_TID_MARKER)==0U)
        {
            /* We know that this happened in a process. Still, cannot be recovered. */
            RVM_DBG_SHS("Sftd: Irrecoverable fault on process thread TID 0x", Thd, ". Rebooting system...\r\n");
            
            /* Print registers so we know how it crashed */
            RVM_ASSERT(RVM_Thd_Print_Reg(Thd)==0);
            
            /* Reboot */
            RVM_ASSERT(0);
        }
        else
        {
#if(RVM_VIRT_NUM!=0U)
            Thd&=~RVM_VIRT_TID_MARKER;
            /* This must have happened in the current virtual machine */
            RVM_DBG_S("Sftd: Recoverable fault in virtual machine ");
            RVM_DBG_S(RVM_Virt_Cur->Map->Name);

            if(Thd==RVM_Virt_Cur->Map->Vct_Thd_Cap)
                RVM_DBG_S(" vector handling thread");
            else if(Thd==RVM_Virt_Cur->Map->Usr_Thd_Cap)
                RVM_DBG_S(" user program thread");
            else
                RVM_DBG_S(" unknown thread");

            RVM_DBG_S(". Recovering...\r\n");
            
            /* Also print registers so that the user can debug. This is abnormal anyway */
            RVM_DBG_S("Sftd: Vector handling thread register set:\r\n");
            RVM_ASSERT(RVM_Thd_Print_Reg(RVM_Virt_Cur->Map->Vct_Thd_Cap)==0);
            RVM_DBG_S("Sftd: User program thread register set:\r\n");
            RVM_ASSERT(RVM_Thd_Print_Reg(RVM_Virt_Cur->Map->Usr_Thd_Cap)==0);

            /* Actually reboot the virtual machine */
            _RVM_Virt_Cur_Recover();
            
            /* Trigger the context switch ASAP */
            RVM_ASSERT(RVM_Sig_Snd(RVM_BOOT_INIT_VCT)==0);
            
            RVM_DBG_S("Sftd: Recovered.\r\n");
#else
            RVM_DBG_S("Sftd: Fault on virtual machine but no virtual machine exists. Rebooting system...\r\n");
            RVM_ASSERT(0);
#endif
        }
    }
}
/* End Function:RVM_Sftd *****************************************************/

/* Begin Function:_RVM_Flagset_Get ********************************************
Description : Get the physical vector source or event source from the interrupt
              set. When this is called, there must be at least one vector or
              event pending in this set.
Input       : volatile struct RVM_Flag* Set - The interrupt set data.
Output      : volatile struct RVM_Flag* Set - The interrupt set data.
Return      : rvm_ptr_t - The interrupt number.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
rvm_ptr_t _RVM_Flagset_Get(volatile struct RVM_Flag* Set)
{
    rvm_ptr_t Group;
    rvm_ptr_t Member;
    
    /* See which source group could possibly have any interrupts */
    Group=RVM_MSB_GET(Set->Group);
    Member=RVM_MSB_GET(Set->Flag[Group]);
    
    /* Clean up the slot now */
    Set->Flag[Group]&=~RVM_POW2(Member);
    if(Set->Flag[Group]==0U)
        Set->Group&=~RVM_POW2(Group);
    
    return (Group<<RVM_WORD_ORDER)|Member;
}
#endif
/* End Function:_RVM_Flagset_Get *********************************************/

/* Begin Function:RVM_Vmmd ****************************************************
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
#if(RVM_VIRT_NUM!=0U)
void RVM_Vmmd(void)
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
    Vctf_Set0=RVM_FLAG_SET(RVM_PHYS_VCTF_BASE, RVM_PHYS_VCTF_SIZE, 0U);
    Vctf_Set1=RVM_FLAG_SET(RVM_PHYS_VCTF_BASE, RVM_PHYS_VCTF_SIZE, 1U);
    Evtf_Set0=RVM_FLAG_SET(RVM_VIRT_EVTF_BASE, RVM_VIRT_EVTF_SIZE, 0U);
    Evtf_Set1=RVM_FLAG_SET(RVM_VIRT_EVTF_BASE, RVM_VIRT_EVTF_SIZE, 1U);

    while(1U)
    {
        /* Blocking multi receive, wait for activation signal */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_BOOT_INIT_VCT, RVM_RCV_BM)>=0);
        
/* Hypd daemon for hypercall handling ****************************************/
        /* See if there is an active VM to handle hypercalls */
        if(RVM_Virt_Cur!=RVM_NULL)
        {
            /* See if the vector is active */
            State=RVM_Virt_Cur->Map->State_Base;
            if(State->Vct_Act!=0U)
                Arg=&(State->Vct);
            else
                Arg=&(State->Usr);

            /* Extract number and parameters - and see if such call is valid */
            Number=Arg->Number;
            if(Number!=RVM_HYP_INVALID)
            {
                Param=Arg->Param;
                Arg->Number=RVM_HYP_INVALID;
                
                /* RVM_DBG_SIS("Hypd: Hypercall ",Number," called.\r\n"); */
            
                switch(Number)
                {
#if(RVM_DEBUG_PRINT==1U)
                    /* Debug hypercalls */
                    case RVM_HYP_PUTCHAR:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Putchar(Param[0]);      /* rvm_ptr_t Char */
                        break;
                    }
#endif
                    /* Interrupt hypercalls */
                    case RVM_HYP_INT_ENA:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Int_Ena();
                        break;
                    }
                    case RVM_HYP_INT_DIS:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Int_Dis();
                        break;
                    }
                    /* Virtual vector hypercalls */
                    case RVM_HYP_VCT_PHYS:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Vct_Phys(Param[0],      /* rvm_ptr_t Phys_Num */
                                                             Param[1]);     /* rvm_ptr_t Vct_Num */
                        break;
                    }
                    case RVM_HYP_VCT_EVT:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Vct_Evt(Param[0],       /* rvm_ptr_t Evt_Num */
                                                            Param[1]);      /* rvm_ptr_t Vct_Num */
                        break;
                    }
                    case RVM_HYP_VCT_DEL:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Vct_Del(Param[0]);      /* rvm_ptr_t Vct_Num */
                        break;
                    }
                    case RVM_HYP_VCT_LCK:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Vct_Lck();
                        break;
                    }
                    case RVM_HYP_VCT_WAIT:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Vct_Wait();
                        break;
                    }
                    /* Event send hypercalls */
                    case RVM_HYP_EVT_ADD:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Evt_Add(Param[0]);      /* rvm_ptr_t Evt_Num */
                        break;
                    }
                    case RVM_HYP_EVT_DEL:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Evt_Del(Param[0]);      /* rvm_ptr_t Evt_Num */
                        break;
                    }
                    case RVM_HYP_EVT_SND:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Evt_Snd(Param[0]);      /* rvm_ptr_t Evt_Num */
                        break;
                    }
                    /* Watchdog hypercall */
                    case RVM_HYP_WDG_CLR:
                    {
                        Param[0]=(rvm_ptr_t)RVM_Hyp_Wdg_Clr();
                        break;
                    }

                    default:break;
                }
                    
                /* RVM_DBG_SIS("Hypd: Hypercall ",Number," processed.\r\n"); */
            }
        }
        
/* Vctd daemon for interrupt relaying ****************************************/
        /* Clear tick detector - this is for Timd, not Vctd */
        Tick=0U;

        /* Vector set 0 */
        Vctf_Set0->Lock=1U;
        /* Detect timer interrupt */
        if(Vctf_Set0->Fast!=0U)
        {
            Vctf_Set0->Fast=0U;
            Tick=1U;
        }
        /* Detect the rest */
        while(Vctf_Set0->Group!=0U)
        {
            /* Process the interrupts in the first group one by one */
            Number=_RVM_Flagset_Get(Vctf_Set0);
            /* Only send if smaller than the number of physical vectors declared */
            if(Number<RVM_PHYS_VCT_NUM)
                _RVM_Virt_Vct_Snd(RVM_Phys, Number);
        }
        Vctf_Set0->Lock=0U;
        
        /* Vector set 1 */
        Vctf_Set1->Lock=1U;
        if(Vctf_Set1->Fast!=0U)
        {
            Vctf_Set1->Fast=0U;
            Tick=1U;
        }
        while(Vctf_Set1->Group!=0U)
        {
            /* Process the interrupts in the first group one by one */
            Number=_RVM_Flagset_Get(Vctf_Set1);
            /* Only send if smaller than the number of physical vectors declared */
            if(Number<RVM_PHYS_VCT_NUM)
                _RVM_Virt_Vct_Snd(RVM_Phys, Number);
        }
        Vctf_Set1->Lock=0U;
        
        /* Event set 0 */
        Evtf_Set0->Lock=1U;
        while(Evtf_Set0->Group!=0U)
        {
            /* Process the interrupts in the first group one by one */
            Number=_RVM_Flagset_Get(Evtf_Set0);
            /* Only send if smaller than the number of events declared */
            if(Number<RVM_VIRT_EVT_NUM)
                _RVM_Virt_Vct_Snd(RVM_Evt, Number);
        }
        Evtf_Set0->Lock=0U;
        
        /* Event set 1 */
        Evtf_Set1->Lock=1U;
        while(Evtf_Set1->Group!=0U)
        {
            /* Process the interrupts in the first group one by one */
            Number=_RVM_Flagset_Get(Evtf_Set1);
            /* Only send if smaller than the number of events declared */
            if(Number<RVM_VIRT_EVT_NUM)
                _RVM_Virt_Vct_Snd(RVM_Evt, Number);
        }
        Evtf_Set1->Lock=0U;
        
/* Timd daemon for time tracking *********************************************/
        /* See if there is a timer tick */
        if(Tick!=0U)
        {
            RVM_Tick++;
            
            /* Notify daemon passes if the debugging output is enabled. This
             * should not pop-up when doing ISR-related coverage or performance
             * tests; 100k ticks are long enough for a complete run. */
#if(RVM_DEBUG_PRINT==1U)
            if((RVM_Tick%100000U)==0U)
                RVM_DBG_S("Timd: 100k ticks passed.\r\n");
#endif
            
            /* See if we need to process the timer wheel to deliver
             * timer interrupts to virtual machines */
            Slot=&(RVM_Wheel[RVM_Tick%RVM_WHEEL_SIZE]);
            Trav=Slot->Next;
            while(Trav!=Slot)
            {
                Virt=RVM_DLY2VM(Trav);
                Trav=Trav->Next;
                /* If the value is more than this, then it means that the time have
                 * already passed and we have to process this */
                if((RVM_Tick-Virt->Sched.Period_Timeout)>=(RVM_ALLBITS>>1))
                    break;
                
                /* This VM should be processed, place it at the new position */
                RVM_List_Del(Virt->Delay.Prev, Virt->Delay.Next);
                _RVM_Wheel_Ins(Virt, Virt->Map->Period);
                
                /* Send special timer interrupt to it */
                _RVM_Tim_Snd(Virt);
            }
            
            /* If there is at least one virtual machine working, check slices and watchdog */
            if(RVM_Virt_Cur!=RVM_NULL)
            {
                /* Is the timeslices exhausted? */
                if(RVM_Virt_Cur->Sched.Slice_Left==0U)   
                {
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
                    RVM_Virt_Cur->Sched.Slice_Left--;
                
                /* Is the watchdog enabled for this virtual machine? */
                if((RVM_Virt_Cur->Sched.State&RVM_VM_WDGENA)!=0U)
                {
                    /* Is the watchdog timeout? */
                    if(RVM_Virt_Cur->Sched.Watchdog_Left==0U)   
                    {
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
                        _RVM_Virt_Cur_Recover();
                        
                        RVM_DBG_S("Timd: Recovered.\r\n");
                    }
                    else
                        RVM_Virt_Cur->Sched.Watchdog_Left--;
                }
            }
        }
    
        /* Check whether a reschedule is necessary */
        if(RVM_Switch!=0U)
        {
            Virt=_RVM_Run_High();
            
            /* It doesn't matter here whether the current or the old virtual machine is
             * RVM_NULL. If one of them is RVM_NULL, it won't equal, and if both is RVM_NULL,
             * we're not doing anything anyway */
            if(Virt!=RVM_Virt_Cur)
            {
                _RVM_Virt_Switch(RVM_Virt_Cur, Virt);
                RVM_Virt_Cur=Virt;
            }
        }
    }
}
#endif
/* End Function:RVM_Vmmd *****************************************************/

/* Begin Function:RVM_Virt_Init ***********************************************
Description : Initialize necessary virtual machine monitor database.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void RVM_Virt_Init(void)
{
    rvm_ptr_t Count;
    
    RVM_Switch=0U;
    
    RVM_Tick=0U;
    for(Count=0U;Count<RVM_WHEEL_SIZE;Count++)
        RVM_List_Crt(&RVM_Wheel[Count]);
    
    RVM_Virt_Cur=0U;
    for(Count=0U;Count<RVM_VPRIO_BITMAP;Count++)
        RVM_Bitmap[Count]=0U;
    for(Count=0U;Count<RVM_PREEMPT_VPRIO_NUM;Count++)
        RVM_List_Crt(&RVM_Run[Count]);
    
    for(Count=0U;Count<RVM_VIRT_EVT_NUM;Count++)
        RVM_List_Crt(&RVM_Evt[Count]);
    for(Count=0U;Count<RVM_PHYS_VCT_NUM;Count++)
        RVM_List_Crt(&RVM_Phys[Count]);
    
    RVM_List_Crt(&RVM_Map_Free);
    for(Count=0U;Count<RVM_VIRT_MAP_NUM;Count++)
        RVM_List_Ins(&RVM_Map[Count].Dst_Head, RVM_Map_Free.Prev, &RVM_Map_Free);
    
    /* Clean up physical vector flags and event flags */
    RVM_Clear((void*)RVM_PHYS_VCTF_BASE, RVM_PHYS_VCTF_SIZE);
    RVM_Clear((void*)RVM_VIRT_EVTF_BASE, RVM_VIRT_EVTF_SIZE);
}
#endif
/* End Function:RVM_Virt_Init ************************************************/

/* Begin Function:RVM_Virt_Crt ************************************************
Description : Set up all virtual machines according to the database.
Input       : struct RVM_Virt_Struct* Virt - The virtual machine database.
              const struct RVM_Vmap_Struct* Vmap - The virtual machine static memory map.
              rvm_ptr_t Virt_Num - The number of virtual machines in the system.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_NUM!=0U)
void RVM_Virt_Crt(struct RVM_Virt_Struct* Virt,
                  const struct RVM_Vmap_Struct* Vmap,
                  rvm_ptr_t Virt_Num)
{
    rvm_ptr_t Count;
    
    RVM_DBG_S("-------------------------------------------------------------------------------\r\n");
    
    for(Count=0U;Count<Virt_Num;Count++)
    {
        /* Initialize basic data structure */
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
        RVM_ASSERT(RVM_Thd_Sched_Prio(Vmap[Count].Usr_Thd_Cap, RVM_WAIT_PRIO)==0U);
        RVM_ASSERT(RVM_Thd_Sched_Prio(Vmap[Count].Vct_Thd_Cap, RVM_WAIT_PRIO)==0U);
        
        /* Clean up all virtual interrupt flags/parameters for this virtual machine */
        RVM_Clear(Vmap[Count].State_Base, Vmap[Count].State_Size);
        
        /* Clean up all event send capabilities for this virtual machine */
        RVM_Clear(Virt[Count].Evt_Cap, RVM_EVTCAP_BITMAP*sizeof(rvm_ptr_t));

        /* Print log */
        RVM_DBG_S("Init:  Created VM ");
        RVM_DBG_S(Vmap[Count].Name);
        RVM_DBG_S(" control block 0x");
        RVM_DBG_H((rvm_ptr_t)&Virt[Count]);
        RVM_DBG_S(" database 0x");
        RVM_DBG_H((rvm_ptr_t)&Vmap[Count]);
        RVM_DBG_S(".\r\n");
    }
    
    /* Now set up the scheduler for the first runnable virtual machine */
    RVM_Virt_Cur=_RVM_Run_High();
    _RVM_Virt_Switch(RVM_NULL, RVM_Virt_Cur);
}
#endif
/* End Function:RVM_Virt_Init ************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
