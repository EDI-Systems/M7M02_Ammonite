/******************************************************************************
Filename    : gen_tool.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the generation tool class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __GEN_TOOL_HPP_DEFS__
#define __GEN_TOOL_HPP_DEFS__
/*****************************************************************************/
#define MACRO_ADD               (0)
#define MACRO_REPLACE           (1)

/* Captbl ID and object ID extraction/stringify */
#define CTID(X)                 ((X)/this->Plat->Plat->Captbl_Max)
#define CTIDS(X)                std::to_string(CTID(X))
#define OID(X)                  ((X)%this->Plat->Plat->Captbl_Max)
#define OIDS(X)                 std::to_string(OID(X))

/* Get current capability table size */
#define CTSIZE(TOTAL, NUM, MAX) (((TOTAL)>=(((NUM)+1)*(MAX)))?(MAX):((TOTAL)%(MAX)))
/*****************************************************************************/
/* __GEN_TOOL_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __GEN_TOOL_HPP_CLASSES__
#define __GEN_TOOL_HPP_CLASSES__
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
    void Kernel_Init_Src(void);
    void Kernel_Handler_Src(void);
    void Kernel_Linker(void);
    void Kernel_Proj(void);

    void Monitor_Inc(std::unique_ptr<std::vector<std::string>>& List);
    void Monitor_Conf_Hdr(void);
    void Monitor_Boot_Hdr(void);
    void Monitor_Boot_Src(void);
    void Monitor_Init_Src(void);
    void Monitor_Linker(void);
    void Monitor_Proj(void);
};

/* Generate capability table creations for kernel objects */
template<class OBJECT>
void RVM_Captbl_Crt_Gen(std::unique_ptr<std::vector<std::string>>& List,
                        const std::vector<OBJECT*>& Vector,
                        const std::string& Kobjname,
                        const std::string& Kobjmacro,
                        ptr_t Captbl_Max)
{
    ptr_t Obj_Cnt;
    ptr_t Captbl_Size;

    for(Obj_Cnt=0;Obj_Cnt<Vector.size();Obj_Cnt+=Captbl_Max)
    {
        Captbl_Size=CTSIZE(Vector.size(), Obj_Cnt, Captbl_Max);
        List->push_back(std::string("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CT")+Kobjmacro+"_"+
                        std::to_string(Obj_Cnt/Captbl_Max)+", Cur_Addr, "+std::to_string(Captbl_Size)+"U)==0U);");
        List->push_back(std::string("    RVM_LOG_SISUS(\"Init:Created ")+Kobjname+" capability table CID \", RVM_BOOT_CT"+Kobjmacro+"_"+
                        std::to_string(Obj_Cnt/Captbl_Max)+", \" @ address 0x\", Cur_Addr, \".\\r\\n\");");
        List->push_back(std::string("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(")+std::to_string(Captbl_Size)+"U));");
    }
}
/*****************************************************************************/
/* __GEN_TOOL_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
