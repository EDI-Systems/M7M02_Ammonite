/******************************************************************************
Filename    : plat_info.cpp
Author      : pry
Date        : 17/08/2021
License     : Proprietary; confidential.
Description : Platform information implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"

#include "set"
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "algorithm"

#define __HDR_DEF__
#include "Plat_Info/plat_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
/* #include "Conf_Info/conf_info.hpp" */
#include "Plat_Info/Compatible/compatible.hpp"
#include "Plat_Info/Default/default.hpp"
#include "Plat_Info/Config/config.hpp"
#include "Plat_Info/plat_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Plat_Info::Plat_Info **********************************************
Description : Constructor for platform information.
Input       : const std::string& Path - The path to the platform information file.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Plat_Info::Plat_Info(const std::string& Path)
{
    wxLogDebug("Plat_Info::Plat_Info: %s",Path);

    std::string Name;
    class wxFileName Temp;
    class wxXmlNode* Platform;
    std::unique_ptr<class wxXmlDocument> Document;

    Temp=Path;
    this->Path=Path;
    /* File name, including suffix */
    this->Filename=Temp.GetFullName();
    this->Fullpath=Temp.GetFullPath();
    /* The location of the file */
    this->Fulldir=Temp.GetPathWithSep();
    /* File name without suffix */
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
        throw std::runtime_error("Cannot load platform files.");

    /* Platform */
    Platform=Document->GetRoot();
    if(Platform->GetName()!="Platform")
        throw std::runtime_error("Platform label not found.");

    /* Name */
    this->Name=Main::Text_Load(Platform, "Name");
    wxLogDebug(wxString(this->Name));
    /* Version */
    this->Version=Main::Text_Load(Platform,"Version");
    /* Wordlength */
    this->Wordlength=Main::Num_Load(Platform, "Wordlength");
    /* Coprocessor */
    Main::CSV_Load(Platform, "Coprocessor", this->Coprocessor);
    /* Buildsystem */
    Main::CSV_Load(Platform, "Buildsystem", this->Buildsystem);
    /* Toolchain */
    Main::CSV_Load(Platform, "Toolchain", this->Toolchain);
    /* Guest */
    Main::CSV_Load(Platform, "Guest", this->Guest);

    /* Compatible */
    Trunk_Load<class Compatible>(Main::Simple_Load(Platform,"Compatible"),"C",this->Compatible);

    /* Default options */
    //this->Default=std::make_unique<class Default>(Main::Simple_Load(Platform,"Default"));

    /* Configs */
    Trunk_Load<class Conf_Info>(Main::Simple_Load(Platform,"Config"),"C",this->Config);
}
/* End Function:Plat_Info::Plat_Info *****************************************/

/* Function:Plat_Info::~Plat_Info *********************************************
Description : Destructor for project information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Plat_Info::~Plat_Info(void)
{

}
/* End Function:Plat_Info::~Plat_Info ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
