/******************************************************************************
Filename    : a7m_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the ARMv7-M platform generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __A7M_GEN_DEF__
#define __A7M_GEN_DEF__
/*****************************************************************************/
/* Endianness */
#define A7M_END_LITTLE                          (0)
#define A7M_END_BIG                             (1)

/* MPU flags */
#define A7M_RBAR_VALID                          POW2(4)
#define A7M_RASR_XN                             POW2(28)
#define A7M_RASR_C                              POW2(17)
#define A7M_RASR_B                              POW2(16)
#define A7M_RASR_RO                             (2<<24)
#define A7M_RASR_RW                             (3<<24)
#define A7M_RASR_SIZE(X)                        ((X-1)<<1)
#define A7M_RASR_ENABLE                         (1)

/* Minimum alignment requirements for A7M - for memory trunks only, not for kernel objects */
#define A7M_MEM_ALIGN                           (0x20)

#define A7M_WORD_BITS                           (32)
#define A7M_INIT_NUM_ORD                        (0)

/* A7M kernel object size */
#define A7M_RAW_PGT_SIZE_NOM(O)             	((4+POW2(O))*4)
#define A7M_RAW_PGT_SIZE_TOP(O, R)          	(((1+(R)*2)*4)+A7M_RAW_PGT_SIZE_NOM(O))
#define A7M_RAW_HYP_SIZE                        (84)
#define A7M_RAW_REG_SIZE                        (48)
#define A7M_RAW_REG_FPU_SIZE                    (112)
#define A7M_RAW_INV_SIZE                        (36)
/*****************************************************************************/
/* __A7M_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __A7M_GEN_CLASS__
#define __A7M_GEN_CLASS__
/*****************************************************************************/
/* A7M-specific information */
class A7M_Gen:public Plat_Gen
{
private:
    ptr_t Pgt_Total_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t* Base);
    ptr_t Pgt_Num_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t Total_Order, ptr_t Base);
    void Page_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                  std::unique_ptr<class Pgtbl>& Pgt);
    void Pgdir_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                   class Process* Owner, std::unique_ptr<class Pgtbl>& Pgt, ptr_t& Total_Static);

public:
    /* void */ A7M_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual ptr_t Mem_Align(ptr_t Base, ptr_t Size, ptr_t Align_Order) final override;
    virtual std::unique_ptr<class Pgtbl> Pgt_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                 class Process* Owner, ptr_t Total_Max, ptr_t& Total_Static) final override;
    virtual std::unique_ptr<std::vector<ptr_t>> Pgt_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                        ptr_t& Total_Static) final override;

    virtual ptr_t Raw_Pgt(ptr_t Size_Order, ptr_t Is_Top) final override;
    virtual ptr_t Raw_Thread(void) final override;
    virtual ptr_t Raw_Invocation(void) final override;
    virtual ptr_t Raw_Register(const std::vector<std::string>& Coprocessor) final override;

    virtual void Kernel_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Monitor_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Process_Main_Hdr(std::unique_ptr<std::vector<std::string>>& List, const class Process* Prc) final override;
};
/*****************************************************************************/
/* __A7M_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
