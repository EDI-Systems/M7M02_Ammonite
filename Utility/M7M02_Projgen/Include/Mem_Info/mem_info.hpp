/******************************************************************************
Filename    : mem_info.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the memory class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __MEM_INFO_HPP_DEFS__
#define __MEM_INFO_HPP_DEFS__
/*****************************************************************************/
/* Reference or concrete */
#define MEM_DECL            0
#define MEM_REF             1

/* Memory type - Code, Data, Device */
#define MEM_CODE            0
#define MEM_DATA            1
#define MEM_DEVICE          2

/* Memory access permissions */
#define MEM_READ            POW2(0)
#define MEM_WRITE           POW2(1)
#define MEM_EXECUTE         POW2(2)
#define MEM_BUFFER          POW2(3)
#define MEM_CACHE           POW2(4)
#define MEM_STATIC          POW2(5)
#define MEM_CODE_KERNEL     (MEM_READ|MEM_EXECUTE|MEM_BUFFER|MEM_CACHE|MEM_STATIC)
#define MEM_DATA_KERNEL     (MEM_READ|MEM_WRITE|MEM_BUFFER|MEM_CACHE|MEM_STATIC)
#define MEM_CODE_MONITOR    (MEM_READ|MEM_EXECUTE|MEM_BUFFER|MEM_CACHE|MEM_STATIC)
#define MEM_DATA_MONITOR    (MEM_READ|MEM_WRITE|MEM_BUFFER|MEM_CACHE|MEM_STATIC)
#define MEM_CODE_USER_LEAST (MEM_READ|MEM_EXECUTE|MEM_STATIC)
#define MEM_DATA_USER_LEAST (MEM_READ|MEM_WRITE|MEM_STATIC)
/* Memory placement */
#define MEM_AUTO            ((ptr_t)(-1LL))
/* Memmap granularity */
#define MAP_ALIGN           (32)
/*****************************************************************************/
/* __MEM_INFO_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __MEM_INFO_HPP_CLASSES__
#define __MEM_INFO_HPP_CLASSES__
/*****************************************************************************/
/* Memory trunk information */
class Mem_Info
{
public:
    /* A reference or a declaration */
    ptr_t Reference;
    /* The name of the memory trunk */
    std::string Name;
    /* The memory base */
    ptr_t Base;
    /* The size */
    ptr_t Size;
    /* Memory type */
    ptr_t Type;
    /* The attributes - read, write, execute, cacheable, bufferable, static */
    ptr_t Attr;

    /* Alignment - to be decided by the architecture - related generator, in real granularity */
    ptr_t Align;

    /* void */ Mem_Info(xml_node_t* Root, ptr_t Ref);
    /* void */ Mem_Info(class Mem_Info* Block);
    /* void */ Mem_Info(const std::string& Name, ptr_t Base, ptr_t Size, ptr_t Type, ptr_t Attr);

    void Check(void);

    static void Overlap_Check(const std::vector<class Mem_Info*>& Code,
                              const std::vector<class Mem_Info*>& Data,
                              const std::vector<class Mem_Info*>& Device,
                              const std::string& Type);
};
/*****************************************************************************/
/* __MEM_INFO_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
