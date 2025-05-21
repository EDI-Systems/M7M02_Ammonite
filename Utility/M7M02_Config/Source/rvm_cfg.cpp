
/******************************************************************************
Filename    : rvm_cfg.cpp
Author      : pry
Date        : 06/04/2021
License     : LGPL v3+; see COPYING for details.
Description : RVM project frontend software.
              File extensions used:
              *.rvp - XML project file.
              *.rva - XML platform description file.
              *.rvc - XML chip description file.
******************************************************************************/

/* Include *******************************************************************/
extern "C"
{
#include "stdio.h"
#include "ctype.h"
#include "time.h"
}

#include "set"
#include "map"
#include "atomic"
#include "thread"
#include "memory"
#include "string"
#include "vector"
#include "algorithm"

#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/notebook.h"
#include "wx/grid.h"
#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/treectrl.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/metafile.h"
#include "wx/mimetype.h"
#include "wx/dnd.h"
#include "wx/regex.h"

extern "C"
{
#ifdef WIN32
#include "imagehlp.h"
#else
#include "unistd.h"
#include "backtrace.h"
#endif
}

#include "Image/logo.xpm"

#define __HDR_DEF__

#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"

#include "Status_Bar/status_bar.hpp"

//#include "Option_Notebook/Memory_Panel/memory_panel.hpp"
//#include "Option_Notebook/Shared_Panel/shared_panel.hpp"
//#include "Option_Notebook/Kernel_Basic_Panel/kernel_basic_panel.hpp"
//#include "Option_Notebook/Kernel_Basic_Panel/kernel_basic_panel.hpp"
//#include "Option_Notebook/Monitor_Basic_Panel/monitor_basic_panel.hpp"
//#include "Option_Notebook/Monitor_Output_Panel/monitor_output_panel.hpp"
//#include "Option_Notebook/Proc_Basic_Panel/proc_basic_panel.hpp"
//#include "Option_Notebook/Proc_Output_Panel/proc_output_panel.hpp"
//#include "Option_Notebook/Virt_Basic_Panel/virt_basic_panel.hpp"
//#include "Option_Notebook/Virt_Output_Panel/virt_output_panel.hpp"
//#include "Option_Notebook/Proc_Thd_Panel/proc_thd_panel.hpp"
//#include "Option_Notebook/Proc_Inv_Panel/proc_inv_panel.hpp"
//#include "Option_Notebook/Proc_Prt_Panel/proc_prt_panel.hpp"
//#include "Option_Notebook/Proc_Rcv_Panel/proc_rcv_panel.hpp"
//#include "Option_Notebook/Proc_Snd_Panel/proc_snd_panel.hpp"
//#include "Option_Notebook/Proc_Vct_Panel/proc_vct_panel.hpp"
//#include "Option_Notebook/Proc_Kfn_Panel/proc_kfn_panel.hpp"
//#include "Option_Notebook/option_notebook.hpp"

/* option panels begin*/
#include "Option_Panel/option_panel.hpp"
//#include "Option_Panel/Basic_Notebook/basic_notebook.hpp"
//#include "Option_Panel/Basic_Notebook/Basic_Panel/basic_panel.hpp"
//#include "Option_Panel/Kernel_Notebook/kernel_notebook.hpp"
//#include "Option_Panel/Monitor_Notebook/monitor_notebook.hpp"
#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Native_Notebook/Native_Basic_Panel/native_basic_panel.hpp"
#include "Option_Panel/Virtual_Notebook/virtual_notebook.hpp"
#include "Option_Panel/Virtual_Notebook/Virtual_Basic_Panel/virtual_basic_panel.hpp"

#include "Option_Panel/Basic_Panel/basic_panel.hpp"
#include "Option_Panel/Memory_Notebook/memory_notebook.hpp"
//#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#include "Option_Panel/Kernel_Panel/kernel_panel.hpp"
#include "Option_Panel/Monitor_Panel/monitor_panel.hpp"

/* option panels end*/

/* data begin */
#include "Plat_Info/plat_info.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#include "Plat_Info/Config/config.hpp"
#include "Chip_Info/chip_info.hpp"
/* data end */

#include "Output_Notebook/output_notebook.hpp"

#include "Config_Tree/config_tree.hpp"
#include "Config_Tree/Config_Popup/config_popup.hpp"

#include "Menu_Bar/menu_bar.hpp"
#include "Tool_Bar/tool_bar.hpp"

#include "About_Dialog/about_dialog.hpp"

#include "Proj_Info/Chip/chip.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Proj_Info/proj_info.hpp"

#include "Setting_Dialog/setting_dialog.hpp"
#include "Choose_Dialog/choose_dialog.hpp"
#include "Name_Dialog/name_dialog.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Global Variables **********************************************************/
class Main* RVM_CFG_App::Main=nullptr;

/* Executable path */
std::string Main::Exe_Folder="";

/* Application states */
ptr_t Main::UI_State=UI_NONE+1;
ptr_t Main::Save_State=SAVE_NONE;

/* Three key relative paths of RVM, RME, RMP */
std::string Main::RVM_Path="";
std::string Main::RME_Path="";
std::string Main::RMP_Path="";

/* Platform and chip */
std::string Main::Platform="";
std::string Main::Chip="";
std::string Main::Type="";

/* The largest number ever used by native process and virtual machine */
cnt_t Main::Native_Cnt=0;
cnt_t Main::Virtual_Cnt=0;

/* Lower-level classes */
class Menu_Bar* Main::Menu_Bar=nullptr;
class Tool_Bar* Main::Tool_Bar=nullptr;
class wxPanel* Main::App_Panel=nullptr;
class wxBoxSizer* Main::App_Sizer=nullptr;
class wxBoxSizer* Main::Config_Sizer=nullptr;
class Config_Tree* Main::Config=nullptr;
class wxBoxSizer* Main::Option_Sizer=nullptr;
class Option_Panel* Main::Option=nullptr;
//class Output_Notebook* Main::Output=nullptr;
class wxTextCtrl* Main::Output_Text=nullptr;
class Status_Bar* Main::Status_Bar=nullptr;

class About_Dialog* Main::About_Dialog=nullptr;
class Setting_Dialog* Main::Setting_Dialog=nullptr;
class Choose_Dialog* Main::Choose_Dialog=nullptr;
class Name_Dialog* Main::Name_Dialog=nullptr;

/* option panels  */
class wxWindow* Main::Last_Option=nullptr;
class Basic_Panel* Main::Basic_Panel=nullptr;
class Memory_Notebook* Main::Memory_Notebook=nullptr;
class Kernel_Panel* Main::Kernel_Panel=nullptr;
class Monitor_Panel* Main::Monitor_Panel=nullptr;
class Native_Notebook* Main::Native_Notebook=nullptr;
class Virtual_Notebook* Main::Virtual_Notebook=nullptr;

/* data  */
std::unique_ptr<class Plat_Info> Main::Plat_Info=nullptr;
std::unique_ptr<class Chip_Info> Main::Chip_Info=nullptr;
std::unique_ptr<class Proj_Info> Main::Proj_Info=nullptr;


/* End Global Variables ******************************************************/

/* Function:Main::Main ********************************************************
Description : Application class constructor. The frame size is 640x480.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Main::Main(void):
wxFrame(nullptr,wxID_ANY,SOFTWARE_NAME,wxDefaultPosition,I2P(wxSize(1024,800)),
        wxSYSTEM_MENU|wxCLOSE_BOX|wxCAPTION|wxCLIP_CHILDREN)
{
    //I2P(wxSize(1024,768)
    this->SetSize(I2P(wxSize(1024,800)));
    this->Center();

    try
    {
        wxLogDebug(wxT("Main::Main"));

        /* Set icon */
        this->SetIcon(wxIcon(M7M02_Logo));

        /* This is a singleton. If this is not nullptr, we must have initialized it */
        ASSERT(RVM_CFG_App::Main==nullptr,"This application is a singleton.");

        /* Event handlers */
        this->Bind(wxEVT_CLOSE_WINDOW,&Main::On_Close_Window,this,this->GetId());

        /* Menubar on the top */
        Main::Menu_Bar=new class Menu_Bar();
        this->SetMenuBar(Main::Menu_Bar);
        /* Application panel in the middle with two toolbars; by default, the build toolbar is active */
        Main::App_Panel=new class wxPanel(this);
        Main::App_Sizer=new class wxBoxSizer(wxVERTICAL);
        Main::App_Panel->SetSizer(Main::App_Sizer);
        /* Tool bar */
        Main::Tool_Bar=new class Tool_Bar(Main::App_Panel);
        Main::App_Sizer->Insert(0,Main::Tool_Bar,0,wxEXPAND);
        /* Option area */
        Main::Config_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        Main::Config=new class Config_Tree(Main::App_Panel);
        Main::Config_Sizer->Add(Main::Config, 1, wxEXPAND);

        Main::Option_Sizer=new class wxBoxSizer(wxVERTICAL);
        Main::Option=new class Option_Panel(Main::App_Panel);
        Main::Option_Sizer->Add(Main::Option, 8, wxEXPAND);

