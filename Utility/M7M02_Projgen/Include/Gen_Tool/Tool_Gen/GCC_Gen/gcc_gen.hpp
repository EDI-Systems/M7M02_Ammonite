/******************************************************************************
Filename    : gcc_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the ARMv7-M platform generator.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __GCC_GEN_HPP_DEFS__
#define __GCC_GEN_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __GCC_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __GCC_GEN_HPP_CLASSES__
#define __GCC_GEN_HPP_CLASSES__
/*****************************************************************************/
/* GCC-specific information */
class GCC_Gen:public Tool_Gen
{
public:
    /* void */ GCC_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual std::string Suffix(ptr_t Type) final override;

    virtual void Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List) final override;
};
/*****************************************************************************/
/* __GCC_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
