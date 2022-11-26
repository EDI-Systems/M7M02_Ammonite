/******************************************************************************
Filename    : send.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the send class.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __SEND_HPP_DEFS__
#define __SEND_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __SEND_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __SEND_HPP_CLASSES__
#define __SEND_HPP_CLASSES__
/*****************************************************************************/
/* Send endpoint information */
class Send:public Kobj
{
public:
    /* Receive endpoint name */
    std::string Name;
    /* Receive endpoing process */
    std::string Process;

    Send(xml_node_t* Root);
};
/*****************************************************************************/
/* __SEND_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