//        Main::Output=new class Output_Notebook(Main::App_Panel);
//        Main::Option_Sizer->Add(Main::Output, 1, wxEXPAND);
        Main::Output_Text=new class wxTextCtrl(Main::App_Panel,wxID_ANY);
        Main::Option_Sizer->Add(Main::Output_Text, 1, wxEXPAND);

        Main::Config_Sizer->Add(Main::Option_Sizer, 4, wxEXPAND);
        Main::App_Sizer->Add(Main::Config_Sizer, 1, wxEXPAND);
        /* Spacers */
        Main::App_Sizer->AddSpacer(10);
        Main::App_Sizer->Layout();
        /* Status bar on the bottom */
        Main::Status_Bar=new class Status_Bar(this);
        this->SetStatusBar(Main::Status_Bar);

        /* Create the heavy-handed dialogs so that they load super-fast */
        Main::About_Dialog=new class About_Dialog(this);
        Main::About_Dialog->Hide();
        /* Create setting dialog */
        Main::Setting_Dialog=new class Setting_Dialog(this);
        Main::Setting_Dialog->Hide();
        /* Create choose dialog */
        Main::Choose_Dialog=new class Choose_Dialog(this);
        Main::Choose_Dialog->Hide();
        /* Create name dialog */
        Main::Name_Dialog=new class Name_Dialog(this);
        Main::Name_Dialog->Hide();

        /* Option panel */
        Main::Basic_Panel=new class Basic_Panel(Main::Option);
        Main::Option->Main_Sizer->Add(Main::Basic_Panel,1,wxEXPAND);
        Main::Basic_Panel->Hide();

        Main::Memory_Notebook=new class Memory_Notebook(Main::Option);
        Main::Option->Main_Sizer->Add(Main::Memory_Notebook,1,wxEXPAND);
        Main::Memory_Notebook->Hide();

        Main::Kernel_Panel=new class Kernel_Panel(Main::Option);
        Main::Option->Main_Sizer->Add(Main::Kernel_Panel,1,wxEXPAND);
        Main::Kernel_Panel->Hide();

        Main::Monitor_Panel=new class Monitor_Panel(Main::Option);
        Main::Option->Main_Sizer->Add(Main::Monitor_Panel,1,wxEXPAND);
        Main::Monitor_Panel->Hide();

        Main::Native_Notebook=new class Native_Notebook(Main::Option);
        Main::Option->Main_Sizer->Add(Main::Native_Notebook,1,wxEXPAND);
        Main::Native_Notebook->Hide();

        Main::Virtual_Notebook=new class Virtual_Notebook(Main::Option);
        Main::Option->Main_Sizer->Add(Main::Virtual_Notebook,1,wxEXPAND);
        Main::Virtual_Notebook->Hide();

        this->Refresh();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Main::Main ***************************************************/

/* Function:Main::~Main *******************************************************
Description : Main class destructor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Main::~Main(void)
{
    this->Unbind(wxEVT_CLOSE_WINDOW,&Main::On_Close_Window,this);
}
/* End Function:Main::~Main **************************************************/

/* Function:Main::Title_Set ***************************************************
Description : Show a message box prompting that there is an error.
Input       : const class wxString& Path - The path to append to the caption.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Title_Set(const class wxString& Path)
{
    wxLogDebug("Main::Title_Set");

    if(Path=="")
        RVM_CFG_App::Main->SetTitle(SOFTWARE_NAME);
    else
        RVM_CFG_App::Main->SetTitle(SOFTWARE_NAME+" - ["+Path+"]");
}
/* End Function:Main::Title_Set **********************************************/

/* Function:Main::Time_Get ****************************************************
Description : Extract the date and time.
Input       : const class wxString& Path - The path to append to the caption.
Output      : None.
Return      : None.
******************************************************************************/
std::string Main::Time_Get(ptr_t Mode)
{
    s8_t Date[64];
    time_t Time_Value;
    struct tm* Time_Struct;

    /* Read current time */
    time(&Time_Value);
    Time_Struct=localtime(&Time_Value);

    /* Print to buffer according to extraction mode */
    switch(Mode)
    {
        case TIME_DATE:
        {
            sprintf(Date,"%d-%02d-%02d",
                         Time_Struct->tm_year+1900, Time_Struct->tm_mon+1, Time_Struct->tm_mday);
            break;
        }
        case TIME_HMS:
        {
            sprintf(Date,"%02d:%02d:%02d",
                         Time_Struct->tm_hour, Time_Struct->tm_min, Time_Struct->tm_sec);
            break;
        }
        case TIME_DATE_HMS:
        {
            sprintf(Date,"%d-%02d-%02d %02d:%02d:%02d",
                         Time_Struct->tm_year+1900, Time_Struct->tm_mon+1, Time_Struct->tm_mday,
                         Time_Struct->tm_hour, Time_Struct->tm_min, Time_Struct->tm_sec);
            break;
        }
        default:ASSERT(0,"Time extraction mode is incorrect.");break;
    }

    return Date;
}
/* End Function:Main::Time_Get ***********************************************/

/* Function:Main::Msgbox_Show *************************************************
Description : Show a message box prompting that there is an error.
Input       : class wxWindow* Parent - The parent window.
              ptr_t Type - The message box type.
              const class wxString& Caption - The caption.
              const class wxString& Text - The text.
Output      : None.
Return      : ret_t - The value from "ShowModal".
******************************************************************************/
ret_t Main::Msgbox_Show(class wxWindow* Parent, ptr_t Type,
                        const class wxString& Caption, const class wxString& Text)
{
    class wxMessageDialog* Dialog;
    ret_t Ret;

    wxLogDebug("Main::Msgbox_Show");

    if(Type==MSGBOX_ERROR)
        Dialog=new wxMessageDialog(Parent,Text,Caption,wxOK|wxICON_ERROR);
    else if(Type==MSGBOX_WARN)
        Dialog=new wxMessageDialog(Parent,Text,Caption,wxOK|wxICON_WARNING);
    else if(Type==MSGBOX_INFO)
        Dialog=new wxMessageDialog(Parent,Text,Caption,wxOK|wxICON_INFORMATION);
    else if(Type==MSGBOX_ASK)
        Dialog=new wxMessageDialog(Parent,Text,Caption,wxOK|wxCANCEL|wxICON_QUESTION);
    else
        ASSERT(0,"Wrong message box type specified.");

    Ret=Dialog->ShowModal();
    Dialog->Destroy();

    return Ret;
}
/* End Function:Main::Msgbox_Show ********************************************/

/* Function:Main::Idtfr_Check *************************************************
Description : Check if the name is good or not.
Input       : const std::string& Name - The name to check.
Output      : None.
Return      : ret_t - If valid, 0; else -1.
******************************************************************************/
ret_t Main::Idtfr_Check(const std::string& Name)
{
    if(Name=="")
        return -1;
    if((Name[0]>='0')&&(Name[0]<='9'))
        return -1;
    if(Name.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")!=Name.npos)
        return -1;

    return 0;
}
/* End Function:Main::Idtfr_Check ********************************************/

/* Function:Main::CSV_Read ****************************************************
Description : Split a CSV into an array.
Input       : const std::string& Input - The CSV sequence.
Output      : std::vector<std::string>& Vector - The splitted content.
Return      : None.
******************************************************************************/
void Main::CSV_Read(const std::string& Input, std::vector<std::string>& Vector)
{
    cnt_t Pivot;
    ptr_t Begin;
    ptr_t End;
    std::string Temp;
    std::string Push;

    Temp=Input;

    do
    {
        /* Split the element */
        Pivot=Temp.find_first_of(",");
        if(Pivot<0)
            Push=Temp;
        else
        {
            Push=Temp.substr(0, Pivot);
            Temp=Temp.substr(Pivot+1);
        }
        /* Strip the whitespaces */
        Begin=Push.find_first_not_of(" \t\v");
        if(Begin!=std::string::npos)
        {
            End=Push.find_last_not_of(" \t\v");
            Vector.push_back(Push.substr(Begin, End-Begin+1));
        }
    }
    while(Pivot>=0);
}
/* End Function:Main::CSV_Read ***********************************************/

/* Function:Main::Simple_Load *************************************************
Description : Save a simple node. This includes the name only.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Expect - The label to expect.
Output      : None.
Return      : class wxXmlNode* - The node found with that name.
******************************************************************************/
class wxXmlNode* Main::Simple_Load(class wxXmlNode* Parent, const std::string& Expect)
{
    class wxXmlNode* Child;

    /* Find the child with the expected name, and return the string */
    for(Child=Parent->GetChildren();Child!=nullptr;Child=Child->GetNext())
    {
        if(Child->GetName()==Expect)
            return Child;
    }
    throw std::runtime_error(Expect+": No such label found.");
}
/* End Function:Main::Simple_Load ********************************************/

/* Function:Main::Text_Load ***************************************************
Description : Load a text node. This includes the name and its content. Will
              throw exceptions if the name is not what is expected, or it contains
              nothing at all.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Expect - The label to expect.
Output      : None.
Return      : std::string - The content returned.
******************************************************************************/
std::string Main::Text_Load(class wxXmlNode* Parent, const std::string& Expect)
{
    class wxXmlNode* Text;
    Text=Main::Simple_Load(Parent,Expect)->GetChildren();
    if(Text==nullptr)
        return "";
    if(Text->GetType()!=wxXML_TEXT_NODE)
        throw std::runtime_error(Expect+": Label does not contain any text.");

    return std::string(Text->GetContent());
}
/* End Function:Main::Text_Load **********************************************/

/* Function:Main::Yesno_Load **************************************************
Description : Load a yes/no node, and return the parsed value directly.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Expect - The label to expect.
Output      : None.
Return      : ptr_t - The option result.
******************************************************************************/
ptr_t Main::Yesno_Load(class wxXmlNode* Parent, const std::string& Expect)
{
    std::string Temp;

    Temp=Main::Text_Load(Parent, Expect);

    if(Temp=="Yes")
        return OPTION_YES;
    else if(Temp=="No")
        return OPTION_NO;

    throw std::runtime_error(Expect+": Label contains something else.");
}
/* End Function:Main::Yesno_Load *********************************************/

/* Function:Main::Num_Load ****************************************************
Description : Load a number node. This includes the name and its content. Will
              throw exceptions if the name is not what is expected, or it contains
              nothing at all.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Expect - The label to expect.
Output      : None.
Return      : ptr_t - The content returned.
******************************************************************************/
ptr_t Main::Num_Load(class wxXmlNode* Parent, const std::string& Expect)
{
    class wxXmlNode* Text;

    Text=Main::Simple_Load(Parent,Expect)->GetChildren();
    if(Text==nullptr)
        throw std::runtime_error(Expect+": Label does not contain any number.");
    if(Text->GetType()!=wxXML_TEXT_NODE)
        throw std::runtime_error(Expect+": Label does not contain any number.");

    /* see if this is a number */
    return std::stoull(std::string(Text->GetContent()),nullptr,0);
}
/* End Function:Main::Num_Load ***********************************************/

/* Function:Main::CSV_Load ****************************************************
Description : Get comma-separated values from the XML entry.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Expect - The label to expect.
Output      : std::vector<std::string>& Vector - The CSV extracted.
Return      : None.
******************************************************************************/
void Main::CSV_Load(class wxXmlNode* Parent, const std::string& Expect,
                    std::vector<std::string>& Vector)
{
    std::string Temp;

    Temp=Main::Text_Load(Parent, Expect);
    Main::CSV_Read(Temp,Vector);
}
/* End Function:Main::CSV_Load ***********************************************/

/* Function:Main::Pair_Load ***************************************************
Description : Load a key-value pair node. This includes many subnodes.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Expect - The label to expect.
Output      : std::map<std::string, std::string>& Map - The key-value pairs.
Return      : ptr_t - The content returned.
******************************************************************************/
void Main::Pair_Load(class wxXmlNode* Parent, const std::string& Expect,
                     std::map<std::string, std::string>& Map)
{
    class wxXmlNode* Child;
    class wxXmlNode* Text;
    std::string Key;
    std::string Value;

    Child=Main::Simple_Load(Parent,Expect)->GetChildren();
    if(Child==nullptr)
        throw std::runtime_error(Expect+": Main label does not contain any pairs.");
    if(Child->GetType()!=wxXML_ELEMENT_NODE)
        throw std::runtime_error(Expect+": Main label does not contain any sublabels.");

    /* Handle sublabels */
    do
    {
        /* See if the child exists in the map */
        Key=Child->GetName();
        if(Map.find(Key)!=Map.end())
            throw std::runtime_error(Expect+": One of the sublabels have duplicates.");
        /* See if the child contains anything valid */
        Text=Child->GetChildren();
        if(Text->GetType()!=wxXML_TEXT_NODE)
            throw std::runtime_error(Expect+": One of the sublabels does not contain any content.");
        Value=Text->GetContent();

        Map.insert(std::make_pair(Key,Value));

        /* Next child */
        Child=Child->GetNext();
    }
    while(Child!=nullptr);
}
/* End Function:Main::Pair_Load **********************************************/

/* Function:Main::Opt_Load ****************************************************
Description : Load a optimization level node.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Expect - The label to expect.
Output      : None.
Return      : ptr_t - The content returned.
******************************************************************************/
ptr_t Main::Opt_Load(class wxXmlNode* Parent, const std::string& Expect)
{
    std::string Temp;

    Temp=Main::Text_Load(Parent,Expect);

    if(Temp=="O0")
        return OPTIMIZATION_O0;
    else if(Temp=="O1")
        return OPTIMIZATION_O1;
    else if(Temp=="O2")
        return OPTIMIZATION_O2;
    else if(Temp=="O3")
        return OPTIMIZATION_O3;
    else if(Temp=="Of")
        return OPTIMIZATION_OF;
    else if(Temp=="Os")
        return OPTIMIZATION_OS;
    else
        throw std::runtime_error(Expect+": Optimization level unrecognized.");

    return OPTIMIZATION_O0;
}
/* End Function:Main::Opt_Load ***********************************************/

/* Function:Main::Simple_Save *************************************************
Description : Save a simple node. This includes the name only.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::Simple_Save(class wxXmlNode* Parent, const std::string& Name)
{
    class wxXmlNode* Temp_N;
    class wxXmlNode* Temp_C;

    /* Add to content - null node helps to preserve the pair even if it is empty */
    Temp_N=new wxXmlNode(nullptr,wxXML_ELEMENT_NODE,Name);
    Temp_C=new wxXmlNode(wxXML_TEXT_NODE,wxEmptyString,"");
    /* Add in this way so that the sequence is preserved */
    Temp_N->AddChild(Temp_C);
    Parent->AddChild(Temp_N);

    return Temp_N;
}
/* End Function:Main::Simple_Save ********************************************/

