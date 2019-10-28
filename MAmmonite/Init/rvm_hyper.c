/******************************************************************************
Filename    : rvm_hyper.c
Author      : pry
Date        : 02/10/2018
Licence     : The Unlicense; see LICENSE for details.
Description : The hypercall implementation file.
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
#include "Init/rvm_hyper.h"

#define __HDR_PUBLIC_MEMBERS__
#include "Platform/rvm_platform.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:_RVM_Set_Rdy ************************************************
Description : Set the virtual machine as ready to schedule.
Input       : struct RVM_Virt* Virt - The virtual machine to put into the runqueue.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void _RVM_Set_Rdy(struct RVM_Virt* Virt)
{
    /* Insert this into the corresponding runqueue */
    RVM_List_Ins(&(Virt->Head),RVM_Run[Virt->Map->Prio].Prev,&(RVM_Run[Virt->Map->Prio]));
    /* Set this runlevel as active */
    RVM_Bitmap[Virt->Map->Prio>>RVM_WORD_ORDER]|=((rvm_ptr_t)1)<<(Virt->Map->Prio&RVM_WORD_MASK);
}
#endif
/* End Function:_RVM_Set_Rdy *************************************************/

/* Begin Function:_RVM_Clr_Rdy ************************************************
Description : Clear the virtual machine from the runqueue.
Input       : struct RVM_Virt* Virt - The virtual machine to clear from the runqueue.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void _RVM_Clr_Rdy(struct RVM_Virt* Virt)
{
    /* See if it is the last thread on the priority level */
    if(Virt->Head.Prev==Virt->Head.Next)
        RVM_Bitmap[Virt->Map->Prio>>RVM_WORD_ORDER]&=~(((rvm_ptr_t)1)<<(Virt->Map->Prio&RVM_WORD_MASK));
    
    /* Delete this from the corresponding runqueue */
    RVM_List_Del(Virt->Head.Prev,Virt->Head.Next);
}
#endif
/* End Function:_RVM_Clr_Rdy *************************************************/

/* Begin Function:_RVM_Get_High_Rdy *******************************************
Description : Get the highest priority ready virtual machine available.
Input       : None.
Output      : None.
Return      : struct RVM_Virt* Virt - The virtual machine. If all virtual machine
                                      have gone asleep, this will return 0.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
struct RVM_Virt* _RVM_Get_High_Rdy(void)
{
    rvm_cnt_t Count;
    
    /* See which one is ready, and pick it */
    for(Count=RVM_PRIO_BITMAP-1;Count>=0;Count--)
    {
        if(RVM_Bitmap[Count]==0)
            continue;
        
        Count=_RVM_MSB_Get(RVM_Bitmap[Count])+(Count<<RVM_WORD_ORDER);
        return (struct RVM_Virt*)(RVM_Run[Count].Next);
    }
    
    return 0;
}
#endif
/* End Function:_RVM_Get_High_Rdy ********************************************/

/* Begin Function:_RVM_Virt_Switch ********************************************
Description : Switch between two virtual machines.
Input       : struct RVM_Virt* From - The source virtual machine.
              struct RVM_Virt* To - The destination virtual machine.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void _RVM_Virt_Switch(struct RVM_Virt* From, struct RVM_Virt* To)
{
    if(From==To)
        return;
    
    /* Just change the thread's priorities */
    if(From!=0)
    {
        RVM_ASSERT(RVM_Thd_Sched_Prio(From->Map->User_Thd_Cap, RVM_WAIT_PRIO)==0);
        RVM_ASSERT(RVM_Thd_Sched_Prio(From->Map->Vect_Thd_Cap, RVM_WAIT_PRIO)==0);
    }
    
    if(To!=0)
    {
        RVM_ASSERT(RVM_Thd_Sched_Prio(To->Map->User_Thd_Cap, RVM_USER_PRIO)==0);
        RVM_ASSERT(RVM_Thd_Sched_Prio(To->Map->Vect_Thd_Cap, RVM_VECT_PRIO)==0);
    }
}
#endif
/* End Function:_RVM_Virt_Switch *********************************************/

/* Begin Function:_RVM_Check_Vect_Pend ****************************************
Description : Check if there is one pending vector for the virtual machine.
Input       : struct RVM_Virt* Virt - The virtual machine to check.
Output      : None.
Return      : If there is one interrupt pending, return 1; else 0.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t _RVM_Check_Vect_Pend(struct RVM_Virt* Virt)
{
    rvm_cnt_t Count;
    
    if(Virt->Map->Vctf_Base->Timer!=0)
        return 1;
    
    if(Virt->Map->Vctf_Base->Ctxsw!=0)
        return 1;
    
    for(Count=0;Count<(Virt->Map->Vect_Num)>>RVM_WORD_ORDER;Count++)
    {
        if(Virt->Map->Vctf_Base->Vect[Count]!=0)
            return 1;
    }
    return 0;
}
#endif
/* End Function:_RVM_Check_Vect_Pend *****************************************/

