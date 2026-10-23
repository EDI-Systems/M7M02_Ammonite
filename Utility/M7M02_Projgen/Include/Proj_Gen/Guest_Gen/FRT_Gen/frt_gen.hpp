/******************************************************************************
Filename    : frt_gen.hpp
Author      : pry, ChatGLM
Date        : 18/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the FreeRTOS guest OS generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __FRT_GEN_DEF__
#define __FRT_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __FRT_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __FRT_GEN_CLASS__
#define __FRT_GEN_CLASS__
/*****************************************************************************/
/* FreeRTOS-specific information */
class FRT_Gen:public Guest_Gen
{
public:
    /* void */ FRT_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    /* Map the platform name onto the port directory prefix that follows the
     * official FreeRTOS port naming (ARM_A6M, ARM_A7M, RISC-V_RV32P). */
    std::string Plat_Get(void);
    /* Map the toolchain name onto the official FreeRTOS port directory name
     * (RVDS = armcc/armclang, IAR = ICCARM, GCC = everything else). */
    std::string Toolchain_Get(class Virtual* Virt);
    /* Map the toolchain and the platform onto the port directory. */
    std::string Port_Dir_Get(class Virtual* Virt);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
};
/*****************************************************************************/
/* __FRT_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
