/******************************************************************************
Filename    : cross_gen.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the toolchain generator class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __TOOL_GEN_DEF__
#define __TOOL_GEN_DEF__
/*****************************************************************************/
#define TOOL_ASSEMBLER          (1)
#define TOOL_LINKER             (2)
/*****************************************************************************/
/* __TOOL_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __TOOL_GEN_CLASS__
#define __TOOL_GEN_CLASS__
/*****************************************************************************/
/* Toolchain generator information */
class Tool_Gen
{
public:
    /* Name */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* All info */
    class Proj_Info* Proj;
    class Plat_Info* Plat;
    class Chip_Info* Chip;

    /* void */ Tool_Gen(const std::string& Name,
                        class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);
    virtual /* void */ ~Tool_Gen(void){};

    virtual std::string Suffix(ptr_t Type)=0;

    virtual void Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List)=0;
    virtual void Monitor_Linker(std::unique_ptr<std::vector<std::string>>& List)=0;
    virtual void Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                                const class Process* Prc)=0;
};
/*****************************************************************************/
/* __TOOL_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