/* Function:Main::Text_Save ***************************************************
Description : Save a text node. This includes the name and its content.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
              const std::string& Content - The content of the label.
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::Text_Save(class wxXmlNode* Parent,
                                 const std::string& Name, const std::string& Content)
{
    class wxXmlNode* Temp_N;
    class wxXmlNode* Temp_C;

    /* Add to content */
    Temp_N=new wxXmlNode(nullptr,wxXML_ELEMENT_NODE,Name);
    Temp_C=new wxXmlNode(wxXML_TEXT_NODE,wxEmptyString,Content);
    /* Add in this way so that the sequence is preserved */
    Temp_N->AddChild(Temp_C);
    Parent->AddChild(Temp_N);

    return Temp_N;
}
/* End Function:Main::Text_Save **********************************************/

/* Function:Main::Hex_Save ****************************************************
Description : Save a hex node. This includes the name and its content.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
              ptr_t Padding - The zeros needed for padding it.
              ptr_t Content - The content of the label.
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::Hex_Save(class wxXmlNode* Parent,
                                const std::string& Name, ptr_t Padding, ptr_t Content)
{
    char Cmd[16];
    char Buf[64];

    /* Convert to hex according to command */
    sprintf(Cmd,"0x%%0%llullX",Padding);
    sprintf(Buf,Cmd,Content);

    return Main::Text_Save(Parent,Name,Buf);
}
/* End Function:Main::Text_Save **********************************************/

/* Function:Main::Yesno_Save **************************************************
Description : Save a yes/no node. The filling is internally processed.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
              ptr_t Yesno - The yes/no option.
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::Yesno_Save(class wxXmlNode* Parent, const std::string& Name, ptr_t Yesno)
{
    if(Yesno==OPTION_YES)
        return Main::Text_Save(Parent,Name,"Yes");
    else if(Yesno==OPTION_NO)
        return Main::Text_Save(Parent,Name,"No");
    else
        ASSERT(0,std::string("The node'")+Name+"' does not contain a valid yes/no value.");
}
/* End Function:Main::Yesno_Save *********************************************/

/* Function:Main::Num_Save ****************************************************
Description : Save a number node. This includes the name and its content.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
              ptr_t Content - The content of the label.
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::Num_Save(class wxXmlNode* Parent,
                                const std::string& Name, ptr_t Content)
{
    return Main::Text_Save(Parent,Name,std::to_string(Content));
}
/* End Function:Main::Num_Save ***********************************************/

/* Function:Main::CSV_Save *************************************************
Description : Save the CSV, use ',' to connect.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
              const std::vector<string>& - The CSV content.
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::CSV_Save(class wxXmlNode* Parent, const std::string& Name,
                                const std::vector<std::string>& Content)
{
    ptr_t Count;
    std::string List;
    class wxXmlNode* Temp_N;
    class wxXmlNode* Temp_C;

    /* Add to content - null node helps to preserve the pair even if it is empty */
    Temp_N=new wxXmlNode(nullptr,wxXML_ELEMENT_NODE,Name);
    /* Use ',' to connect every element of content*/
    if(!Content.empty())
    {
        List=Content[0];
        for(Count=1;Count<Content.size();Count++)
            List+=","+Content[Count];
    }

    Temp_C=new wxXmlNode(wxXML_TEXT_NODE,wxEmptyString,List);
    /* Add in this way so that the sequence is preserved */
    Temp_N->AddChild(Temp_C);
    Parent->AddChild(Temp_N);

    return Temp_N;
}
/* End Function:Main::CSV_Save ********************************************/

/* Function:Main::Pair_Save ***************************************************
Description : Save a key-value pair node. This includes many subnodes.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
              const std::map<std::string, std::string>& Map - The content of the pairs.
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::Pair_Save(class wxXmlNode* Parent,
                                 const std::string& Name,
                                 const std::map<std::string, std::string>& Map)
{
    class wxXmlNode* Child;

    /* Create main node */
    Child=new wxXmlNode(nullptr,wxXML_ELEMENT_NODE,Name);
    /* Save subnodes maintaining the order */
    for(const std::pair<const std::string, std::string>& Pair:Map)
        Main::Text_Save(Child,Pair.first,Pair.second);
    /* Add to parent */
    Parent->AddChild(Child);

    return Child;
}
/* End Function:Main::Pair_Save **********************************************/

/* Function:Main::Opt_Save ****************************************************
Description : Save a optimization level node.
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
              ptr_t Content - The content of the label.
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::Opt_Save(class wxXmlNode* Parent,
                                const std::string& Name, ptr_t Content)
{
    std::string Temp;

    switch(Content)
    {
        case OPTIMIZATION_O0:Temp="O0";break;
        case OPTIMIZATION_O1:Temp="O1";break;
        case OPTIMIZATION_O2:Temp="O2";break;
        case OPTIMIZATION_O3:Temp="O3";break;
        case OPTIMIZATION_OF:Temp="Of";break;
        case OPTIMIZATION_OS:Temp="Os";break;
        default:ASSERT(0,std::string("The node'")+Name+"' does not contain a valid optimization level.");
    }

    return Main::Text_Save(Parent,Name,Temp);
}
/* End Function:Main::Opt_Save ***********************************************/

