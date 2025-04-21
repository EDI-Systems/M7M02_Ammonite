/******************************************************************************
Filename    : proj_info.cpp
Author      : pry
Date        : 17/08/2021
License     : Proprietary; confidential.
Description : Project information implementation.
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
#include "Proj_Info/proj_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Proj_Info/proj_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Proj_Info::Proj_Info **********************************************
Description : Constructor for project information. This just creates the class.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Proj_Info::Proj_Info(void)
{

}
/* End Function:Proj_Info::Proj_Info *****************************************/

/* Function:Proj_Info::~Proj_Info *********************************************
Description : Destructor for project information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Proj_Info::~Proj_Info(void)
{

}
/* End Function:Proj_Info::~Proj_Info ****************************************/

/* Function:Proj_Info::Create *************************************************
Description : Load the default project, and save to file. This needs to assume
              a chip name, the kernel source folder, and the monitor folder.
              The default project will have no processes, and the kernel data
              segments will be place just behind the memory trunk.
Input       : const std::string& Path - The path to the file.
              const std::string& Chip_Class - The class name of the chip.
              const std::string& Chip_Name - The exact name of the chip.
              const std::string& Kernel_Root - The root folder of the kernel.
              const std::string& Monitor_Root - The root folder of the monitor.
Output      : None.
Return      : ret_t - Whether the load is successful.
******************************************************************************/
ret_t Proj_Info::Create(const std::string& Path,
                        const std::string& Chip_Class, const std::string& Chip_Name,
                        const std::string& Kernel_Root, const std::string& Monitor_Root)
{

	class wxFileName Temp;
    std::string Name;
    std::unique_ptr<class wxXmlDocument> Document;
    class wxXmlNode* Proj;

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
        throw std::runtime_error("The project file probably does not exist.");

    /* Load from disk */
    Document=std::make_unique<class wxXmlDocument>();
    if(Document->Load(this->Path)==false)
        throw std::runtime_error("Cannot load project.");

    /* Project root */
    Proj=Document->GetRoot();
    if(Proj->GetName()!="Project")
        throw std::runtime_error("Project root not found.");


}
/* End Function:Proj_Info::Create ********************************************/

/* Function:Proj_Info::Load ***************************************************
Description : Load an existing project.
Input       : const std::string& Path - The path.
Output      : None.
Return      : ret_t - Whether the load is successful.
******************************************************************************/
ret_t Proj_Info::Load(const std::string& Path)
{
    class wxFileName Temp;
    std::string Name;
    std::unique_ptr<class wxXmlDocument> Document;
    class wxXmlNode* Project;

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
    {
        wxLogDebug("The project probably does not exist. Consider permission or memory failure.");
        return -1;
    }

    /* Load from disk */
    Document=std::make_unique<class wxXmlDocument>();
    if(Document->Load(this->Path)==false)
        throw std::runtime_error("Cannot load project.");

    /* Platform */
    Project=Document->GetRoot();
    if(Project->GetName()!="Project")
        throw std::runtime_error("Project label not found.");

    /* Name */
    this->Name=Main::Text_Load(Project, "Name");
    /* Version */
    this->Version=Main::Text_Load(Project, "Version");
    /* Assert */
    this->Assert_Enable=Main::Yesno_Load(Project, "Assert_Enable");
    /* Debug logging */
    this->Debug_Log_Enable=Main::Yesno_Load(Project, "Debug_Log_Enable");
    /* Raw page table */
    this->Pgtbl_Raw_Enable=Main::Yesno_Load(Project, "Pgtbl_Raw_Enable");
    /* Buildsystem */
    this->Buildsystem=Main::Text_Load(Project, "Buildsystem");

    /* TODO: fill in here */


    return 0;
}
/* End Function:Proj_Info::Load **********************************************/

/* Function:Proj_Info::Save ***************************************************
Description : Save the project to disk.
Input       : None.
Output      : None.
Return      : ret_t - Whether the save is successful.
******************************************************************************/
ret_t Proj_Info::Save(void)
{
    std::unique_ptr<class wxXmlDocument> Document;
    class wxXmlNode* Project;

    /* Main project file structure */
    Project=new wxXmlNode(nullptr,wxXML_ELEMENT_NODE,"Project");
    Main::Text_Save(Project,"Name",this->Name);
    Main::Text_Save(Project,"Version",std::string(SOFTWARE_VERSION));


    /* Write to disk */
    Document=std::make_unique<class wxXmlDocument>();
    Document->SetRoot(Project);
    if(Document->Save(this->Path)==false)
        return -1;

    return 0;
}
/* End Function:Proj_Info::Save **********************************************/

/* Function:Proj_Info::Rel_Conv ***********************************************
Description : Convert absolute path to relative ones; the path is always relative
              to the project file. For those that cannot be converted (on another
              volume), we opt to leave it there. Also, all '\\' will be converted
              to '/' for consistency.
Input       : const std::string& Relpath - The relative path.
              ptr_t Type - If a file, 0; else 1.
Output      : None.
Return      : std::string - The path after the conversion.
******************************************************************************/
std::string Proj_Info::Rel_Conv(const std::string& Relpath,ptr_t Type)
{
    class wxFileName Path;
    std::string Temp;

    if(Relpath=="")
        Temp="";
    else
    {
        /* If this is a folder, we need to end it with the delimiter if it doesn't already have one */
        if((Type==PATH_DIR)&&(Relpath.back()!='/'))
            Path=Relpath+"/";
        else
            Path=Relpath;
        /* Compute relative path */
        Path.MakeAbsolute();
        Path.MakeRelativeTo(this->Fulldir);
        Temp=Path.GetFullPath();
        /* See if this have a volume label */
        if(Path.GetVolume()=="")
            Temp=std::string("./")+Temp;
        /* Canonicalize */
        std::replace(Temp.begin(),Temp.end(),'\\','/');
    }

    return Temp;
}
/* End Function:Proj_Info::Rel_Conv ******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