/* Begin Function:_RVM_Set_Vect_Flag ******************************************
Description : Set an interrupt's flag for the virtual machine.
Input       : struct RVM_Virt* Virt - The virtual machine to set flag for.
              rvm_ptr_t Vect_Num - The vector number to set flag for.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void _RVM_Set_Vect_Flag(struct RVM_Virt* Virt, rvm_ptr_t Vect_Num)
{
    Virt->Map->Vctf_Base->Vect[Vect_Num>>RVM_WORD_ORDER]|=RVM_POW2(Vect_Num&RVM_WORD_MASK);
}
#endif
/* End Function:_RVM_Set_Vect_Flag ********************************************/

/* Begin Function:RVM_Hyp_Ena_Int *********************************************
Description : Enable interrupts for a virtual machine. Need to call this when
              the virtual machine fave finished all its initialization routines
              or it wouldn't be able to receive interrupts.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Ena_Int(void)
{
    if((RVM_Cur_Virt->Sched.State&RVM_VM_INTENA)!=0)
        return RVM_ERR_STATE;
    else
    {
        /* If the virtual machine ever calls this, then it means that it have done booting */
        if((RVM_Cur_Virt->Sched.State&RVM_VM_BOOTDONE)==0)
        {
            RVM_Cur_Virt->Sched.State|=RVM_VM_BOOTDONE;
            RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_Cur_Virt->Map->Vect_Thd_Cap, RVM_VECT_PRIO)==0);
        }
        RVM_Cur_Virt->Sched.State|=RVM_VM_INTENA;
        /* See if we do have excess interrupt to process. If yes, send to the endpoint now */
        if(_RVM_Check_Vect_Pend(RVM_Cur_Virt)!=0)
            RVM_ASSERT(RVM_Sig_Snd(RVM_Cur_Virt->Map->Vect_Sig_Cap)==0);
    }

    return 0;
}
#endif
/* End Function:RVM_Hyp_Ena_Int **********************************************/

