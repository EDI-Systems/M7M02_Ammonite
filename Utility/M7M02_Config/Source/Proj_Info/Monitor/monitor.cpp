/******************************************************************************
Filename    : monitor.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Monitor information implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/proj_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Monitor::Monitor ****************************************************
Description : Constructor for kernel information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Monitor::Monitor(class wxXmlNode* Node)
{
    /* Code_Size */
    this->Code_Size=Main::Num_Load(Node,"Code_Size");
    /* Data_Size */
    this->Data_Size=Main::Num_Load(Node,"Data_Size");
    /* Init_Stack_Size */
    this->Init_Stack_Size=Main::Num_Load(Node,"Init_Stack_Size");
    /* Sftd_Stack_Size */
    this->Sftd_Stack_Size=Main::Num_Load(Node,"Sftd_Stack_Size");
//    /* Vctd_Stack_Size (Old Item) */
//    this->Vctd_Stack_Size=Main::Num_Load(Node,"Vctd_Stack_Size");
//    /* Timd_Stack_Size (Old Item)  */
//    this->Timd_Stack_Size=Main::Num_Load(Node,"Timd_Stack_Size");
//    /* Hypd_Stack_Size (Old Item)  */
//    this->Hypd_Stack_Size=Main::Num_Load(Node,"Hypd_Stack_Size");
    /* Vmmd_Stack_Size, this is a new item */
    this->Vmmd_Stack_Size=Main::Num_Load(Node,"Vmmd_Stack_Size");
    /* Extra_Captbl */
    this->Extra_Captbl=Main::Num_Load(Node,"Extra_Captbl");
    /* Idle_Sleep_Enable, this is a new item */
    this->Idle_Sleep_Enable=Main::Yesno_Load(Node,"Idle_Sleep_Enable");
    /* Virt_Prio */
    this->Virt_Prio=Main::Num_Load(Node,"Virt_Prio");
    /* Virt_Event */
    this->Virt_Event=Main::Num_Load(Node,"Virt_Event");
    /* Virt_Map */
    this->Virt_Map=Main::Num_Load(Node,"Virt_Map");
    /* Buildsystem */
    this->Buildsystem=Main::Text_Load(Node,"Buildsystem");
    /* Toolchain */
    this->Toolchain=Main::Text_Load(Node,"Toolchain");
    /* Optimization */
    this->Optimization=Main::Opt_Load(Node,"Optimization");
    /* Project_Output */
    this->Project_Output=Main::Text_Load(Node,"Project_Output");
    /* Project_Overwrite */
    this->Project_Overwrite=Main::Yesno_Load(Node,"Project_Overwrite");
    /* Linker_Output */
    this->Linker_Output=Main::Text_Load(Node,"Linker_Output");
    /* Config_Header_Output */
    this->Config_Header_Output=Main::Text_Load(Node,"Config_Header_Output");
    /* Boot_Header_Output */
    this->Boot_Header_Output=Main::Text_Load(Node,"Boot_Header_Output");
    /* Boot_Source_Output */
    this->Boot_Source_Output=Main::Text_Load(Node,"Boot_Source_Output");
    /* Hook_Source_Output */
    this->Hook_Source_Output=Main::Text_Load(Node,"Hook_Source_Output");
    /* Hook_Source_Overwrite */
    this->Hook_Source_Overwrite=Main::Yesno_Load(Node,"Hook_Source_Overwrite");
}
/* End Function:Monitor::Monitor ***********************************************/

/* Function:Monitor::Monitor ****************************************************
Description : Constructor for kernel information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Monitor::Monitor(const class Plat_Info* Plat_Info, const class Chip_Info* Chip_Info)
{
    /* Set default value */
    this->Code_Size=0;
    this->Data_Size=0;
    this->Init_Stack_Size=0;
    this->Sftd_Stack_Size=0;
    this->Vmmd_Stack_Size=0;
    this->Extra_Captbl=0;
    this->Idle_Sleep_Enable=0;
    this->Virt_Prio=32;
    this->Virt_Event=32;
    this->Virt_Map=32;
    this->Buildsystem="Makefile";
    this->Toolchain="GCC";
    this->Optimization=0;
    this->Project_Output="./Monitor/Project/";
    this->Project_Overwrite=0;
    this->Linker_Output="./";
    this->Config_Header_Output="../Include/";
    this->Boot_Header_Output="../Include/";;
    this->Boot_Source_Output="../Source/";
    this->Hook_Source_Output="../Source/";
    this->Hook_Source_Overwrite=0;
}
/* End Function:Monitor::Monitor ***********************************************/

