/******************************************************************************
Filename    : rvm_virtlib.c
Author      : pry
Date        : 11/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : Paravirtualization library implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_platform.h"
#include "Syslib/rvm_syslib.h"
#include "Virtlib/rvm_virtlib.h"
#undef __HDR_DEF__

#define __HDR_STRUCT__
#include "rvm_platform.h"
#include "Syslib/rvm_syslib.h"
#include "Virtlib/rvm_virtlib.h"
#undef __HDR_STRUCT__

#include "Virtlib/rvm_virtlib.h"

#define __HDR_PUBLIC__
#include "rvm_platform.h"
#include "Syslib/rvm_syslib.h"
#undef __HDR_PUBLIC__
/* End Include ***************************************************************/

/* Function:RVM_Prc_Evt_Snd ***************************************************
Description : Send to an event source from a process. This is only available in
              native processes and not available in virtual machines; for the
              latter, please use RVM_Hyp_Evt_Snd instead.
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
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
#if(RVM_VIRT_LIB_ENABLE!=0U)
void RVM_Virt_Init(void)
{
    /* Interrupt masked/disabled on boot, no vector pending */
    RVM_Int_Mask=1U;
    RVM_Int_Disable=1U;
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
                                  arguments and return nothing. If NULL is 
                                  supplied, we're deregistering this slot.
              rvm_ptr_t Vct_Attr - Interrupt attributes, can be either
                                   RVM_VCT_MASKABLE or RVM_VCT_TRANSPARENT.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Virt_Vct_Reg(rvm_ptr_t Vct_Num,
                           void (*Vct)(void),
                           rvm_ptr_t Vct_Attr)
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
    
    /* Avoid race condition when setting parameters */
    RVM_Handler.Vct[Vct_Num]=RVM_NULL;
    RVM_Handler.Vct_Attr[Vct_Num]=Vct_Attr;
    RVM_Handler.Vct[Vct_Num]=Vct;
    
    return 0;
}
/* End Function:RVM_Virt_Vct_Reg *********************************************/

/* Function:RVM_Virt_Vct_Trig *************************************************
Description : Trigger a certain designated virtual vector from within the VM.
              Can be used as a software interrupt handler.
Input       : rvm_ptr_t Vct_Num - The vector number to trigger.
Output      : None.
Return      : rvm_ret_t - If successful, 0; or an error code.
******************************************************************************/
rvm_ret_t RVM_Virt_Vct_Trig(rvm_ptr_t Vct_Num)
{
    /* See if the vector index is overrange */
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
    
    /* Can't send to a vector that is not assigned */
    if(RVM_Handler.Vct[Vct_Num]==RVM_NULL)
    {
        RVM_COV_MARKER();
        
        return RVM_ERR_NULL;
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
        
        /* If interrupt is altogether disabled, don't send at all */
        if(RVM_Int_Disable!=0U)
        {
            RVM_COV_MARKER();
            
            RVM_Vct_Pend=1U;
        }
        /* If interrupt is not disabled, but is masked, see if the target
         * is transparent; if it is, we send, if it is not, we still pend */
        else
        {
            RVM_COV_MARKER();
            
            if(RVM_Int_Mask!=0U)
            {
                RVM_COV_MARKER();
                
                /* Maskable, don't send */
                if(RVM_Handler.Vct_Attr[Vct_Num]==RVM_VCT_MASKABLE)
                {
                    RVM_COV_MARKER();
                    
                    RVM_Vct_Pend=1U;
                }
                /* Transparent, send regardless */
                else
                {
                    RVM_COV_MARKER();
                    
                    RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VCT,1U)==0);
                }
            }
            else
            {
                RVM_COV_MARKER();
                
                RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VCT,1U)==0);
            }
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
    if((RVM_Int_Disable==0U)&&(RVM_Vct_Pend!=0U))
    {
        RVM_COV_MARKER();
        
        RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VCT,1U)==0);
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
    
    /* Trigger if the interrupt is enabled, not masked and we are not in interrupt */
    if(RVM_STATE->Vct_Act==0U)
    {
        RVM_COV_MARKER();
        
        if((RVM_Int_Disable==0U)&&(RVM_Int_Mask==0U))
        {
            RVM_COV_MARKER();
            
            RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VCT,1U)==0);
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
    RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_HYP,1U)==0);
    
    /* The call must be processed */
    RVM_ASSERT(Arg->Number==RVM_HYP_SPURIOUS);
    
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

/* Function:RVM_Hyp_Reboot ****************************************************
Description : Reboot the current virtual machine.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Hyp_Reboot(void)
{
    /* Must be successful */
    RVM_Hyp(RVM_HYP_REBOOT,0U,0U,0U,0U);
}
/* End Function:RVM_Hyp_Reboot ***********************************************/

