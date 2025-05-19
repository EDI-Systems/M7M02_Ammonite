/******************************************************************************
Filename    : shmem.hpp
Author      : pry
Date        : 15/01/2023
License     : Proprietary; confidential.
Description : Shared memory reference class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __SHMEM_TYPE__
#define __SHMEM_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __SHMEM_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __SHMEM_DEF__
#define __SHMEM_DEF__
/*****************************************************************************/
/* Memory access permissions */
#define MEM_READ            POW2(0)
#define MEM_WRITE           POW2(1)
#define MEM_EXECUTE         POW2(2)
#define MEM_BUFFER          POW2(3)
#define MEM_CACHE           POW2(4)
#define MEM_STATIC          POW2(5)
/*****************************************************************************/
/* __SHMEM_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __SHMEM_CLASS__
#define __SHMEM_CLASS__
/*****************************************************************************/
class Shmem
{
public:
    /* Name */
    std::string Name;
    /* Attributes */
    ptr_t Attr;

//    /* Memory type */
//    ptr_t Type;
//    /* Base address */
//    ptr_t Base;
//    /* Size */
//    ptr_t Size;
//    /* Align */
//    ptr_t Align;

    /* void */ Shmem(class wxXmlNode* Node);
//    /* void */ Shmem(const std::string& Name, ptr_t Attr,ptr_t Type, ptr_t Base, ptr_t Size, ptr_t Align);
    /* void */ Shmem(const std::string& Name, ptr_t Attr);
    /* void */ ~Shmem(void);

    void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __SHMEM_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
