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
/* Memory access permissions */
#define MEM_READ            POW2(0)
#define MEM_WRITE           POW2(1)
#define MEM_EXECUTE         POW2(2)
#define MEM_BUFFERABLE      POW2(3)
#define MEM_CACHEABLE       POW2(4)
#define MEM_STATIC          POW2(5)
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
    /* The name of the memory trunk */
    std::string Name;
    /* The memory base */
    ptr_t Base;
    /* The size */
    ptr_t Size;
    /* Memory type */
    std::string Type;
    /* The attributes - read, write, execute, cacheable, bufferable, static */
    ptr_t Attr;
    /* The alignment granularity */
    ptr_t Align;
    /* Is this a shared memory? this is only used when generating macros for it */
    ptr_t Is_Shared;

    /* void */ Mem_Info(xml_node_t* Root);
    /* void */ Mem_Info(class Mem_Info* Block);
    /* void */ Mem_Info(ptr_t Base, ptr_t Size, std::string& Type, ptr_t Attr, ptr_t Align);
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