/* Function:Monitor::~Monitor ***************************************************
Description : Destructor for kernel information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Monitor::~Monitor(void)
{

}
/* End Function:Monitor::~Monitor **********************************************/

/* Function:Monitor::Save ******************************************************
Description : Save target information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Monitor::Save(class wxXmlNode* Parent)
{
    /* Code_Size */
    Main::Hex_Save(Parent,"Code_Size",4,this->Code_Size);
    /* Data_Size */
    Main::Hex_Save(Parent,"Data_Size",4,this->Data_Size);
    /* Init_Stack_Size */
    Main::Hex_Save(Parent,"Init_Stack_Size",4,this->Init_Stack_Size);
    /* Sftd_Stack_Size */
    Main::Hex_Save(Parent,"Sftd_Stack_Size",4,this->Sftd_Stack_Size);
    /* Vctd_Stack_Size, this is an old item */
//    Main::Hex_Save(Parent,"Vctd_Stack_Size",4,this->Vctd_Stack_Size);
//    /* Timd_Stack_Size, this is an old item */
//    Main::Hex_Save(Parent,"Timd_Stack_Size",4,this->Timd_Stack_Size);
//    /* Hypd_Stack_Size, this is an old item */
//    Main::Hex_Save(Parent,"Hypd_Stack_Size",4,this->Hypd_Stack_Size);
//    /* Vmmd_Stack_Size, this is a new item */
    Main::Hex_Save(Parent,"Vmmd_Stack_Size",4,this->Vmmd_Stack_Size);
    /* Extra_Captbl */
    Main::Num_Save(Parent,"Extra_Captbl",this->Extra_Captbl);
    /* Idle_Sleep_Enable, this is a new item */
    Main::Yesno_Save(Parent,"Idle_Sleep_Enable",this->Idle_Sleep_Enable);
    /* Virt_Prio */
    Main::Num_Save(Parent,"Virt_Prio",this->Virt_Prio);
    /* Virt_Event */
    Main::Num_Save(Parent,"Virt_Event",this->Virt_Event);
    /* Virt_Map */
    Main::Num_Save(Parent,"Virt_Map",this->Virt_Map);
    /* Buildsystem */
    Main::Text_Save(Parent,"Buildsystem",this->Buildsystem);
    /* Toolchain */
    Main::Text_Save(Parent,"Toolchain",this->Toolchain);
    /* Optimization */
    Main::Opt_Save(Parent,"Optimization",this->Optimization);
    /* Project_Output */
    Main::Text_Save(Parent,"Project_Output",this->Project_Output);
    /* Project_Overwrite */
    Main::Yesno_Save(Parent,"Project_Overwrite",this->Project_Overwrite);
    /* Linker_Output */
    Main::Text_Save(Parent,"Linker_Output",this->Linker_Output);
    /* Config_Header_Output */
    Main::Text_Save(Parent,"Config_Header_Output",this->Config_Header_Output);
    /* Boot_Header_Output */
    Main::Text_Save(Parent,"Boot_Header_Output",this->Boot_Header_Output);
    /* Boot_Source_Output */
    Main::Text_Save(Parent,"Boot_Source_Output",this->Boot_Source_Output);
    /* Hook_Source_Output */
    Main::Text_Save(Parent,"Hook_Source_Output",this->Hook_Source_Output);
    /* Hook_Source_Overwrite */
    Main::Yesno_Save(Parent,"Hook_Source_Overwrite",this->Hook_Source_Overwrite);
}
/* End Function:Monitor::Save ************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
