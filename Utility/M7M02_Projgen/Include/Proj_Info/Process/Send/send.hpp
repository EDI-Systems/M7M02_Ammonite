/******************************************************************************
Filename    : send.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the send class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __SEND_DEF__
#define __SEND_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __SEND_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __SEND_CLASS__
#define __SEND_CLASS__
/*****************************************************************************/
/* Send endpoint information */
class Send:public Kobj
{
public:
    /* Target receive endpoint process */
    std::string Process;

    /* void */ Send(xml_node_t* Root, class Process* Owner);

    virtual std::string Report(void) final override;
};
/*****************************************************************************/
/* __SEND_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
