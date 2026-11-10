/******************************************************************************
Filename    : zpr_gen.hpp
Author      : Evo-Devo Instrum.
Date        : 08/11/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the ZPR (Zephyr-on-RVM) guest generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __ZPR_GEN_DEF__
#define __ZPR_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __ZPR_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __ZPR_GEN_CLASS__
#define __ZPR_GEN_CLASS__
/*****************************************************************************/
class ZPR_Gen:public Guest_Gen
{
public:
    /* void */ ZPR_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    std::string Root_Get(void);
    std::string Board_Get(void);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
    virtual void Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                                const class Process* Prc, class Tool_Gen* Tool) final override;
    virtual std::string Desc_Usr_Entry(void) final override;
};
/*****************************************************************************/
/* __ZPR_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
