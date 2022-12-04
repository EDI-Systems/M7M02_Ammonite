/******************************************************************************
Filename    : guest_gen.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the guest OS generator class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __GUEST_GEN_HPP_DEFS__
#define __GUEST_GEN_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __GUEST_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __GUEST_GEN_HPP_CLASSES__
#define __GUEST_GEN_HPP_CLASSES__
/*****************************************************************************/
/* Buildsystem generator information */
class Guest_Gen
{
public:
    /* Name */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* All info */
    class Proj_Info* Proj;
    class Plat_Info* Plat;
    class Chip_Info* Chip;

    /* void */ Guest_Gen(const std::string& Name,
                         class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);
    virtual /* void */ ~Guest_Gen(void){};
};
/*****************************************************************************/
/* __GUEST_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
