/******************************************************************************
Filename    : plat_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the platform generator.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __PLAT_GEN_HPP_DEFS__
#define __PLAT_GEN_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __PLAT_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __PLAT_GEN_HPP_CLASSES__
#define __PLAT_GEN_HPP_CLASSES__
/*****************************************************************************/
/* Platform generator */
class Plat_Gen
{
public:
    /* Platform name */
    std::string Name;

    /* void */ Plat_Gen(const std::string& Name);
    virtual /* void */ ~Plat_Gen(void){};

    virtual void Compatible_Get(std::vector<std::tuple<std::string,std::string,std::string>>& List)=0;
    virtual ptr_t Mem_Align(ptr_t Base, ptr_t Size)=0;
    virtual std::unique_ptr<class Pgtbl> Pgtbl_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                   ptr_t Total_Max, ptr_t& Total_Static)=0;
};
/*****************************************************************************/
/* __PLAT_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
