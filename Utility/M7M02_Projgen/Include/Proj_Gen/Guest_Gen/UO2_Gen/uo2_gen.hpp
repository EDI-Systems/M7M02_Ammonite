/*****************************************************************************
Filename    : uo2_gen.hpp
Author      : pry, ds41f
Date        : 02/11/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the uC/OS-II guest OS generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __UO2_GEN_DEF__
#define __UO2_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __UO2_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __UO2_GEN_CLASS__
#define __UO2_GEN_CLASS__
/*****************************************************************************/
/* uC/OS-II-specific information */
class UO2_Gen:public Guest_Gen
{
public:
    /* void */ UO2_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    /* Map the platform name onto the uC/OS-II port directory name. */
    std::string Plat_Get(void);
    /* Map the toolchain name onto the uC/OS-II compiler file suffix. */
    std::string Toolchain_Get(class Virtual* Virt);
    /* Map the toolchain and the platform onto the port directory of the
     * uC/OS-II tree. */
    std::string Port_Dir_Get(class Virtual* Virt);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
};
/*****************************************************************************/
/* __UO2_GEN_CLASS__ */
#endif
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
