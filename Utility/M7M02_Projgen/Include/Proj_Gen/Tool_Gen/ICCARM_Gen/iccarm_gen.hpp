/******************************************************************************
Filename    : iccarm_gen.hpp
Author      : pry ; Cline
Date        : 21/10/2026
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the IAR ICCARM toolchain generator.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __ICCARM_GEN_DEF__
#define __ICCARM_GEN_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __ICCARM_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __ICCARM_GEN_CLASS__
#define __ICCARM_GEN_CLASS__
/*****************************************************************************/
/* ICCARM-specific information */
class ICCARM_Gen:public Tool_Gen
{
public:
    /* void */ ICCARM_Gen(class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    virtual std::string Suffix(ptr_t Type) final override;

    virtual void Kernel_Linker(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Monitor_Linker(std::unique_ptr<std::vector<std::string>>& List) final override;
    virtual void Process_Linker(std::unique_ptr<std::vector<std::string>>& List,
                                const class Process* Prc) final override;
};
/*****************************************************************************/
/* __ICCARM_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
