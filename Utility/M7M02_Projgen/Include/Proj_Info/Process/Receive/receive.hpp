/******************************************************************************
Filename    : receive.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the receive class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __RECEIVE_HPP_DEFS__
#define __RECEIVE_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __RECEIVE_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __RECEIVE_HPP_CLASSES__
#define __RECEIVE_HPP_CLASSES__
/*****************************************************************************/
/* Receive endpoint information */
class Receive:public Kobj
{
public:
    /* Receive endpoint name */
    std::string Name;

    /* void */  Receive(xml_node_t* Root);
};
/*****************************************************************************/
/* __RECEIVE_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
