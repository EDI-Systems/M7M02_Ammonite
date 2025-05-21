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
    /* Name */
    this->Name=Main::Text_Load(Node,"Name");
    /* Stack_Size */
    this->Stack_Size=Main::Num_Load(Node,"Stack_Size");
    /* Parameter */
    this->Parameter=Main::Num_Load(Node,"Parameter");
    /* Priority */
    this->Priority=Main::Num_Load(Node,"Priority");
}
/* End Function:Thread::Thread ***********************************************/

/* Function:Thread::Thread ****************************************************
Description : Constructor for thread information.
Input       : const std::string Name - The name.
              const ptr_t& Stack_Size - The stack size.
              const ptr_t& Parameter - The parameter.
              const ptr_t& Priority - The priority.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thread::Thread(const std::string Name,const ptr_t& Stack_Size,
                          const ptr_t& Parameter,const ptr_t& Priority)
{
    /* Name */
    this->Name=Name;
    /* Stack_Size */
    this->Stack_Size=Stack_Size;
    /* Parameter */
    this->Parameter=Parameter;
    /* Priority */
    this->Priority=Priority;
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
    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);
    /* Stack_Size */
    Main::Hex_Save(Parent,"Stack_Size",4,this->Stack_Size);
    /* Parameter */
    Main::Hex_Save(Parent,"Parameter",4,this->Parameter);
    /* Priority */
    Main::Num_Save(Parent,"Priority",this->Priority);
}
/* End Function:Thread::Save *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
