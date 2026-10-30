/*****************************************************************************
Filename    : DJY_Gen.hpp
Author      : pry, Cline
Date        : 28/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the DjyOS guest OS generator.
/******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __DJY_GEN_DEF__
#define __DJY_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __DJY_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __DJY_GEN_CLASS__
#define __DJY_GEN_CLASS__
/*****************************************************************************/
/* DjyOS-specific information */
class DJY_Gen:public Guest_Gen
{
public:
    /* void */ DJY_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    /* Map the platform name onto the DjyOS bsp architecture directory name. */
    std::string Plat_Get(void);
    /* Map the toolchain name onto the DjyOS compiler file suffix. */
    std::string Toolchain_Get(class Virtual* Virt);
    /* Map the toolchain and the platform onto the port directory of the
     * DjyOS tree. */
    std::string Port_Dir_Get(class Virtual* Virt);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
};
/*****************************************************************************/
/* __DJY_GEN_CLASS__ */
#endif
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
