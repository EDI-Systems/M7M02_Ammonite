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
#include "Option_Notebook/option_notebook.hpp"
#include "Output_Notebook/output_notebook.hpp"

#include "Config_Tree/config_tree.hpp"

#include "Menu_Bar/menu_bar.hpp"
#include "Tool_Bar/tool_bar.hpp"

#include "About_Dialog/about_dialog.hpp"

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
/* Global Variables **********************************************************/
class Main* RVM_CFG_App::Main=nullptr;

/* Executable path */
std::string Main::Exe_Folder="";

/* Application states */
ptr_t Main::UI_State=UI_NONE;
ptr_t Main::Save_State=SAVE_NONE;

/* Lower-level classes */
class Proj_Info* Main::Proj=nullptr;
class Menu_Bar* Main::Menu_Bar=nullptr;
class Tool_Bar* Main::Tool_Bar=nullptr;
class wxPanel* Main::App_Panel=nullptr;
class wxBoxSizer* Main::App_Sizer=nullptr;
class wxBoxSizer* Main::Config_Sizer=nullptr;
class Config_Tree* Main::Config=nullptr;
class wxBoxSizer* Main::Option_Sizer=nullptr;
class Option_Notebook* Main::Option=nullptr;
class Output_Notebook* Main::Output=nullptr;
class Status_Bar* Main::Status_Bar=nullptr;

class About_Dialog* Main::About_Dialog=nullptr;
/* End Global Variables ******************************************************/

/* Function:Main::Main ********************************************************
Description : Application class constructor. The frame size is 640x480.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Main::Main(void):
wxFrame(nullptr,wxID_ANY,SOFTWARE_NAME,wxDefaultPosition,I2P(wxSize(1024,768)),
        wxSYSTEM_MENU|wxCLOSE_BOX|wxCAPTION|wxCLIP_CHILDREN)
{
    this->SetSize(I2P(wxSize(1024,768)));
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
        Main::Option=new class Option_Notebook(Main::App_Panel);
        Main::Option_Sizer->Add(Main::Option, 4, wxEXPAND);
        Main::Output=new class Output_Notebook(Main::App_Panel);
        Main::Option_Sizer->Add(Main::Output, 1, wxEXPAND);

        Main::Config_Sizer->Add(Main::Option_Sizer, 4, wxEXPAND);
        Main::App_Sizer->Add(Main::Config_Sizer, 1, wxEXPAND);
        /* Spacers */
        Main::App_Sizer->AddSpacer(10);
        Main::App_Sizer->Layout();
        /* Status bar on the bottom */
        Main::Status_Bar=new class Status_Bar(this);
        this->SetStatusBar(Main::Status_Bar);

        /* Create the heavy-handed dialogs so that they load super-fast */
//        Main::Proj_Dialog=new class Proj_Dialog(this);
//        Main::Proj_Dialog->Hide();
//        Main::Proc_Dialog=new class Proc_Dialog(this);
//        Main::Proc_Dialog->Hide();
//        Main::Virt_Dialog=new class Virt_Dialog(this);
//        Main::Virt_Dialog->Hide();
        Main::About_Dialog=new class About_Dialog(this);
        Main::About_Dialog->Hide();

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

/* Function:Main::CSV_Save *************************************************
Description : Save the type of vector<string>, use ',' to connect
Input       : class wxXmlNode* Parent - The parent node.
              const std::string& Name - The name of the label.
              const std::vector<string>& - The CSV content
Output      : None.
Return      : class wxXmlNode* - The node added.
******************************************************************************/
class wxXmlNode* Main::CSV_Save(class wxXmlNode* Parent, const std::string& Name,
		const std::vector<std::string>& Content)
{
    class wxXmlNode* Temp_N;
    class wxXmlNode* Temp_C;

    /* Add to content - null node helps to preserve the pair even if it is empty */
    Temp_N=new wxXmlNode(nullptr,wxXML_ELEMENT_NODE,Name);
    /* Use ',' to connect every element of content*/
    std::string Temp_Content="";
    if(!Content.empty()){
    	Temp_Content=Content[0];
        for(cnt_t Cnt=1;Cnt<Content.size();Cnt++){
        	Temp_Content.append(","+Content[Cnt]);
        }
    }

    Temp_C=new wxXmlNode(wxXML_TEXT_NODE,wxEmptyString,Temp_Content);
    /* Add in this way so that the sequence is preserved */
    Temp_N->AddChild(Temp_C);
    Parent->AddChild(Temp_N);

    return Temp_N;
}
/* End Function:Main::CSV_Save ********************************************/

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

/* Function:Main::Proj_New ****************************************************
Description : Create a new file, at the location specified by the full path.
Input       : const std::string& Path - The path, including the suffix.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Proj_New(const std::string& Path)
{
    wxLogDebug("Main::Proj_New");
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

    /* Update save status */
    Main::State_Set(STATE_SAVE,SAVE_NONE);
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
    Main::Config->State_Set(Type);
    Main::Option->State_Set(Type);
    Main::Output->State_Set(Type);
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