/* Function:Main::File_List ***************************************************
Description : List all concrete files in a folder. The list only includes the
              filename.
Input       : const std::string& Path - The path for this folder. It could be
                                        a relative one or an absolute one.
Output      : std::vector<std::string>& List - The list containing concrete files.
Return      : ret_t - If 0, folder empty; else -1.
******************************************************************************/
ret_t Main::File_List(const std::string& Path, std::vector<std::string>& List)
{
    ret_t Empty;
    size_t Pos;
    std::string Abspath;

    wxLogDebug("Main::File_List");

    List.clear();
    Empty=0;

    /* Canonicalize first, just in case */
    Abspath=Main::Path_Absolute(PATH_DIR, "", Path);

#ifdef WIN32
    WIN32_FIND_DATAA Find;
    void* Handle;
    std::string Search_Path;
    std::string Filename;

    /* Forge the search path */
    Search_Path = Abspath+"*";

    /* Search for files - use the multibyte version */
    Handle=FindFirstFileA(Search_Path.c_str(),&Find);
    if(Handle==INVALID_HANDLE_VALUE)
        wxLogDebug("Windows FindFirstFile failed for path '%s', maybe wrong path.",Abspath.c_str());
    else
    {
        do
        {
            Filename=Find.cFileName;
            if((Filename!=".")&&(Filename!=".."))
                Empty=-1;
            /* We only need files whose extension that matches whatever is listed here */
            if((Find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
            {
                wxLogDebug("Processing file '%s'.",Find.cFileName);
                Pos=Filename.find_last_of('.');
                /* Does the file ever have a extension? */
                if((Pos==0)||(Pos==std::string::npos))
                {
                    wxLogDebug("File '%s' doesn't have a name or extension, not included.",Find.cFileName);
                    continue;
                }
                List.push_back(Filename);
            }
        }
        while(FindNextFileA(Handle,&Find)!=0);
        if(GetLastError()!=ERROR_NO_MORE_FILES)
            wxLogDebug("Windows FindNextFile failed for path '%s', maybe wrong path.",Abspath.c_str());
        FindClose(Handle);
    }
#else
#error THIS IS NEVER TESTED ON LINUX
    DIR* Dir;
    struct dirent* Info;
    std::string Filename;

    /* Search for files */
    Dir=opendir(this->Abspath.c_str());
    if(Dir==0)
        Main::Info("Linux opendir failed for path '%s', maybe wrong path.",this->Relpath);
    else
    {
        while(1)
        {
            Info=readdir(Dir);
            if(Info==0)
                break;
            Filename=Info->d_name;
            if((Filename!=".")&&(Filename!=".."))
                Empty=-1;
            /* We only need files whose extension that matches whatever is listed here */
            if(Info->d_type!=DT_DIR)
            {
                /* Does the file ever have a extension? */
                Pos=Extension.find_last_of('.');
                if((Pos==0)||(Pos==std::string::npos))
                {
                    Main::Info("File '%s' doesn't have a name or extension, not included.",Info->d_name);
                    continue;
                }
                List.push_back(Filename);
            }
        }
        closedir(Dir);
    }
#endif

    std::sort(List.begin(),List.end());
    return Empty;
}
/* End Function:Main::File_List ************************************************/

/* Function:Main::Dir_List *****************************************************
Description : List all concrete dir in a folder. The list only includes the
              dir name.
Input       : const std::string& Path - The path for this folder. It could be
                                        a relative one or an absolute one.
Output      : std::vector<std::string>& List - The list containing concrete files.
Return      : ret_t - If 0, folder empty; else -1.
********************************************************************************/
ret_t Main::Dir_List(const std::string& Path, std::vector<std::string>& List)
{
    ret_t Empty;
    size_t Pos;
    std::string Abspath;

    wxLogDebug("Main::Dir_List");

    List.clear();
    Empty=0;

    /* Canonicalize first, just in case */
    Abspath=Main::Path_Absolute(PATH_DIR, "", Path);

#ifdef WIN32
    WIN32_FIND_DATAA Find;
    void* Handle;
    std::string Search_Path;
    std::string Filename;

    /* Forge the search path */
    Search_Path = Abspath + "*";
    /* Search for files - use the multibyte version */
    Handle = FindFirstFileA(Search_Path.c_str(), &Find);
    if (Handle == INVALID_HANDLE_VALUE) {
        wxLogDebug("Windows FindFirstFile failed for path '%s'.", Abspath.c_str());
    } else {
        do {
            Filename = Find.cFileName;
            if ((Filename != ".") && (Filename != ".."))
                Empty = -1;
            else
                continue;
            /* Dir only*/
            if ((Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
                List.push_back(Filename);
            }
        } while (FindNextFileA(Handle, &Find) != 0);

        if (GetLastError() != ERROR_NO_MORE_FILES)
            wxLogDebug("Windows FindNextFile failed for path '%s'.", Abspath.c_str());

        FindClose(Handle);
    }
#else
#error THIS IS NEVER TESTED ON LINUX
    DIR* Dir;
    struct dirent* Info;
    std::string Filename;

    /* Search for files */
    Dir=opendir(this->Abspath.c_str());
    if(Dir==0)
        Main::Info("Linux opendir failed for path '%s', maybe wrong path.",this->Relpath);
    else
    {
        while(1)
        {
            Info=readdir(Dir);
            if(Info==0)
                break;
            Filename=Info->d_name;
            if((Filename!=".")&&(Filename!=".."))
                Empty=-1;
            else
                continue;
            /* We only need dir whose extension that matches whatever is listed here */
            if(Info->d_type==DT_DIR)
            {
                /* Does the file ever have a extension? */
                Pos=Extension.find_last_of('.');
                if((Pos==0)||(Pos==std::string::npos))
                {
                    Main::Info("File '%s' doesn't have a name or extension, not included.",Info->d_name);
                    continue;
                }
                List.push_back(Filename);
            }
        }
        closedir(Dir);
    }
#endif

    std::sort(List.begin(),List.end());
    return Empty;
}
/* End Function:Main::File_List ************************************************/

/* Function:Main::Row_Swap ******************************************************
Description : Swap two rows in a given grid.
Input       : class wxGrid* Grid - The grid to swap contents.
              ptr_t First - The position of the first row.
              ptr_t Second - The position of the second row.
Output      : None.
Return      : None.
********************************************************************************/
void Main::Row_Swap(class wxGrid* Grid,ptr_t First,ptr_t Second)
{
    cnt_t Count;
    class wxString Temp;

    wxLogDebug("Main::Row_Swap");

    /* See if the grid have the two rows. If it doesn't contain either of the two, bug */
    ASSERT(First<(ptr_t)Grid->GetNumberRows(),"First row exceeded grid size.");
    ASSERT(Second<(ptr_t)Grid->GetNumberRows(),"First row exceeded grid size.");

    /* Swap the content one by one. Validators are left aside because they're irrelevant */
    for(Count=0;Count<Grid->GetNumberCols();Count++)
    {
        Temp=Grid->GetCellValue(First,Count);
        Grid->SetCellValue(First,Count,Grid->GetCellValue(Second,Count));
        Grid->SetCellValue(Second,Count,Temp);
    }
}
/* End Function:Main::Row_Swap *************************************************/

/* Function:Main::Row_Get *******************************************************
Description : Get the currently selected row in a given grid. If more than one
              row is selected, we're returning -1.
Input       : class wxGrid* Grid - The grid to check.
Output      : None.
Return      : ret_t - The row selected. If nothing, -1.
********************************************************************************/
ret_t Main::Row_Get(class wxGrid* Grid)
{
    class wxArrayInt Rows;

    //wxLogDebug("Main::Row_Get");

    /* See if any line is selected. If no, we can't do anything */
    Rows=Grid->GetSelectedRows();
    if((Rows.IsEmpty())||(Rows.GetCount()!=1))
        return -1;
    return Rows[0];
}
/* End Function:Main::Row_Get **************************************************/

/* Function:Main::Row_Add *******************************************************
Description : Add one row in a given grid.
Input       : class wxGrid* Grid - The grid to add a row.
Output      : None.
Return      : ret_t - The position that the row is at.
********************************************************************************/
ret_t Main::Row_Add(class wxGrid* Grid)
{
    ret_t Row;

    //wxLogDebug("Main::Row_Add");

    Row=Main::Row_Get(Grid);
    /* If nothing is selected, or multiple ones are selected, we add to the end */
    if(Row==-1)
        Row=Grid->GetNumberRows();
    else
        Row+=1;
    Grid->InsertRows(Row,1,false);
    Grid->SelectRow(Row);

    return Row;
}
/* End Function:Main::Row_Add **************************************************/

/* Function:Main::Row_Pick ******************************************************
Description : Pick one row in a given grid, should the user select more than one.
Input       : class wxGrid* Grid - The grid to pick one row.
Output      : None.
Return      : None.
********************************************************************************/
void Main::Row_Pick(class wxGrid* Grid)
{
    class wxArrayInt Rows;

    //wxLogDebug("Main::Row_Pick");

    Rows=Grid->GetSelectedRows();
    /* No need to pick, nothing or only one selected */
    if((Rows.IsEmpty())||(Rows.Count()==1))
        return;
    Grid->SelectRow(Rows[0]);
}
/* End Function:Main::Row_Pick *************************************************/

/* Function:Main::Row_Remove ****************************************************
Description : Remove one row in a given grid.
Input       : class wxGrid* Grid - The grid to remove a row.
Output      : None.
Return      : None.
********************************************************************************/
void Main::Row_Remove(class wxGrid* Grid)
{
    ret_t Row;
    ret_t Remain;
    class wxString Temp;

    //wxLogDebug("Main::Row_Remove");

    Row=Main::Row_Get(Grid);
    if(Row==-1)
        return;

    Grid->DeleteRows(Row);
    Remain=Grid->GetNumberRows();
    if(Remain!=0)
    {
        if(Row>=Remain)
            Grid->SelectRow(Remain-1);
        else
            Grid->SelectRow(Row);
    }
}
/* End Function:Main::Row_Remove ***********************************************/

/* Function:Main::Row_Move_Up ***************************************************
Description : Move a selected row up in a given grid.
Input       : class wxGrid* Grid - The grid to operate.
Output      : None.
Return      : None.
********************************************************************************/
void Main::Row_Move_Up(class wxGrid* Grid)
{
    ret_t Row;
    class wxString Temp;

    //wxLogDebug("Main::Row_Move_Up");

    Row=Main::Row_Get(Grid);
    if((Row==-1)||(Row==0))
        return;

    Main::Row_Swap(Grid,Row,Row-1);
    Grid->SelectRow(Row-1);
}
/* End Function:Main::Row_Move_Up **********************************************/

/* Function:Main::Row_Move_Down *************************************************
Description : Move a selected row down in a given grid.
Input       : class wxGrid* Grid - The grid to operate.
Output      : None.
Return      : None.
********************************************************************************/
void Main::Row_Move_Down(class wxGrid* Grid)
{
    ret_t Row;
    class wxString Temp;

    //wxLogDebug("Main::Row_Move_Down");

    Row=Main::Row_Get(Grid);
    if((Row==-1)||(Row==(Grid->GetNumberRows()-1)))
        return;

    Main::Row_Swap(Grid,Row,Row+1);
    Grid->SelectRow(Row+1);
}
/* End Function:Main::Row_Move_Down ********************************************/

/* Function:Main::Row_Reorder ***************************************************
Description : Update sequence number of the grid.
Input       : class wxGrid* Grid - The grid to operate.
Output      : None.
Return      : None.
********************************************************************************/
void Main::Row_Reorder(class wxGrid* Grid)
{
    ret_t Row;

    for(Row=0;Row<(cnt_t)Grid->GetNumberRows();++Row)
        Grid->SetCellValue(Row,0,std::to_string(Row+1));
}
/* End Function:Main::Row_Reorder **********************************************/

/* Function:Main::Row_Clear_Content *********************************************
Description : Clear the grid content.
Input       : class wxGrid* Grid - The grid to operate.
Output      : None.
Return      : None.
********************************************************************************/
void Main::Gird_Clear_Content(class wxGrid* Grid)
{
    ret_t Row;
    ret_t Tot_Row;

    Tot_Row = Grid->GetNumberRows();
    //wxLogDebug("Main::Row_Clear_Content: Total %lld",Tot_Row);
    for (Row=Tot_Row;Row>0;--Row)
        Grid->DeleteRows(Row-1);
}
/* End Function:Main::Row_Clear_Content ****************************************/

/* Function:Main::Num_GZ_Check **************************************************
Description : Check whether 'Num' is a valid decimal positive integer, 0, 00, 01
              are not allow.
Input       : std::string Num - The number that needs check.
Output      : None.
Return      : ret_t - If 0, check pass; else -1.
********************************************************************************/
ret_t Main::Num_GZ_Check(std::string Num)
{
    if(Num=="")
        return -1;
    /* 0, 00, 01 are not allow */
    if((Num.size()>1||Num.size()==1)&&Num[0]=='0')
        return -1;
    if(Num.find_first_not_of("0123456789")!=Num.npos)
        return -1;
    return 0;
}
/* End Function:Main::Num_GZ_Check *********************************************/

/* Function:Main::Num_GEZ_Check *************************************************
Description : Check whether 'Num' is a valid decimal nonnegative integer.
Input       : std::string Num - The number that needs check.
Output      : None.
Return      : ret_t - If 0, check pass; else -1.
********************************************************************************/
ret_t Main::Num_GEZ_Check(std::string Num)
{
    if(Num=="")
        return -1;
//    /* Leading zeros are not allowed, such as '0x01' */
//    if (Num.size()>1&&Num[0]=='0')
//        return -1;
    if(Num.find_first_not_of("0123456789")!=Num.npos)
        return -1;
    return 0;
}
/* End Function:Main::Num_GEZ_Check ********************************************/

/* Function:Main::Num_GEZ_Hex_Check *********************************************
Description : Check whether 'Num' is a valid hexadecimal nonnegative integer.
Input       : std::string Num - The number that needs check.
Output      : None.
Return      : ret_t - If 0, check pass; else -1.
********************************************************************************/
ret_t Main::Num_GEZ_Hex_Check(std::string Num)
{
    if(Num.size()<2)
        return -1;
    if(Num[0]!='0'||(Num[1]!='x'&&Num[1]!='X'))
        return -1;
    Num=Num.substr(2);

    if(Num=="")
        return -1;
//    /* Leading zeros are not allowed, such as '0x01' */
//    if (Num.size()>1&&Num[0]=='0')
//        return -1;
    if(Num.find_first_not_of("ABCDEFabcdef0123456789")!=Num.npos)
        return -1;
    return 0;
}
/* End Function:Main::Num_GEZ_Hex_Check ****************************************/

/* Function:Main::Row_Name_Check ************************************************
Description : Check if any name is repeated.
Input       : const ptr_t& Type - Whether to allow the name to be a blank string.
              class wxGrid* Grid - The grid that needs check.
              const ptr_t& Name_Col - The column of Name.
              const std::string& Location - The location of the grid.
Output      : None.
Return      : ret_t - If 0, check pass; else -1.
********************************************************************************/
ret_t Main::Row_Name_Check(class wxGrid* Grid, const std::string& Location, const ptr_t& Type, const ptr_t& Name_Col)
{
    cnt_t Row;
    std::string Name;
    std::string Name_Upper;
    std::set<std::string>Unique_Name_Check;

    if(Type==BLANK_NAME_FORBID)
    {
        for(Row=0;Row<(cnt_t)Grid->GetNumberRows();Row++)
        {
            /* Name */
            Name=Grid->GetCellValue(Row,Name_Col);
            Name_Upper=Grid->GetCellValue(Row,Name_Col).Upper();
            if(Main::Idtfr_Check(Name_Upper))
                    {
                        Main::Msgbox_Show(Grid,MSGBOX_ERROR,
                                          _(Location),
                                          _("There is a invalid C identifier, row "+std::to_string(Row+1)));
                        return -1;
                    }

            /* Identifiers must be unique regardless of case */
            if(Unique_Name_Check.find(Name_Upper)==Unique_Name_Check.end())
                Unique_Name_Check.insert(Name_Upper);
            else
            {
                Main::Msgbox_Show(Grid,MSGBOX_ERROR,
                                  _(Location),
                                  _("Identifiers must be unique regardless of case, "+
                                  Name+" is repeated, row "+std::to_string(Row+1)));
                return -1;
            }
        }
        return 0;
    }
    else if(Type==BLANK_NAME_PERMIT)
    {
        for(Row=0;Row<(cnt_t)Grid->GetNumberRows();Row++)
        {
            /* Name */
            Name=Grid->GetCellValue(Row,Name_Col);
            Name_Upper=Grid->GetCellValue(Row,Name_Col).Upper();
            /* Identifiers must be unique regardless of case */
            if(Name!="")
            {
                if(Unique_Name_Check.find(Name_Upper)==Unique_Name_Check.end())
                    Unique_Name_Check.insert(Name_Upper);
                else
                {
                    Main::Msgbox_Show(Grid,MSGBOX_ERROR,
                                      _(Location),
                                      _("Identifiers must be unique regardless of case, "+
                                      Name+" is repeated, or you can leave it blank, row "+std::to_string(Row+1)));
                    return -1;
                }
            }
        }
        return 0;
    }
    wxLogError("No BLANK_NAME_FORBID or BLANK_NAME_PERMIT is specified");
    return -1;
}
/* End Function:Main::Row_Name_Check *******************************************/

/* Function:Main::Comp_Check ****************************************************
Description : check if it is a legal path
Input       : const wxString& Guest - The guest type.
              const wxString& Toolchain - The toolchain.
              const wxString& Buildsystem - The buildsystem.
Output      : None.
Return      : ret_t - If 0, check pass; else -1.
********************************************************************************/
ret_t Main::Comp_Check(const wxString& Buildsystem, const wxString& Toolchain, const wxString& Guest)
{
    for(const std::unique_ptr<class Compatible>&Comp : Main::Plat_Info->Compatible)
        if(Buildsystem==Comp->Buildsystem&&Toolchain==Comp->Toolchain&&Guest==Comp->Guest)
            return 0;
    return -1;
}
/* End Function:Main::Comp_Check ***********************************************/


/* Function:Main::File_Validate *************************************************
Description : See if the filename or path is valid. The filename must be all lower
              case letters plus the 0-9 and _.
Input       : const std::string& File - The name or path to validate.
Output      : None.
Return      : ret_t - If 0, valid; else invalid.
********************************************************************************/
ret_t Main::File_Validate(const std::string& Filename)
{
    std::string Ext;
    std::string Name;
    class wxFileName Temp;

    wxLogDebug("Main::File_Validate");

    Temp=Filename;
    Name=Temp.GetName();
    Ext=Temp.GetExt();

    /* Name and extension cannot be empty */
    if((Name=="")||(Ext==""))
        return -1;

    /* Name and suffix cannot include anything suspicious */
    if(Name.find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyz_")!=Name.npos)
        return -1;
    if(Ext.find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyz_")!=Ext.npos)
        return -1;

    /* May not begin with a number */
    if((Name[0]>='0')&&(Name[0]<='9'))
        return -1;
    if((Ext[0]>='0')&&(Ext[0]<='9'))
        return -1;

    return 0;
}
/* End Function:Main::File_Validate ********************************************/

/* Function:Main::Check_And_Save_Current_Edit ***********************************
Description : Check and try to save the current panel. Note: If a certain panel is
              currently being shown, the user may have modified its information,
              so it needs to be checked and saved. This function is necessary before
              Check_And_Save_All, especially when you finish editing a panel and leave
              it in a shown state before clicking Save or Save As.
Input       : None.
Output      : None.
Return      : ret_t - If 0, check pass; else -1.
********************************************************************************/
ret_t Main::Check_And_Save_Current_Edit()
{
    /* Check and try to save the current panel */
    if(Main::Basic_Panel->IsShown())
    {
        if(Main::Basic_Panel->Check())
            return -1;
        Main::Basic_Panel->Save();
    }
    else if(Main::Memory_Notebook->IsShown())
    {
        if(Main::Memory_Notebook->Check())
            return -1;
        Main::Memory_Notebook->Save();
    }
    else if(Main::Kernel_Panel->IsShown())
    {
        if(Main::Kernel_Panel->Check())
            return -1;
        Main::Kernel_Panel->Save();
    }
    else if(Main::Monitor_Panel->IsShown())
    {
        if(Main::Monitor_Panel->Check())
            return -1;
        Main::Monitor_Panel->Save();
    }
    else if(Main::Native_Notebook->IsShown())
    {
        if(Main::Native_Notebook->Check())
            return -1;
        Main::Native_Notebook->Save();
    }
    else if(Main::Virtual_Notebook->IsShown())
    {
        if(Main::Virtual_Notebook->Check())
            return -1;
        Main::Virtual_Notebook->Save();
    }
    return 0;
}
/* End Function:Main::Check_And_Save_Current_Edit ******************************/

///* Function:Main::Check_And_Save_ALL ********************************************
//Description : Load information from Proj_Info into the GUI in sequence and check its
//              validity (the validation can only be performed via the GUI). If the
//              information is valid, save it.()
//Input       : None.
//Output      : None.
//Return      : ret_t - If 0, check pass; else -1.
//********************************************************************************/
//ret_t Main::Check_And_Save_All()
//{
//    /* Basic configuration */
//    Main::Basic_Panel->Load();
//    if(Basic_Panel->Check()!=0)
//    {
//        if(Main::Last_Option!=nullptr)
//            Main::Last_Option->Hide();
//        Main::Basic_Panel->Show();
//        Main::Option->Layout();
//        Main::Last_Option=Main::Basic_Panel;
//        return -1;
//    }
//    Main::Basic_Panel->Save();
//
//    /* Memory notebook */
//    Main::Memory_Notebook->Load();
//    if(Memory_Notebook->Check()!=0)
//    {
//        if(Main::Last_Option!=nullptr)
//            Main::Last_Option->Hide();
//        Main::Memory_Notebook->Show();
//        Main::Option->Layout();
//        Main::Last_Option=Main::Memory_Notebook;
//        return -1;
//    }
//    Main::Memory_Notebook->Save();
//
//    /* Kernel configuration */
//    Main::Kernel_Panel->Load();
//    if(Kernel_Panel->Check()!=0)
//    {
//        if(Main::Last_Option!=nullptr)
//            Main::Last_Option->Hide();
//        Main::Kernel_Panel->Show();
//        Main::Option->Layout();
//        Main::Last_Option=Main::Kernel_Panel;
//        return -1;
//    }
//    Main::Kernel_Panel->Save();
//
//    /* Monitor configuration */
//    Main::Monitor_Panel->Load();
//    if(Monitor_Panel->Check()!=0)
//    {
//        if(Main::Last_Option!=nullptr)
//            Main::Last_Option->Hide();
//        Main::Monitor_Panel->Show();
//        Main::Option->Layout();
//        Main::Last_Option=Main::Monitor_Panel;
//        return -1;
//    }
//    Main::Monitor_Panel->Save();
//
//    /* Native process configuration */
//    for(std::unique_ptr<class Native>&Native : Main::Proj_Info->Native)
//    {
//
//        Main::Native_Notebook->Load(Native->Name);
//        if(Main::Native_Notebook->Check()!=0)
//        {
//            if(Main::Last_Option!=nullptr)
//                Main::Last_Option->Hide();
//            Main::Native_Notebook->Show();
//            Main::Option->Layout();
//            Main::Last_Option=Main::Native_Notebook;
//            return -1;
//        }
//        Main::Native_Notebook->Save();
//    }
//
//    /* Virtual machine configuration */
//    for(std::unique_ptr<class Virtual>&Virtual : Main::Proj_Info->Virtual)
//    {
//        Main::Virtual_Notebook->Load(Virtual->Name);
//        if(Main::Virtual_Notebook->Check()!=0)
//        {
//            if(Main::Last_Option!=nullptr)
//                Main::Last_Option->Hide();
//            Main::Virtual_Notebook->Show();
//            Main::Option->Layout();
//            Main::Last_Option=Main::Virtual_Notebook;
//            return -1;
//        }
//        Main::Virtual_Notebook->Save();
//    }
//    return 0;
//}
///* End Function:Main::Check_And_Save_ALL ***************************************/

/* Function:Main::Update_GUI ****************************************************
Description : Chip config ,coprocessor, build system, tool chain, and guest can be
              load only after loading Plat_Info and Chip_Info. Load these necessary
              information, then update the config tree and show default panel.
Input       : None.
Output      : None.
Return      : None.
********************************************************************************/
void Main::GUI_Update()
{
    if(!Main::Plat_Info)
    {
        Main::Msgbox_Show(RVM_CFG_App::Main,MSGBOX_ERROR,
                          _("GUI_Update"),
                          _("Platform information is wrong"));
        return;
    }
    if(!Main::Chip_Info)
    {
        Main::Msgbox_Show(RVM_CFG_App::Main,MSGBOX_ERROR,
                          _("GUI_Update"),
                          _("Chip information is wrong"));
        return;
    }

    /* Update chip configuration, coprocessor, build system, tool chain and guest*/
    Main::Basic_Panel->Config_Set();
    Main::Basic_Panel->Coprocessor_Set();
    Main::Basic_Panel->Compatible_Set();
    Main::Kernel_Panel->Compatible_Set();
    Main::Kernel_Panel->Kernel_Prio_Set();
    Main::Monitor_Panel->Compatible_Set();
    Main::Monitor_Panel->Virt_Prio_Set();
    Main::Monitor_Panel->Virt_Event_Set();
    Main::Native_Notebook->Basic->Coprocessor_Set();
    Main::Native_Notebook->Basic->Compatible_Set();
    Main::Virtual_Notebook->Basic->Coprocessor_Set();
    Main::Virtual_Notebook->Basic->Compatible_Set();

    /* Update config tree */
    Main::Config->Root_Text=Main::Proj_Info->Name;
    Main::Config->Load();
    for(std::unique_ptr<class Native>&It : Main::Proj_Info->Native)
    {
        Main::Config->AppendItem(Main::Config->Native_Config,It->Name);
    }
    for(std::unique_ptr<class Virtual>&It : Main::Proj_Info->Virtual)
    {
        Main::Config->AppendItem(Main::Config->Virtual_Config,It->Name);
    }
    Main::Config->ExpandAll();
    Main::Config->Show();

    /* Show default panel */
    Main::Basic_Panel->Load();
    Main::Basic_Panel->Show();
    Main::Last_Option=Main::Basic_Panel;
    Main::Option->Layout();
}
/* End Function:Main::Update_GUI ***********************************************/

/* Function:Main::Plat_Load *****************************************************
Description : Load the .rva file and list the available chips that belong to the
              platform selected by user.
Input       : const std::string& Plat - The platform selected by user.
Output      : None.
Return      : std::vector<std::string> - The available chips.
********************************************************************************/
std::vector<std::string> Main::Plat_Load(const std::string& Plat)
{
    std::string Abs_Path;
    std::vector<std::string> Chip_Vec;

    wxLogDebug("Main::Load_Plat");
    /* Try to get .rva */
    Abs_Path=Main::Path_Absolute(PATH_DIR,
                                wxPathOnly(wxStandardPaths::Get().GetExecutablePath()).ToStdString(),
                                Main::RVM_Path+"Include/Platform/"+
                                Plat);
    wxLogDebug("Main::Load_Plat .rva is in %s",Abs_Path);
    Main::Plat_Info=std::make_unique<class Plat_Info>(Abs_Path+"/rvm_platform_"+Plat+".rva");

    /* Get chips selections */
    Abs_Path=Abs_Path+"Chip/";

    Main::Dir_List(Abs_Path,Chip_Vec);

    return Chip_Vec;
}
/* End Function:Main::Plat_Load ************************************************/

/* Function:Main::Chip_Load *****************************************************
Description : Load the .rvc file and list the specific type of chip that belong to
              the chip class selected by user.
Input       : const std::string& Plat - The platform selected by user.
              const std::string& Chip - The chip class selected by user.
Output      : None.
Return      : std::vector<std::string> - The specific type of chip.
********************************************************************************/
std::vector<std::string> Main::Chip_Load(const std::string& Plat,const std::string& Chip)
{
    std::string Abs_Path;
    std::vector<std::string> Type_Vec;

    wxLogDebug("Main::Load_Plat_and_Chip");
    /* Try to get .rva */
    Abs_Path=Main::Path_Absolute(PATH_DIR,
                                wxPathOnly(wxStandardPaths::Get().GetExecutablePath()).ToStdString(),
                                Main::RVM_Path+"Include/Platform/"+
                                Plat);

    /* Get chips selections */
    Abs_Path=Abs_Path+"Chip/";
    Abs_Path=Abs_Path+Chip+"/rvm_platform_"+Chip+".rvc";
    wxLogDebug("Choose_Dialog::On_After_Chip: .rvc is in %s",Abs_Path);
    Main::Chip_Info=std::make_unique<class Chip_Info>(Abs_Path);

    for(std::string&Cop : Main::Chip_Info->Compatible)
        Type_Vec.push_back(Cop);

    return Type_Vec;
}
/* End Function:Main::Chip_Load ************************************************/

///* Function:Main::Output_Update ***********************************************
//Description : Update the output panel.
//Input       : std::vector<std::string>& Reply - The replies to show.
//              ptr_t Panel - The panel to show onto.
//              ptr_t Append - Whether to append to it.
//Output      : None.
//Return      : None.
//******************************************************************************/
//void Main::Output_Update(std::vector<std::string>& Reply,ptr_t Panel,ptr_t Append)
//{
//    class wxTextCtrl* Text;
//
//    switch(Panel)
//    {
//        case OUTPUT_GEN:Text=Main::Info_Text;break;
//        case OUTPUT_PROC:Text=Main::Status_Text;break;
//        case OUTPUT_STAT:Text=Main::Status_Text;break;
//        default:ASSERT(0,"Panel is invalid.");
//    }
//
//    /* Post text to the panel */
//    if(Append==0)
//        Text->Clear();
//    for(std::string& Temp:Reply)
//    {
//        if(Temp.back()!='\n')
//            Text->AppendText(Temp+"\n");
//        else
//            Text->AppendText(Temp);
//    }
//}
///* End Function:Main::Output_Update ******************************************/
//
///* Function:Main::Output_Clear ************************************************
//Description : Clear the output panel.
//Input       : ptr_t Panel - The panel to clear.
//Output      : None.
//Return      : None.
//******************************************************************************/
//void Main::Output_Clear(ptr_t Panel)
//{
//    switch(Panel)
//    {
//        case OUTPUT_INFO:Main::Info_Text->Clear();break;
//        case OUTPUT_STATUS:Main::Status_Text->Clear();break;
//        default:ASSERT(0,"Panel is invalid.");
//    }
//}
///* End Function:Main::Output_Clear *******************************************/

/* Function:Main::Settings ****************************************************
Description : Set locations of the folders.
              <Settings>
                  <Path>
                      <RME> </RME> stored as relative path to this software.
                      <RVM> </RVM>
                      <RMP> </RMP>
                  </Path>
              </Settings>
              rme location, rvm location, rmp location -> use xml  .rvi
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Setting(void)
{
    class wxXmlNode* Root;
    class wxString Exe_Path;
    class wxString Rvi_Path;
    class wxXmlDocument Doc;

    /* log */
    wxLogDebug("Main::Setting");

    /* preferences check if the locations are set or not, if not, pop up and tell user to set this */
    Exe_Path=wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
    Rvi_Path=Exe_Path+"/setting.rvi";
    wxLogDebug(Rvi_Path);
    if (wxFileExists(Exe_Path+wxFileName::GetPathSeparator()+"setting.rvi"))
    {
        wxLogDebug("The Setting.rvi is existed");
        if (Doc.Load(Rvi_Path))
        {
            Root=Doc.GetRoot();
            Main::RVM_Path=Main::Text_Load(Root, "RVM");
            Main::RME_Path=Main::Text_Load(Root, "RME");
            Main::RMP_Path=Main::Text_Load(Root, "RMP");
            wxLogDebug("RVM_Path has been set %s",Main::RVM_Path);
            wxLogDebug("RME_Path has been set %s",Main::RME_Path);
            wxLogDebug("RMP_Path has been set %s",Main::RMP_Path);
        }
    }
    else
    {
        wxLogDebug("The Setting.rvi is not existed");
        Main::Setting_Begin();
    }
}
/* End Function:Main::Proj_New ***********************************************/

/* Function:Main::Path_Absolute ***********************************************
Description : Get the canonicalized absolute path for a file or a folder.
Input       : ptr_t Type - The path type.
              const std::string& Root - The root path; when empty, it is treated
                                        as the current working directory.
              const std::string& Path - The potentially relative path.
Output      : None.
Return      : std::string - The canonicalized absolute path.
******************************************************************************/
std::string Main::Path_Absolute(ptr_t Type,
                                const std::string& Root, const std::string& Path)
{
    std::string Abspath;
    class wxFileName Temp;

    wxLogDebug("Main::Path_Absolute - %s - %s", Root.c_str(), Path.c_str());

    if(Path=="")
        return "";

    /* Canonicalize the full path to absolute path */
    Temp=Path;
    /* If we don't have any root, just drop in the path */
    if(Root!="")
    {
        /* Take care of volume labels like C:\ (Windows) or / (Linux) */
        if((Temp.GetVolume()=="")&&(Path.front()!='/'))
            Temp=Root+Path;
    }
    Temp.MakeAbsolute();
    Abspath=Temp.GetFullPath();
    std::replace(Abspath.begin(),Abspath.end(),'\\','/');

    /* If this is a folder, end it with the delimiter if it doesn't already have one */
    if((Type==PATH_DIR)&&(Abspath.back()!='/'))
        Abspath+="/";

    return Abspath;
}
/* End Function:Main::Path_Absolute ******************************************/

/* Function:Main::Path_Relative ***********************************************
Description : Get the canonicalized relative path for a file or a folder.
Input       : ptr_t Type - The path type.
              const std::string& Root - The root path, must not be empty.
              const std::string& Path - The potentially full path.
Output      : None.
Return      : std::string - The canonicalized relative path.
******************************************************************************/
std::string Main::Path_Relative(ptr_t Type,
                                const std::string& Root, const std::string& Path)
{
    std::string Relpath;
    class wxFileName Temp;

    wxLogDebug("Main::Path_Relative - %s - %s", Root.c_str(), Path.c_str());

    if(Path=="")
        return "";

    /* Compute relative path */
    Temp=Path;
    Temp.MakeAbsolute();
    Temp.MakeRelativeTo(Root);
    Relpath=Temp.GetFullPath();
    /* Take care of volume labels like C:\ (Windows) or / (Linux) */
    if((Temp.GetVolume()=="")&&(Path.front()!='/'))
        Relpath=std::string("./")+Relpath;
    /* Canonicalize */
    std::replace(Relpath.begin(),Relpath.end(),'\\','/');

    /* If this is a folder, end it with the delimiter if it doesn't already have one */
    if((Type==PATH_DIR)&&(Relpath.back()!='/'))
        Relpath+="/";

    return Relpath;
}
/* End Function:Main::Path_Relative ******************************************/

/* Function:Main::Setting_Begin ***********************************************
Description : Show setting dialog.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Setting_Begin(void)
{
    wxLogDebug("Main::Setting_Begin");
    Main::Setting_Dialog->Set_Setting(Main::RVM_Path, Main::RME_Path, Main::RMP_Path);
    Main::Setting_Dialog->ShowModal();
}
/* End Function:Main::Setting_Begin ******************************************/

/* Function:Main::Choose_Begin ************************************************
Description : Show choose dialog.
Input       : const std::string& Path - The path of the newly created file.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Choose_Begin(const std::string& Path)
{
    wxLogDebug("Main::Modal_Begin");
    Main::Choose_Dialog->RVP_File_Path=Path;
    Main::Choose_Dialog->Load();
    Main::Choose_Dialog->ShowModal();
}
/* End Function:Main::Choose_Begin *******************************************/

/* Function:Main::Proj_New ****************************************************
Description : Create a new file, at the location specified by the full path.
Input       : const std::string& Path - The path, including the suffix.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Proj_New(const std::string& Path)
{
    wxLogDebug("Main::Proj_New");

    /* Filename should not include anything suspicious */
    if(Main::File_Validate(Path)!=0)
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("New"),
                          _("Filenames may only contain lower case letters, numbers and underscores, and may not begin with numbers."));
        return;
    }

    /* Detect path empty */
    if((Path=="")||(Path.length()<=4))
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("New"),
                          _("Cannot create project at the location."));
        return;
    }

    /* preferences check if the locations are set or not, if not, pop up and tell user to set this */
    if(!wxFileName::DirExists(Main::RVM_Path)||!wxFileName::DirExists(Main::RME_Path))
    {
        wxLogDebug("Check RVM path and RME path");
        wxLogDebug("Invalid RVM path: %s",Main::RVM_Path);
        wxLogDebug("Invalid RME path: %s",Main::RME_Path);
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("New"),
                          _("Invalid RVM path or invalid RME path, please set again."));
        Main::Setting_Begin();
    }
    else
    {
        wxLogDebug("Valid RVM path: %s",Main::RVM_Path);
        wxLogDebug("Valid RME path: %s",Main::RME_Path);
    }

    /* choose platform and chip. */
    Main::Choose_Begin(Path);
    if(Main::Platform==""||Main::Chip=="")
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("New"),
                          _("Failed to create the new project, platform or chip is valid."));
        wxLogDebug("Choose right platform and chip");
        return;
    }

    Main::GUI_Update();
}
/* End Function:Main::Proj_New ***********************************************/

