/******************************************************************************
Filename    : rvm_gen.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the mcu tool.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __RVM_GEN_TYPE__
#define __RVM_GEN_TYPE__
/*****************************************************************************/
typedef char                                s8_t;
typedef short                               s16_t;
typedef int                                 s32_t;
typedef long long                           s64_t;
typedef unsigned char                       u8_t;
typedef unsigned short                      u16_t;
typedef unsigned int                        u32_t;
typedef unsigned long long                  u64_t;
/* Make things compatible in 32-bit or 64-bit environments */
typedef s32_t                               pos_t;
typedef s32_t                               cnt_t;
typedef s64_t                               ret_t;
typedef u64_t                               ptr_t;
/*****************************************************************************/
/* __RVM_GEN_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __RVM_GEN_DEF__
#define __RVM_GEN_DEF__
/*****************************************************************************/
/* Error asserts */
#define ASSERT(X) \
do \
{ \
    if((X)==0) \
        Main::Error(std::string("M9999: Internal failure: ")+__DATE__+", "+std::string(__FILE__)+", "+std::to_string(__LINE__)+"."); \
} \
while(0)

/* Rounding macros */
#define ROUND_DOWN(X,R)                     ((X)/(R)*(R))
#define ROUND_UP(X,R)                       ROUND_DOWN((X)+(R)-1, R)
#define ROUND_DIV(X,R)                      (((X)+(R)-1)/(R))

/* Power of 2 macros */
#define POW2(POW)                           (((ptr_t)1)<<(POW))
#define ROUND_DOWN_POW2(X,POW)              (((X)>>(POW))<<(POW))
#define ROUND_UP_POW2(X,POW)                ROUND_DOWN_POW2((X)+POW2(POW)-1,POW)

#define ROUND_KOBJ(X)                       ROUND_UP_POW2(X,this->Proj->Kernel->Kom_Order)

/* The code generator author name */
#define CODE_AUTHOR                         "The RVM project generator."
/* The license for the generator */
#define CODE_LICENSE                        "Unlicense; see COPYING for details."
/* Generator macro alignment */
#define MACRO_ALIGN                         (56-4-8)
/* Buffer size for all temporary buffers */
#define BUF_SIZE                            (2048)

/* Maximum number of virtual events */
#define VIRT_EVENT_MAX                      (1024)

/* Priority limit for threads */
#define PRC_THD_PRIO_MIN                   (5)

/* Maximum file length allowed */
#define MAX_FILE_SIZE                       (16*1024*1024)

/* Path flags */
#define PATH_FILE                           (0)
#define PATH_DIR                            (1)
/*****************************************************************************/
/* __RVM_GEN_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __RVM_GEN_CLASS__
#define __RVM_GEN_CLASS__
/*****************************************************************************/
/* The application instance class */
class Main
{
public:
    char* Buffer;
    static std::string Time;

    static std::string Project_Input;
    static std::string Kernel_Root;
    static std::string Monitor_Root;
    static std::string Guest_RMP_Root;
    static std::string Guest_FRT_Root;
    static std::string Guest_RTT_Root;
    static std::string Guest_UO2_Root;
    static std::string Guest_UO3_Root;
    static std::string Workspace_Output;
    static std::string Report_Output;
    static ptr_t Verbose;
    static ptr_t Dryrun;
    static ptr_t Benchmark;

    /* XML database */
    std::unique_ptr<class Proj_Info> Proj;
    std::unique_ptr<class Chip_Info> Chip;
    std::unique_ptr<class Plat_Info> Plat;
    std::unique_ptr<class Proj_Gen> Gen;

    /* void */ Main(int argc, char* argv[]);

    void Proj_Parse(void);
    void Plat_Parse(void);
    void Chip_Parse(void);
    void Parse(void);

    void Standalone_Check(void);
    void Compatible_Check(void);
    void Config_Check(void);
    void Physical_Check(void);
    void Static_Check(void);
    void Reference_Check(void);
    void Genpath_Check(void);
    void Check(void);

    void Setup(void);

    void Mem_Align(void);

    void Code_Alloc(void);
    void Data_Alloc(void);
    void Device_Alloc(void);
    void Mem_Alloc(void);

    void Kobj_Init(void);
    void Shmem_Add(void);
    void Pgt_Alloc(void);

    void Cap_Alloc(void);
    void Cap_Link(void);

    void Kom_Alloc(ptr_t Init_Capsz);
    void Obj_Alloc(void);

    void Kernel_Gen(void);
    void Monitor_Gen(void);
    void Process_Gen(void);
    void Workspace_Gen(void);
    void Report_Gen(void);

    static std::string Vec2CSV(const std::vector<std::string>& Vector);

    static std::string XML_Get_String(xml_node_t* Root, const std::string& Name,
                                      const std::string& Errno0, const std::string& Errno1);
    static ptr_t XML_Get_Number(xml_node_t* Root, const std::string& Name,
                                const std::string& Errno0, const std::string& Errno1);
    static ptr_t XML_Get_Yesno(xml_node_t* Root, const std::string& Name,
                               const std::string& Errno0, const std::string& Errno1);
    static void XML_Get_CSV(xml_node_t* Root, const std::string& Name,
                            std::vector<std::string>& Vector,
                            const std::string& Errno0, const std::string& Errno1);
    static void XML_Get_KVP(xml_node_t* Root, const std::string& Name,
                            std::map<std::string,std::string>& Map,
                            const std::string& Errno0, const std::string& Errno1);

