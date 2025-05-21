/******************************************************************************
Filename    : process.hpp
Author      : pry
Date        : 15/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Generic process class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __PROCESS_TYPE__
#define __PROCESS_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __PROCESS_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __PROCESS_DEF__
#define __PROCESS_DEF__
/*****************************************************************************/
#define PROCESS_NATIVE      0
#define PROCESS_VIRTUAL     1
/*****************************************************************************/
/* __PROCESS_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PROCESS_CLASS__
#define __PROCESS_CLASS__
/*****************************************************************************/
class Process
{
public:
    /* The common items of Native and VM */
    /* Name */
    std::string Name;
    std::string Name_Upper;
    /* Extra_Captbl */
    ptr_t Extra_Captbl;
    /* Coprocessor */
    std::vector<std::string> Coprocessor;
    /* Buildsystem */
    std::string Buildsystem;
    /* Toolchain */
    std::string Toolchain;
    /* Optimization */
    ptr_t Optimization;
    /* Project output & overwrite */
    std::string Project_Output;
    ptr_t Project_Overwrite;
    /* Linker_Output */
    std::string Linker_Output;
    /* Main_Header_Output */
    std::string Main_Header_Output;
    /* Main_Source_Output */
    std::string Main_Source_Output;
    /* Memory */
    std::vector<std::unique_ptr<class Mem_Info>> Memory;
    /* Shmem */
    std::vector<std::unique_ptr<class Shmem>> Shmem;
    /* Send */
    std::vector<std::unique_ptr<class Send>> Send;
    /* Kfunc */
    std::vector<std::unique_ptr<class Kfunc>> Kfunc;


    /* void */ Process(const std::string& Name,
                       ptr_t Type,
                       const std::string& Root,
                       const class Plat_Info* Plat);
    /* void */ Process(class wxXmlNode* Node, ptr_t Type);
    /* Change */
    /* void */ Process(const std::string& Name);
    /* void */ ~Process(void);

    virtual void Save(class wxXmlNode* Parent)=0;
    //void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __PROCESS_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
