/******************************************************************************
Filename    : makefile_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the makefile buildsystem generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __MAKEFILE_GEN_DEF__
#define __MAKEFILE_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __MAKEFILE_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __MAKEFILE_GEN_CLASS__
#define __MAKEFILE_GEN_CLASS__
/*****************************************************************************/
/* Makefile-specific information */
class Makefile_Gen:public Build_Gen
{
public:
    /* void */ Makefile_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual std::string Suffix(ptr_t Type) final override;

   virtual void Makefile_Proj(std::unique_ptr<std::vector<std::string>>& List,
                              const std::string& After1,
                              const std::string& Target, const std::string& Optimization,
							  const std::vector<std::string>& Coprocessor,
                              const std::vector<std::string>& Include,
                              const std::vector<std::string>& Source,
                              const std::string& Linker);

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
                              const class Process* Prc) final override;

    virtual void Workspace_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                const std::vector<std::string>& Project) final override;
};
/*****************************************************************************/
/* __MAKEFILE_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
