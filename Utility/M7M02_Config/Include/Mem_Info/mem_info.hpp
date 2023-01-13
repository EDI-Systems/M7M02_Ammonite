/******************************************************************************
Filename    : mem_info.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Mem_Info class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __MEM_INFO_HPP_TYPES__
#define __MEM_INFO_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __MEM_INFO_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __MEM_INFO_HPP_DEFS__
#define __MEM_INFO_HPP_DEFS__
/*****************************************************************************/
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

/* Memory placement */
#define MEM_AUTO            ((ptr_t)(-1LL))
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
class Mem_Info
{
public:
    /* Memory trunk name */
    std::string Name;
    /* Base address */
    ptr_t Base;
    /* Size */
    ptr_t Size;
    /* Memory type */
    ptr_t Type;
    /* Access attributes */
    ptr_t Attr;

    /* void */ Mem_Info(const std::string& Name,
                        ptr_t Base, ptr_t Size, ptr_t Type, ptr_t Attr);
    /* void */ Mem_Info(class wxXmlNode* Node);
    /* void */ ~Mem_Info(void);

    void Save(class wxXmlNode* Parent);
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
