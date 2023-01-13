/******************************************************************************
Filename    : proj_info.hpp
Author      : zjx
Date        : 03/09/2021
License     : Proprietary; confidential.
Description : Project information class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __PROJ_INFO_HPP_TYPES__
#define __PROJ_INFO_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __PROJ_INFO_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __PROJ_INFO_HPP_DEFS__
#define __PROJ_INFO_HPP_DEFS__
/*****************************************************************************/
/* Path type */
#define PATH_FILE                       (0)
#define PATH_DIR                        (1)
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
class Proj_Info
{
public:
    /* Path information */
    std::string Path;
    std::string Filename;
    std::string Fullpath;
    std::string Fulldir;

    /* The name of the project */
    std::string Name;
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
    //std::vector<std::unique_ptr<class Process>> Process;
    //std::map<std::string,class Process*> Process_Map;
    /* The virtual machine information - some processes are VMs */
    //std::vector<class Virtual*> Virtual;

    /* void */ Proj_Info(void);
    /* void */ ~Proj_Info(void);

    /* Load up the project file */
    ret_t Create(const std::string& Path,
                 const std::string& Chip_Class, const std::string& Chip_Name,
                 const std::string& Kernel_Root, const std::string& Monitor_Root);
    ret_t Load(const std::string& Path);

    /* Save the project file */
    ret_t Save(void);

    /* Path conversion */
    std::string Rel_Conv(const std::string& Relpath,ptr_t Type);
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
