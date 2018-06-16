/******************************************************************************
Filename    : guest_cmx.c
Author      : pry
Date        : 09/02/2018
Licence     : LGPL v3+; see COPYING for details.
Description : The guest OS Cortex-M wrapper.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_PRIVATE_MEMBERS__
#include "guest_cmx.h"
#undef __HDR_PRIVATE_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:RVM_Init ****************************************************
Description : Initialize the RVM-related components.
Input       : None
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Init(void)
{
    cnt_t Count;
    /* Clear all VM-related flags and registration tables */
    RVM_Int_Enable=0;
    RVM_Int_Pend=0;
    RVM_Int_Active=0;
    /* Clean up all global variables */
    for(Count=0;Count<RVM_VECT_BITMAP;Count++)
        RVM_Flag[Count]=0;
    for(Count=0;Count<RVM_MAX_INTVECT;Count++)
        RVM_Vect[Count]=0;
}
/* End Function:RVM_Init *****************************************************/

/* Begin Function:RVM_Vect_Init ***********************************************
Description : Initialize an interrupt handler.
Input       : None
Output      : None.
Return      : None.
******************************************************************************/
ret_t RVM_Vect_Init(ptr_t Num, void* Handler)
{
    if(Num>=RVM_MAX_INTVECT)
        return -1;
    
    RVM_Vect[Num]=(ptr_t)Handler;
    return 0;
}
/* End Function:RVM_Vect_Init ************************************************/

/* Begin Function:RVM_Hypercall ***********************************************
Description : Do a hypercall to the virtual machine.
Input       : ptr_t Number - The hypercall number.
              ptr_t Param1 - The second input.
              ptr_t Param2 - The third input.
              ptr_t Param3 - The fourth ptr_t Param4
Output      : None.
Return      : None.
******************************************************************************/
ret_t RVM_Hypercall(ptr_t Number, ptr_t Param1, ptr_t Param2, ptr_t Param3, ptr_t Param4)
{
    /* Pass the parameters */
    RVM_Param.Number=Number;
    RVM_Param.Param[0]=Param1;
    RVM_Param.Param[1]=Param2;
    RVM_Param.Param[2]=Param3;
    RVM_Param.Param[3]=Param4;
    
    /* Do the hypercall */
    _RVM_Hypercall();
    
    /* Return the result */
    return RVM_Param.Param[0];
}
/* End Function:RVM_Hypercall ************************************************/

/* Begin Function:RVM_Enable_Int **********************************************
Description : Enable interrupts.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Enable_Int(void)
{
    /* Must be successful */
    RVM_Int_Enable=1;
    RVM_Hypercall(RVM_HYP_ENAINT,0,0,0,0);
}
/* End Function:RVM_Enable_Int ***********************************************/

/* Begin Function:RVM_Disable_Int *********************************************
Description : Disable interrupts.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Disable_Int(void)
{
    /* Must be successful */
    RVM_Hypercall(RVM_HYP_DISINT,0,0,0,0);
    RVM_Int_Enable=0;
}
/* End Function:RVM_Disable_Int **********************************************/

/* Begin Function:RVM_Mask_Int ************************************************
Description : Mask interrupts; this does not stop the interrupt delivery, but will
              postpone the processing until we unmask them.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Mask_Int(void)
{
    RVM_Int_Pend=0;
    RVM_Int_Enable=0;
}
/* End Function:RVM_Mask_Int *************************************************/

/* Begin Function:RVM_Unmask_Int **********************************************
Description : Unmask pending interrupts so we can process them.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Unmask_Int(void)
{
    RVM_Int_Enable=1;
    /* Trigger interrupt processing if there are pending ones */
    if(RVM_Int_Pend!=0)
    {
        RVM_Int_Pend=0;
        _RVM_Yield();
    }
}
/* End Function:RVM_Unmask_Int ***********************************************/

/* Begin Function:RVM_Reg_Int *************************************************
Description : Register an interrupt line. This will link the physical vector with
              ID Vect_Num to virtual vector with ID Int_Num.
Input       : ptr_t Vect_Num - The physical vector number's place in the kernel function table.
              ptr_t Int_Num - The virtual vector number.
Output      : None.
Return      : ret_t - If successful, the interrupt registration ID; else an error code.
******************************************************************************/
ret_t RVM_Reg_Int(ptr_t Vect_Num, ptr_t Int_Num)
{
    if(Vect_Num>=RVM_Img.Kcap_Num)
        return -1;
    
    return RVM_Hypercall(RVM_HYP_REGINT,RVM_Img.Kcap[Vect_Num],Int_Num,0,0);
}
/* End Function:RVM_Reg_Int **************************************************/

