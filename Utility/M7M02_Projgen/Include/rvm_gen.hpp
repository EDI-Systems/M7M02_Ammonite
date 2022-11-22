/******************************************************************************
Filename    : rme_mcu.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the mcu tool.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __RME_MCU_HPP_TYPES__
#define __RME_MCU_HPP_TYPES__
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
/* __RME_MCU_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __RME_MCU_HPP_DEFS__
#define __RME_MCU_HPP_DEFS__
/*****************************************************************************/
/* Error asserts */
#define ASSERT(X) \
do \
{ \
    if((X)==0) \
        Main::Error(std::string("M9999: Internal failure: ")+__DATE__+", "+std::to_string(__LINE__)+"."); \
} \
while(0)

/* Power of 2 macros */
#define POW2(POW)                           (((ptr_t)1)<<(POW))
#define ROUND_DOWN(X,POW)                   (((X)>>(POW))<<(POW))
#define ROUND_UP(X,POW)                     ROUND_DOWN((X)+POW2(POW)-1,POW)

/* The code generator author name */
#define CODE_AUTHOR                         "The RME project generator."
/* The license for the generator */
#define CODE_LICENSE                        "LGPL v3+; see COPYING for details."
/* Generator macro alignment */
#define MACRO_ALIGN                         (56)
/* Buffer size for all temporary buffers */
#define BUF_SIZE                            (1024)

/* Vector flag area size (in bytes), fixed across all architectures */
#define KERNEL_VCTF_SIZE                    (512)
/* Event flag area size (in bytes), fixed across all architectures */
#define KERNEL_EVTF_SIZE                    (512)
/* Entry point slot size (in words), fixed across all architectures */
#define ENTRY_SLOT_SIZE                     (8)

/* Priority limit for threads */
#define PROC_THD_PRIO_MIN                   (5)

/* Maximum file length allowed */
#define MAX_FILE_SIZE                       (16*1024*1024)
/*****************************************************************************/
/* __RME_MCU_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __RME_MCU_HPP_CLASSES__
#define __RME_MCU_HPP_CLASSES__
/*****************************************************************************/
/* The application instance class */
class Main
{
public:
    char* Buffer;
    std::string Time;

    std::string Input;
    static ptr_t Verbose;

    std::unique_ptr<class Proj_Info> Proj;
    std::unique_ptr<class Chip_Info> Chip;
    //std::unique_ptr<class Platform> Plat;

    Main(int argc, char* argv[]);

    void Parse(void);
    void Proj_Parse(void);
    void Chip_Parse(void);

    void Check(void);

    static std::string XML_Get_String(xml_node_t* Root, const char* Name,
                                      const char* Errno0, const char* Errno1);
    static ptr_t XML_Get_Number(xml_node_t* Root, const char* Name,
                                const char* Errno0, const char* Errno1);
    static ptr_t XML_Get_Yesno(xml_node_t* Root, const char* Name,
                               const char* Errno0, const char* Errno1);
    static void XML_Get_CSV(xml_node_t* Root, const char* Name,
                            std::vector<std::string>& Vector,
                            const char* Errno0, const char* Errno1);
    static void XML_Get_KVP(xml_node_t* Root, const char* Name,
                            std::map<std::string,std::string>& Map,
                            const char* Errno0, const char* Errno1);

    static void Idtfr_Check(const std::string& Idtfr, const char* Name,
                            const char* Errno0, const char* Errno1);

    static void Info(const char* Format, ...);
    static void Info(const std::string& Format);
    static void Error[[noreturn]](const char* Format, ...);
    static void Error[[noreturn]](const std::string& Format);
};


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
/*****************************************************************************/
/* __RME_MCU_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
