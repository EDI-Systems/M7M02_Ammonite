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

    /* void */ Shmem(const std::string& Name, ptr_t Attr);
    /* void */ Shmem(class wxXmlNode* Node);
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
