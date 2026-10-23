/****************************************************************************** 
Filename    : UO3_Gen.hpp
Author      : pry, Cline
Date        : 22/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the uC/OS-III guest OS generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __UO3_GEN_DEF__
#define __UO3_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __UO3_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __UO3_GEN_CLASS__
#define __UO3_GEN_CLASS__
/*****************************************************************************/
/* uC/OS-III-specific information */
class UO3_Gen:public Guest_Gen
{
public:
    /* void */ UO3_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    /* Map the platform name onto the uC/OS-III architecture directory name. */
    std::string Plat_Get(void);
    /* Map the toolchain name onto the Micrium compiler directory name. */
    std::string Toolchain_Get(class Virtual* Virt);
    /* Map the toolchain and the platform onto the OS-side and the CPU-side
     * port directories of the uC/OS-III tree. */
    std::pair<std::string, std::string> Port_Dir_Get(class Virtual* Virt);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
};
/*****************************************************************************/
/* __UO3_GEN_CLASS__ */
#endif
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
