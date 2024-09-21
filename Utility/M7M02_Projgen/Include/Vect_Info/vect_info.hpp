/******************************************************************************
Filename    : vect_info.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the vector class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __VECT_INFO_DEF__
#define __VECT_INFO_DEF__
/*****************************************************************************/
/* Config types */
#define VECTOR_RANGE                (0)
#define VECTOR_SELECT               (1)
/*****************************************************************************/
/* __VECT_INFO_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __VECT_INFO_CLASS__
#define __VECT_INFO_CLASS__
/*****************************************************************************/
/* The vector information */
class Vect_Info:public Kobj
{
public:
    /* Vector number */
    ptr_t Number;

    /* void */ Vect_Info(xml_node_t* Root, class Process* Owner);

    void Check(void);

    virtual std::string Report(void) final override;
};
/*****************************************************************************/
/* __VECT_INFO_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
