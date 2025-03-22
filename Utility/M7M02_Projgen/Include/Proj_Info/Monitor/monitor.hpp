/******************************************************************************
Filename    : monitor.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the virtual machine monitor information.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __MONITOR_DEF__
#define __MONITOR_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __MONITOR_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __MONITOR_CLASS__
#define __MONITOR_CLASS__
/*****************************************************************************/
/* VMM information */
class Monitor
{
public:
    /* RVM code section base/size */
    ptr_t Code_Base;
    ptr_t Code_Size;
    std::unique_ptr<class Mem_Info> Code;
    /* RVM data section base/size */
    ptr_t Data_Base;
    ptr_t Data_Size;
    std::unique_ptr<class Mem_Info> Data;
    /* RVM stack size for all three daemons */
    ptr_t Init_Stack_Size;
    ptr_t Sftd_Stack_Size;
    ptr_t Vmmd_Stack_Size;
    /* Extra capability table size */
    ptr_t Extra_Captbl;
    /* Whether to use idle sleep */
    ptr_t Idle_Sleep_Enable;
    /* Virtual machine priorities */
    ptr_t Virt_Prio;
    /* Virtual machine events */
    ptr_t Virt_Event;
    /* Virtual machine mappings */
    ptr_t Virt_Map;

    /* Build system to use */
    std::string Buildsystem;
    /* Toolchain to use for that build system */
    std::string Toolchain;
    /* Compiler optimization level */
    std::string Optimization;
    /* Project output folder/overwrite */
    std::string Project_Output;
    ptr_t Project_Overwrite;
    /* Linker output folder */
    std::string Linker_Output;
    /* Configuration header output folder */
    std::string Config_Header_Output;
    /* Boot header/source output folder */
    std::string Boot_Header_Output;
    std::string Boot_Source_Output;
    /* Initialization hook source folder/overwrite */
    std::string Hook_Source_Output;
    ptr_t Hook_Source_Overwrite;

    /* Global captbl containing all sorts of kernel objects */
    std::vector<class Captbl*> Captbl;
    std::vector<class Pgtbl*> Pgtbl;
    std::vector<class Process*> Process;
    std::vector<class Thread*> Thread;
    std::vector<class Invocation*> Invocation;
    std::vector<class Port*> Port;
    std::vector<class Receive*> Receive;
    std::vector<class Send*> Send;
    std::vector<class Vect_Info*> Vector;
    std::vector<class Kfunc*> Kfunc;

    /* The ultimate capability table size */
    ptr_t Captbl_Size;
    /* Init daemon stack */
    ptr_t Init_Stack_Base;
    /* Safety daemon stack */
    ptr_t Sftd_Stack_Base;
    /* VMM daemon stack */
    ptr_t Vmmd_Stack_Base;
    /* Initial state for RVM setup */
    ptr_t Before_Cap_Front;
    ptr_t Before_Kom_Front;
    /* When we begin creating virtual machine endpoints */
    ptr_t Vep_Cap_Front;
    ptr_t Vep_Kom_Front;
    /* When we begin creating capability tables */
    ptr_t Cpt_Cap_Front;
    ptr_t Cpt_Kom_Front;
    /* When we begin creating page tables */
    ptr_t Pgt_Cap_Front;
    ptr_t Pgt_Kom_Front;
    /* When we begin creating processes */
    ptr_t Prc_Cap_Front;
    ptr_t Prc_Kom_Front;
    /* When we begin creating threads */
    ptr_t Thd_Cap_Front;
    ptr_t Thd_Kom_Front;
    /* When we begin creating invocations */
    ptr_t Inv_Cap_Front;
    ptr_t Inv_Kom_Front;
    /* When we begin creating receive endpoints */
    ptr_t Rcv_Cap_Front;
    ptr_t Rcv_Kom_Front;
    /* After the booting all finishes */
    ptr_t After_Cap_Front;
    ptr_t After_Kom_Front;

    /* Linker filename */
    std::string Linker_Filename;
    /* Project filename */
    std::string Project_Filename;

    /* void */ Monitor(xml_node_t* Root, ptr_t Code_Base, ptr_t Code_Size);

    void Mem_Alloc(ptr_t Kom_Order);
};
/*****************************************************************************/
/* __MONITOR_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
