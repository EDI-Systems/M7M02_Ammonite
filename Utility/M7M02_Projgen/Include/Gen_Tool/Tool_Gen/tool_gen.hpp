/******************************************************************************
Filename    : tool_gen.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the toolchain generator class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __TOOL_GEN_HPP_DEFS__
#define __TOOL_GEN_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __TOOL_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __TOOL_GEN_HPP_CLASSES__
#define __TOOL_GEN_HPP_CLASSES__
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

    virtual std::string Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List)=0;
};
/*****************************************************************************/
/* __TOOL_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