/* Begin Function:RVM_Hyp_Dis_Int *********************************************
Description : Disable the interrupt for the virtual machine. All interrupts for a
              virtual machine, including the tick timer interrupt,is disabled on
              startup.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Dis_Int(void)
{
    if((RVM_Cur_Virt->Sched.State&RVM_VM_INTENA)==0)
        return RVM_ERR_STATE;
    else
        RVM_Cur_Virt->Sched.State&=~RVM_VM_INTENA;

    return 0;
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
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Reg_Phys(rvm_ptr_t Phys_Num, rvm_ptr_t Vect_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if both numbers are overrange */
    if((Phys_Num>=RVM_VECT_NUM)||(Vect_Num>=(RVM_Cur_Virt->Map->Vect_Num)))
        return RVM_ERR_RANGE;

    /* See if we have locked ourselves down */
    if((RVM_Cur_Virt->Sched.State&RVM_VM_LOCKVECT)!=0)
        return RVM_ERR_STATE;

    /* Check if the VM have registered for this physical vector before */
    Trav=RVM_Phys[Phys_Num].Next;
    while(Trav!=&RVM_Phys[Phys_Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        
        if(Map->Virt==RVM_Cur_Virt)
            return RVM_ERR_PHYS;
        
        Trav=Trav->Next;
    }
    
    /* Check if the VM's vector already have something mapped */
    Trav=RVM_Cur_Virt->Vect_Head.Next;
    while(Trav!=&(RVM_Cur_Virt->Vect_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vect_Num==Vect_Num)
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
    RVM_List_Ins(&(Map->Dst_Head),RVM_Cur_Virt->Vect_Head.Prev, &(RVM_Cur_Virt->Vect_Head));
    
    Map->Virt=RVM_Cur_Virt;
    Map->Vect_Num=Vect_Num;
    
    return 0;
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
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Reg_Evt(rvm_ptr_t Evt_Num, rvm_ptr_t Vect_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if both numbers are overrange */
    if((Evt_Num>=RVM_EVT_NUM)||(Vect_Num>=(RVM_Cur_Virt->Map->Vect_Num)))
        return RVM_ERR_RANGE;

    /* See if we have locked ourselves down */
    if((RVM_Cur_Virt->Sched.State&RVM_VM_LOCKVECT)!=0)
        return RVM_ERR_STATE;

    /* Check if the VM have registered for this event channel before */
    Trav=RVM_Evt[Evt_Num].Next;
    while(Trav!=&RVM_Evt[Evt_Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        
        if(Map->Virt==RVM_Cur_Virt)
            return RVM_ERR_EVT;
        
        Trav=Trav->Next;
    }
    
    /* Check if the VM's vector already have something mapped */
    Trav=RVM_Cur_Virt->Vect_Head.Next;
    while(Trav!=&(RVM_Cur_Virt->Vect_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vect_Num==Vect_Num)
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
    RVM_List_Ins(&(Map->Dst_Head),RVM_Cur_Virt->Vect_Head.Prev, &(RVM_Cur_Virt->Vect_Head));
    
    Map->Virt=RVM_Cur_Virt;
    Map->Vect_Num=Vect_Num;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Reg_Evt **********************************************/

/* Begin Function:RVM_Hyp_Del_Vect ********************************************
Description : Deregister the vector of an virtual machine.
Input       : rvm_ptr_t Vect_Num - The virtual vector to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Del_Vect(rvm_ptr_t Vect_Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* See if the number is overrange */
    if(Vect_Num>=RVM_Cur_Virt->Map->Vect_Num)
        return RVM_ERR_RANGE;

    /* See if we have locked ourselves down */
    if((RVM_Cur_Virt->Sched.State&RVM_VM_LOCKVECT)!=0)
        return RVM_ERR_STATE;

    /* Is it registered to anything? */
    Trav=RVM_Cur_Virt->Vect_Head.Next;
    while(Trav!=&(RVM_Cur_Virt->Vect_Head))
    {
        Map=RVM_DST2MAP(Trav);
        
        if(Map->Vect_Num==Vect_Num)
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
/* End Function:RVM_Hyp_Del_Vect *********************************************/

/* Begin Function:RVM_Hyp_Lock_Vect *******************************************
Description : Lockdown the vector mapping of an virtual machine.
Input       : rvm_ptr_t Vect_Num - The virtual vector to deregister.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Lock_Vect(void)
{
    /* See if we have locked ourselves down */
    if((RVM_Cur_Virt->Sched.State&RVM_VM_LOCKVECT)!=0)
        return RVM_ERR_STATE;
    
    /* Lock down mappings */
    RVM_Cur_Virt->Sched.State|=RVM_VM_LOCKVECT;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Lock_Vect ********************************************/

/* Begin Function:RVM_Hyp_Wait_Vect *******************************************
Description : Set the virtual machine to sleep until a vector comes in.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Wait_Vect(void)
{
    struct RVM_Virt* Next;
    
    /* See if it have interrupt disabled */
    if((RVM_Cur_Virt->Sched.State&RVM_VM_INTENA)==0)
        return RVM_ERR_STATE;

    /* Now suspend the virtual machine and switch to somebody else */
    RVM_VM_STATE_SET(RVM_Cur_Virt->Sched.State,RVM_VM_WAITING);
    _RVM_Clr_Rdy(RVM_Cur_Virt);
    
    /* The next could be zero, which means that there are no VM running now */
    Next=_RVM_Get_High_Rdy();
    _RVM_Virt_Switch(RVM_Cur_Virt, Next);
    RVM_Cur_Virt=Next;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Wait_Vect ********************************************/

/* Begin Function:_RVM_Send_Virt_Vects ****************************************
Description : Send to all virtual machine vectors registered on this physical 
              physical interrupt channel or event.
Input       : struct RVM_List* Array - The array containing lists of registered
                                       virtual vectors.
              rvm_ptr_t Num - The physical vector ID or event ID.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void _RVM_Send_Virt_Vects(struct RVM_List* Array, rvm_ptr_t Num)
{
    struct RVM_List* Trav;
    struct RVM_Map_Struct* Map;
    
    /* Is it registered to anything? */
    Trav=Array[Num].Next;
    while(Trav!=&Array[Num])
    {
        Map=(struct RVM_Map_Struct*)Trav;
        
        /* Send to everyone on that list */
        _RVM_Set_Vect_Flag(Map->Virt, Map->Vect_Num);
        RVM_ASSERT(RVM_Sig_Snd(Map->Virt->Map->Vect_Sig_Cap)==0);
        /* If it is waiting, notify it of new interrupts */
        if(RVM_VM_STATE(Map->Virt->Sched.State)==RVM_VM_WAITING)
        {
            RVM_VM_STATE_SET(RVM_Cur_Virt->Sched.State,RVM_VM_RUNNING);
            _RVM_Set_Rdy(Map->Virt);
        }
        
        Trav=Trav->Next;
    }

    /* We do not reschedule here because we will reschedule at last */
}
#endif
/* End Function:_RVM_Send_Virt_Vects *****************************************/

/* Begin Function:RVM_Hyp_Send_Evt ********************************************
Description : Send an event to the event channel.
Input       : rvm_ptr_t Evt_Num - The event channel ID.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Send_Evt(rvm_ptr_t Evt_Num)
{   
    /* See if the number is overrange */
    if(Evt_Num>=RVM_EVT_NUM)
        return RVM_ERR_RANGE;
    
    _RVM_Send_Virt_Vects(RVM_Evt, Evt_Num);
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Send_Evt *********************************************/

/* Begin Function:RVM_Hyp_Feed_Wdog *******************************************
Description : Start and feed the watchdog for the current virtual machine.
Input       : None.
Output      : None.
Return      : rvm_ret_t - If successful, 0; else an error code.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ret_t RVM_Hyp_Feed_Wdog(void)
{   
    /* See if this virtual machine have watchdog enabled */
    if(RVM_Cur_Virt->Map->Watchdog==0)
        return RVM_ERR_STATE;
    
    /* Set state and reset watchdog counter */
    RVM_Cur_Virt->Sched.State|=RVM_VM_WDOGENA;
    RVM_Cur_Virt->Sched.Watchdog_Left=RVM_Cur_Virt->Map->Watchdog;
    
    return 0;
}
#endif
/* End Function:RVM_Hyp_Feed_Wdog ********************************************/

/* Begin Function:_RVM_Wheel_Ins **********************************************
Description : Insert one VM into the timer wheel.
Input       : struct RVM_Virt* Virt - The virtual machine to insert.
              rvm_ptr_t Period - The period of the timer interrupt.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void _RVM_Wheel_Ins(struct RVM_Virt* Virt, rvm_ptr_t Period)
{
    struct RVM_List* Slot;
    struct RVM_List* Trav_Ptr;
    struct RVM_Virt* Trav_Virt;
    
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

/* Begin Function:_RVM_Timer_Send *********************************************
Description : Send an timer interrupt to a virtual machine.
Input       : struct RVM_Virt* Virt - The pointer to the virtual machine.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void _RVM_Timer_Send(struct RVM_Virt* Virt)
{
    if(RVM_VM_STATE(Virt->Sched.State)==RVM_VM_WAITING)
    {
        _RVM_Set_Rdy(Virt);
        RVM_VM_STATE_SET(Virt->Sched.State,RVM_VM_RUNNING);
    }
    
    /* Set the timer bit */
    Virt->Map->Vctf_Base->Timer=1;
    
    /* Send interrupt to it, if its interrupts are enabled */
    if((Virt->Sched.State&RVM_VM_INTENA)!=0)
        RVM_ASSERT(RVM_Sig_Snd(Virt->Map->Vect_Sig_Cap)==0);
    
    /* We do not reschedule here because we will reschedule at the place where all
     * timer interrupts are sent */
}
#endif
/* End Function:_RVM_Timer_Send **********************************************/

/* Begin Function:_RVM_Recover_Cur_Virt ***************************************
Description : Recover the currently running virtual machine.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void _RVM_Recover_Cur_Virt(void)
{
    struct RVM_Map_Struct* Map;
    rvm_ptr_t Init_Stack_Addr;
    
    /* Unbind the threads from the core then rebind in case they are still blocked. 
     * This can break the assumption that the vector thread shall run first */
    RVM_ASSERT(RVM_Thd_Sched_Free(RVM_Cur_Virt->Map->Vect_Thd_Cap)==0);
    RVM_ASSERT(RVM_Thd_Sched_Free(RVM_Cur_Virt->Map->User_Thd_Cap)==0);
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Cur_Virt->Map->Vect_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, 
                                  RVM_Cur_Virt->Map->Vect_TID|RVM_VIRT_THDID_MARKER, RVM_VECT_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_Cur_Virt->Map->User_Thd_Cap, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, 
                                  RVM_Cur_Virt->Map->User_TID|RVM_VIRT_THDID_MARKER, RVM_USER_PRIO)==0);
    
    /* Set the execution properties for both threads */
    Init_Stack_Addr=RVM_Stack_Init(RVM_Cur_Virt->Map->Vect_Stack_Base, RVM_Cur_Virt->Map->Vect_Stack_Size,
                                   RVM_Cur_Virt->Map->Vect_Entry, RVM_Cur_Virt->Map->Entry_Code_Front);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Cur_Virt->Map->Vect_Thd_Cap, RVM_Cur_Virt->Map->Vect_Entry, Init_Stack_Addr, 0)==0);
    Init_Stack_Addr=RVM_Stack_Init(RVM_Cur_Virt->Map->User_Stack_Base, RVM_Cur_Virt->Map->User_Stack_Size,
                                   RVM_Cur_Virt->Map->User_Entry, RVM_Cur_Virt->Map->Entry_Code_Front);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_Cur_Virt->Map->User_Thd_Cap, RVM_Cur_Virt->Map->User_Entry, Init_Stack_Addr ,0)==0);
    
    /* Delegate infinite time to both threads */
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Cur_Virt->Map->Vect_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_Cur_Virt->Map->User_Thd_Cap, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);

    /* Set the state to running, interrupt disabled, vector unlocked, and watchdog disabled */
    RVM_Cur_Virt->Sched.State=RVM_VM_RUNNING;
    
    /* If it registered for anything, delete them and return them to database */
    while(RVM_Cur_Virt->Vect_Head.Next!=&(RVM_Cur_Virt->Vect_Head))
    {
        Map=RVM_DST2MAP(RVM_Cur_Virt->Vect_Head.Next);
        RVM_List_Del(Map->Src_Head.Prev,Map->Src_Head.Next);
        RVM_List_Del(Map->Dst_Head.Prev,Map->Dst_Head.Next);
        RVM_List_Ins(&(Map->Src_Head),&RVM_Map_Free,RVM_Map_Free.Next);
    }
    
    /* Reinsert this into the wheel */
    RVM_List_Del(RVM_Cur_Virt->Delay.Prev, RVM_Cur_Virt->Delay.Next);
    _RVM_Wheel_Ins(RVM_Cur_Virt,RVM_Cur_Virt->Map->Period);
}
#endif
/* End Function:_RVM_Recover_Cur_Virt ****************************************/

