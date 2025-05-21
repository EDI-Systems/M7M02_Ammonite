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

#include <set>
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
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Send/send.hpp"
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
    /*Set default value*/
    this->Name=_("Project");
    this->Assert_Enable=0;
    this->Debug_Log_Enable=0;
    this->Pgtbl_Raw_Enable=0;
    this->Workspace_Overwrite=0;

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
Output      : None.
Return      : ret_t - Whether the load is successful.
******************************************************************************/
ret_t Proj_Info::Create(const std::string& Path,
                        const std::string& Chip_Class, const std::string& Chip_Name)
{

    return 0;
}
/* End Function:Proj_Info::Create ********************************************/

/* Function:Proj_Info::Create *************************************************
Description : Load the default project, and save to file. This needs to assume
              a chip name, the kernel source folder, and the monitor folder.
              The default project will have no processes, and the kernel data
              segments will be place just behind the memory trunk.
Input       : const std::string& Path - The path to the file.
              const std::string& Plat_Name - The name of the platform.
              const std::string& Chip_Class - The class name of the chip.
              const std::string& Chip_Name - The exact name of the chip.
Output      : None.
Return      : ret_t - Whether the load is successful.
******************************************************************************/
ret_t Proj_Info::Create(const std::string& Path,
                        const class Plat_Info* Plat_Info, const class Chip_Info* Chip_Info,
                        const std::string& Plat_Name, const std::string& Chip_Class, const std::string& Chip_Name)
{
    class wxFileName Temp;
    std::string Name;

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

    /* Default value */
    /* Name */
    this->Name="Project";

    /* Assert_Enable */
    this->Assert_Enable=OPTION_NO;

    /* Debug_Log_Enable */
    this->Debug_Log_Enable=OPTION_NO;

    /* Pgtbl_Raw_Enable */
    this->Pgtbl_Raw_Enable=OPTION_NO;

    /* Buildsystem */
    this->Buildsystem="None";

    /* Workspace_Overwrite */
    this->Workspace_Overwrite=OPTION_NO;

    /* Chip information */
    this->Chip=std::make_unique<class Chip>(Plat_Info, Chip_Info,Plat_Name,Chip_Class,Chip_Name);

    /* The RME kernel information */
    this->Kernel=std::make_unique<class Kernel>(Plat_Info, Chip_Info);

    /* The RME kernel information */
    this->Monitor=std::make_unique<class Monitor>(Plat_Info, Chip_Info);

    return 0;
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

    /* Project */
    Project=Document->GetRoot();
    if(Project->GetName()!="Project")
        throw std::runtime_error("Project label not found.");

    try
    {
        /* Name */
        this->Name=Main::Text_Load(Project, "Name");

        /* Version */
        this->Version=Main::Text_Load(Project, "Version");

        /* Assert_Enable */
        this->Assert_Enable=Main::Yesno_Load(Project, "Assert_Enable");

        /* Debug_Log_Enable */
        this->Debug_Log_Enable=Main::Yesno_Load(Project, "Debug_Log_Enable");

        /* Pgtbl_Raw_Enable */
        this->Pgtbl_Raw_Enable=Main::Yesno_Load(Project, "Pgtbl_Raw_Enable");

        /* Buildsystem */
        this->Buildsystem=Main::Text_Load(Project, "Buildsystem");

        /* Workspace_Overwrite */
        this->Workspace_Overwrite=Main::Yesno_Load(Project, "Workspace_Overwrite");

        /* Chip information */
        this->Chip=std::make_unique<class Chip>(Main::Simple_Load(Project,"Chip"));

        /* Extmem information */
        Trunk_Load<class Mem_Info>(Main::Simple_Load(Project,"Extmem"),"E",this->Extmem);

        /* Shmem information */
        Trunk_Load<class Mem_Info>(Main::Simple_Load(Project,"Shmem"),"S",this->Shmem);

        /* Kernel information */
        this->Kernel=std::make_unique<class Kernel>(Main::Simple_Load(Project,"Kernel"));

        /* Monitor information */
        this->Monitor=std::make_unique<class Monitor>(Main::Simple_Load(Project,"Monitor"));

        /* Native information */
        Trunk_Load<class Native>(Main::Simple_Load(Project,"Native"),"N",this->Native);

        /* VM information */
        Trunk_Load<class Virtual>(Main::Simple_Load(Project,"Virtual"),"V",this->Virtual);

        /* Record names of native process and virtual machine */
        for(std::unique_ptr<class Native>&It : this->Native)
            this->Native_Map.insert(std::make_pair(It->Name,It.get()));

        for(std::unique_ptr<class Virtual>&It : this->Virtual)
            this->Virtual_Map.insert(std::make_pair(It->Name,It.get()));
    }
    catch(const std::exception& Expect)
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_WARN,
                          _("Project information load"),
                          _(std::string(Expect.what())+" The file is corrupted, and some features may not work properly."));
    }

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
    /* Name */
    Main::Text_Save(Project,"Name",this->Name);
    /* Version */
    Main::Text_Save(Project,"Version",std::string(SOFTWARE_VERSION));
    /* Assert_Enable */
    Main::Yesno_Save(Project,"Assert_Enable",this->Assert_Enable);
    /* Debug_Log_Enable */
    Main::Yesno_Save(Project,"Debug_Log_Enable",this->Debug_Log_Enable);
    /* Pgtbl_Raw_Enable */
    Main::Yesno_Save(Project,"Pgtbl_Raw_Enable",this->Pgtbl_Raw_Enable);
    /* Buildsystem */
    Main::Text_Save(Project,"Buildsystem",this->Buildsystem);
    /* Workspace_Overwrite */
    Main::Yesno_Save(Project,"Workspace_Overwrite",this->Workspace_Overwrite);
    /* Chip */
    this->Chip->Save(Main::Simple_Save(Project,"Chip"));
    /* Extmem */
    Trunk_Save<class Mem_Info>(Main::Simple_Save(Project,"Extmem"),"E",this->Extmem);
    /* Shmem */
    Trunk_Save<class Mem_Info>(Main::Simple_Save(Project,"Shmem"),"S",this->Shmem);
    /* Kernel */
    this->Kernel->Save(Main::Simple_Save(Project,"Kernel"));
    /* Monitor */
    this->Monitor->Save(Main::Simple_Save(Project,"Monitor"));
    /* Native information */
    Trunk_Save<class Native>(Main::Simple_Save(Project,"Native"),"N",this->Native);
    /* VM information */
    Trunk_Save<class Virtual>(Main::Simple_Save(Project,"Virtual"),"V",this->Virtual);

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
*******************************************************************************/
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

