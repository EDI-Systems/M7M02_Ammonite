/******************************************************************************
Filename    : shmem.cpp
Author      : lbc
Date        : 21/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Shmem information implementation.
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
#include "Proj_Info/Process/Shmem/shmem.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Shmem::Shmem ********************************************
Description : Constructor for shmem information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem::Shmem(class wxXmlNode* Node)
{
	/* Name */
	this->Name=Main::Text_Load(Node,"Name");
    /* Attributes */
    this->Attr=Main::Num_Load(Node,"Attributes");
}
/* End Function:Shmem::Shmem ***************************************/

/* Function:Shmem::~Shmem *******************************************
Description : Destructor for shmem information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem::~Shmem(void)
{

}
/* End Function:Shmem::~Shmem **************************************/

/* Function:Shmem::Save **************************************************
Description : Save shmem information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem::Save(class wxXmlNode* Parent)
{

}
/* End Function:Shmem::Save *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
