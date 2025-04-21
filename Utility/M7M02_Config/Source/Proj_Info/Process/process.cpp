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
	/* The common items of Native and VM */
	/* Name */
	this->Name = Main::Text_Load(Node, "Name");
	/* Extra_Captbl */
	this->Extra_Captbl = Main::Num_Load(Node, "Extra_Captbl");
	/* Coprocessor */
	 Main::CSV_Load(Node,"Coprocessor",this->Coprocessor);
	/* Buildsystem */
	this->Buildsystem = Main::Text_Load(Node, "Buildsystem");
	/* Toolchain */
	this->Toolchain = Main::Text_Load(Node, "Toolchain");
	/* Optimization */
	this->Optimization = Main::Opt_Load(Node, "Optimization");
	/* Project output & overwrite */
	this->Project_Output = Main::Text_Load(Node, "Project_Output");
	this->Project_Overwrite = Main::Yesno_Load(Node, "Project_Overwrite");
	/* Linker_Output */
	this->Linker_Output = Main::Text_Load(Node, "Linker_Output");
	/* Main_Header_Output */
	this->Main_Header_Output = Main::Text_Load(Node, "Main_Header_Output");
	/* Main_Source_Output */
	this->Main_Source_Output = Main::Text_Load(Node, "Main_Source_Output");
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
	/* Name */
    Main::Text_Save(Parent,"Name",this->Name);
	/* Extra_Captbl */
    Main::Num_Save(Parent,"Extra_Captbl",this->Extra_Captbl);
	/* Coprocessor */
	 Main::CSV_Save(Parent,"Coprocessor",this->Coprocessor);
	/* Buildsystem */
	 Main::Text_Save(Parent,"Buildsystem",this->Buildsystem);
	/* Toolchain */
	 Main::Text_Save(Parent,"Toolchain",this->Toolchain);
	/* Optimization */
	 Main::Opt_Save(Parent,"Optimization",this->Optimization);
	/* roject_Output */
	 Main::Text_Save(Parent,"Project_Output",this->Project_Output);
	/* Project_Overwrite */
	 Main::Yesno_Save(Parent,"Project_Overwrite",this->Project_Overwrite);
	/* Linker_Output */
	 Main::Text_Save(Parent,"Linker_Output",this->Linker_Output);
	/* Main_Header_Output */
	 Main::Text_Save(Parent,"Main_Header_Output",this->Main_Header_Output);
	/* Main_Source_Output */
	 Main::Text_Save(Parent,"Main_Source_Output",this->Main_Source_Output);
}
/* End Function:Process::Save ************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
