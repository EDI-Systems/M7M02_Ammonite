/******************************************************************************
Filename    : kernel.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Kernel information implementation.
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
#include "Proj_Info/Kernel/kernel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/proj_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Kernel::Kernel ****************************************************
Description : Default constructor for kernel information. This will initialize
              with the platform/chip description file.
Input       : const class Plat_Info* Plat - The platform information.
              const class Chip_Info* Chip - The chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kernel::Kernel(const class Plat_Info* Plat, const class Chip_Info* Chip)
{
    /* Set default value */
    this->Code_Base=0;
    this->Code_Size=0;
    this->Data_Base=0;
    this->Data_Size=0;
    this->Stack_Size=0;
    this->Extra_Kom=0;
    this->Kom_Order=16;
    this->Kern_Prio=32;
    this->Buildsystem="Makefile";
    this->Toolchain="GCC";
    this->Optimization=0;
    this->Full_Image=0;
    this->Project_Output="./Kernel/Project/";
    this->Project_Overwrite=0;
    this->Linker_Output="./";
    this->Config_Header_Output="../Include/";
    this->Boot_Header_Output="../Include/";
    this->Boot_Source_Output="../Source/";
    this->Hook_Source_Output="../Source/";
    this->Hook_Source_Overwrite=0;
    this->Handler_Source_Output="../Source/";
    this->Handler_Source_Overwrite=0;
}
/* End Function:Kernel::Kernel ***********************************************/

/* Function:Kernel::Kernel ****************************************************
Description : Constructor for kernel information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kernel::Kernel(class wxXmlNode* Node)
{
    /* Code_Base */
    this->Code_Base=Main::Num_Load(Node,"Code_Base");
    /* Code_Size */
    this->Code_Size=Main::Num_Load(Node,"Code_Size");
    /* Data_Base */
    this->Data_Base=Main::Num_Load(Node,"Data_Base");
    /* Data_Size */
    this->Data_Size=Main::Num_Load(Node,"Data_Size");
    /* Stack_Size */
    this->Stack_Size=Main::Num_Load(Node,"Stack_Size");
    /* Extra_Kom */
    this->Extra_Kom=Main::Num_Load(Node,"Extra_Kom");
    /* Kom_Order */
    this->Kom_Order=Main::Num_Load(Node,"Kom_Order");
    /* Kern_Prio */
    this->Kern_Prio=Main::Num_Load(Node,"Kern_Prio");
    /* Buildsystem */
    this->Buildsystem=Main::Text_Load(Node,"Buildsystem");
    /* Toolchain */
    this->Toolchain=Main::Text_Load(Node,"Toolchain");
    /* Optimization */
    this->Optimization=Main::Opt_Load(Node,"Optimization");
    /* Full_Image */
    this->Full_Image=Main::Yesno_Load(Node,"Full_Image");
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
    /* Handler_Source_Output */
    this->Handler_Source_Output=Main::Text_Load(Node,"Handler_Source_Output");
    /* Handler_Source_Overwrite */
    this->Handler_Source_Overwrite=Main::Yesno_Load(Node,"Handler_Source_Overwrite");
}
/* End Function:Kernel::Kernel ***********************************************/

/* Function:Kernel::~Kernel ***************************************************
Description : Destructor for kernel information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kernel::~Kernel(void)
{

}
/* End Function:Kernel::~Kernel **********************************************/

/* Function:Kernel::Save ******************************************************
Description : Save target information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel::Save(class wxXmlNode* Parent)
{
    /* Code_Base */
    Main::Hex_Save(Parent,"Code_Base",4,this->Code_Base);
    /* Code_Size */
    Main::Hex_Save(Parent,"Code_Size",4,this->Code_Size);
    /* Data_Base */
    Main::Hex_Save(Parent,"Data_Base",4,this->Data_Base);
    /* Data_Size */
    Main::Hex_Save(Parent,"Data_Size",4,this->Data_Size);
    /* Stack_Size */
    Main::Hex_Save(Parent,"Stack_Size",4,this->Stack_Size);
    /* Extra_Kom */
    Main::Hex_Save(Parent,"Extra_Kom",4,this->Extra_Kom);
    /* Kom_Order */
    Main::Num_Save(Parent,"Kom_Order",this->Kom_Order);
    /* Kern_Prio */
    Main::Num_Save(Parent,"Kern_Prio",this->Kern_Prio);
    /* Buildsystem */
    Main::Text_Save(Parent,"Buildsystem",this->Buildsystem);
    /* Toolchain */
    Main::Text_Save(Parent,"Toolchain",this->Toolchain);
    /* Optimization */
    Main::Opt_Save(Parent,"Optimization",this->Optimization);
    /* Full_Image */
    Main::Yesno_Save(Parent,"Full_Image",this->Full_Image);
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
    /* Handler_Source_Output */
    Main::Text_Save(Parent,"Handler_Source_Output",this->Handler_Source_Output);
    /* Handler_Source_Overwrite */
    Main::Yesno_Save(Parent,"Handler_Source_Overwrite",this->Handler_Source_Overwrite);
}
/* End Function:Kernel::Save *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
