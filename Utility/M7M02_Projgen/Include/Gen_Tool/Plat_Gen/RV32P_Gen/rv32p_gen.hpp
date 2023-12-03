/******************************************************************************
Filename    : rv32p_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the RISC-V 32-bit PMP platform generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __RV32P_GEN_DEF__
#define __RV32P_GEN_DEF__
/*****************************************************************************/
/* Endianness */
#define RV32P_END_LITTLE                   		(0)
#define RV32P_END_BIG                           (1)

/* Minimum alignment requirements for RV32P - for memory trunks only, not for kernel objects */
#define RV32P_MEM_ALIGN                         (0x10)

#define RV32P_WORD_BITS                         (32)
#define RV32P_INIT_NUM_ORD                      (0)

/* RV32P kernel object size */
#define RV32P_RAW_PGT_SIZE_NOM(O)               ((2+POW2(O))*4+ROUND_UP(POW2(O),4))
#define RV32P_RAW_PGT_SIZE_TOP(O, R)            ((4+ROUND_UP(R,4)+(R)*4)+RV32P_RAW_PGT_SIZE_NOM(O))
#define RV32P_RAW_HYP_SIZE                      (84)
#define RV32P_RAW_REG_SIZE                      (144)
#define RV32P_RAW_REG_RVF_SIZE                  (276)
#define RV32P_RAW_REG_RVD_SIZE                  (404)
#define RV32P_RAW_INV_SIZE                      (36)
/*****************************************************************************/
/* __RV32P_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __RV32P_GEN_CLASS__
#define __RV32P_GEN_CLASS__
/*****************************************************************************/
/* RV32P-specific information */
class RV32P_Gen:public Plat_Gen
{
private:
    ptr_t Pgt_Total_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t* Base);
    ptr_t Pgt_Num_Order(std::vector<std::unique_ptr<class Mem_Info>>& List, ptr_t Total_Order, ptr_t Base);
    void Page_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                  std::unique_ptr<class Pgtbl>& Pgt);
    void Pgdir_Map(std::vector<std::unique_ptr<class Mem_Info>>& List,
                   class Process* Owner, std::unique_ptr<class Pgtbl>& Pgt, ptr_t& Total_Static);

public:
    /* void */ RV32P_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual ptr_t Mem_Align(ptr_t Base, ptr_t Size, ptr_t Align_Order) final override;
    virtual std::unique_ptr<class Pgtbl> Pgt_Gen(std::vector<std::unique_ptr<class Mem_Info>>& List,
                                                   class Process* Owner, ptr_t Total_Max, ptr_t& Total_Static) final override;

    virtual ptr_t Raw_Pgt(ptr_t Size_Order, ptr_t Is_Top) final override;
    virtual ptr_t Raw_Thread(void) final override;
    virtual ptr_t Raw_Invocation(void) final override;
    virtual ptr_t Raw_Register(const std::vector<std::string>& Coprocessor) final override;

    virtual void Kernel_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Monitor_Conf_Hdr(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Process_Main_Hdr(std::unique_ptr<std::vector<std::string>>& List, const class Process* Prc) final override;
};
/*****************************************************************************/
/* __RV32P_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
