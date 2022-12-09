/******************************************************************************
Filename    : build_gen.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the buildsystem generator class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __BUILD_GEN_HPP_DEFS__
#define __BUILD_GEN_HPP_DEFS__
/*****************************************************************************/
#define BUILD_PROJECT            (0)
/*****************************************************************************/
/* __BUILD_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __BUILD_GEN_HPP_CLASSES__
#define __BUILD_GEN_HPP_CLASSES__
/*****************************************************************************/
/* Buildsystem generator information */
class Build_Gen
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

    /* void */ Build_Gen(const std::string& Name,
                         class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);
    virtual /* void */ ~Build_Gen(void){};

    virtual std::string Suffix(ptr_t Type)=0;

    virtual void Kernel_Proj(std::unique_ptr<std::vector<std::string>>& List,
                             const std::vector<std::string>& Include,
                             const std::vector<std::string>& Source,
                             const std::vector<std::string>& Linker)=0;

    virtual void Monitor_Proj(std::unique_ptr<std::vector<std::string>>& List,
                              const std::vector<std::string>& Include,
                              const std::vector<std::string>& Source,
                              const std::vector<std::string>& Linker)=0;
};
/*****************************************************************************/
/* __BUILD_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