/* Function:Main::Proj_Open ***************************************************
Description : Open an existing file, at the location specified by the full path.
Input       : const std::string& Path - The path, including the suffix.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Proj_Open(const std::string& Path)
{
    wxLogDebug("Main::Proj_Open");

    /* Detect path empty */
    if((Path=="")||(Path.length()<=4))
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Open"),
                          _("Cannot open project at the location."));
        return;
    }

    /* Create a new Proj_Info */
    Main::Proj_Info=std::make_unique<class Proj_Info>();
    Main::Proj_Info.get()->Load(Path);

    Main::Plat_Load(Main::Proj_Info->Chip->Platform);
    Main::Chip_Load(Main::Proj_Info->Chip->Platform,Main::Proj_Info->Chip->Class);
    Main::GUI_Update();
}
/* End Function:Main::Proj_Open **********************************************/

/* Function:Main::Proj_Close **************************************************
Description : Close the current open file.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Proj_Close(void)
{
    wxLogDebug("Main::Proj_Close");
}
/* End Function:Main::Proj_Close *********************************************/

/* Function:Main::Proj_Save ***************************************************
Description : Save a current open file that haven't been saved.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Proj_Save(void)
{
    wxLogDebug("Main::Proj_Save");

    if(Main::Check_And_Save_Current_Edit()!=0)
        return;
//    if(Main::Check_And_Save_All()!=0)
//        return;

    /* Update save status */
    Main::State_Set(STATE_SAVE,SAVE_NONE);

    if(Main::Proj_Info->Save()!=0)
        wxMessageBox("Fail");
    else
        wxMessageBox("Success");
}
/* End Function:Main::Proj_Save **********************************************/