/* Function:Proj_Info::Native_Remove ******************************************
Description : Remove the native process.
Input       : const std::string& Name - The Name of native process which should
              be removed.
Output      : None.
Return      : ret_t - Return 1, if native process was removed successfully.
******************************************************************************/
ret_t Proj_Info::Native_Remove(const std::string& Name)
{
    std::map<std::string,class Native*>::iterator Map_Target;
    std::vector<std::unique_ptr<class Native>>::iterator Vec_Target;

    wxLogDebug("Proj_Info::Native_Remove");

    if(this->Native_Map.find(Name)==this->Native_Map.end())
        return 0;

    Map_Target=this->Native_Map.find(Name);
    if(Map_Target!=this->Native_Map.end())
        this->Native_Map.erase(Map_Target);
    for(Vec_Target=this->Native.begin();Vec_Target!=this->Native.end();++Vec_Target)
    {
        if(Vec_Target->get()->Name==Name)
        {
            this->Native.erase(Vec_Target);
            break;
        }
    }
    return 1;
}
/* End Function:Proj_Info::~Proj_Info ****************************************/

/* Function:Proj_Info::Virtual_Remove *****************************************
Description : Remove the virtual machine.
Input       : const std::string& Name - The Name of vitual machine which should
              be removed.
Output      : None.
Return      : ret_t - Return 1, if virtual machine was removed successfully.
******************************************************************************/
ret_t Proj_Info::Virtual_Remove(const std::string& Name)
{
    std::map<std::string,class Virtual*>::iterator Map_Target;
    std::vector<std::unique_ptr<class Virtual>>::iterator Vec_Target;

    wxLogDebug("Proj_Info::Virtual_Remove");


    if(this->Virtual_Map.find(Name)==this->Virtual_Map.end())
        return 0;

    Map_Target=this->Virtual_Map.find(Name);
    if(Map_Target!=this->Virtual_Map.end())
        this->Virtual_Map.erase(Map_Target);
    for (Vec_Target=this->Virtual.begin();Vec_Target!=this->Virtual.end();++Vec_Target)
    {
        if(Vec_Target->get()->Name==Name)
        {
            this->Virtual.erase(Vec_Target);
            break;
        }
    }
    return 1;
}
/* End Function:Proj_Info::Virtual_Remove *************************************/

/* Function:Proj_Info::Native_Rename *******************************************
Description : Rename the native process at the information level, the Native and
              Native_Map in Proj_Info will be modify.
Input       : const std::string& Original - The original name of native process.
              const std::string& Current - The current name of native process.
Output      : None.
Return      : ret_t - Return 1, if the native process is renamed successfully.
*******************************************************************************/
ret_t Proj_Info::Native_Rename(const std::string& Original,const std::string& Current)
{
    std::string Current_Upper;
    class wxString Project_Output_Path;
    std::map<std::string,class Native*>::iterator Map_Iter;

    /* Used to check for repeated names */
    Current_Upper=Current;
    std::transform(Current_Upper.begin(),Current_Upper.end(),Current_Upper.begin(),::toupper);

    if(Original==Current)
        return 1;
    if(this->Native_Map.find(Original)==this->Native_Map.end())
        return 0;
    for(std::unique_ptr<class Native>&It : this->Native)
        if(It->Name_Upper==Current_Upper)
            return 0;
    for(std::unique_ptr<class Virtual>&It : this->Virtual)
        if(It->Name_Upper==Current_Upper)
            return 0;

    /* Delete the original information */
    Map_Iter=this->Native_Map.find(Original);
    this->Native_Map.erase(Map_Iter);
    for(std::unique_ptr<class Native>&It : this->Native)
    {
        if(It->Name==Original)
        {
            wxLogDebug("Proj_Info::Native_Rename: %s -> %s",Original,Current);

            It->Name=Current;
            It->Name_Upper=Current_Upper;

            this->Native_Map.insert(std::make_pair(Current,It.get()));

            /* Update the path of project output if it meets the format requirements */
            Project_Output_Path=It->Project_Output;
            if(Project_Output_Path.starts_with("./"+Original))
            {
                Project_Output_Path.Replace(Original,Current);
                It->Project_Output=Project_Output_Path;
            }
            return 1;
        }
    }
    return 0;
}
/* End Function:Proj_Info::Native_Rename *****************************************/

