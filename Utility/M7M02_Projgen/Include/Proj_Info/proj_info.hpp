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
    /* The project version */
    std::string Version;
    /* Chip information */
    std::unique_ptr<class Chip> Chip;

    /* Externally mounted memory (EMIF) */
    std::vector<std::unique_ptr<class Mem_Info>> Extmem;
    std::vector<class Mem_Info*> Extmem_Code;
    std::vector<class Mem_Info*> Extmem_Data;
    std::vector<class Mem_Info*> Extmem_Device;

    /* Shared memory declarations */
    std::vector<std::unique_ptr<class Mem_Info>> Shmem;
    std::vector<class Mem_Info*> Shmem_Code;
    std::vector<class Mem_Info*> Shmem_Data;
    std::vector<class Mem_Info*> Shmem_Device;
    
    /* The RME kernel information */
    std::unique_ptr<class Kernel> Kernel;
    /* The RVM user-library information */
    std::unique_ptr<class Monitor> Monitor;
    /* The process information */
    std::vector<std::unique_ptr<class Process>> Process;
    /* The virtual machine information - some processes are VMs */
    std::vector<class Virtual*> Virtual;

    /* void */ Proj_Info(xml_node_t* Root);
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
