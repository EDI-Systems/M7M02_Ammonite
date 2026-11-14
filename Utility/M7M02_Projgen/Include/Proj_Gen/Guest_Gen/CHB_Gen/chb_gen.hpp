/*****************************************************************************
Filename    : chb_gen.hpp
Author      : Evo-Devo Instrum.
Date        : 2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the ChibiOS guest OS generator.
*******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __CHB_GEN_DEF__
#define __CHB_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __CHB_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __CHB_GEN_CLASS__
#define __CHB_GEN_CLASS__
/*****************************************************************************/
/* ChibiOS-specific information */
class CHB_Gen:public Guest_Gen
{
public:
    /* void */ CHB_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    /* Map the platform name onto the ChibiOS port directory name. */
    std::string Plat_Get(void);
    /* Map the toolchain name onto the ChibiOS port directory name. */
    std::string Toolchain_Get(class Virtual* Virt);
    /* Map the toolchain and the platform onto the port directory of the
     * ChibiOS tree. */
    std::string Port_Dir_Get(class Virtual* Virt);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
};
/*****************************************************************************/
/* __CHB_GEN_CLASS__ */
#endif
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