/* Function:Proj_Info::Virtual_Rename *********************************************
Description : Rename the virtual machine at the information level, the Virtual and
              Virtual_Map in Proj_Info will be modify.
Input       : const std::string& Original - The original name of virtual machine.
              const std::string& Current - The current name of virtual machine.
Output      : None.
Return      : ret_t - Return 1, if the virtual machine is renamed successfully.
**********************************************************************************/
ret_t Proj_Info::Virtual_Rename(const std::string& Original,const std::string& Current)
{
    std::string Current_Upper;
    class wxString Project_Output_Path;
    std::map<std::string,class Virtual*>::iterator Map_Iter;

    /* Used to check for repeated names */
    Current_Upper=Current;
    std::transform(Current_Upper.begin(),Current_Upper.end(),Current_Upper.begin(),::toupper);

    if(Original==Current)
        return 1;
    if(this->Virtual_Map.find(Original)==this->Virtual_Map.end())
        return 0;
    for(std::unique_ptr<class Native>&It : this->Native)
        if(It->Name_Upper==Current_Upper)
            return 0;
    for(std::unique_ptr<class Virtual>&It : this->Virtual)
        if(It->Name_Upper==Current_Upper)
            return 0;

    /* Delete the original information */
    Map_Iter=this->Virtual_Map.find(Original);
    this->Virtual_Map.erase(Map_Iter);

    for(std::unique_ptr<class Virtual>&It : Main::Proj_Info->Virtual)
    {
        if(It->Name==Original)
        {
            wxLogDebug("Proj_Info::Virtual_Rename: %s -> %s",Original,Current);

            It->Name=Current;
            It->Name_Upper=Current_Upper;

            this->Virtual_Map.insert(std::make_pair(Current,It.get()));

            /* Update the path of project output if it meets the format requirements */
            Project_Output_Path=It->Project_Output;
            if(Project_Output_Path.starts_with("./"+Original))
            {
                Project_Output_Path.Replace(Original,Current);
                It->Project_Output=Project_Output_Path;
            }
            return 1;
        }
    }

    return 0;
}
/* End Function:Proj_Info::Virtual_Rename ************************************/

/* Function:Proj_Info::Native_Add *********************************************
Description : Add a new native process.
Input       : const std::string& Name - The name of the new native process.
Output      : None.
Return      : ret_t - Return 1, if the native process is added successfully.
******************************************************************************/
ret_t Proj_Info::Native_Add(const std::string& Name)
{
    std::string Name_Upper;

    /* Name check */
    Name_Upper=Name;
    std::transform(Name_Upper.begin(),Name_Upper.end(),Name_Upper.begin(),::toupper);

    /* The name cannot be repetitive */
    for(std::unique_ptr<class Native>&It : this->Native)
        if(It->Name_Upper==Name_Upper)
            return 0;
    for(std::unique_ptr<class Virtual>&It : this->Virtual)
        if(It->Name_Upper==Name_Upper)
            return 0;

    /* Name is valid*/
    this->Native.push_back(std::make_unique<class Native>(Name));
    this->Native_Map.insert(std::make_pair(Name,this->Native.back().get()));
    return 1;
}
/* End Function:Proj_Info::Native_Add ****************************************/

/* Function:Proj_Info::Virtual_Add ********************************************
Description : Add a new virtual machine.
Input       : const std::string& Name - The name of the new virtual machine.
Output      : None.
Return      : ret_t - Return 1, if the virtual machine is added successfully.
******************************************************************************/
ret_t Proj_Info::Virtual_Add(const std::string& Name)
{
    std::string Name_Upper;

    /* Ask for a name */
    Name_Upper=Name;
    std::transform(Name_Upper.begin(),Name_Upper.end(),Name_Upper.begin(),::toupper);

    /* The name cannot be repetitive */
    for(std::unique_ptr<class Native>&It : this->Native)
        if(It->Name_Upper==Name_Upper)
            return 0;
    for(std::unique_ptr<class Virtual>&It : this->Virtual)
        if(It->Name_Upper==Name_Upper)
            return 0;

    /* Name is valid */
    this->Virtual.push_back(std::make_unique<class Virtual>(Name));
    this->Virtual_Map.insert(std::make_pair(Name,this->Virtual.back().get()));
    return 1;
}
/* End Function:Proj_Info::Virtual_Add ***************************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
