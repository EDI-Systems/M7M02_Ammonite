/******************************************************************************
Filename    : receive.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the receive class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __RECEIVE_DEF__
#define __RECEIVE_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __RECEIVE_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __RECEIVE_CLASS__
#define __RECEIVE_CLASS__
/*****************************************************************************/
/* Receive endpoint information */
class Receive:public Kobj
{
public:

    /* void */  Receive(xml_node_t* Root, class Process* Owner);
};
/*****************************************************************************/
/* __RECEIVE_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
