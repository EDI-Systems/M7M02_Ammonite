/******************************************************************************
Filename    : monitor.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the virtual machine monitor information.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __MONITOR_HPP_DEFS__
#define __MONITOR_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __MONITOR_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __MONITOR_HPP_CLASSES__
#define __MONITOR_HPP_CLASSES__
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
    /* RVM stack size */
    ptr_t Stack_Size;
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
    /* Monitor source root folder for reference */
    std::string Monitor_Root;
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
    /* Kernel initialization source folder/overwrite */
    std::string Init_Source_Output;
    ptr_t Init_Source_Overwrite;

    /* void */ Monitor(xml_node_t* Root, ptr_t Code_Base, ptr_t Code_Size);
};
/*****************************************************************************/
/* __MONITOR_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
