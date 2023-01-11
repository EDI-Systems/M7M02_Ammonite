/******************************************************************************
Filename    : a6m_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the ARMv6-M platform generator.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __A6M_GEN_HPP_DEFS__
#define __A6M_GEN_HPP_DEFS__
/*****************************************************************************/
/* Endianness */
#define A6M_END_LITTLE                          (0)
#define A6M_END_BIG                             (1)

/* Minimum alignment requirements for A6M */
#define A6M_MEM_ALIGN                           (0x20)

#define A6M_WORD_BITS                           (32)
#define A6M_INIT_NUM_ORD                        (0)

/* A6M kernel object size */
#define A6M_RAW_PGT_SIZE_NOM(O)               	((5+POW2(O))*4)
#define A6M_RAW_PGT_SIZE_TOP(O, R)            	(((R)*2*4)+A6M_RAW_PGT_SIZE_NOM(O))
#define A6M_RAW_THD_SIZE                        (0x7C)
#define A6M_RAW_INV_SIZE                        (0x24)
#define A6M_RAW_REG_SIZE                        (0x2C)
/*****************************************************************************/
/* __A6M_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __A6M_GEN_HPP_CLASSES__
#define __A6M_GEN_HPP_CLASSES__
/*****************************************************************************/
/* A6M-specific information */
class A6M_Gen:public Plat_Gen
{
private:
    ptr_t Pgt_Total_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t* Base);
    ptr_t Pgt_Num_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t Total_Order, ptr_t Base);
    void Page_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                  std::unique_ptr<class Pgtbl>& Pgt);
    void Pgdir_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                   class Process* Owner, std::unique_ptr<class Pgtbl>& Pgt, ptr_t& Total_Static);

public:
    /* void */ A6M_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual ptr_t Mem_Align(ptr_t Base, ptr_t Size) final override;
    virtual std::unique_ptr<class Pgtbl> Pgt_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                   class Process* Owner, ptr_t Total_Max, ptr_t& Total_Static) final override;

    virtual ptr_t Raw_Pgt(ptr_t Size_Order, ptr_t Is_Top) final override;
    virtual ptr_t Raw_Thread(void) final override;
    virtual ptr_t Raw_Invocation(void) final override;
    virtual ptr_t Raw_Register(void) final override;

    virtual void Kernel_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Monitor_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Process_Main_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
};
/*****************************************************************************/
/* __A6M_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