/* Function:Main::Proj_Save_As ************************************************
Description : Save the current open file as another file.
Input       : const std::string& Path - The path, including the suffix.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Proj_Save_As(const std::string& Path)
{
    wxLogDebug("Main::Proj_Save_As");

    /* Update save status */
    Main::State_Set(STATE_SAVE,SAVE_NONE);
    Main::Proj_Info->Path=Path;

    /* There should currently be a project, or we cannot save file */
    if(Main::Proj_Info==nullptr)
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Save As"),
                          _("There should currently be a project, or we cannot save file"));
        return;
    }

    /* Filename should not include anything suspicious */
    if(Main::File_Validate(Path)!=0)
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Save As"),
                          _("Filenames may only contain lower case letters, numbers and underscores, and may not begin with numbers."));
        return;
    }

    /* Detect path empty */
    if((Path=="")||(Path.length()<=4))
    {
        Main::Msgbox_Show(RVM_CFG_App::Main, MSGBOX_ERROR,
                          _("Save As"),
                          _("Cannot create project at the location."));
        return;
    }

    /* Write to disk */
    if(Main::Proj_Info->Save()!=0)
        wxMessageBox("Fail");
    else
        wxMessageBox("Success");

}
/* End Function:Main::Proj_Save_As *******************************************/

