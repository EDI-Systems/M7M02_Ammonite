/***************************************************************************** 
Filename    : RTT_Gen.hpp
Author      : pry, Cline
Date        : 26/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the RT-Thread guest OS generator.
/******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __RTT_GEN_DEF__
#define __RTT_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __RTT_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __RTT_GEN_CLASS__
#define __RTT_GEN_CLASS__
/*****************************************************************************/
/* RT-Thread-specific information */
class RTT_Gen:public Guest_Gen
{
public:
    /* void */ RTT_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    /* Map the platform name onto the RT-Thread libcpu architecture directory name. */
    std::string Plat_Get(void);
    /* Map the toolchain name onto the RT-Thread compiler file suffix. */
    std::string Toolchain_Get(class Virtual* Virt);
    /* Map the toolchain and the platform onto the port directory of the
     * RT-Thread tree. */
    std::string Port_Dir_Get(class Virtual* Virt);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
};
/*****************************************************************************/
/* __RTT_GEN_CLASS__ */
#endif
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
