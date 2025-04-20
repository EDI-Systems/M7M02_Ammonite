/******************************************************************************
Filename    : thread.cpp
Author      : pry
Date        : 13/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Thread information implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Proj_Info/Process/Thread/thread.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Thread::Thread ****************************************************
Description : Constructor for thread information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thread::Thread(class wxXmlNode* Node)
{

}
/* End Function:Thread::Thread ***********************************************/

/* Function:Thread::~Thread ***************************************************
Description : Destructor for thread information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thread::~Thread(void)
{

}
/* End Function:Thread::~Thread **********************************************/

/* Function:Thread::Save ******************************************************
Description : Save thread information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Thread::Save(class wxXmlNode* Parent)
{

}
/* End Function:Thread::Save *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
