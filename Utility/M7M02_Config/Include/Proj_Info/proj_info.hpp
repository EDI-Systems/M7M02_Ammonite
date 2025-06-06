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

    /* Cache of the existing project's XML, for comparison */
    std::unique_ptr<class wxStringOutputStream> Cache;

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
    /* Workspace overwrite - output folder is always the project folder */
    ptr_t Workspace_Overwrite;

    /* Chip information */
    std::unique_ptr<class Chip> Chip;

    /* External memory declarations */
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

    /* The Native information */
    std::vector<std::unique_ptr<class Native>> Native;
    std::map<std::string,class Native*> Native_Map;

    /* The VM information */
    std::vector<std::unique_ptr<class Virtual>> Virtual;
    std::map<std::string,class Virtual*> Virtual_Map;

    /* void */ Proj_Info(void);
    /* void */ ~Proj_Info(void);

    /* Load defaults */
    ret_t Default_Load(const std::string& Path,
                       const Plat_Info* Plat_Info,
                       const Chip_Info* Chip_Info, const std::string& Chipname);
    /* Load existing project */
    ret_t Existing_Load(const std::string& Path);

    /* Save the project file */
    void Doc_Save(std::unique_ptr<class wxXmlDocument>& Document);
    ret_t Change_Detect(void);
    void Save(void);

    /* Path conversion */
    std::string Rel_Conv(const std::string& Relpath,ptr_t Type);

    ret_t Proc_Name_Dup_Check(const std::string& Name);

    ret_t Native_Add(const std::string& Name);
    ret_t Native_Remove(const std::string& Name);
    ret_t Native_Move(const std::string& Name, const std::string& After);
    ret_t Native_Rename(const std::string& Original,const std::string& Current);

    ret_t Virtual_Add(const std::string& Name);
    ret_t Virtual_Remove(const std::string& Name);
    ret_t Virtual_Move(const std::string& Name, const std::string& After);
    ret_t Virtual_Rename(const std::string& Original,const std::string& Current);
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
