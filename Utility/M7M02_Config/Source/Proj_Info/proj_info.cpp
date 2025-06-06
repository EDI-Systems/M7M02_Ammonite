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
#include "wx/sstream.h"
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

/* Function:Proj_Info::Default_Load *******************************************
Description : Load the default project, and save to file. This needs to assume
              a chip name, the kernel source folder, and the monitor folder.
              The default project will have no processes, and the kernel data
              segments will be place just behind the memory trunk.
Input       : const std::string& Path - The path to the file.
              const class Plat_Info* Plat_Info - The platform information.
              const class Chip_Info* Chip_Info - The chip information.
              const std::string& Chipname - The exact name of the chip.
Output      : None.
Return      : ret_t - Always successful.
******************************************************************************/
ret_t Proj_Info::Default_Load(const std::string& Path,
                              const class Plat_Info* Plat_Info,
                              const class Chip_Info* Chip_Info,const std::string& Chipname)
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
    this->Chip=std::make_unique<class Chip>(Plat_Info, Chip_Info, Chipname);

    /* The RME kernel information */
    this->Kernel=std::make_unique<class Kernel>(Plat_Info, Chip_Info);

    /* The RME kernel information */
    this->Monitor=std::make_unique<class Monitor>(Plat_Info, Chip_Info);

    /* Project cache - empty because it has never been saved */
    this->Cache=std::make_unique<class wxStringOutputStream>();

    return 0;
}
/* End Function:Proj_Info::Default_Load **************************************/

/* Function:Proj_Info::Existing_Load ******************************************
Description : Load an existing project.
Input       : const std::string& Path - The path.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Existing_Load(const std::string& Path)
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
        wxLogDebug("The project does not exist. Consider permission or memory failure.");
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

        /* Native process */
        Trunk_Load<class Native>(Main::Simple_Load(Project,"Native"),"N",this->Native);

        /* Virtual machine */
        Trunk_Load<class Virtual>(Main::Simple_Load(Project,"Virtual"),"V",this->Virtual);

        /* Record names of native process and virtual machine */
        for(std::unique_ptr<class Native>&It:this->Native)
            this->Native_Map.insert(std::make_pair(It->Name,It.get()));

        for(std::unique_ptr<class Virtual>&It:this->Virtual)
            this->Virtual_Map.insert(std::make_pair(It->Name,It.get()));

        /* Project cache - must use a new document derived from the save because wxXML
         * will use self-closing tags by default, interfering with closing checks. */
        Document=std::make_unique<class wxXmlDocument>();
        this->Cache=std::make_unique<class wxStringOutputStream>();

        this->Doc_Save(Document);
        Document->Save(*(this->Cache));
    }
    catch(const std::exception& Exc)
    {
        wxLogDebug(Exc.what());
        return -1;
    }

    return 0;
}
/* End Function:Proj_Info::Existing_Load *************************************/

/* Function:Proj_Info::Doc_Save ***********************************************
Description : Save the project to XML document.
Input       : std::unique_ptr<class wxXmlDocument>& Document - The XML document.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Info::Doc_Save(std::unique_ptr<class wxXmlDocument>& Document)
{
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
    /* Native process */
    Trunk_Save<class Native>(Main::Simple_Save(Project,"Native"),"N",this->Native);
    /* Virtual machine */
    Trunk_Save<class Virtual>(Main::Simple_Save(Project,"Virtual"),"V",this->Virtual);

    /* Attach root node to document */
    Document->SetRoot(Project);
}
/* End Function:Proj_Info::Doc_Save ******************************************/

/* Function:Proj_Info::Change_Detect ******************************************
Description : See if the project info has changed since the last save. We'd just
              drop everything to XML and see if that changes, because attaching
              hooks too all widgets are just too tedious.
Input       : None.
Output      : None.
Return      : ret_t - If saved, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Change_Detect(void)
{
    std::unique_ptr<class wxXmlDocument> Document;
    std::unique_ptr<class wxStringOutputStream> Stream;

    Document=std::make_unique<class wxXmlDocument>();
    Stream=std::make_unique<class wxStringOutputStream>();

    this->Doc_Save(Document);
    Document->Save(*Stream);

    /* See if there are any changes - cannot use raw compare on references */
    if(Stream->GetString().Cmp(this->Cache->GetString())!=0)
    {
        wxLogDebug("Proj_Info::Change_Detect - changes detected");
        return -1;
    }
    else
        wxLogDebug("Proj_Info::Change_Detect - no change detected");

    return 0;
}
/* End Function:Proj_Info::Change_Detect *************************************/

