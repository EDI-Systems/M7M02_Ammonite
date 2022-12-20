/******************************************************************************
Filename    : proj_info.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the project reader.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __PROJ_INFO_HPP_DEFS__
#define __PROJ_INFO_HPP_DEFS__
/*****************************************************************************/
#define RAW_CAPTBL_SIZE(BITS,NUM)               ((BITS)/8*8*(NUM))
/*****************************************************************************/
/* __PROJ_INFO_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __PROJ_INFO_HPP_CLASSES__
#define __PROJ_INFO_HPP_CLASSES__
/*****************************************************************************/
/* For parsing and storing project information */
class Proj_Info
{
public:
    /* The name of the project */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* The project version */
    std::string Version;
    /* Whether we assume all asserts to be correct */
    ptr_t Assert_Correct;
    /* Whether we enable debug prints */
    ptr_t Debug_Print;
    /* Workspace build system */
    std::string Buildsystem;
    /* Workspace output folder & overwrite */
    std::string Workspace_Output;
    ptr_t Workspace_Overwrite;
    /* Chip information */
    std::unique_ptr<class Chip> Chip;

    /* Externally mounted memory (EMIF) declarations */
    std::vector<std::unique_ptr<class Mem_Info>> Extmem;

    /* All memory trunk combined (Chip + EMIF) */
    std::vector<class Mem_Info*> Memory_Code;
    std::vector<class Mem_Info*> Memory_Data;
    std::vector<class Mem_Info*> Memory_Device;

    /* Shared memory declarations */
    std::vector<std::unique_ptr<class Mem_Info>> Shmem;
    std::map<std::string,class Mem_Info*> Shmem_Map;
    std::vector<class Mem_Info*> Shmem_Code;
    std::vector<class Mem_Info*> Shmem_Data;
    std::vector<class Mem_Info*> Shmem_Device;

    /* The RME kernel information */
    std::unique_ptr<class Kernel> Kernel;
    /* The RVM user-library information */
    std::unique_ptr<class Monitor> Monitor;
    /* The process information */
    std::vector<std::unique_ptr<class Process>> Process;
    std::map<std::string,class Process*> Process_Map;
    /* The virtual machine information - some processes are VMs */
    std::vector<class Virtual*> Virtual;

    /* Workspace filename */
    std::string Workspace_Filename;

    /* void */ Proj_Info(xml_node_t* Root);

    void Check(void);

    static ptr_t Flag_Alloc(ptr_t Source, ptr_t Wordlength, ptr_t Kmem_Order);
};
/*****************************************************************************/
/* __PROJ_INFO_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