/* Function:RVM_Hyp_Int_Ena ***************************************************
Description : Enable interrupts. This must be successful so it does not have
              a return value. This is more than a mere hypercall, because the 
              nesting relationships of RVM_Hyp_Int_Ena and RVM_Int_Mask have the
              following four cases:
              1. RVM_Hyp_Int_Dis - RVM_Int_Mask - RVM_Int_Unmask - RVM_Hyp_Int_Ena
                 This case is fine because after disabling interrupts no interrupts
                 could be pending after RVM_Int_Mask, and RVM_Hyp_Int_Ena does not
                 need to trigger Vct pending interrupt processing anyway.
              2. RVM_Int_Mask - RVM_Hyp_Int_Dis - RVM_Hyp_Int_Ena - RVM_Int_Unmask
                 This case is also fine because any pending interrupts between 
                 RVM_Int_Mask and RVM_Hyp_Int_Dis will be triggered for processing
                 at the final RVM_Int_Unmask.
              3. RVM_Hyp_Int_Dis - RVM_Int_Mask - RVM_Hyp_Int_Ena - RVM_Int_Unmask
                 This case is fine as well because it's impossible to have pending
                 interrupts at all.
              4. RVM_Int_Mask - RVM_Hyp_Int_Dis - RVM_Int_Unmask - RVM_Hyp_Int_Ena
                 This case would be the most problematic, and the RVM_Hyp_Int_Ena
                 at the end must be responsible for triggering Vct pending interrupt
                 processing. Hence, we must add this mechanism to RVM_Hyp_Int_Ena,
                 and to facilitate that we need another variable RVM_Int_Disable to
                 avoid consulting hypervisor for interrupt disabling state.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Hyp_Int_Ena(void)
{
    RVM_Int_Disable=0U;
    
    /* Must be successful */
    RVM_Hyp(RVM_HYP_INT_ENA,0U,0U,0U,0U);
    
    /* Trigger interrupt processing if there are pending ones. No race
     * conditions are possible between this and the RVM_Int_Unmask's
     * equivalent portion because one of them must be able to enter the
     * sending branch, thus no interrupts will be erronously delayed. 
     * Also note that here we don't check the mask because what were pended
     * could include transparent interrupts, which the mask cannot stop. */
    if(RVM_Vct_Pend!=0U)
    {
        RVM_COV_MARKER();
        
        RVM_ASSERT(RVM_Sig_Snd(RVM_SIG_VCT,1U)==0);
    }
    else
    {
        RVM_COV_MARKER();
        /* No operation required */
    }
}
/* End Function:RVM_Hyp_Int_Ena **********************************************/

/* Function:RVM_Hyp_Int_Dis ***************************************************
Description : Disable interrupts. This must be successful so it does not have
              a return value.
              A note on the RVM_Int_Disable in addition to the RVM_Int_Mask:
              If this is just a wrapper around the hypercall, then this alone
              just stops the interrupt injection, but does not stop the Vct
              thread from running software interrupts triggered within the VM
              (like context switch interrupts from RVM_Virt_Yield or regular
              ones from RVM_Virt_Vct_Trig). Thus, when interrupts are disabled,
              we need a variable to tell Vct to pend further interrupts should
              it receive any. RVM_Int_Mask can't be reused for this purpose,
              otherwise RVM_Hyp_Int_Dis will be able to unmask interrupts, which
              is not in accordance with the behavior of real microcontrollers,
              and we need an independent variable to remember this.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Hyp_Int_Dis(void)
{
    RVM_Int_Disable=1U;
    
    /* Must be successful */
    RVM_Hyp(RVM_HYP_INT_DIS,0U,0U,0U,0U);
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
              This function automatically filters out the interrupts that are
              maskable when the mask is set, and will set the pending flag
              accordingly.
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
                    
                    /* If interrupt is not masked, just return whatever we found, and if
                     * it is masked, we return only when the interrupt is transparent */
                    if((RVM_Int_Mask==0)||
                       ((RVM_Int_Mask!=0)&&(RVM_Handler.Vct_Attr[Pos]!=RVM_VCT_MASKABLE)))
                    {
                        RVM_COV_MARKER();
                        
                        /* Clear flag then return byte position */
                        ((volatile rvm_u8_t*)RVM_VCTF->Vct)[Pos]=0U;
                        return (rvm_ret_t)Pos;
                    }
                    /* If we don't send, just say that there is still a pending one */
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
            
            /* There must be a non-zero byte; at this point it must be pended */
            RVM_ASSERT(RVM_Vct_Pend!=0U);
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
        
        /* Try to get interrupts if interrupt enabled */
        if(RVM_Int_Disable==0U)
        {
            RVM_COV_MARKER();
            
            /* Indicate vector execution mode active */
            RVM_STATE->Vct_Act=1U;
            /* Clear the pending flag - set when we can't process immediately */
            RVM_Vct_Pend=0U;
            /* Look for interrupts to handle from the first vector number */
            Vct_Num=RVM_Vct_Get();
            
            /* Handle the vectors here - all vectors tail-chained, but lower number
             * ones will have higher priority due to scanning sequence of interrupts */
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
            
            if(RVM_Int_Mask==0U)
            {
                /* Handle timer interrupts if needed */
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
            }
            else
            {
                RVM_COV_MARKER();
                
                /* We will not respond to timer interrupts and context switch interrupts
                 * if our interrupt is masked, and when they are triggered, pend them */
                if((RVM_VCTF->Tim!=0U)||(RVM_VCTF->Ctx!=0U))
                {
                    RVM_COV_MARKER();
                    
                    RVM_Vct_Pend=1U;
                }
                else
                {
                    RVM_COV_MARKER();
                    /* No operation required */
                }
            }
            
            /* Deactivate once we finish */
            RVM_STATE->Vct_Act=0U;
        }
        else
        {
            RVM_COV_MARKER();
            
            /* Interrupt disabled altogether, will try to process later */
            RVM_Vct_Pend=1U;
        }
    }
}
#endif
/* End Function:RVM_Vct_Loop *************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