/* Begin Function:RVM_Del_Int *************************************************
Description : Delete one interrupt line.
Input       : cnt_t Int_ID - The interrupt registration ID returned by RVM_Reg_Int.
Output      : None.
Return      : ret_t - If successful, 0; else an error code.
******************************************************************************/
ret_t RVM_Del_Int(cnt_t Int_ID)
{
    return RVM_Hypercall(RVM_HYP_DELEVT,Int_ID,0,0,0);
}
/* End Function:RVM_Del_Int **************************************************/

/* Begin Function:RVM_Reg_Evt *************************************************
Description : Register an event channel for the current virtual machine.
Input       : ptr_t Int_Num - The interrupt number for this channel.
              ptr_t VMID - The sender's VMID. Only the VM with this ID is allowed to
                           send to the channel.
Output      : None.
Return      : ret_t - If successful, 0; else an error code.
******************************************************************************/
ret_t RVM_Reg_Evt(ptr_t Int_Num, ptr_t VMID)
{
    return RVM_Hypercall(RVM_HYP_REGEVT,Int_Num,VMID,0,0);
}
/* End Function:RVM_Reg_Evt **************************************************/

/* Begin Function:RMP_Del_Evt *************************************************
Description : Delete an event channel previously created.
Input       : ptr_t Evt_ID - The event channel to delete. The event channel must be
                             created by this VM.
Output      : None.
Return      : ret_t - If successful, 0; else an error code.
******************************************************************************/
ret_t RMP_Del_Evt(cnt_t Evt_ID)
{
    return RVM_Hypercall(RVM_HYP_DELEVT,Evt_ID,0,0,0);
}
/* End Function:RMP_Del_Evt **************************************************/

/* Begin Function:RVM_Wait_Evt ************************************************
Description : Wait for an event. The interrupt must be enabled for this VM.
Input       : None.
Output      : None.
Return      : ret_t - If successful, 0; else an error code.
******************************************************************************/
ret_t RVM_Wait_Evt(void)
{
    return RVM_Hypercall(RVM_HYP_WAITEVT,0,0,0,0);
}
/* End Function:RVM_Wait_Evt *************************************************/

/* Begin Function:RVM_Send_Evt ************************************************
Description : Send an event to the event channel. This VM must have permissions
              to send to that channel.
Input       : ptr_t Evt_ID - The event channel to send to.
Output      : None.
Return      : ret_t - If successful, 0; else an error code.
******************************************************************************/
ret_t RVM_Send_Evt(ptr_t Evt_ID)
{
    return RVM_Hypercall(RVM_HYP_SENDEVT,Evt_ID,0,0,0);
}
/* End Function:RVM_Send_Evt *************************************************/

/* Begin Function:RVM_Query_Evt ***********************************************
Description : See if there is a channel in the target VM that this VM can send to.
Input       : None.
Output      : None.
Return      : ret_t - If successful, the channel ID; else an error code.
******************************************************************************/
ret_t RVM_Query_Evt(ptr_t VMID)
{
    return RVM_Hypercall(RVM_HYP_QUERYEVT,VMID,0,0,0);
}
/* End Function:RVM_Query_Evt ************************************************/

/* Begin Function:RVM_Query_VM ************************************************
Description : See if there is a VM with this name. If there is, return its VM ID.
Input       : char* Name - The name of the VM.
Output      : None.
Return      : ret_t - If successful, 0; else an error code.
******************************************************************************/
ret_t RVM_Query_VM(char* Name)
{
    cnt_t Count;
    char* Name_Array;
    
    /* Pass the parameters */
    RVM_Param.Number=RVM_HYP_QUERYVM;
    Name_Array=(char*)(RVM_Param.Param);
    for(Count=0;Count<16;Count++)
    {
        if(Name[Count]=='\0')
            break;
        Name_Array[Count]=Name[Count];
    }
    
    /* Do the hypercall */
    _RVM_Hypercall();
    
    /* Return the result */
    return RVM_Param.Param[0];
}
/* End Function:RVM_Query_VM *************************************************/

/* Begin Function:RVM_Tim_Prog ************************************************
Description : See if there is a VM with this name. If there is, return its VM ID.
Input       : ptr_t Period - The new period of timer interrupts.
Output      : None.
Return      : ret_t - If successful, 0; else an error code.
******************************************************************************/
ret_t RVM_Tim_Prog(ptr_t Period)
{
    return RVM_Hypercall(RVM_HYP_TIMPROG,Period,0,0,0);
}
/* End Function:RVM_Tim_Prog *************************************************/

