/******************************************************************************
Filename    : port.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the port class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __PORT_HPP_DEFS__
#define __PORT_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __PORT_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __PORT_HPP_CLASSES__
#define __PORT_HPP_CLASSES__
/*****************************************************************************/
/* Port information */
class Port
{
public:
    /* Port name */
    std::string Name;
    /* Port process */
    std::string Process;

    /* void */ Port(xml_node_t* Root);
};
/*****************************************************************************/
/* __PORT_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