    static void Idtfr_Check(const std::string& Idtfr, const std::string& Name,
                            const std::string& Errno0, const std::string& Errno1);
    static std::string Path_Absolute(ptr_t Type, const std::string& Root, const std::string& Path);
    static std::string Path_Relative(ptr_t Type, const std::string& Root, const std::string& Path);
    static std::string File_Dir(const std::string& Path);
    static std::string File_Name(const std::string& Path);

    static std::string Hex(ptr_t Number);

    static void Upper(std::string& Str);
    static void Lower(std::string& Str);
    static ret_t Strcicmp(const std::string& Str1, const std::string& Str2);

    static void Info(const char* Format, ...);
    static void Info(const std::string& Format);
    static void Warning(const char* Format, ...);
    static void Warning(const std::string& Format);
    static void Error[[noreturn]](const char* Format, ...);
    static void Error[[noreturn]](const std::string& Format);
};

/* Name generation */
template <typename T>
void Name_Gen(T* This)
{
    This->Name_Lower=This->Name;
    Main::Lower(This->Name_Lower);
    This->Name_Upper=This->Name;
    Main::Upper(This->Name_Upper);
}

/* Name generation */
template <typename T>
void None_Filter(T& List)
{
    typename T::iterator Iter;

    Iter=List.begin();
    while(Iter!=List.end())
    {
        if(*Iter=="None")
            Iter=List.erase(Iter);
        else
            Iter++;
    }
}

/* XML trunk parsing */
template <typename CONT, typename ELEM>
void Trunk_Parse(xml_node_t* Root, const std::string& Section,
                 std::vector<std::unique_ptr<CONT>>& Vect,
                 const std::string& Errno0, const std::string& Errno1)
{
    xml_node_t* Trunk;
    xml_node_t* Temp;

    if((XML_Child(Root,(xml_s8_t*)Section.c_str(),&Trunk)<0)||(Trunk==0))
        Main::Error(std::string(Errno0)+": '"+Section+"' section is missing.");
    if(XML_Child(Trunk,0,&Temp)<0)
        Main::Error(std::string(Errno1)+": '"+Section+"' section parsing internal error.");
    while(Temp!=nullptr)
    {
        Vect.push_back(std::make_unique<ELEM>(Temp));
        if(XML_Child(Trunk,(xml_s8_t*)"",&Temp)<0)
            Main::Error(std::string(Errno1)+": '"+Section+"' section parsing internal error.");
    }
}
/* XML trunk parsing with one parameter */
template <typename CONT, typename ELEM, typename PARAM>
void Trunk_Parse_Param(xml_node_t* Root, const std::string& Section,
                       std::vector<std::unique_ptr<CONT>>& Vect, PARAM Param,
                       const std::string& Errno0, const std::string& Errno1)
{
    xml_node_t* Trunk;
    xml_node_t* Temp;

    if((XML_Child(Root,(xml_s8_t*)Section.c_str(),&Trunk)<0)||(Trunk==0))
        Main::Error(std::string(Errno0)+": '"+Section+"' section is missing.");
    if(XML_Child(Trunk,0,&Temp)<0)
        Main::Error(std::string(Errno1)+": '"+Section+"' section parsing internal error.");
    while(Temp!=nullptr)
    {
        Vect.push_back(std::make_unique<ELEM>(Temp,Param));
        if(XML_Child(Trunk,(char*)"",&Temp)<0)
            Main::Error(std::string(Errno1)+": '"+Section+"' section parsing internal error.");
    }
}

/* Duplicate field checking */
template<typename T>
std::string inline To_String(const T& t)
{
    return std::to_string(t);
}

std::string inline To_String(const char* t) {
    return t;
}

std::string inline To_String(const std::string& t) {
    return t;
}

template <typename ELEM, typename FIELD>
void Duplicate_Check(std::vector<std::unique_ptr<ELEM>>& Vect,
                     std::map<FIELD, ELEM*>& Map, FIELD (*Func)(std::unique_ptr<ELEM>& Elem),
                     const std::string& Errno, const std::string& Field, const std::string& Section)
{
    FIELD Key;

    for(std::unique_ptr<ELEM>& Var:Vect)
    {
        Key=Func(Var);
        if(Map.find(Key)==Map.end())
            Map.insert(std::pair<FIELD,ELEM*>(Key,Var.get()));
        else
            Main::Error(std::string(Errno)+": Duplicate "+Field+" '"+To_String(Key)+"' found in '"+Section+"' section.");
    }
}

template <typename STRING>
void Duplicate_Check(std::vector<STRING>& Vect, std::set<STRING>& Set,
                     const std::string& Errno, const std::string& Field, const std::string& Section)
{
    for(STRING& Var:Vect)
    {
        if(Set.find(Var)==Set.end())
            Set.insert(Var);
        else
            Main::Error(std::string(Errno)+": Duplicate "+Field+" '"+To_String(Var)+"' found in '"+Section+"' section.");
    }
}

template <typename STRING, typename PRC>
void Path_Check(const STRING& Path, PRC* Prc, std::map<STRING, PRC*>& Map,
                const std::string& Errno, const std::string& Field)
{
    std::string Abspath;
    typename std::map<STRING,PRC*>::iterator Iter;

    Abspath=Main::Path_Absolute(PATH_DIR,".",Path);
    Iter=Map.find(Abspath);
    if(Iter==Map.end())
        Map.insert(std::pair<STRING,PRC*>(Abspath,Prc));
    else
    {
        if(Iter->second!=Prc)
        {
            if(Iter->second==nullptr)
                Main::Error(std::string(Errno)+": Path '"+Field+"' has already been used in kernel or monitor.");
            else
                Main::Error(std::string(Errno)+": Path '"+Field+"' has already been used in process '"+Iter->second->Name+"'.");
        }
    }
}
/*****************************************************************************/
/* __RVM_GEN_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