/* Function:Proj_Info::Save ***************************************************
Description : Save the project to disk.
Input       : std::unique_ptr<class wxXmlDocument>& Document - The XML document.
Output      : None.
Return      : None.
******************************************************************************/
void Proj_Info::Save(void)
{
    std::unique_ptr<class wxXmlDocument> Document;
    std::unique_ptr<class wxStringOutputStream> Stream;

    Document=std::make_unique<class wxXmlDocument>();
    Stream=std::make_unique<class wxStringOutputStream>();

    /* Save to disk */
    this->Doc_Save(Document);
    Document->Save(this->Path);

    /* And update cache as well */
    Document->Save(*Stream);
    this->Cache=std::move(Stream);
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

/* Function:Proj_Info::Proc_Name_Dup_Check ************************************
Description : Check for possible duplicate process names.
Input       : const std::string& Name - The name to check.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Proc_Name_Dup_Check(const std::string& Name)
{
    std::string Name_Upper;

    /* Name check */
    Name_Upper=Name;
    std::transform(Name_Upper.begin(),Name_Upper.end(),Name_Upper.begin(),::toupper);

    /* The name cannot be repetitive */
    for(std::unique_ptr<class Native>&It:this->Native)
    {
        if(It->Name_Upper==Name_Upper)
            return -1;
    }
    for(std::unique_ptr<class Virtual>&It:this->Virtual)
    {
        if(It->Name_Upper==Name_Upper)
            return -1;
    }

    return 0;
}
/* End Function:Proj_Info::Proc_Name_Dup_Check *******************************/

/* Function:Proj_Info::Native_Add *********************************************
Description : Add a new native process.
Input       : const std::string& Name - The name of the new native process.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Native_Add(const std::string& Name)
{
    wxLogDebug("Proj_Info::Native_Add");

    /* Check name duplication */
    if(this->Proc_Name_Dup_Check(Name)!=0)
        return -1;

    this->Native.push_back(std::make_unique<class Native>(Name));
    this->Native_Map.insert(std::make_pair(Name,this->Native.back().get()));

    return 0;
}
/* End Function:Proj_Info::Native_Add ****************************************/

/* Function:Proj_Info::Native_Remove ******************************************
Description : Remove the native process.
Input       : const std::string& Name - The name of the native process to remove.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Native_Remove(const std::string& Name)
{
    std::vector<std::unique_ptr<class Native>>::iterator Iter;

    wxLogDebug("Proj_Info::Native_Remove");

    if(this->Native_Map.find(Name)==this->Native_Map.end())
        return -1;

    this->Native_Map.erase(Name);
    for(Iter=this->Native.begin();Iter!=this->Native.end();Iter++)
    {
        if((*Iter)->Name==Name)
        {
            this->Native.erase(Iter);
            break;
        }
    }

    return 0;
}
/* End Function:Proj_Info::Native_Remove *************************************/

/* Function:Proj_Info::Native_Move ********************************************
Description : Move the native process after another.
Input       : const std::string& Name - The name of the native process to move.
              const std::string& After - The name that it should follow.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Native_Move(const std::string& Name, const std::string& After)
{
    std::unique_ptr<class Native> Native;
    std::vector<std::unique_ptr<class Native>>::iterator Iter;

    wxLogDebug("Proj_Info::Native_Move");

    if((this->Native_Map.find(Name)==this->Native_Map.end())||
       (this->Native_Map.find(After)==this->Native_Map.end()))
        return -1;

    /* Get this native process */
    for(Iter=this->Native.begin();Iter!=this->Native.end();Iter++)
    {
        if((*Iter)->Name==Name)
        {
            Native=std::move(*Iter);
            this->Native.erase(Iter);
            break;
        }
    }

    /* And insert it after the designated node */
    for(Iter=this->Native.begin();Iter!=this->Native.end();Iter++)
    {
        if((*Iter)->Name==After)
        {
            this->Native.insert(Iter+1,std::move(Native));
            break;
        }
    }

    return 0;
}
/* End Function:Proj_Info::Native_Move ***************************************/

/* Function:Proj_Info::Native_Rename ******************************************
Description : Rename the native process at the information level, the Native and
              Native_Map in Proj_Info will be modify.
Input       : const std::string& Original - The original name of native process.
              const std::string& Current - The current name of native process.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Native_Rename(const std::string& Original,const std::string& Current)
{
    class wxString Project_Output;

    /* The name did not change */
    if(Original==Current)
        return 0;

    /* Not found in the database */
    if(this->Native_Map.find(Original)==this->Native_Map.end())
        return -1;

    /* Check name duplication */
    if(this->Proc_Name_Dup_Check(Current)!=0)
        return -1;

    /* Delete the original information */
    this->Native_Map.erase(Original);
    for(std::unique_ptr<class Native>& Nat:this->Native)
    {
        if(Nat->Name==Original)
        {
            wxLogDebug("Proj_Info::Native_Rename: %s -> %s",Original,Current);

            Nat->Name=Current;
            Nat->Name_Upper=Nat->Name;
            std::transform(Nat->Name_Upper.begin(),
                           Nat->Name_Upper.end(),
                           Nat->Name_Upper.begin(),::toupper);

            this->Native_Map.insert(std::make_pair(Current,Nat.get()));

            /* Update the path of project output if it meets the format requirements */
            Project_Output=Nat->Project_Output;
            if(Project_Output.starts_with("./"+Original))
            {
                Project_Output.Replace(Original,Current);
                Nat->Project_Output=Project_Output;
            }

            return 0;
        }
    }

    return -1;
}
/* End Function:Proj_Info::Native_Rename *************************************/