/* Begin Function:RVM_Sftd ****************************************************
Description : The safety daemon against system partial or total failures.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Sftd(void)
{
    rvm_tid_t Thd;
    rvm_ptr_t Fault;
    
    RVM_LOG_S("Sftd:Safety guard daemon initialization complete.\r\n");

    /* Main cycle - recover faults if possible */
    while(1)
    {
        /* Blocking single receive */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_Sftd_Sig_Cap, RVM_RCV_BS)>=0);
        
        Thd=RVM_Thd_Sched_Rcv(RVM_Sftd_Thd_Cap, &Fault);
        
        /* Is an error a fault? If not, then we must have done something wrong - This is really bad */
        if(((Thd&RVM_THD_FAULT_FLAG)==0)||(Thd<0))
        {
            /* Hang the machine because this error is unrecoverable */
            RVM_LOG_SUS("Sftd:Intangible fault on return value 0x",Thd,". Shutting down system...\r\n");
            RVM_ASSERT(0);
        }
        
        /* Print the reason of the fault */
        RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
        RVM_Thd_Print_Fault(Fault);
        
        /* See what thread faulted. If it is any of the process threads, then the system is done */
        Thd&=~((rvm_ptr_t)RVM_THD_FAULT_FLAG);
        
        /* Whatever thread faults, we will need to print their registers, so we can figure out
         * what the fault is. There should be a print somewhere, but shouldn't be in the kernel. */
        if(Thd<RVM_CAPID_2L)
        {
            /* We know that this happened within RVM itself. Sad, cannot recover */
            RVM_LOG_S("Sftd:Unrecoverable fault on RVM ");
            if(Thd==RVM_Sftd_Thd_Cap)
                RVM_LOG_S("Sftd");
#if(RVM_MAX_PREEMPT_VPRIO!=0)
            else if(Thd==RVM_Timd_Thd_Cap)
                RVM_LOG_S("Timd");
            else if(Thd==RVM_Vmmd_Thd_Cap)
                RVM_LOG_S("Vmmd");
            else if(Thd==RVM_Vctd_Thd_Cap)
                RVM_LOG_S("Vctd");
#endif
            else
                RVM_LOG_SIS("unrecognized thread TID ", Thd,"");
            
            RVM_LOG_S(". Rebooting system...\r\n");
            
            /* Print registers so we know how it crashed */
            RVM_ASSERT(RVM_Thd_Print_Regs(Thd)==0);
            /* Reboot */
            RVM_ASSERT(0);
        }
        else if((Thd&RVM_VIRT_THDID_MARKER)==0)
        {
            /* We know that this happened in a process. Still, cannot be recovered. */
            RVM_LOG_SIS("Sftd:Unrecoverable fault on process thread TID ", Thd, ". Rebooting system...\r\n");
            
            /* Print registers so we know how it crashed */
            RVM_ASSERT(RVM_Thd_Print_Regs(Thd)==0);
            
            /* Reboot */
            RVM_ASSERT(0);
        }
        else
        {
#if(RVM_MAX_PREEMPT_VPRIO!=0)
            Thd&=~RVM_VIRT_THDID_MARKER;
            /* This must have happened in the current virtual machine */
            RVM_LOG_S("Sftd:Recoverable fault in virtual machine ");
            RVM_LOG_S(RVM_Cur_Virt->Map->Name);

            if(Thd==RVM_Cur_Virt->Map->Vect_Thd_Cap)
                RVM_LOG_S(" vector handling thread");
            else if(Thd==RVM_Cur_Virt->Map->User_Thd_Cap)
                RVM_LOG_S(" user program thread");
            else
                RVM_LOG_S(" unknown thread");

            RVM_LOG_S(". Recovering...\r\n");
            
            /* Also print registers so that the user can debug. This is abnormal anyway */
            RVM_LOG_S("Sftd:Vector handling thread register set:\r\n");
            RVM_ASSERT(RVM_Thd_Print_Regs(RVM_Cur_Virt->Map->Vect_Thd_Cap)==0);
            RVM_LOG_S("Sftd:User program thread register set:\r\n");
            RVM_ASSERT(RVM_Thd_Print_Regs(RVM_Cur_Virt->Map->User_Thd_Cap)==0);

            /* Actually reboot the virtual machine */
            _RVM_Recover_Cur_Virt();
            
            RVM_LOG_S("Sftd:Recovered.\r\n");
#else
            RVM_LOG_S("Sftd:Fault on virtual machine but no virtual machine exists. Rebooting system...\r\n");
            RVM_ASSERT(0);
#endif
        }
    }
}
/* End Function:RVM_Sftd *****************************************************/

