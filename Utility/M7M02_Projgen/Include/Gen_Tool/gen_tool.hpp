/******************************************************************************
Filename    : gen_tool.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the generation tool class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __GEN_TOOL_HPP_DEFS__
#define __GEN_TOOL_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __GEN_TOOL_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __GEN_TOOL_HPP_CLASSES__
#define __GEN_TOOL_HPP_CLASSES__
/*****************************************************************************/
/* Generation toolset information */
class Gen_Tool
{
public:
    /* Platform-specific generator */
    std::unique_ptr<class Plat_Gen> Plat;

    /* Buildsystem-specific generator */
    std::vector<std::unique_ptr<class Build_Gen>> Build;
    std::map<std::string,class Build_Gen*> Build_Map;

    /* Toolchain-specific generator */
    std::vector<std::unique_ptr<class Tool_Gen>> Tool;
    std::map<std::string,class Tool_Gen*> Tool_Map;

    /* Guest OS-specific tool */
    std::vector<std::unique_ptr<class Guest_Gen>> Guest;
    std::map<std::string,class Guest_Gen*> Guest_Map;

    /* void */ Gen_Tool(const std::string& Name,
                        class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    void Build_Load(const std::string& Name);
    void Tool_Load(const std::string& Name);
    void Guest_Load(const std::string& Name);
};
/*****************************************************************************/
/* __GEN_TOOL_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
