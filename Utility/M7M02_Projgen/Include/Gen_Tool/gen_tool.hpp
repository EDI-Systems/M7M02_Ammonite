/******************************************************************************
Filename    : gen_tool.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the generation tool class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __GEN_TOOL_DEF__
#define __GEN_TOOL_DEF__
/*****************************************************************************/
#define MACRO_ADD               (0)
#define MACRO_REPLACE           (1)

/* Main ID and slot ID extraction/stringify */
#define MID(X)                  ((X)/this->Plat->Plat->Captbl_Max)
#define MIDS(X)                 std::to_string(MID(X))
#define SID(X)                  ((X)%this->Plat->Plat->Captbl_Max)
#define SIDS(X)                 std::to_string(SID(X))

/* Get total capability table number */
#define CTNUM(TOTAL)            ROUND_DIV(TOTAL, this->Plat->Plat->Captbl_Max)
/* Get current capability table size */
#define CTSIZE(TOTAL, NUM)      (((TOTAL)>=(((NUM)+1)*(this->Plat->Plat->Captbl_Max)))? \
                                 (this->Plat->Plat->Captbl_Max): \
                                 ((TOTAL)%(this->Plat->Plat->Captbl_Max)))
/*****************************************************************************/
/* __GEN_TOOL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __GEN_TOOL_CLASS__
#define __GEN_TOOL_CLASS__
/*****************************************************************************/
/* Generation toolset information */
class Gen_Tool
{
public:
    /* Platform-specific generator */
    std::unique_ptr<class Plat_Gen> Plat;

    /* Buildsystem-specific generator */
    std::vector<std::unique_ptr<class Build_Gen>> Build;
    std::map<std::string,class Build_Gen*> Build_Map;

    /* Toolchain-specific generator */
    std::vector<std::unique_ptr<class Tool_Gen>> Tool;
    std::map<std::string,class Tool_Gen*> Tool_Map;

    /* Guest OS-specific tool */
    std::vector<std::unique_ptr<class Guest_Gen>> Guest;
    std::map<std::string,class Guest_Gen*> Guest_Map;

    /* Initialization numbers - these must be remembered for generation */
    ptr_t Cpt_Init_Total;
    ptr_t Cpt_Kfn_Total;
    ptr_t Pgt_Con_Total;

    /* void */ Gen_Tool(const std::string& Name,
                        class Proj_Info* Proj, class Plat_Info* Plat, class Chip_Info* Chip);

    void Build_Load(const std::string& Name);
    void Tool_Load(const std::string& Name);
    void Guest_Load(const std::string& Name);

    static std::unique_ptr<std::vector<std::string>> Line_Read(const std::string& Path);
    static void Line_Write(std::unique_ptr<std::vector<std::string>>& List, const std::string& Path);

    static void Macro_String(std::unique_ptr<std::vector<std::string>>& List,
                             const std::string& Macro, const std::string& Value, ptr_t Mode);
    static void Macro_Hex(std::unique_ptr<std::vector<std::string>>& List,
                          const std::string& Macro, ptr_t Value, ptr_t Mode);
    static void Macro_Int(std::unique_ptr<std::vector<std::string>>& List,
                          const std::string& Macro, ptr_t Value, ptr_t Mode);

    static void Src_Head(std::unique_ptr<std::vector<std::string>>& List,
                         const std::string& Name, const std::string& Desc);
    static void Src_Foot(std::unique_ptr<std::vector<std::string>>& List);
    static void Func_Head(std::unique_ptr<std::vector<std::string>>& List,
                          const std::string& Name,
                          const std::string& Description,
                          const std::vector<std::string>& Input,
                          const std::vector<std::string>& Output,
                          const std::string& Return);
    static void Func_Foot(std::unique_ptr<std::vector<std::string>>& List, const std::string& Name);
    static void Path_Conv(const std::string& Root, std::vector<std::string>& List);

    void Kernel_Inc(std::unique_ptr<std::vector<std::string>>& List);
    void Kernel_Conf_Hdr(void);
    void Kernel_Boot_Hdr(void);
    void Kernel_Boot_Src(void);
    void Kernel_Hook_Src(void);
    void Kernel_Handler_Src(void);
    void Kernel_Linker(void);
    void Kernel_Proj(void);

    void Monitor_Inc(std::unique_ptr<std::vector<std::string>>& List);
    void Monitor_Conf_Hdr(void);
    ptr_t Monitor_Cpt_Init(std::unique_ptr<std::vector<std::string>>& List, ptr_t Is_Kfn);
    ptr_t Monitor_Pgt_Con(std::unique_ptr<std::vector<std::string>>& List, const class Pgtbl* Pgt);
    ptr_t Monitor_Pgt_Add(std::unique_ptr<std::vector<std::string>>& List, const class Pgtbl* Pgt, ptr_t Init_Size_Ord);
    ptr_t Monitor_Thd_Init(std::unique_ptr<std::vector<std::string>>& List);
    ptr_t Monitor_Inv_Init(std::unique_ptr<std::vector<std::string>>& List);
    void Monitor_Boot_Hdr(void);
    void Monitor_Main_Crt(std::unique_ptr<std::vector<std::string>>& List, ptr_t Number, const std::string& Macro);
    void Monitor_Boot_Src(void);
    void Monitor_Hook_Src(void);
    void Monitor_Linker(void);
    void Monitor_Proj(void);

    void Process_Inc(std::unique_ptr<std::vector<std::string>>& List, class Process* Prc);
    void Process_Main_Hdr_Mem(std::unique_ptr<std::vector<std::string>>& List, const class Mem_Info* Mem);
    void Process_Main_Hdr(class Process* Prc);
    void Process_Entry_Src(class Process* Prc);
    void Process_Desc_Src(class Process* Prc);
    void Process_Main_Src(class Process* Prc);
    void Process_Virt_Hdr(class Virtual* Virt);
    void Process_Virt_Src(class Virtual* Virt);
    void Process_Linker(class Process* Prc);
    void Process_Proj(class Process* Prc);

    void Workspace_Proj(void);
};
/*****************************************************************************/
/* __GEN_TOOL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
