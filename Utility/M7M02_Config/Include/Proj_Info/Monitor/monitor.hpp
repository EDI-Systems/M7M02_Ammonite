/******************************************************************************
Filename    : monitor.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Monitor class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __MONITOR_TYPE__
#define __MONITOR_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __MONITOR_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
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
class Monitor
{
public:
    /* Monitor source root folder */
    std::string Monitor_Root;
    /* Code size */
    ptr_t Code_Size;
    /* Data size */
    ptr_t Data_Size;
    /* Stack sizes */
    ptr_t Init_Stack_Size;
    ptr_t Sftd_Stack_Size;
    ptr_t Vctd_Stack_Size;
    ptr_t Timd_Stack_Size;
    ptr_t Hypd_Stack_Size;
    /* Extra capability table slots */
    ptr_t Extra_Captbl;
    /* Virtual machine priorities */
    ptr_t Virt_Prio;
    /* Number of virtual events */
    ptr_t Virt_Event;
    /* Number of virtual interrupt mappings */
    ptr_t Virt_Map;
    /* Buildsystem */
    std::string Buildsystem;
    /* Toolchain */
    std::string Toolchain;
    /* Optimization */
    ptr_t Optimization;
    /* Project output & overwrite */
    std::string Project_Output;
    ptr_t Project_Overwrite;
    /* Linker output */
    std::string Linker_Output;
    /* Config header output */
    std::string Config_Header_Output;
    /* Boot header/source output */
    std::string Boot_Header_Output;
    std::string Boot_Source_Output;
    /* Hook source & overwrite */
    std::string Hook_Source_Output;
    ptr_t Hook_Source_Overwrite;

    /* void */ Monitor(const std::string& Kernel_Root,
                       const class Plat_Info* Plat,
                       const class Plat_Info* Chip);
    /* void */ Monitor(class wxXmlNode* Node);
    /* void */ ~Monitor(void);

    void Save(class wxXmlNode* Parent);
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
