/******************************************************************************
Filename    : iarew_gen.hpp
Author      : pry ; Cline
Date        : 21/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the IAREW (IAR Embedded Workbench) buildsystem
              generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __IAREW_GEN_DEF__
#define __IAREW_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __IAREW_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __IAREW_GEN_CLASS__
#define __IAREW_GEN_CLASS__
/*****************************************************************************/
/* IAREW-specific information */
class IAREW_Gen:public Build_Gen
{
public:
    /* void */ IAREW_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual std::string Suffix(ptr_t Type) final override;

    void Raw_Proj(std::unique_ptr<std::vector<std::string>>& List,
                  const std::string& Postbuild,
                  const std::string& Target, const std::string& Optimization,
                  const std::vector<std::string>& Coprocessor,
                  const std::vector<std::string>& Include,
                  const std::vector<std::string>& Source,
                  const std::vector<std::string>& Library,
                  const std::string& Linker);

    virtual void Kernel_Proj(std::unique_ptr<std::vector<std::string>>& List,
                             const std::vector<std::string>& Include,
                             const std::vector<std::string>& Source,
                             const std::vector<std::string>& Library,
                             const std::vector<std::string>& Linker) final override;

    virtual void Monitor_Proj(std::unique_ptr<std::vector<std::string>>& List,
                              const std::vector<std::string>& Include,
                              const std::vector<std::string>& Source,
                              const std::vector<std::string>& Library,
                              const std::vector<std::string>& Linker) final override;

    virtual void Process_Proj(std::unique_ptr<std::vector<std::string>>& List,
                              const std::vector<std::string>& Include,
                              const std::vector<std::string>& Source,
                              const std::vector<std::string>& Library,
                              const std::vector<std::string>& Linker,
                              const class Process* Prc) final override;

    void Workspace_Sect(std::unique_ptr<std::vector<std::string>>& List,
                        const std::vector<std::string>& Project);

    virtual void Workspace_Proj(std::unique_ptr<std::vector<std::string>>& List,
                                const std::vector<std::string>& Kernel,
                                const std::vector<std::string>& Monitor,
                                const std::vector<std::string>& Native,
                                const std::vector<std::string>& Virtual) final override;
};
/*****************************************************************************/
/* __IAREW_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
