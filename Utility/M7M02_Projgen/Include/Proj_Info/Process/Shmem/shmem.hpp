/******************************************************************************
Filename    : shmem.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the shared memory class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
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
/* Shared memory reference information */
class Shmem
{
public:
    /* The name of the memory trunk */
    std::string Name;
    std::string Name_Upper;
    std::string Name_Lower;
    /* The attributes - read, write, execute, cacheable, bufferable, static */
    ptr_t Attr;

    /* void */ Shmem(xml_node_t* Root);

    void Check(void);
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
