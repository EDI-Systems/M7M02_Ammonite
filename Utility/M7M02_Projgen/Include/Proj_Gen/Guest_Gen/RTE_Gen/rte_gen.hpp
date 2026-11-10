/******************************************************************************
Filename    : rte_gen.hpp
Author      : Evo-Devo Instrum.
Date        : 06/11/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the RTE (RTEMS-on-RVM) guest generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __RTE_GEN_DEF__
#define __RTE_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __RTE_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __RTE_GEN_CLASS__
#define __RTE_GEN_CLASS__
/*****************************************************************************/
/* RTE-specific information */
class RTE_Gen:public Guest_Gen
{
public:
    /* void */ RTE_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    /* RTEMS tree, toolchain prefix and in-tree application name. */
    std::string Root_Get(void);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
    virtual void Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                                const class Process* Prc, class Tool_Gen* Tool) final override;
};
/*****************************************************************************/
/* __RTE_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