/* Begin Function:RVM_Timd ****************************************************
Description : The timer daemon for timer interrupt handling and time accounting.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void RVM_Timd(void)
{
    struct RVM_Virt* Virt;
    struct RVM_List* Slot;
    struct RVM_List* Trav;

    RVM_LOG_S("Timd:Timer relaying daemon initialization complete.\r\n");

    /* We will receive a timer interrupt every tick here */
    while(1)
    {
        RVM_ASSERT(RVM_Sig_Rcv(RVM_BOOT_INIT_TIMER, RVM_RCV_BS)>=0);
        RVM_Tick++;
        /*
        if((RVM_Tick%1000)==0)
            RVM_LOG_S("Timd:Timer daemon passed 1000 ticks.\r\n");
        */
        Slot=&(RVM_Wheel[RVM_Tick%RVM_WHEEL_SIZE]);
        
        Trav=Slot->Next;
        /* See if we need to process the timer wheel to deliver timer interrupts to virtual machines */
        while(Trav!=Slot)
        {
            Virt=RVM_DLY2VM(Trav);
            Trav=Trav->Next;
            /* If the value is more than this, then it means that the time have
             * already passed and we have to process this */
            if((RVM_Tick-Virt->Sched.Period_Timeout)>=(RVM_ALLBITS>>1))
                break;
            
            /* This VM should be processed, place it at the new position */
            RVM_List_Del(Virt->Delay.Prev,Virt->Delay.Next);
            _RVM_Wheel_Ins(Virt,Virt->Map->Period);
            
            /* Send special timer interrupt to it */
            _RVM_Timer_Send(Virt);
        }
        
        /* If there is at least one virtual machine working, check slices and watchdog */
        if(RVM_Cur_Virt!=0)
        {
            /* Is the timeslices exhausted? */
            if(RVM_Cur_Virt->Sched.Slices_Left==0)   
            {
                RVM_Cur_Virt->Sched.Slices_Left=RVM_Cur_Virt->Map->Slices;
                /* Place it at the end of the run queue */
                RVM_List_Del(RVM_Cur_Virt->Head.Prev,RVM_Cur_Virt->Head.Next);
                RVM_List_Ins(&(RVM_Cur_Virt->Head),
                             RVM_Run[RVM_Cur_Virt->Map->Prio].Prev,
                             &(RVM_Run[RVM_Cur_Virt->Map->Prio]));
            }
            else
                RVM_Cur_Virt->Sched.Slices_Left--;
            
            /* Is the watchdog enabled for this virtual machine? */
            if((RVM_Cur_Virt->Sched.State&RVM_VM_WDOGENA)!=0)
            {
                /* Is the watchdog timeout? */
                if(RVM_Cur_Virt->Sched.Watchdog_Left==0)   
                {
                    /* Watchdog timeout - seek to reboot the VM */
                    RVM_LOG_S("Timd:Watchdog overflow in virtual machine ");
                    RVM_LOG_S(RVM_Cur_Virt->Map->Name);
                    RVM_LOG_S(". Recovering...\r\n");
                    
                    /* Also print registers so that the user can debug. This is abnormal anyway */
                    RVM_LOG_S("Timd:Vector handling thread register set:\r\n");
                    RVM_ASSERT(RVM_Thd_Print_Regs(RVM_Cur_Virt->Map->Vect_Thd_Cap)==0);
                    RVM_LOG_S("Timd:User program thread register set:\r\n");
                    RVM_ASSERT(RVM_Thd_Print_Regs(RVM_Cur_Virt->Map->User_Thd_Cap)==0);

                    /* Actually reboot the virtual machine */
                    _RVM_Recover_Cur_Virt();
                    
                    RVM_LOG_S("Timd:Recovered.\r\n");
            
                }
                else
                    RVM_Cur_Virt->Sched.Watchdog_Left--;
            }
        }
        
        /* There must at least be a virtual machine that is active. We mandate the switch
         * even under the same priority level because the current VM has just been moved to
         * the end of the runqueue, and we need to round-robin */
        Virt=_RVM_Get_High_Rdy();
        _RVM_Virt_Switch(RVM_Cur_Virt, Virt);
        RVM_Cur_Virt=Virt;
    }
}
#endif
/* End Function:RVM_Timd *****************************************************/

