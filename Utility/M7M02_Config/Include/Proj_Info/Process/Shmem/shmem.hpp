/******************************************************************************
Filename    : shmem.hpp
Author      : pry
Date        : 15/01/2023
License     : Proprietary; confidential.
Description : Shared memory reference class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __SHMEM_HPP_TYPES__
#define __SHMEM_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __SHMEM_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __SHMEM_HPP_DEFS__
#define __SHMEM_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __SHMEM_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __SHMEM_HPP_CLASSES__
#define __SHMEM_HPP_CLASSES__
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
/* __SHMEM_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
