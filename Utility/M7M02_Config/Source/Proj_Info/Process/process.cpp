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
#include "Proj_Info/Process/Shmem/Shmem.hpp"
#include "Proj_Info/Process/Kfunc/Kfunc.hpp"
#include "Proj_Info/Process/Send/Send.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
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
    this->Name=Main::Text_Load(Node, "Name");
    this->Name_Upper=this->Name;
    std::transform(this->Name_Upper.begin(), this->Name_Upper.end(), this->Name_Upper.begin(),::toupper);
    /* Extra_Captbl */
    this->Extra_Captbl=Main::Num_Load(Node, "Extra_Captbl");
    /* Coprocessor */
    Main::CSV_Load(Node,"Coprocessor",this->Coprocessor);
    /* Buildsystem */
    this->Buildsystem=Main::Text_Load(Node, "Buildsystem");
    /* Toolchain */
    this->Toolchain = Main::Text_Load(Node, "Toolchain");
    /* Optimization */
    this->Optimization=Main::Opt_Load(Node, "Optimization");
    /* Project output & overwrite */
    this->Project_Output=Main::Text_Load(Node, "Project_Output");
    this->Project_Overwrite=Main::Yesno_Load(Node, "Project_Overwrite");
    /* Linker_Output */
    this->Linker_Output=Main::Text_Load(Node, "Linker_Output");
    /* Main_Header_Output */
    this->Main_Header_Output=Main::Text_Load(Node, "Main_Header_Output");
    /* Main_Source_Output */
    this->Main_Source_Output=Main::Text_Load(Node, "Main_Source_Output");
    /* Memory */
    Trunk_Load<class Mem_Info>(Main::Simple_Load(Node,"Memory"),"M",this->Memory);
    /* Shmem */
    Trunk_Load<class Shmem>(Main::Simple_Load(Node,"Shmem"),"S",this->Shmem);
    /* Send */
    Trunk_Load<class Send>(Main::Simple_Load(Node,"Send"),"S",this->Send);
    /* Kfunc */
    Trunk_Load<class Kfunc>(Main::Simple_Load(Node,"Kfunc"),"K",this->Kfunc);
}
/* End Function:Process::Process *********************************************/

/* Function:Process::Process *************************************************
Description : Destructor for process information.
Input       : const std::string& Name - The name of process.
              ptr_t Type - The type of the process, native or virtual.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Process::Process(const std::string& Name, ptr_t Type)
{
    /* Set default value */
    this->Name=Name;
    this->Name_Upper=Name;
    std::transform(this->Name_Upper.begin(),this->Name_Upper.end(),this->Name_Upper.begin(),::toupper);
    this->Extra_Captbl=0;
    this->Buildsystem="Makefile";
    this->Toolchain="GCC";
    this->Optimization=0;
    this->Project_Output="./"+Name+"/Project/";
    this->Project_Overwrite=0;
    this->Linker_Output="./";
    this->Main_Header_Output="../Include/";
    this->Main_Source_Output="../Source/";
}
/* End Function:Process::Process **********************************************/

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
//	/* Name */
//    Main::Text_Save(Parent,"Name",this->Name);
//    /* Extra_Captbl */
//    Main::Num_Save(Parent,"Extra_Captbl",this->Extra_Captbl);
//    /* Coprocessor */
//    Main::CSV_Save(Parent,"Coprocessor",this->Coprocessor);
//    /* Buildsystem */
//    Main::Text_Save(Parent,"Buildsystem",this->Buildsystem);
//    /* Toolchain */
//    Main::Text_Save(Parent,"Toolchain",this->Toolchain);
//    /* Optimization */
//    Main::Opt_Save(Parent,"Optimization",this->Optimization);
//    /* roject_Output */
//    Main::Text_Save(Parent,"Project_Output",this->Project_Output);
//    /* Project_Overwrite */
//    Main::Yesno_Save(Parent,"Project_Overwrite",this->Project_Overwrite);
//    /* Linker_Output */
//    Main::Text_Save(Parent,"Linker_Output",this->Linker_Output);
//    /* Main_Header_Output */
//    Main::Text_Save(Parent,"Main_Header_Output",this->Main_Header_Output);
//    /* Main_Source_Output */
//    Main::Text_Save(Parent,"Main_Source_Output",this->Main_Source_Output);
//    /* Memory */
//    Trunk_Save<class Mem_Info>(Main::Simple_Save(Parent,"Memory"),"T",this->Memory);
//    /* Shmem */
//    Trunk_Save<class Shmem>(Main::Simple_Save(Parent,"Shmem"),"S",this->Shmem);
//    /* Send */
//    Trunk_Save<class Send>(Main::Simple_Save(Parent,"Send"),"S",this->Send);
//    /* Kfunc */
//    Trunk_Save<class Kfunc>(Main::Simple_Save(Parent,"Kfunc"),"K",this->Kfunc);

}
/* End Function:Process::Save ************************************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