/* Begin Function:RVM_Vmmd ****************************************************
Description : The virtual machine monitor daemon for hypercall handling.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void RVM_Vmmd(void)
{
    rvm_ptr_t Number;
    rvm_ptr_t* Param;
    RVM_LOG_S("Vmmd:Hypercall handling daemon initialization complete.\r\n");
    
    /* Main cycle */
    while(1)
    {
        /* Blocking multi receive */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_Vmmd_Sig_Cap, RVM_RCV_BS)>=0);

        /* See if the vector is active */
        if(RVM_Cur_Virt->Map->Param_Base->Vect_Active!=0)
        {
            Number=RVM_Cur_Virt->Map->Param_Base->Vect.Number;
            Param=RVM_Cur_Virt->Map->Param_Base->Vect.Param;
        }
        else
        {
            Number=RVM_Cur_Virt->Map->Param_Base->User.Number;
            Param=RVM_Cur_Virt->Map->Param_Base->User.Param;
        }
        
        /* RVM_LOG_SIS("Vmmd:Hypercall ",Number," called.\r\n"); */
            
        switch(Number)
        {
            case RVM_HYP_ENAINT:
            {
                Param[0]=RVM_Hyp_Ena_Int();
                break;
            }
            case RVM_HYP_DISINT:
            {
                Param[0]=RVM_Hyp_Dis_Int();
                break;
            }
            case RVM_HYP_REGPHYS:
            {
                Param[0]=RVM_Hyp_Reg_Phys(Param[0] /* rvm_ptr_t Phys_Num */,
                                          Param[1] /* rvm_ptr_t Vect_Num */);
                break;
            }
            case RVM_HYP_REGEVT:
            {
                Param[0]=RVM_Hyp_Reg_Evt(Param[0] /* rvm_ptr_t Evt_Num */,
                                         Param[1] /* rvm_ptr_t Vect_Num */);
                break;
            }
            case RVM_HYP_DELVECT:
            {
                Param[0]=RVM_Hyp_Del_Vect(Param[0] /* rvm_ptr_t Vect_Num */);
                break;
            }
            case RVM_HYP_LOCKVECT:
            {
                Param[0]=RVM_Hyp_Lock_Vect();
                break;
            }
            case RVM_HYP_WAITVECT:
            {
                Param[0]=RVM_Hyp_Wait_Vect();
                break;
            }
            case RVM_HYP_SENDEVT:
            {
                Param[0]=RVM_Hyp_Send_Evt(Param[0] /* rvm_ptr_t Evt_Num */);
                break;
            }
            case RVM_HYP_FEEDWDOG:
            {
                Param[0]=RVM_Hyp_Feed_Wdog();
                break;
            }

            default:break;
        }
        
        /* RVM_LOG_SIS("Vmmd:Hypercall ",Number," processed.\r\n"); */
    }
}
#endif
/* End Function:RVM_Vmmd *****************************************************/

