/******************************************************************************
Filename    : guest_gen.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the guest OS generator class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __GUEST_GEN_DEF__
#define __GUEST_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __GUEST_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __GUEST_GEN_CLASS__
#define __GUEST_GEN_CLASS__
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

    virtual void Process_Virt_Hdr(class Virtual* Virt)=0;
    virtual void Process_Virt_Src(class Virtual* Virt, class Tool_Gen* Tool)=0;
};
/*****************************************************************************/
/* __GUEST_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