/* Begin Function:RVM_Print ***************************************************
Description : Trigger a print to the console. The print address is predetermined.
Input       : None.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t RVM_Print(void)
{
    return RVM_Hypercall(RVM_HYP_PRINT,0,0,0,0);
}
/* End Function:RVM_Print ****************************************************/

/* Begin Function:RVM_Yield ***************************************************
Description : Yield the current thread by triggering the PendSV.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Yield(void)
{
    RVM_Fetch_Or(RVM_Flag,0x02);
    
    /* We send a new trigger if the interrupt is not masked and we are not in interrupt */
    if(RVM_Int_Active==0)
    {
        if(RVM_Int_Enable!=0)
            _RVM_Yield();
        else
            RVM_Int_Pend=1;
    }
}
/* End Function:RVM_Yield ****************************************************/

/* Begin Function:RVM_HW_Int_Enable *******************************************
Description : Enable a hardware interrupt.
Input       : None.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t RVM_HW_Int_Enable(ptr_t Int_ID)
{
    if(Int_ID>=RVM_Img.Kcap_Num)
        return -1;
    
    return _RVM_Kern(Int_ID, RVM_CMX_KERN_INT(RVM_Img.Kcap[Int_ID]), RVM_CMX_INT_OP, RVM_CMX_INT_ENABLE);
}
/* End Function:RVM_HW_Int_Enable ********************************************/
    
/* Begin Function:RVM_HW_Int_Disable ******************************************
Description : Disable a hardware interrupt.
Input       : None.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t RVM_HW_Int_Disable(ptr_t Int_ID)
{
    if(Int_ID>=RVM_Img.Kcap_Num)
        return -1;
    
    return _RVM_Kern(Int_ID, RVM_CMX_KERN_INT(RVM_Img.Kcap[Int_ID]), RVM_CMX_INT_OP, RVM_CMX_INT_DISABLE);
}
/* End Function:RVM_HW_Int_Disable *******************************************/

/* Begin Function:RVM_HW_Int_Prio *********************************************
Description : Change the priority of a hardware (physical) interrupt.
Input       : None.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t RVM_HW_Int_Prio(ptr_t Int_ID, ptr_t Prio)
{
    if(Int_ID>=RVM_Img.Kcap_Num)
        return -1;
    
    return _RVM_Kern(Int_ID, RVM_CMX_KERN_INT(RVM_Img.Kcap[Int_ID]), RVM_CMX_INT_PRIO, Prio);
}
/* End Function:RVM_HW_Int_Prio **********************************************/

/* Begin Function:RVM_Get_Int *************************************************
Description : Get the interrupt number to handle. After returning the vector, clean
              up the corresponding bit.
Input       : None.
Output      : None.
Return      : ret_t - If there is interrupt pending, the interrupt number; else -1.
******************************************************************************/
ret_t RVM_Get_Int(void)
{
    cnt_t Count;
    cnt_t Pos;
    
    /* See if interrupt enabled */
    if(RVM_Int_Enable==0)
        return -1;
    
    /* See which one is ready, and pick it */
    Pos=-1;
    for(Count=RVM_VECT_BITMAP-1;Count>=0;Count--)
    {
        if(RVM_Flag[Count]==0)
            continue;
        
        Pos=RVM_MSB_Get(RVM_Flag[Count]);
        Pos+=(Count<<RVM_WORD_ORDER);
        break;
    }
    /* Now kill the bit */
    if(Pos>=0)
        RVM_Fetch_And(&RVM_Flag[Count],~(((ptr_t)1)<<Pos));
    
    return Pos;
}
/* End Function:RVM_Get_Int **************************************************/

/* Begin Function:RVM_Int *****************************************************
Description : The interrupt entry of RMP.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Int(void)
{
    cnt_t Int_Num;
    
    while(1)
    {
        RVM_Int_Rcv();
        /* Only try to get interrupts if we didn't mask it */
        if(RVM_Int_Enable!=0)
        {
            RVM_Int_Pend=0;
            /* Look for interrupts to handle from the first */
            Int_Num=RVM_Get_Int();
            /* Handle the interrupt here - the interrupt is tail-chained */
            while(Int_Num>=0)
            {
                if(RVM_Vect[Int_Num]!=0)
                {
                    RVM_Int_Active=1;
                    ((void(*)(ptr_t))RVM_Vect[Int_Num])(Int_Num);
                    RVM_Int_Active=0;
                }
                Int_Num=RVM_Get_Int();
            }
        }
        else
            RVM_Int_Pend=1;
    }
}
/* End Function:RVM_Int ******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