/* Function:Main::Manual_Open *************************************************
Description : Open the manual.
Input       : const std::string& Manual - The name of the manual.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Manual_Open(const std::string& Manual)
{
    ptr_t Locale;
    std::string Path;
    class wxMimeTypesManager Manager;

    wxLogDebug("Main::Manual_Open");

    /* Make filename according to version */
    Path=std::string("M7M02_")+
         "R"+std::to_string(FIRMWARE_VERSION>>8)+"T"+std::to_string(FIRMWARE_VERSION&0xFF)+
         "_"+Manual+"_Manual.pdf";

    Locale=wxLocale::GetSystemLanguage();
    if((Locale==wxLANGUAGE_CHINESE)||
       (Locale==wxLANGUAGE_CHINESE_SIMPLIFIED)||
       (Locale==wxLANGUAGE_CHINESE_TRADITIONAL)||
       (Locale==wxLANGUAGE_CHINESE_HONGKONG)||
       (Locale==wxLANGUAGE_CHINESE_MACAU)||
       (Locale==wxLANGUAGE_CHINESE_SINGAPORE)||
       (Locale==wxLANGUAGE_CHINESE_TAIWAN))
        Path=Main::Exe_Folder+"Manual/CN_"+Path;
    else
        Path=Main::Exe_Folder+"Manual/EN_"+Path;

    /* See if the file is readable. If no, we fail silently */
    if(wxIsReadable(Path)==false)
    {
        wxLogDebug("Main::Manual_Open - %s manual does not exist.",Path.c_str());
        return;
    }

    /* Open with system default viewer */
    wxExecute(Manager.GetFileTypeFromExtension("PDF")->GetOpenCommand(Path));
}
/* End Function:Main::Manual_Open ********************************************/

/* Function:Main::State_Update ************************************************
Description : Update state to what it is now.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Main::State_Update(ptr_t Type)
{
    Main::Menu_Bar->State_Set(Type);
    Main::Tool_Bar->State_Set(Type);
    //Main::Config->State_Set(Type);
    //Main::Option->State_Set(Type);
    //Main::Output->State_Set(Type);
    Main::Status_Bar->State_Set(Type);
}
/* End Function:Main::State_Update *******************************************/