/* Function:Proj_Info::Virtual_Add ********************************************
Description : Add a new virtual machine.
Input       : const std::string& Name - The name of the new virtual machine.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Virtual_Add(const std::string& Name)
{
    wxLogDebug("Proj_Info::Virtual_Add");

    /* Check name duplication */
    if(this->Proc_Name_Dup_Check(Name)!=0)
        return -1;

    this->Virtual.push_back(std::make_unique<class Virtual>(Name));
    this->Virtual_Map.insert(std::make_pair(Name,this->Virtual.back().get()));

    return 0;
}
/* End Function:Proj_Info::Virtual_Add ***************************************/

/* Function:Proj_Info::Virtual_Remove *****************************************
Description : Remove the virtual machine.
Input       : const std::string& Name - The Name of vitual machine which should
                                        be removed.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Virtual_Remove(const std::string& Name)
{
    std::vector<std::unique_ptr<class Virtual>>::iterator Iter;

    wxLogDebug("Proj_Info::Virtual_Remove");

    if(this->Virtual_Map.find(Name)==this->Virtual_Map.end())
        return -1;

    this->Virtual_Map.erase(Name);
    for(Iter=this->Virtual.begin();Iter!=this->Virtual.end();Iter++)
    {
        if((*Iter)->Name==Name)
        {
            this->Virtual.erase(Iter);
            break;
        }
    }

    return 0;
}
/* End Function:Proj_Info::Virtual_Remove ************************************/

/* Function:Proj_Info::Virtual_Move *******************************************
Description : Move the virtual machine after another.
Input       : const std::string& Name - The name of virtual machine which should
                                        be moved.
              const std::string& After - The name that it should follow.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Virtual_Move(const std::string& Name, const std::string& After)
{
    std::unique_ptr<class Virtual> Virtual;
    std::vector<std::unique_ptr<class Virtual>>::iterator Iter;

    wxLogDebug("Proj_Info::Virtual_Move");

    if((this->Virtual_Map.find(Name)==this->Virtual_Map.end())||
       (this->Virtual_Map.find(After)==this->Virtual_Map.end()))
        return -1;

    /* Get this native process */
    for(Iter=this->Virtual.begin();Iter!=this->Virtual.end();Iter++)
    {
        if((*Iter)->Name==Name)
        {
            Virtual=std::move(*Iter);
            this->Virtual.erase(Iter);
            break;
        }
    }

    /* And insert it after the designated node */
    for(Iter=this->Virtual.begin();Iter!=this->Virtual.end();Iter++)
    {
        if((*Iter)->Name==After)
        {
            this->Virtual.insert(Iter+1,std::move(Virtual));
            break;
        }
    }

    return 0;
}
/* End Function:Proj_Info::Virtual_Move **************************************/

/* Function:Proj_Info::Virtual_Rename *****************************************
Description : Rename the virtual machine at the information level, the Virtual and
              Virtual_Map in Proj_Info will be modify.
Input       : const std::string& Original - The original name of virtual machine.
              const std::string& Current - The current name of virtual machine.
Output      : None.
Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
ret_t Proj_Info::Virtual_Rename(const std::string& Original,const std::string& Current)
{
    class wxString Project_Output;

    /* Name did not change */
    if(Original==Current)
        return 0;

    /* Not found in the database */
    if(this->Virtual_Map.find(Original)==this->Virtual_Map.end())
        return -1;

    /* Check name duplication */
    if(this->Proc_Name_Dup_Check(Current)!=0)
        return -1;

    /* Delete the original information */
    this->Virtual_Map.erase(Original);
    for(std::unique_ptr<class Virtual>& Virt:Main::Proj_Info->Virtual)
    {
        if(Virt->Name==Original)
        {
            wxLogDebug("Proj_Info::Virtual_Rename: %s -> %s",Original,Current);

            Virt->Name=Current;
            Virt->Name_Upper=Virt->Name;
            std::transform(Virt->Name_Upper.begin(),
                           Virt->Name_Upper.end(),
                           Virt->Name_Upper.begin(),::toupper);

            this->Virtual_Map.insert(std::make_pair(Current,Virt.get()));

            /* Update the path of project output if it meets the format requirements */
            Project_Output=Virt->Project_Output;
            if(Project_Output.starts_with("./"+Original))
            {
                Project_Output.Replace(Original,Current);
                Virt->Project_Output=Project_Output;
            }

            return 0;
        }
    }

    return -1;
}
/* End Function:Proj_Info::Virtual_Rename ************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