/* Begin Function:_RVM_Flagset_Get ********************************************
Description : Get the physical vector source or event source from the interrupt
              set. When this is called, there must be at least one vector or
              event pending in this set.
Input       : struct RVM_Flag_Set* Set - The interrupt set data.
Output      : struct RVM_Flag_Set* Set - The interrupt set data.
Return      : rvm_ptr_t - The interrupt number.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
rvm_ptr_t _RVM_Flagset_Get(struct RVM_Flag_Set* Set)
{
    rvm_ptr_t Group;
    rvm_ptr_t Member;
    
    /* See which source group could possibly have any interrupts */
    Group=_RVM_MSB_Get(Set->Group);
    Member=_RVM_MSB_Get(Set->Flags[Group]);
    
    /* Clean up the slot now */
    Set->Flags[Group]&=~(((rvm_ptr_t)1)<<Member);
    if(Set->Flags[Group]==0)
        Set->Group&=~(((rvm_ptr_t)1)<<Group);
    
    return (Group<<RVM_WORD_ORDER)|Member;
}
#endif
/* End Function:_RVM_Flagset_Get *********************************************/

/* Begin Function:RVM_Vctd ****************************************************
Description : The vector daemon for interrupt handling.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void RVM_Vctd(void)
{
    rvm_cnt_t Num;
    struct RVM_Virt* Virt;
    struct RVM_Phys_Flags* Vect_Flags;
    struct RVM_Phys_Flags* Evt_Flags;
    
    Vect_Flags=(struct RVM_Phys_Flags*)RVM_A7M_VECT_FLAG_ADDR;
    Evt_Flags=(struct RVM_Phys_Flags*)RVM_A7M_EVT_FLAG_ADDR;
    
    RVM_LOG_S("Vctd:Vector handling daemon initialization complete.\r\n");
    
    /* Main cycle - keep getting vectors and passing them to virtual machines */
    while(1)
    {
        /* Blocking multi receive */
        RVM_ASSERT(RVM_Sig_Rcv(RVM_BOOT_INIT_VECT, RVM_RCV_BM)>=0);

        /* Deal with the vectors */
        Vect_Flags->Set0.Lock=1;
        while(Vect_Flags->Set0.Group!=0)
        {
            /* Process the interrupts in the first group one by one */
            Num=_RVM_Flagset_Get(&(Vect_Flags->Set0));
            /* Only send if smaller than the number of physical vectors declared */
            if(Num<RVM_VECT_NUM)
                _RVM_Send_Virt_Vects(RVM_Phys, Num);
        }
        Vect_Flags->Set0.Lock=0;
        Vect_Flags->Set1.Lock=1;
        while(Vect_Flags->Set1.Group!=0)
        {
            /* Process the interrupts in the first group one by one */
            Num=_RVM_Flagset_Get(&(Vect_Flags->Set1));
            /* Only send if smaller than the number of physical vectors declared */
            if(Num<RVM_VECT_NUM)
                _RVM_Send_Virt_Vects(RVM_Phys, Num);
        }
        Vect_Flags->Set1.Lock=0;
        
        /* Deal with the events */
        Evt_Flags->Set0.Lock=1;
        while(Evt_Flags->Set0.Group!=0)
        {
            /* Process the interrupts in the first group one by one */
            Num=_RVM_Flagset_Get(&(Evt_Flags->Set0));
            /* Only send if smaller than the number of events declared */
            if(Num<RVM_EVT_NUM)
                _RVM_Send_Virt_Vects(RVM_Evt, Num);
        }
        Evt_Flags->Set0.Lock=0;
        Evt_Flags->Set1.Lock=1;
        while(Evt_Flags->Set1.Group!=0)
        {
            /* Process the interrupts in the first group one by one */
            Num=_RVM_Flagset_Get(&(Evt_Flags->Set1));
            /* Only send if smaller than the number of events declared */
            if(Num<RVM_EVT_NUM)
                _RVM_Send_Virt_Vects(RVM_Evt, Num);
        }
        Evt_Flags->Set1.Lock=0;
        
        /* Only when we have something of a higher priority do we need to reschedule */
        Virt=_RVM_Get_High_Rdy();
        if(Virt->Map->Prio>RVM_Cur_Virt->Map->Prio)
        {
            _RVM_Virt_Switch(RVM_Cur_Virt, Virt);
            RVM_Cur_Virt=Virt;
        }
    }
}
#endif
/* End Function:RVM_Vctd *****************************************************/

