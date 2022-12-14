/******************************************************************************
Filename    : rmp_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the RMP guest OS generator.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __RMP_GEN_HPP_DEFS__
#define __RMP_GEN_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __RMP_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __RMP_GEN_HPP_CLASSES__
#define __RMP_GEN_HPP_CLASSES__
/*****************************************************************************/
/* RMP-specific information */
class RMP_Gen:public Guest_Gen
{
public:
    /* void */ RMP_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual void Process_Virt_Hdr(class Virtual* Virt) final override;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool) final override;
};
/*****************************************************************************/
/* __RMP_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
