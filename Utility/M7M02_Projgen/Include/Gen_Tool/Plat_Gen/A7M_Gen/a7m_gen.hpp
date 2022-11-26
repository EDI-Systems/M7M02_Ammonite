/******************************************************************************
Filename    : a7m_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the ARMv7-M platform generator.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __A7M_GEN_HPP_DEFS__
#define __A7M_GEN_HPP_DEFS__
/*****************************************************************************/
/* NVIC grouping */
#define A7M_NVIC_P0S8                           (7)    
#define A7M_NVIC_P1S7                           (6)
#define A7M_NVIC_P2S6                           (5)
#define A7M_NVIC_P3S5                           (4)
#define A7M_NVIC_P4S4                           (3)
#define A7M_NVIC_P5S3                           (2)
#define A7M_NVIC_P6S2                           (1)
#define A7M_NVIC_P7S1                           (0)
/* CPU type */
#define A7M_CPU_CM0P                            (0)
#define A7M_CPU_CM3                             (1)
#define A7M_CPU_CM4                             (2)
#define A7M_CPU_CM7                             (3)
/* FPU type */
#define A7M_FPU_NONE                            (0)
#define A7M_FPU_FPV4                            (1)
#define A7M_FPU_FPV5_SP                         (2)
#define A7M_FPU_FPV5_DP                         (3)
/* Endianness */
#define A7M_END_LITTLE                          (0)
#define A7M_END_BIG                             (1)

/* Minimum alignment requirements for A7M */
#define A7M_MEM_ALIGN                           (0x20)

#define A7M_WORD_BITS                           (32)
#define A7M_INIT_NUM_ORD                        (0)

/* A7M kernel object size */
#define A7M_RAW_THD_SIZE                        (0xBC)
#define A7M_RAW_INV_SIZE                        (0x24)
#define A7M_RAW_PGTBL_SIZE_TOP_NOREGIONS(ORDER) (0x14+0x04+POW2(ORDER)*4)
#define A7M_RAW_PGTBL_SIZE_NOM(ORDER)           (0x14+POW2(ORDER)*4)
/*****************************************************************************/
/* __A7M_GEN_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __A7M_GEN_HPP_CLASSES__
#define __A7M_GEN_HPP_CLASSES__
/*****************************************************************************/
/* A7M-specific information */
class A7M_Gen:public Plat_Gen
{
private:
    ptr_t Pgtbl_Total_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t* Base);
    ptr_t Pgtbl_Num_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t Total_Order, ptr_t Base);
    void Page_Map(std::vector<std::unique_ptr<class Mem_Info>>& List, std::unique_ptr<class Pgtbl>& Pgtbl);
    void Pgdir_Map(std::vector<std::unique_ptr<class Mem_Info>>& List, std::unique_ptr<class Pgtbl>& Pgtbl, ptr_t& Total_Static);

public:
    /* void */ A7M_Gen(void);

    virtual void Compatible_Get(std::vector<std::tuple<std::string,std::string,std::string>>& List) final override;
    virtual ptr_t Mem_Align(ptr_t Base, ptr_t Size) final override;
    virtual std::unique_ptr<class Pgtbl> Pgtbl_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                   ptr_t Total_Max, ptr_t& Total_Static) final override;
};
/*****************************************************************************/
/* __A7M_GEN_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
