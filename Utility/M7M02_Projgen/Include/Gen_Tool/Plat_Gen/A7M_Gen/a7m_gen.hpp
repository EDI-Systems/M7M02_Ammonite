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
/* Endianness */
#define A7M_END_LITTLE                          (0)
#define A7M_END_BIG                             (1)

/* Minimum alignment requirements for A7M */
#define A7M_MEM_ALIGN                           (0x20)

#define A7M_WORD_BITS                           (32)
#define A7M_INIT_NUM_ORD                        (0)

/* A7M kernel object size */
#define A7M_RAW_PGTBL_SIZE_TOP_NOREGIONS(ORDER) (0x14+0x04+POW2(ORDER)*4)
#define A7M_RAW_PGTBL_SIZE_NOM(ORDER)           (0x14+POW2(ORDER)*4)
#define A7M_RAW_THD_SIZE                        (0x7C)
#define A7M_RAW_THD_FPU_SIZE                    (0xBC)
#define A7M_RAW_INV_SIZE                        (0x24)
#define A7M_RAW_REG_SIZE                        (0x28)
#define A7M_RAW_REG_FPU_SIZE                    (0x68)
#define A7M_RAW_PARAM_SIZE                      (0x2C)
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
    void Page_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                  std::unique_ptr<class Pgtbl>& Pgtbl);
    void Pgdir_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                   class Process* Owner, std::unique_ptr<class Pgtbl>& Pgtbl, ptr_t& Total_Static);

public:
    /* void */ A7M_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual void Compatible_Get(std::vector<std::tuple<std::string,std::string,std::string>>& List) final override;
    virtual ptr_t Mem_Align(ptr_t Base, ptr_t Size) final override;
    virtual std::unique_ptr<class Pgtbl> Pgtbl_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                   class Process* Owner, ptr_t Total_Max, ptr_t& Total_Static) final override;

    virtual ptr_t Raw_Pgtbl(ptr_t Size_Order, ptr_t Is_Top) final override;
    virtual ptr_t Raw_Thread(void) final override;
    virtual ptr_t Raw_Invocation(void) final override;
    virtual ptr_t Raw_Register(void) final override;
    virtual ptr_t Raw_Parameter(void) final override;

    virtual void Kernel_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Monitor_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
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
