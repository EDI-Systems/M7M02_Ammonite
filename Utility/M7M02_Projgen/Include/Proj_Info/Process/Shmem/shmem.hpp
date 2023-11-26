/******************************************************************************
Filename    : shmem.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the shared memory class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
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
/* __SHMEM_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
