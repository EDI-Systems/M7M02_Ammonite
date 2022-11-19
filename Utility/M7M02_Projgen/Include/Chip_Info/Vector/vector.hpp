/******************************************************************************
Filename    : vector.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the vector class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __VECTOR_HPP_DEFS__
#define __VECTOR_HPP_DEFS__
/*****************************************************************************/
/* Config types */
#define VECTOR_RANGE                (0)
#define VECTOR_SELECT               (1)
/*****************************************************************************/
/* __VECTOR_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __VECTOR_HPP_CLASSES__
#define __VECTOR_HPP_CLASSES__
/*****************************************************************************/
/* The vector information */
class Vector
{
public:
    /* Name*/
    std::string Name;
    /* Vector number */
    ptr_t Number;

    /* void */ Vector(xml_node_t* Node);
};
/*****************************************************************************/
/* __VECTOR_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
