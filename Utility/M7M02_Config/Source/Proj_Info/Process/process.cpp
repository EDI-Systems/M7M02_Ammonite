/******************************************************************************
Filename    : process.cpp
Author      : pry
Date        : 13/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description :  information implementation.
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
#include "Proj_Info/Process/process.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Process/process.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Process::Process **************************************************
Description : Default constructor for process information.
Input       : const std::string& Name - The exact chip name.
              ptr_t Type - The type of the process, native or virtual.
              const std::string& Root - The root folder path.
              const class Plat_Info* Plat - The platform information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Process::Process(const std::string& Name,
                            ptr_t Type,
                            const std::string& Root,
                            const class Plat_Info* Plat)
{

}
/* End Function:Process::Process *********************************************/

/* Function:Process::Process **************************************************
Description : Constructor for process information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - The type of the process, native or virtual.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Process::Process(class wxXmlNode* Node, ptr_t Type)
{

}
/* End Function:Process::Process *********************************************/

/* Function:Process::~Process *************************************************
Description : Destructor for process information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Process::~Process(void)
{

}
/* End Function:Process::~Process **********************************************/

/* Function:Process::Save *****************************************************
Description : Save process information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Process::Save(class wxXmlNode* Parent)
{

}
/* End Function:Process::Save ************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
