/******************************************************************************
Filename    : proj_info.hpp
Author      : zjx
Date        : 03/09/2021
License     : Proprietary; confidential.
Description : Project information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __PROJ_INFO_TYPE__
#define __PROJ_INFO_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __PROJ_INFO_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __PROJ_INFO_DEF__
#define __PROJ_INFO_DEF__
/*****************************************************************************/
/* Path type */
#define PATH_FILE                       (0)
#define PATH_DIR                        (1)
/*****************************************************************************/
/* __PROJ_INFO_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PROJ_INFO_CLASS__
#define __PROJ_INFO_CLASS__
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
    /* Whether we enable asserts */
    ptr_t Assert_Enable;
    /* Whether we enable debug prints */
    ptr_t Debug_Log_Enable;
    /* Whether we enable raw page tables */
    ptr_t Pgtbl_Raw_Enable;
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
/* __PROJ_INFO_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
