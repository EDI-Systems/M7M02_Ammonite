/******************************************************************************
Filename    : keil_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the Keil buildsystem generator.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __KEIL_GEN_HPP_DEFS__
#define __KEIL_GEN_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __KEIL_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __KEIL_GEN_HPP_CLASSES__
#define __KEIL_GEN_HPP_CLASSES__
/*****************************************************************************/
/* Keil-specific information */
class Keil_Gen:public Build_Gen
{
public:
    /* void */ Keil_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual std::string Suffix(ptr_t Type) final override;

    void Raw_Proj(std::unique_ptr<std::vector<std::string>>& List,
                  const std::string& After1, const std::string& After2,
                  const std::string& Target, const std::string& Optimization,
                  const std::vector<std::string>& Include,
                  const std::vector<std::string>& Source,
                  const std::string& Linker, const std::string& Linker_Misc);

    virtual void Kernel_Proj(std::unique_ptr<std::vector<std::string>>& List,
                             const std::vector<std::string>& Include,
                             const std::vector<std::string>& Source,
                             const std::vector<std::string>& Linker) final override;

    virtual void Monitor_Proj(std::unique_ptr<std::vector<std::string>>& List,
                              const std::vector<std::string>& Include,
                              const std::vector<std::string>& Source,
                              const std::vector<std::string>& Linker) final override;

    virtual void Process_Proj(std::unique_ptr<std::vector<std::string>>& List,
                              const std::vector<std::string>& Include,
                              const std::vector<std::string>& Source,
                              const std::vector<std::string>& Linker,
                              const class Process* Proc) final override;

    virtual void Workspace_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                const std::vector<std::string>& Project) final override;
};
/*****************************************************************************/
/* __KEIL_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
