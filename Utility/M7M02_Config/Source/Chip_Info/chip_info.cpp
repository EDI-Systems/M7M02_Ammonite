/******************************************************************************
Filename    : chip_info.cpp
Author      : pry
Date        : 15/01/2023
License     : Unlicense.
Description : Chip information class implementation.
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
#include "vector"
#include "memory"

#define __HDR_DEF__
#include "Chip_Info/chip_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Chip_Info/chip_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Chip_Info::Chip_Info **********************************************
Description : Constructor for chip information.
Input       : class wxXmlNode* Node - The node containing information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip_Info::Chip_Info(const std::string& Path)
{
	class wxFileName Temp;
    std::string Name;
    std::unique_ptr<class wxXmlDocument> Document;
    class wxXmlNode* Chip;

    this->Path=Path;
    Temp=Path;
    this->Filename=Temp.GetFullName();
    this->Fullpath=Temp.GetFullPath();
    this->Fulldir=Temp.GetPathWithSep();
    this->Name=Temp.GetName();
    this->Version=SOFTWARE_VERSION;

    /* Canonicalize all of them */
    std::replace(this->Path.begin(),this->Path.end(),'\\','/');
    std::replace(this->Fullpath.begin(),this->Fullpath.end(),'\\','/');
    std::replace(this->Fulldir.begin(),this->Fulldir.end(),'\\','/');
    std::transform(this->Name.begin(),this->Name.end(),this->Name.begin(),(int(*)(int))std::toupper);

    /* See if it does exist */
    if(wxIsReadable(this->Path)==false)
        throw std::runtime_error("The platform file probably does not exist. Consider permission or memory failure.");

    /* Load from disk */
    Document=std::make_unique<class wxXmlDocument>();
    if(Document->Load(this->Path)==false)
        throw std::runtime_error("Cannot load project.");

    /* Platform */
    Chip=Document->GetRoot();
    if(Chip->GetName()!="Chip")
        throw std::runtime_error("Chip label not found.");

    /* Name */
    this->Name=Main::Text_Load(Chip,"Name");
    /* Version */
    this->Version=Main::Text_Load(Chip,"Version");
    /* Platform */
    this->Platform=Main::Text_Load(Chip,"Platform");
    /* Compatible */
    Main::CSV_Load(Chip,"Compatible",this->Compatible);
    /* Vendor */
    this->Vendor=Main::Text_Load(Chip,"Vendor");
    /* Vector */
    this->Vector=Main::Num_Load(Chip,"Vector");
    /* Region */
    this->Region=Main::Num_Load(Chip,"Region");
    /* Iregion */
    this->Iregion=Main::Num_Load(Chip,"Iregion");
    /* Dregion */
    this->Dregion=Main::Num_Load(Chip,"Dregion");
    /* Coprocessor */
    Main::CSV_Load(Chip,"Coprocessor",this->Coprocessor);
    /* Attribute */
    Main::Pair_Load(Chip,"Attribute",this->Attribute);
    /* Memory */
    Trunk_Load<class Mem_Info>(Main::Simple_Load(Chip,"Memory"),"M",this->Memory);
}
/* End Function:Chip_Info::Chip_Info *****************************************/

/* Function:Chip_Info::~Chip_Info *********************************************
Description : Destructor for chip information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip_Info::~Chip_Info(void)
{

}
/* End Function:Chip_Info::~Chip_Info ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
