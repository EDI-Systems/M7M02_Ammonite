/******************************************************************************
Filename    : rvm_virtlib.h
Author      : pry
Date        : 25/06/2017
Licence     : The Unlicense; see LICENSE for details.
Description : Paravirtualization library header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_VIRTLIB_DEF__
#define __RVM_VIRTLIB_DEF__
/*****************************************************************************/
#if(RVM_VIRT_LIB_ENABLE!=0U)
/* Register set space - might be changed by hypervisor */
#define RVM_REG                                     ((volatile struct RVM_Thd_Reg*)RVM_VIRT_REG_BASE)
/* State block space - might be changed by hypervisor */
#define RVM_STATE                                   ((volatile struct RVM_State*)RVM_VIRT_STATE_BASE)
/* Parameter block space (part of state block) */
#define RVM_PARAM                                   (&(RVM_STATE->Usr))
/* Vector space (part of state block) */
#define RVM_VCTF                                    (&(RVM_STATE->Flag))
/* Vector space words */
#define RVM_VCTF_WORD_SIZE                          (RVM_ROUND_UP(RVM_VIRT_VCT_NUM, RVM_WORD_ORDER-3U)/RVM_WORD_BYTE)

/* Virtualization signal endpoints */
/* Hypervisor signal endpoint */
#define RVM_SIG_HYP                                 (0U)
/* Vector signal endpoint */
#define RVM_SIG_VCT                                 (1U)
#endif
/*****************************************************************************/
/* __RVM_VIRTLIB_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Struct ********************************************************************/
#ifdef __HDR_STRUCT__
#ifndef __RVM_VIRTLIB_STRUCT__
#define __RVM_VIRTLIB_STRUCT__
/* We used structs in the header */

/* Use defines in these headers */
#define __HDR_DEF__
#undef __HDR_DEF__
/*****************************************************************************/
#if(RVM_VIRT_LIB_ENABLE!=0U)
/* Interrupt handlers */
struct RVM_Handler_Struct
{
    void (*Tim)(void);
    void (*Ctx)(void);
    void (*Vct[RVM_VIRT_VCT_NUM])(void);
};
#endif
/*****************************************************************************/
/* __RVM_VIRTLIB_STRUCT__ */
#endif
/* __HDR_STRUCT__ */
#endif
/* End Struct ****************************************************************/

/* Private Variable **********************************************************/
#if(!(defined __HDR_DEF__||defined __HDR_STRUCT__))
#ifndef __RVM_VIRTLIB_MEMBER__
#define __RVM_VIRTLIB_MEMBER__

/* In this way we can use the data structures and definitions in the headers */
#define __HDR_DEF__

#undef __HDR_DEF__

#define __HDR_STRUCT__

#undef __HDR_STRUCT__

/* If the header is not used in the public mode */
#ifndef __HDR_PUBLIC__
/*****************************************************************************/
#if(RVM_VIRT_LIB_ENABLE!=0U)
static volatile struct RVM_Handler_Struct RVM_Handler;
static volatile rvm_ptr_t RVM_Int_Mask;
static volatile rvm_ptr_t RVM_Int_Disable;
static volatile rvm_ptr_t RVM_Vct_Pend;
/* Paramater space location provided for assembly convenience */
volatile struct RVM_Param* const RVM_Usr_Param=RVM_PARAM;
#endif
/*****************************************************************************/
/* End Private Variable ******************************************************/

/* Private Function **********************************************************/ 
/*****************************************************************************/

/*****************************************************************************/
#define __RVM_EXTERN__
/* End Private Function ******************************************************/

/* Public Variable ***********************************************************/
/* __HDR_PUBLIC__ */
#else
#define __RVM_EXTERN__ RVM_EXTERN 
/* __HDR_PUBLIC__ */
#endif

/*****************************************************************************/

/*****************************************************************************/

/* End Public Variable *******************************************************/

/* Public Function ***********************************************************/
/*****************************************************************************/
#if(RVM_VIRT_LIB_ENABLE==0U)
/* Process event operation */
__RVM_EXTERN__ rvm_ret_t RVM_Prc_Evt_Snd(rvm_ptr_t Evt_Num);
#else
/* Virtual machine library call */
__RVM_EXTERN__ rvm_ret_t RVM_Virt_Vct_Reg(rvm_ptr_t Vct_Num,
                                          void (*Vct)(void));
__RVM_EXTERN__ rvm_ret_t RVM_Virt_Vct_Trig(rvm_ptr_t Vct_Num);
__RVM_EXTERN__ void RVM_Virt_Tim_Reg(void (*Tim)(void));
__RVM_EXTERN__ void RVM_Virt_Ctx_Reg(void (*Ctx)(void));
__RVM_EXTERN__ void RVM_Virt_Int_Mask(void);
__RVM_EXTERN__ void RVM_Virt_Int_Unmask(void);
__RVM_EXTERN__ void RVM_Virt_Yield(void);

/* Hypercall */
__RVM_EXTERN__ rvm_ret_t RVM_Hyp(rvm_ptr_t Number,
                                 rvm_ptr_t Param1,
                                 rvm_ptr_t Param2,
                                 rvm_ptr_t Param3,
                                 rvm_ptr_t Param4);
                                 
__RVM_EXTERN__ void RVM_Hyp_Putchar(char Char);

__RVM_EXTERN__ void RVM_Hyp_Reboot(void);

__RVM_EXTERN__ void RVM_Hyp_Int_Ena(void);
__RVM_EXTERN__ void RVM_Hyp_Int_Dis(void);

__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Vct_Phys(rvm_ptr_t Phys_Num,
                                          rvm_ptr_t Vct_Num);
__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Vct_Evt(rvm_ptr_t Evt_Num,
                                         rvm_ptr_t Vct_Num);
__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Vct_Del(rvm_ptr_t Vct_Num);
__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Vct_Lck(void);
__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Vct_Wait(void);

__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Evt_Add(rvm_ptr_t Evt_Num);
__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Evt_Del(rvm_ptr_t Evt_Num);
__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Evt_Snd(rvm_ptr_t Evt_Num);

__RVM_EXTERN__ rvm_ret_t RVM_Hyp_Wdg_Clr(void);

/* Vct thread dedicated operation */
__RVM_EXTERN__ void RVM_Virt_Init(void);
__RVM_EXTERN__ rvm_ret_t RVM_Vct_Get(void);
__RVM_EXTERN__ void RVM_Vct_Loop(void);
#endif
/*****************************************************************************/
/* Undefine "__RVM_EXTERN__" to avoid redefinition */
#undef __RVM_EXTERN__
/* __RVM_VIRTLIB_MEMBER__ */
#endif
/* !(defined __HDR_DEF__||defined __HDR_STRUCT__) */
#endif
/* End Public Function *******************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
