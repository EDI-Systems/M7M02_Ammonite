/******************************************************************************
Filename    : port.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the port class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __PORT_DEF__
#define __PORT_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __PORT_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PORT_CLASS__
#define __PORT_CLASS__
/*****************************************************************************/
/* Port information */
class Port:public Kobj
{
public:
    /* Port process */
    std::string Process;

    /* void */ Port(xml_node_t* Root, class Process* Owner);

    virtual std::string Report(void) final override;
};
/*****************************************************************************/
/* __PORT_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
