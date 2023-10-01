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
    std::string Name_Upper;
    std::string Name_Lower;
    /* All info */
    class Proj_Info* Proj;
    class Plat_Info* Plat;
    class Chip_Info* Chip;

    /* void */ Plat_Gen(const std::string& Name,
                        class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);
    virtual /* void */ ~Plat_Gen(void) {};

    /* Align memory segments */
    virtual ptr_t Mem_Align(ptr_t Base, ptr_t Size, ptr_t Align_Order)=0;
    virtual std::unique_ptr<class Pgtbl> Pgt_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                 class Process* Owner, ptr_t Total_Max, ptr_t& Total_Static)=0;

    /* Get size of kernel objects */
    ptr_t Size_Cpt(ptr_t Slot);
    ptr_t Size_Pgt(ptr_t Size_Order, ptr_t Is_Top);
    ptr_t Size_Thread(void);
    ptr_t Size_Invocation(void);
    ptr_t Size_Register(void);
    /* These are to be filled by the underlying platform */
    virtual ptr_t Raw_Pgt(ptr_t Size_Order, ptr_t Is_Top)=0;
    virtual ptr_t Raw_Thread(void)=0;
    virtual ptr_t Raw_Invocation(void)=0;
    virtual ptr_t Raw_Register(void)=0;

    virtual void Kernel_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List)=0;
    virtual void Monitor_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List)=0;
    virtual void Process_Main_Hdr(std::unique_ptr<std::vector<std::string>>& List)=0;
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