/* Begin Function:RVM_Virt_Init ***********************************************
Description : Initialize necessary virtual machine monitor database.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void RVM_Virt_Init(void)
{
    rvm_ptr_t Count;
    
    RVM_Tick=0;
    for(Count=0;Count<RVM_WHEEL_SIZE;Count++)
        RVM_List_Crt(&RVM_Wheel[Count]);
    
    RVM_Cur_Virt=0;
    for(Count=0;Count<RVM_PRIO_BITMAP;Count++)
        RVM_Bitmap[Count]=0;
    for(Count=0;Count<RVM_MAX_PREEMPT_VPRIO;Count++)
        RVM_List_Crt(&RVM_Run[Count]);
    
    for(Count=0;Count<RVM_EVT_NUM;Count++)
        RVM_List_Crt(&RVM_Evt[Count]);
    for(Count=0;Count<RVM_VECT_NUM;Count++)
        RVM_List_Crt(&RVM_Phys[Count]);
    
    RVM_List_Crt(&RVM_Map_Free);
    for(Count=0;Count<RVM_MAP_NUM;Count++)
        RVM_List_Ins(&RVM_Map[Count].Dst_Head, RVM_Map_Free.Prev, &RVM_Map_Free);
    
    /* Clean up physical vector flags and event flags */
    RVM_Clear((void*)RVM_A7M_VECT_FLAG_ADDR, sizeof(struct RVM_Phys_Flags));
    RVM_Clear((void*)RVM_A7M_EVT_FLAG_ADDR, sizeof(struct RVM_Phys_Flags));
}
#endif
/* End Function:RVM_Virt_Init ************************************************/

/* Begin Function:RVM_Virt_Crt ************************************************
Description : Set up all virtual machines according to the database.
Input       : struct RVM_Virt* Virt_DB - The virtual machine database.
              struct RVM_Virt_Map* Virt_Map - The virtual machine static memory map.
              rvm_ptr_t Virt_Size - The number of virtual machines in the system.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_MAX_PREEMPT_VPRIO!=0)
void RVM_Virt_Crt(struct RVM_Virt* Virt_DB, struct RVM_Virt_Map* Virt_Map, rvm_ptr_t Virt_Size)
{
    rvm_ptr_t Count;
    rvm_ptr_t Vect_Size;
    
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    
    for(Count=0;Count<Virt_Size;Count++)
    {
        /* Initialize basic data structure */
        /* State set to running, but interrupts are disabled, and VM uninitailized */
        Virt_DB[Count].Sched.State=RVM_VM_RUNNING;
        Virt_DB[Count].Sched.Slices_Left=Virt_Map[Count].Slices;
        RVM_List_Crt(&Virt_DB[Count].Vect_Head);
        Virt_DB[Count].Map=&Virt_Map[Count];
        
        /* Insert into timer wheel - +Count here to make sure that they distribute
         * on the timer wheel as evenly as possible */
        _RVM_Wheel_Ins(&Virt_DB[Count],Virt_Map[Count].Period+Count);
        
        /* Set all these virtual machines as ready, but all the threads' priority at idle */
        _RVM_Set_Rdy(&Virt_DB[Count]);
        RVM_ASSERT(RVM_Thd_Sched_Prio(Virt_Map[Count].User_Thd_Cap, RVM_WAIT_PRIO)==0);
        RVM_ASSERT(RVM_Thd_Sched_Prio(Virt_Map[Count].Vect_Thd_Cap, RVM_WAIT_PRIO)==0);
        
        /* Clean up all virtual interrupt flags for this virtual machine */
        Vect_Size=sizeof(struct RVM_Vect_Flag)+sizeof(rvm_ptr_t)*((Virt_Map[Count].Vect_Num>>RVM_WORD_ORDER)-1);
        RVM_Clear((void*)(Virt_Map[Count].Vctf_Base), Vect_Size);
        
        /* Print log */
        RVM_LOG_S("Vmmd: Created VM ");
        RVM_LOG_S(Virt_Map[Count].Name);
        RVM_LOG_S(" register 0x");
        RVM_LOG_U((rvm_ptr_t)(Virt_Map[Count].Reg_Base));
        RVM_LOG_S(" control block 0x");
        RVM_LOG_U((rvm_ptr_t)&Virt_DB[Count]);
        RVM_LOG_S(" database 0x");
        RVM_LOG_U((rvm_ptr_t)&Virt_Map[Count]);
        RVM_LOG_S(".\r\n");
    }
    
    /* Now set up the scheduler for the first runnable virtual machine */
    RVM_Cur_Virt=_RVM_Get_High_Rdy();
    _RVM_Virt_Switch(0, RVM_Cur_Virt);
}
#endif
/* End Function:RVM_Virt_Init ************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
