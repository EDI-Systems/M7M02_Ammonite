/******************************************************************************
Filename    : mem_info.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the memory class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __MEM_INFO_DEF__
#define __MEM_INFO_DEF__
/*****************************************************************************/
/* Physical block or reference */
#define MEM_PHYS            0
#define MEM_PSEG            1
#define MEM_SSEG            2

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

/* Full permission */
#define MEM_FULL            (MEM_READ|MEM_WRITE|MEM_EXECUTE|MEM_BUFFER|MEM_CACHE|MEM_STATIC)

/* Kernel/monitor default basic permissions */
#define MEM_CODE_KERNEL     (MEM_READ|MEM_EXECUTE|MEM_STATIC)
#define MEM_DATA_KERNEL     (MEM_READ|MEM_WRITE|MEM_STATIC)
#define MEM_CODE_MONITOR    (MEM_READ|MEM_EXECUTE|MEM_STATIC)
#define MEM_DATA_MONITOR    (MEM_READ|MEM_WRITE|MEM_STATIC)

/* Basic process memory permissions */
#define MEM_CODE_LEAST      (MEM_READ|MEM_EXECUTE)
#define MEM_DATA_LEAST      (MEM_READ|MEM_WRITE)

/* Prime process memory permissions */
#define MEM_CODE_PRIME      (MEM_READ|MEM_EXECUTE|MEM_STATIC)
#define MEM_DATA_PRIME      (MEM_READ|MEM_WRITE|MEM_STATIC)

/* Memory placement */
#define MEM_AUTO            ((ptr_t)(-1LL))
/* Memmap granularity - always allocate in units of 16 bytes */
#define MAP_ALIGN           (16)

/* Try or mark */
#define MEM_TRY             (0)
#define MEM_MARK            (1)
/*****************************************************************************/
/* __MEM_INFO_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __MEM_INFO_CLASS__
#define __MEM_INFO_CLASS__
/*****************************************************************************/
/* Memory trunk information */
class Mem_Info
{
public:
    /* Physical memory or segment allocation */
    ptr_t Reference;
    /* The name of the memory trunk */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* The memory base */
    ptr_t Base;
    /* The size */
    ptr_t Size;
    /* The suggested alignment, in order of 2 */
    ptr_t Align_Order;
    /* Memory type */
    ptr_t Type;
    /* The attributes - read, write, execute, cacheable, bufferable, static */
    ptr_t Attr;

    /* Alignment - to be decided by the architecture - related generator, in real granularity */
    ptr_t Align;
    /* Memory map - only populated for physical memory declarations, to be used by the memory allocator */
    std::vector<bool> Map;
    /* Memory map - identifies sharing relationships in individual projects */
    ptr_t Is_Shared;

    /* void */ Mem_Info(xml_node_t* Root, ptr_t Reference);
    /* void */ Mem_Info(class Mem_Info* Block, ptr_t Attr_New);
    /* void */ Mem_Info(const std::string& Name, ptr_t Base, ptr_t Size, ptr_t Type, ptr_t Attr);

    void Check(void);

    static void Overlap_Check(const std::vector<class Mem_Info*>& Code,
                              const std::vector<class Mem_Info*>& Data,
                              const std::vector<class Mem_Info*>& Device,
                              const std::string& Type);

    /* Memory map operations - only called on physical memory declarations */
    void Memmap_Init(void);
    ret_t Memmap_Mark(ptr_t Base, ptr_t Size, ptr_t Mark);

    /* Memory fitting - called on shared/private declarations */
    ret_t Static_Fit(std::vector<class Mem_Info*>& Map);
    ret_t Auto_Fit(std::vector<class Mem_Info*>& Map);
};
/*****************************************************************************/
/* __MEM_INFO_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