/* Function:Main::State_Set ***************************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
              ptr_t State - The actual state.
Output      : None.
Return      : None.
******************************************************************************/
void Main::State_Set(ptr_t Type, ptr_t State)
{
    /* Keep a copy of this global state */
    switch(Type)
    {
        case STATE_UI:Main::UI_State=State;break;
        case STATE_SAVE:Main::Save_State=State;break;
        default:ASSERT(0,"Wrong state specified.");
    }

    /* Propagate only the updated state to the whole application */
    Main::State_Update(Type);
}
/* End Function:Main::State_Set **********************************************/

/* Function:Main::On_Close_Window *********************************************
Description : wxEVT_CLOSE_WINDOW handler.
Input       : class wxCloseEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Main::On_Close_Window(class wxCloseEvent& Event)
{
    wxLogDebug("Main::On_Close_Window");

    /* See if we can veto this event. If we can't, then this is forced */
    if(Event.CanVeto()==false)
        exit(-1);

    /* We don't exit directly here - wait for wxWidgets to clean everything up */
    Event.Skip();
}
/* End Function:Main::On_Close_Window ****************************************/

/* Function:RVM_CFG_App::OnInit ************************************************
Description : Entry of the application.
Input       : None.
Output      : None.
Return      : bool - True: continue processing, false: exit immediately.
******************************************************************************/
/* Create an instance of application class. */
IMPLEMENT_APP(RVM_CFG_App)
bool RVM_CFG_App::OnInit(void)
{
    class wxFileName Root;

    /* Executable absolute path - useful for configs, library copying, etc */
    Root=wxFileName(wxStandardPaths::Get().GetExecutablePath());
    Main::Exe_Folder=Root.GetPathWithSep();
    std::replace(Main::Exe_Folder.begin(),Main::Exe_Folder.end(),'\\','/');

    /* Register a signal handler just in case we have div by zero, etc */
#ifdef WIN32
    /* On Windows, there might be errors that this cannot catch, but that's rare */
    SetUnhandledExceptionFilter(RVM_CFG_App::OnSystemFatalException);
#else
    signal(SIGSEGV,RVM_CFG_App::OnSystemFatalException);
    signal(SIGABRT,RVM_CFG_App::OnSystemFatalException);
    signal(SIGFPE,RVM_CFG_App::OnSystemFatalException);
    signal(SIGILL,RVM_CFG_App::OnSystemFatalException);
#endif

    /* Enable logging if we're debugging */
#ifdef DEBUG
    wxLog::SetLogLevel(wxLOG_Trace);
    wxLog::SetActiveTarget(new class wxLogWindow(NULL,wxT("Log")));
#else
    wxLog::SetLogLevel(wxLOG_FatalError);
    wxDisableAsserts();
#endif

    wxLogDebug(wxT("RVM_CFG_App::OnInit"));

    /* Detect language */
    this->Locale_Set();

    /* Start application */
    RVM_CFG_App::Main=new class Main();
    RVM_CFG_App::Main->Show();

    /* Setting check */
   RVM_CFG_App::Main->Setting();

    return true;
}
/* End Function:RVM_CFG_App::OnInit *******************************************/

/* Function:RVM_CFG_App::Locale_Set ********************************************
Description : Set locale for the whole execution environment.
Input       : None.
Output      : None.
Return      : int - The language ID returned.
******************************************************************************/
int RVM_CFG_App::Locale_Set(void)
{
    /* See if we need to override language */
#ifndef LANGUAGE_OVERRIDE
#define LANGUAGE_OVERRIDE wxLANGUAGE_DEFAULT
#endif
    /* Load language if possible, fall back to English otherwise */
    if(wxLocale::IsAvailable(LANGUAGE_OVERRIDE))
    {
        this->Locale=new wxLocale(LANGUAGE_OVERRIDE);
        this->Locale->AddCatalogLookupPathPrefix(Main::Exe_Folder+".");
        this->Locale->AddCatalogLookupPathPrefix(Main::Exe_Folder+"./Locale");
        this->Locale->AddCatalogLookupPathPrefix(Main::Exe_Folder+"..");
        this->Locale->AddCatalogLookupPathPrefix(Main::Exe_Folder+"../Locale");
        /* When loading, the last takes precedence */
        this->Locale->AddCatalog("rvm_cfg");

        if(!this->Locale->IsOk())
        {
            delete this->Locale;
            this->Locale=new wxLocale(wxLANGUAGE_ENGLISH);
        }
        else
        {
            if(LANGUAGE_OVERRIDE!=wxLANGUAGE_DEFAULT)
                return wxLANGUAGE_ENGLISH;
            else
                return this->Locale->GetSystemLanguage();
        }
    }
    else
        this->Locale=new wxLocale(wxLANGUAGE_ENGLISH);

    return wxLANGUAGE_ENGLISH;
}
/* End Function:RVM_CFG_App::Locale_Set ***************************************/

/* Function:RVM_CFG_App::OnExceptionInMainLoop *********************************
Description : This will be called when there are exceptions in the main loop.
              The application will be terminated, but before the termination,
              we'll try to rescue stuff. This does not catch system-level exceptions,
              like a null pointer dereference.
Input       : None.
Output      : None.
Return      : bool - True: continue processing, false: exit immediately.
******************************************************************************/
bool RVM_CFG_App::OnExceptionInMainLoop(void)
{
    wxLogDebug("RVM_CFG_App::OnExceptionInMainLoop");

    try
    {
        throw;
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,(std::string(_("Unhandled exception"))+": "+Exc.what()).c_str());
    }
    return false;
}
/* End Function:RVM_CFG_App::OnExceptionInMainLoop ****************************/

/* Function:RVM_CFG_App::OnSystemFatalException ********************************
Description : Catches system-level exceptions; still try to rescue stuff.
              Note that this won't fuss when we're exiting in the release mode:
              wxWidgets ITSELF have some known bugs (i.e. loading/unloading fonts,
              etc) that may mess up in the end. This GDI+ private font stuff is
              fixed (3.1.3->3.1.5), but who knows whether there are more.
Input       : int Exc/EXCEPTION_POINTERS* Exc - The exception info.
Output      : None.
Return      : None.
******************************************************************************/
#ifdef WIN32
LONG WINAPI RVM_CFG_App::OnSystemFatalException(EXCEPTION_POINTERS* Exc)
#else
void RVM_CFG_App::OnSystemFatalException(int Exc)
#endif
{
    wxLogDebug("RVM_CFG_App::OnSystemFatalException");

#ifdef WIN32
    int Count;
    std::string Info;
    /* Static allocations so nasty issues can't happen */
    static char Buf[1024];
    static DWORD Stack[6];
    static STACKFRAME Frame;

    memset(&Frame,0,sizeof(STACKFRAME));
    memset(&Stack,0,sizeof(Stack));
    SymInitialize(GetCurrentProcess(),0,true);
    /* setup initial stack frame */
    Frame.AddrPC.Offset=Exc->ContextRecord->Eip;
    Frame.AddrPC.Mode=AddrModeFlat;
    Frame.AddrStack.Offset=Exc->ContextRecord->Esp;
    Frame.AddrStack.Mode=AddrModeFlat;
    Frame.AddrFrame.Offset=Exc->ContextRecord->Ebp;
    Frame.AddrFrame.Mode=AddrModeFlat;
    /* Walk the stack */
    Count=0;
    while(StackWalk(IMAGE_FILE_MACHINE_I386,
                    GetCurrentProcess(),
                    GetCurrentThread(),
                    &Frame,
                    Exc->ContextRecord,
                    0,
                    SymFunctionTableAccess,
                    SymGetModuleBase,
                    0)!=0)
    {
        Stack[Count]=Frame.AddrPC.Offset;
        sprintf(Buf,"\n0x%08lX ",Frame.AddrPC.Offset);
        Info+=Buf;
        /* Show at most 6 stack entries. More are useless */
        Count++;
        if(Count>=6)
            break;
    }
    SymCleanup(GetCurrentProcess());

    /* Stack trace info is only available when -g3 is enabled */
#ifdef DEBUG
    sprintf(Buf,"echo ------------------------------------- &&"
                "addr2line -f -p -e rvm_cfg.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e rvm_cfg.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e rvm_cfg.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e rvm_cfg.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e rvm_cfg.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e rvm_cfg.exe %08lX &&"
                "echo ------------------------------------- && pause",
                Stack[0],Stack[1],Stack[2],Stack[3],Stack[4],Stack[5]);
    system(Buf);
#endif
    /* Don't fuss when we're in the release mode */
#ifndef DEBUG
    return 0;
#endif
    /* Print stack trace */
    switch(Exc->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_STACK_OVERFLOW:
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        case EXCEPTION_DATATYPE_MISALIGNMENT:
        case EXCEPTION_ACCESS_VIOLATION:ASSERT(0,std::string(_("Memory access error.")+Info).c_str());
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_OVERFLOW:
        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
        case EXCEPTION_FLT_UNDERFLOW:ASSERT(0,std::string(_("Floating point error.")+Info).c_str());
        case EXCEPTION_PRIV_INSTRUCTION:
        case EXCEPTION_ILLEGAL_INSTRUCTION:ASSERT(0,std::string(_("Illegal instruction.")+Info).c_str());
        case EXCEPTION_IN_PAGE_ERROR:
        case EXCEPTION_INVALID_DISPOSITION:
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:ASSERT(0,std::string(_("Execution abort.")+Info).c_str());
        default:ASSERT(0,std::string(_("Unknown error.")).c_str());
    }
#else
    /* Don't fuss when we're exiting in the release mode */
#ifndef DEBUG
    if(Main::Exit_Flag!=0)
        return;
#endif
    switch(Exc)
    {
        case SIGSEGV:ASSERT(0,std::string(_("Memory access error.")).c_str());
        case SIGABRT:ASSERT(0,std::string(_("Execution abort.")).c_str());
        case SIGFPE:ASSERT(0,std::string(_("Floating point error.")).c_str());
        case SIGILL:ASSERT(0,std::string(_("Illegal instruction.")).c_str());
        default:ASSERT(0,std::string(_("Unknown error.")).c_str());
    }
#endif
}
/* End Function:RVM_CFG_App::OnSystemFatalException ***************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
