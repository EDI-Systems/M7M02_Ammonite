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

/* Function:Plat_Info::Compat_Check *******************************************
Description : Check whether the option is compatible, for kernel and monitor.
Input       : const std::string& Toolchain - The current toolchain.
              const std::string& Buildsystem - The current buildsystem.
Output      : None.
Return      : ret_t - If compatible, 0; else -1.
******************************************************************************/
ret_t Plat_Info::Compat_Check(const std::string& Toolchain,
                              const std::string& Buildsystem)
{
    for(const std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if(Compat->Guest=="Native")
        {
            if((Compat->Buildsystem==Buildsystem)&&(Compat->Toolchain==Toolchain))
                return 0;
        }
    }

    return -1;
}
/* End Function:Plat_Info::Compat_Check **************************************/

/* Function:Plat_Info::Compat_Check *******************************************
Description : Check whether the option is compatible, for kernel and monitor.
Input       : const std::string& Toolchain - The current toolchain.
              const std::string& Buildsystem - The current buildsystem.
              const std::string& Guest - The guest operating system.
Output      : None.
Return      : ret_t - If compatible, 0; else -1.
******************************************************************************/
ret_t Plat_Info::Compat_Check(const std::string& Toolchain,
                              const std::string& Buildsystem,
                              const std::string& Guest)
{
    for(const std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if(Compat->Guest==Guest)
        {
            if((Compat->Buildsystem==Buildsystem)&&(Compat->Toolchain==Toolchain))
                return 0;
        }
    }

    return -1;
}
/* End Function:Plat_Info::Compat_Check **************************************/

/* Function:Plat_Info::Buildsystem_Workspace **********************************
Description : Find valid buildsystem options for the workspace.
Input       : None.
Output      : std::vector<std::string>& Build_Avail - The possible buildsystems.
Return      : None.
******************************************************************************/
void Plat_Info::Buildsystem_Workspace(std::vector<std::string>& Build_Avail)
{
    Build_Avail.clear();

    for(const std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if(Compat->Guest=="Native")
            Build_Avail.push_back(Compat->Buildsystem);
    }
}
/* End Function:Plat_Info::Buildsystem_Workspace *****************************/

/* Function:Plat_Info::Toolchain_Native ***************************************
Description : Find valid toolchain options for native processes.
Input       : None.
Output      : std::vector<std::string>& Build_Avail - The possible toolchains.
Return      : None.
******************************************************************************/
void Plat_Info::Toolchain_Native(std::vector<std::string>& Tool_Avail)
{
    Tool_Avail.clear();

    for(const std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if(Compat->Guest=="Native")
            Tool_Avail.push_back(Compat->Toolchain);
    }
}
/* End Function:Plat_Info::Toolchain_Native **********************************/

/* Function:Plat_Info::Buildsystem_Native *************************************
Description : Find valid buildsystem options given the toolchain.
Input       : const std::string& Toolchain - The current toolchain.
Output      : std::vector<std::string>& Build_Avail - The possible buildsystems.
Return      : None.
******************************************************************************/
void Plat_Info::Buildsystem_Native(const std::string& Toolchain,
                                   std::vector<std::string>& Build_Avail)
{
    Build_Avail.clear();

    for(const std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if((Compat->Guest=="Native")&&(Compat->Toolchain==Toolchain))
            Build_Avail.push_back(Compat->Buildsystem);
    }
}
/* End Function:Plat_Info::Buildsystem_Native ********************************/

/* Function:Plat_Info::Toolchain_Virtual **************************************
Description : Find valid toolchain options for virtual machines.
Input       : None.
Output      : std::vector<std::string>& Build_Avail - The possible toolchains.
Return      : None.
******************************************************************************/
void Plat_Info::Toolchain_Virtual(std::vector<std::string>& Tool_Avail)
{
    Tool_Avail.clear();

    for(const std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if(Compat->Guest!="Native")
            Tool_Avail.push_back(Compat->Toolchain);
    }
}
/* End Function:Plat_Info::Toolchain_Virtual *********************************/

/* Function:Plat_Info::Buildsystem_Virtual ************************************
Description : Find valid buildsystem options given the toolchain.
Input       : const std::string& Toolchain - The current toolchain.
Output      : std::vector<std::string>& Build_Avail - The possible buildsystems.
Return      : None.
******************************************************************************/
void Plat_Info::Buildsystem_Virtual(const std::string& Toolchain,
                                    std::vector<std::string>& Build_Avail)
{
    Build_Avail.clear();

    for(const std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if((Compat->Guest!="Native")&&(Compat->Toolchain==Toolchain))
            Build_Avail.push_back(Compat->Buildsystem);
    }
}
/* End Function:Plat_Info::Buildsystem_Virtual *******************************/

/* Function:Plat_Info::Guest_Virtual ******************************************
Description : Find valid guest options given the toolchain and buildsystem.
Input       : const std::string& Toolchain - The current toolchain.
              const std::string& Buildsystem - The current buildsystem.
Output      : std::vector<std::string>& Build_Avail - The possible buildsystems.
Return      : None.
******************************************************************************/
void Plat_Info::Guest_Virtual(const std::string& Toolchain,
                              const std::string& Buildsystem,
                              std::vector<std::string>& Guest_Avail)
{
    Guest_Avail.clear();

    for(const std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if((Compat->Buildsystem==Buildsystem)&&(Compat->Toolchain==Toolchain))
        {
            if(Compat->Guest!="Native")
                Guest_Avail.push_back(Compat->Guest);
        }
    }
}
/* End Function:Plat_Info::Guest_Virtual *************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
