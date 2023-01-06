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

/* Includes ******************************************************************/
extern "C"
{
#include "stdio.h"
#include "ctype.h"
#include "time.h"
}

#include "set"
#include "atomic"
#include "thread"
#include "memory"
#include "string"
#include "algorithm"

#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/filepicker.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/metafile.h"
#include "wx/mimetype.h"

extern "C"
{
#ifdef WIN32
#include "imagehlp.h"
#else
#include "unistd.h"
#include "backtrace.h"
#endif
}

#include "Images/logo.xpm"

#define __HDR_DEFS__

#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"

#include "Status_Bar/status_bar.hpp"

#include "Menu_Bar/menu_bar.hpp"

#include "About_Dialog/about_dialog.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Global Variables **********************************************************/
class Main* RVM_CFG_App::Main=nullptr;
/* Current locale */
ptr_t RVM_CFG_App::Locale=0;
/* Current state */
ptr_t Main::Action_State=ACTION_NONE;
/* Exit flag */
ptr_t Main::Exit_Flag=0;

/* Executable path */
std::string Main::Exe_Folder="";

/* Auto-download timer */
class wxTimer* Main::Auto_Timer=nullptr;
/* Remote detection timer */
class wxTimer* Main::Remote_Timer=nullptr;

/* Lower-level classes */
class Menu_Bar* Main::Menu_Bar=nullptr;
class wxPanel* Main::Panel=nullptr;
class wxBoxSizer* Main::Panel_Sizer=nullptr;
class wxBoxSizer* Main::Border_Sizer=nullptr;
class wxBoxSizer* Main::Main_Sizer=nullptr;

class wxStaticBoxSizer* Main::File_Sizer=nullptr;
class wxStaticText* Main::File_Label=nullptr;
class wxFilePickerCtrl* Main::File_Picker=nullptr;

class wxFlexGridSizer* Main::Grid_Sizer=nullptr;

class wxStaticBoxSizer* Main::Info_Sizer=nullptr;
class wxTextCtrl* Main::Info_Text=nullptr;

class wxStaticBoxSizer* Main::Option_Sizer=nullptr;
class wxFlexGridSizer* Main::Option_Grid_Sizer=nullptr;
class wxStaticText* Main::Verify_After_Download_Label=nullptr;
class wxChoice* Main::Verify_After_Download_Choice=nullptr;
class wxStaticText* Main::Calib_When_Download_Label=nullptr;
class wxChoice* Main::Calib_When_Download_Choice=nullptr;
class wxStaticText* Main::Late_Remote_Pair_Label=nullptr;
class wxChoice* Main::Late_Remote_Pair_Choice=nullptr;
class wxStaticText* Main::Late_Serial_Config_Label=nullptr;
class wxChoice* Main::Late_Serial_Config_Choice=nullptr;
class wxStaticText* Main::Late_Clock_Calib_Label=nullptr;
class wxChoice* Main::Late_Clock_Calib_Choice=nullptr;
class wxStaticText* Main::Allow_Debug_Label=nullptr;
class wxChoice* Main::Allow_Debug_Choice=nullptr;

class wxStaticBoxSizer* Main::Status_Sizer=nullptr;
class wxTextCtrl* Main::Status_Text=nullptr;

class wxStaticBoxSizer* Main::Action_Sizer=nullptr;
class wxFlexGridSizer* Main::Action_Grid_Sizer=nullptr;
class wxCheckBox* Main::Auto_Download=nullptr;
class wxCheckBox* Main::Beep_When_Complete=nullptr;
class wxButton* Main::Detect_Hardware=nullptr;
class wxButton* Main::Calib_Clock=nullptr;
class wxButton* Main::Toggle_Remote_Detect=nullptr;
class wxButton* Main::Toggle_Download=nullptr;

class Status_Bar* Main::Status_Bar=nullptr;

class About_Dialog* Main::About_Dialog=nullptr;

/* Pre-loaded daemons */
class Download_Daemon* Main::Download_Daemon=nullptr;
/* End Global Variables ******************************************************/

/* Begin Function:Main::Main **************************************************
Description : Application class constructor. The frame size is 640x480.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Main::Main(void):
wxFrame(nullptr,wxID_ANY,SOFTWARE_NAME,wxDefaultPosition,I2P(wxSize(640,480)),
        wxSYSTEM_MENU|wxCLOSE_BOX|wxCAPTION|wxCLIP_CHILDREN)
{
    this->SetSize(I2P(wxSize(640,480)));
    this->Center();

    try
    {
        wxLogDebug(wxT("Main::Main"));

        /* Set icon */
        this->SetIcon(wxIcon(C4G02_Logo));

        /* This is a singleton. If this is not nullptr, we must have initialized it */
        ASSERT(RVM_CFG_App::Main==nullptr,"This application is a singleton.");

        /* Event handlers */
        this->Bind(wxEVT_CLOSE_WINDOW,&Main::On_Close_Window,this,this->GetId());

        /* Auto-download timer */
        Main::Auto_Timer=new wxTimer(this);
        this->Bind(wxEVT_TIMER,&Main::On_Auto_Timer,this,Main::Auto_Timer->GetId());
        /* Remote detection timer */
        Main::Remote_Timer=new wxTimer(this);
        this->Bind(wxEVT_TIMER,&Main::On_Remote_Timer,this,Main::Remote_Timer->GetId());

        /* Menubar on the top */
        Main::Menu_Bar=new class Menu_Bar();
        this->SetMenuBar(Main::Menu_Bar);
        /* Application sizer in the middle */
        Main::Panel=new class wxPanel(this);
        Main::Panel_Sizer=new class wxBoxSizer(wxVERTICAL);
        Main::Border_Sizer=new class wxBoxSizer(wxVERTICAL);
        Main::Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        Main::File_Sizer=new class wxStaticBoxSizer(wxHORIZONTAL,Main::Panel,sT("Application Image"));
        Main::File_Label=new class wxStaticText(Main::Panel,wxID_ANY,sT("File Path"));
        Main::File_Sizer->Add(Main::File_Label,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
        Main::File_Sizer->AddStretchSpacer(1);
        Main::File_Picker=new class wxFilePickerCtrl(Main::Panel,wxID_ANY,wxEmptyString,sT("Open File"),
                                                     sT("Application Image")+wxT(" (*.dma)|*.dma"),
                                                     wxDefaultPosition,wxDefaultSize,wxFLP_DEFAULT_STYLE|wxFLP_SMALL);
        Main::File_Picker->GetTextCtrl()->SetEditable(false);
        this->Bind(wxEVT_FILEPICKER_CHANGED,&Main::On_File_Picker,this,Main::File_Picker->GetId());
        Main::File_Sizer->Add(Main::File_Picker,20,wxEXPAND);

        Main::Grid_Sizer=new class wxFlexGridSizer(2,I2P(5),I2P(5));
        Main::Grid_Sizer->AddGrowableCol(0,9);
        Main::Grid_Sizer->AddGrowableCol(1,1);

        Main::Info_Sizer=new class wxStaticBoxSizer(wxVERTICAL,Main::Panel,sT("Image Information"));
        Main::Info_Text=new class wxTextCtrl(Main::Panel,wxID_ANY,wxT(""),wxDefaultPosition,I2P(wxSize(-1,200)),wxTE_MULTILINE);
        Main::Info_Sizer->Add(Main::Info_Text,1,wxEXPAND);

        Main::Grid_Sizer->Add(Main::Info_Sizer,1,wxEXPAND);

        Main::Option_Sizer=new class wxStaticBoxSizer(wxVERTICAL,Main::Panel,sT("Download Options"));
        Main::Option_Grid_Sizer=new class wxFlexGridSizer(2,I2P(10),I2P(5));
        Main::Option_Grid_Sizer->AddGrowableCol(0,9);
        Main::Option_Grid_Sizer->AddGrowableCol(1,1);

        Main::Verify_After_Download_Label=new class wxStaticText(Main::Panel,wxID_ANY,sT("Verify After Downloading"));
        Main::Option_Grid_Sizer->Add(Main::Verify_After_Download_Label,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
        Main::Verify_After_Download_Choice=new class wxChoice(Main::Panel,wxID_ANY,wxDefaultPosition,I2P(wxSize(75,-1)));
        Main::Verify_After_Download_Choice->Append(sT("Yes"));
        Main::Verify_After_Download_Choice->Append(sT("No"));
        Main::Verify_After_Download_Choice->SetSelection(0);
        Main::Option_Grid_Sizer->Add(Main::Verify_After_Download_Choice,1,wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);

        Main::Calib_When_Download_Label=new class wxStaticText(Main::Panel,wxID_ANY,sT("Calibrate Clock When Downloading"));
        Main::Option_Grid_Sizer->Add(Main::Calib_When_Download_Label,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
        Main::Calib_When_Download_Choice=new class wxChoice(Main::Panel,wxID_ANY,wxDefaultPosition,I2P(wxSize(75,-1)));
        Main::Calib_When_Download_Choice->Append(sT("Yes"));
        Main::Calib_When_Download_Choice->Append(sT("No"));
        Main::Calib_When_Download_Choice->SetSelection(0);
        Main::Option_Grid_Sizer->Add(Main::Calib_When_Download_Choice,1,wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);

        Main::Late_Remote_Pair_Label=new class wxStaticText(Main::Panel,wxID_ANY,sT("Late Remote Pairing"));
        Main::Option_Grid_Sizer->Add(Main::Late_Remote_Pair_Label,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
        Main::Late_Remote_Pair_Choice=new class wxChoice(Main::Panel,wxID_ANY,wxDefaultPosition,I2P(wxSize(75,-1)));
        Main::Late_Remote_Pair_Choice->Append(sT("Allow"));
        Main::Late_Remote_Pair_Choice->Append(sT("Disallow"));
        Main::Late_Remote_Pair_Choice->SetSelection(0);
        Main::Option_Grid_Sizer->Add(Main::Late_Remote_Pair_Choice,1,wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);

        Main::Late_Serial_Config_Label=new class wxStaticText(Main::Panel,wxID_ANY,sT("Late Serial Configuration"));
        Main::Option_Grid_Sizer->Add(Main::Late_Serial_Config_Label,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
        Main::Late_Serial_Config_Choice=new class wxChoice(Main::Panel,wxID_ANY,wxDefaultPosition,I2P(wxSize(75,-1)));
        Main::Late_Serial_Config_Choice->Append(sT("Allow"));
        Main::Late_Serial_Config_Choice->Append(sT("Disallow"));
        Main::Late_Serial_Config_Choice->SetSelection(0);
        Main::Option_Grid_Sizer->Add(Main::Late_Serial_Config_Choice,1,wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);

        Main::Late_Clock_Calib_Label=new class wxStaticText(Main::Panel,wxID_ANY,sT("Late Clock Calibration"));
        Main::Option_Grid_Sizer->Add(Main::Late_Clock_Calib_Label,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
        Main::Late_Clock_Calib_Choice=new class wxChoice(Main::Panel,wxID_ANY,wxDefaultPosition,I2P(wxSize(75,-1)));
        Main::Late_Clock_Calib_Choice->Append(sT("Allow"));
        Main::Late_Clock_Calib_Choice->Append(sT("Disallow"));
        Main::Late_Clock_Calib_Choice->SetSelection(0);
        Main::Option_Grid_Sizer->Add(Main::Late_Clock_Calib_Choice,1,wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);

        Main::Allow_Debug_Label=new class wxStaticText(Main::Panel,wxID_ANY,sT("Allow Future Debugging"));
        Main::Option_Grid_Sizer->Add(Main::Allow_Debug_Label,0,wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
        Main::Allow_Debug_Choice=new class wxChoice(Main::Panel,wxID_ANY,wxDefaultPosition,I2P(wxSize(75,-1)));
        Main::Allow_Debug_Choice->Append(sT("Allow"));
        Main::Allow_Debug_Choice->Append(sT("Disallow"));
        Main::Allow_Debug_Choice->SetSelection(0);
        Main::Option_Grid_Sizer->Add(Main::Allow_Debug_Choice,1,wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL);

        Main::Option_Sizer->AddStretchSpacer(1);
        Main::Option_Sizer->Add(Main::Option_Grid_Sizer,100,wxEXPAND);
        Main::Option_Sizer->AddStretchSpacer(1);
        Main::Grid_Sizer->Add(Main::Option_Sizer,1,wxEXPAND);

        Main::Status_Sizer=new class wxStaticBoxSizer(wxHORIZONTAL,Main::Panel,sT("Status Log"));
        Main::Status_Text=new class wxTextCtrl(Main::Panel,wxID_ANY,wxT(""),wxDefaultPosition,I2P(wxSize(-1,100)),wxTE_MULTILINE);
        Main::Status_Sizer->Add(Main::Status_Text,1,wxEXPAND);
        Main::Grid_Sizer->Add(Main::Status_Sizer,1,wxEXPAND);

        Main::Action_Sizer=new class wxStaticBoxSizer(wxVERTICAL,Main::Panel,sT("Download Actions"));
        Main::Action_Grid_Sizer=new class wxFlexGridSizer(2,I2P(10),I2P(5));
        Main::Action_Grid_Sizer->AddGrowableCol(0,2);
        Main::Action_Grid_Sizer->AddGrowableCol(1,1);
        Main::Beep_When_Complete=new class wxCheckBox(Main::Panel,wxID_ANY,sT("Beep When Complete"));
        Main::Beep_When_Complete->SetValue(true);
        Main::Action_Grid_Sizer->Add(Main::Beep_When_Complete,1,wxEXPAND);
        Main::Auto_Download=new class wxCheckBox(Main::Panel,wxID_ANY,sT("Auto Download"));
        Main::Auto_Download->SetValue(false);
        Main::Action_Grid_Sizer->Add(Main::Auto_Download,1,wxEXPAND);
        Main::Detect_Hardware=new class wxButton(Main::Panel,wxID_ANY,sT("Detect Hardware"));
        this->Bind(wxEVT_BUTTON,&Main::On_Detect_Hardware,this,Main::Detect_Hardware->GetId());
        Main::Action_Grid_Sizer->Add(Main::Detect_Hardware,1,wxEXPAND);
        Main::Calib_Clock=new class wxButton(Main::Panel,wxID_ANY,sT("Calibrate Clock"));
        this->Bind(wxEVT_BUTTON,&Main::On_Calib_Clock,this,Main::Calib_Clock->GetId());
        Main::Action_Grid_Sizer->Add(Main::Calib_Clock,1,wxEXPAND);
        Main::Toggle_Remote_Detect=new class wxButton(Main::Panel,wxID_ANY,sT("Start Remote Detection"));
        this->Bind(wxEVT_BUTTON,&Main::On_Toggle_Remote_Detect,this,Main::Toggle_Remote_Detect->GetId());
        Main::Action_Grid_Sizer->Add(Main::Toggle_Remote_Detect,1,wxEXPAND);
        Main::Toggle_Download=new class wxButton(Main::Panel,wxID_ANY,sT("Start Download"));
        this->Bind(wxEVT_BUTTON,&Main::On_Toggle_Download,this,Main::Toggle_Download->GetId());
        Main::Action_Grid_Sizer->Add(Main::Toggle_Download,1,wxEXPAND);

        Main::Action_Sizer->AddStretchSpacer(1);
        Main::Action_Sizer->Add(Main::Action_Grid_Sizer,100,wxEXPAND);
        Main::Action_Sizer->AddStretchSpacer(1);
        Main::Grid_Sizer->Add(Main::Action_Sizer,1,wxEXPAND);

        Main::Main_Sizer->Add(Main::File_Sizer,10,wxEXPAND);
        Main::Main_Sizer->AddStretchSpacer(1);
        Main::Main_Sizer->Add(Main::Grid_Sizer,90,wxEXPAND);

        Main::Border_Sizer->Add(Main::Main_Sizer,1,wxEXPAND|wxALL,I2P(5));
        Main::Panel->SetSizer(Main::Border_Sizer);
        Main::Panel_Sizer->Add(Main::Panel,1,wxEXPAND);
        Main::SetSizer(Main::Panel_Sizer);
        Main::Panel_Sizer->Layout();

        /* Status bar on the bottom */
        Main::Status_Bar=new class Status_Bar(this);
        this->SetStatusBar(Main::Status_Bar);

        Main::About_Dialog=new class About_Dialog(this);
        Main::About_Dialog->Hide();

        /* Boot up the daemon thread */
        Main::Download_Daemon=new class Download_Daemon();

        /* Enable all selection as default */
        Main::Verify_After_Download_Choice->SetSelection(0);
        Main::Calib_When_Download_Choice->SetSelection(0);
        Main::Late_Remote_Pair_Choice->SetSelection(0);
        Main::Late_Serial_Config_Choice->SetSelection(0);
        Main::Late_Clock_Calib_Choice->SetSelection(0);
        Main::Allow_Debug_Choice->SetSelection(0);

        Main::State_Set(ACTION_NONE);

        this->Refresh();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Main::Main ***************************************************/

/* Begin Function:Main::~Main *************************************************
Description : Main class destructor.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Main::~Main(void)
{
    Main::Auto_Timer->Stop();
    Main::Remote_Timer->Stop();
    this->Unbind(wxEVT_TIMER,&Main::On_Auto_Timer,this);
    this->Unbind(wxEVT_TIMER,&Main::On_Remote_Timer,this);
    this->Unbind(wxEVT_FILEPICKER_CHANGED,&Main::On_File_Picker,this);
    this->Unbind(wxEVT_BUTTON,&Main::On_Calib_Clock,this);
    this->Unbind(wxEVT_BUTTON,&Main::On_Detect_Hardware,this);
    this->Unbind(wxEVT_BUTTON,&Main::On_Toggle_Remote_Detect,this);
    this->Unbind(wxEVT_BUTTON,&Main::On_Toggle_Download,this);
    this->Unbind(wxEVT_CLOSE_WINDOW,&Main::On_Close_Window,this);

    /* The threads should end soon - exit flag already set on window close */
    Main::Download_Daemon->Thread->join();
    delete Main::Download_Daemon;
    delete Main::Remote_Timer;
    delete Main::Auto_Timer;
}
/* End Function:Main::~Main **************************************************/

/* Begin Function:Main::Model_Get *********************************************
Description : Get the target model in string format.
Input       : ret_t Prod_ID - The product ID.
Output      : None.
Return      : std::string - The model string returned.
******************************************************************************/
std::string Main::Model_Get(ret_t Prod_ID)
{
    char Buf[16];

    switch(Prod_ID)
    {
        case PRODID_C2S0404:return "C2S0404";
        case PRODID_C2S0405:return "C2S0405";
        case PRODID_C4G0202:return "C4G0202";
        case PRODID_C4G0203:return "C4G0203";
        case PRODID_C4G0204:return "C4G0204";
        case PRODID_C4G0205:return "C4G0205";
        case PRODID_C4G0206:return "C4G0206";
        case PRODID_C4G0212:return "C4G0212";
        case PRODID_C4G0218:return "C4G0218";
        case PRODID_C4G0101:return std::string("C4G0101 ")+std::string(sT("(Incompatible)"));
        case PRODID_C4G0102:return std::string("C4G0102 ")+std::string(sT("(Incompatible)"));
        case PRODID_C4G0103:return std::string("C4G0103 ")+std::string(sT("(Incompatible)"));
        default:
        {
            sprintf(Buf,"0x%04llX", Prod_ID);
            return std::string(sT("Unknown module"))+" ("+Buf+") "+std::string(sT("(Incompatible)"));
        }
    }
}
/* End Function:Main::Model_Get **********************************************/

/* Begin Function:Main::Title_Set *********************************************
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

/* Begin Function:Main::Time_Get **********************************************
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
                         Time_Struct->tm_year+1900,Time_Struct->tm_mon+1,Time_Struct->tm_mday);
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
                         Time_Struct->tm_year+1900,Time_Struct->tm_mon+1,Time_Struct->tm_mday,
                         Time_Struct->tm_hour, Time_Struct->tm_min, Time_Struct->tm_sec);
            break;
        }
        default:ASSERT(0,"Time extraction mode is incorrect.");break;
    }

    return Date;
}
/* End Function:Main::Time_Get ***********************************************/

/* Begin Function:Main::Beep **************************************************
Description : Beep the beeper.
Input       : ptr_t Stack - The maximum stack usage.
Output      : None.
Return      : ret_t - If there were errors, 0; else -1.
******************************************************************************/
void Main::Beep(void)
{
    if(Main::Beep_When_Complete->GetValue()==true)
    {
#ifdef WIN32
        PlaySound((LPCTSTR)SND_ALIAS_SYSTEMSTART,NULL,SND_ALIAS_ID|SND_ASYNC);
#else
        /* This may or may not work depending on the linux distribution */
        wxExecute("bash -c \"echo -ne \'\\007\'\"");
#endif
    }
}
/* End Function:Main::Beep ***************************************************/

/* Begin Function:Main::Msgbox_Show *******************************************
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

/* Begin Function:Main::Output_Update *****************************************
Description : Update the output panel.
Input       : std::vector<std::string>& Reply - The replies to show.
              ptr_t Panel - The panel to show onto.
              ptr_t Append - Whether to append to it.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Output_Update(std::vector<std::string>& Reply,ptr_t Panel,ptr_t Append)
{
    class wxTextCtrl* Text;

    switch(Panel)
    {
        case OUTPUT_INFO:Text=Main::Info_Text;break;
        case OUTPUT_STATUS:Text=Main::Status_Text;break;
        default:ASSERT(0,"Panel is invalid.");
    }

    /* Post text to the panel */
    if(Append==0)
        Text->Clear();
    for(std::string& Temp:Reply)
    {
        if(Temp.back()!='\n')
            Text->AppendText(Temp+"\n");
        else
            Text->AppendText(Temp);
    }
}
/* End Function:Main::Output_Update ******************************************/

/* Begin Function:Main::Output_Clear ******************************************
Description : Clear the output panel.
Input       : ptr_t Panel - The panel to clear.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Output_Clear(ptr_t Panel)
{
    switch(Panel)
    {
        case OUTPUT_INFO:Main::Info_Text->Clear();break;
        case OUTPUT_STATUS:Main::Status_Text->Clear();break;
        default:ASSERT(0,"Panel is invalid.");
    }
}
/* End Function:Main::Output_Clear *******************************************/

/* Begin Function:Main::Gauge_Update *****************************************
Description : Update the gauge bar.
Input       : ptr_t Pos - The position to set the gauge to.
Output      : None.
Return      : None.
******************************************************************************/
void Main::Gauge_Update(ptr_t Pos)
{
    Main::Status_Bar->Status_Gauge->SetValue(Pos);
}
/* End Function:Main::Gauge_Update *******************************************/

/* Begin Function:Main::Manual_Open *******************************************
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
    Path=std::string("C4G02_")+
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

/* Begin Function:Main::State_Set *********************************************
Description : Set the current action state, and decide what controls are usable.
Input       : ptr_t State - The actual state.
Output      : None.
Return      : None.
******************************************************************************/
void Main::State_Set(ptr_t State)
{
    Main::Action_State=State;

    /* Option states */
    if((State==ACTION_NONE)&&(Main::File_Picker->GetPath()!=""))
    {
        if(Main::Download_Daemon->Target.Has_Remote==OPTION_YES)
            Main::Late_Remote_Pair_Choice->Enable();
        else
            Main::Late_Remote_Pair_Choice->Disable();

        if((Main::Download_Daemon->Target.Has_Serial==OPTION_YES)&&
           (Main::Download_Daemon->Target.Has_Flexcom==OPTION_YES))
            Main::Late_Serial_Config_Choice->Enable();
        else
            Main::Late_Serial_Config_Choice->Disable();

        if(Main::Download_Daemon->Target.Has_Clock==OPTION_YES)
        {
            Main::Late_Clock_Calib_Choice->Enable();
            if(Main::Download_Daemon->Target.Has_Debug==OPTION_YES)
                Main::Calib_When_Download_Choice->Enable();
            else
                Main::Calib_When_Download_Choice->Disable();
        }
        else
        {
            Main::Late_Clock_Calib_Choice->Disable();
            Main::Calib_When_Download_Choice->Disable();
        }

        if(Main::Download_Daemon->Target.Has_Debug==OPTION_YES)
        {
            Main::Verify_After_Download_Choice->Enable();
            Main::Allow_Debug_Choice->Enable();
        }
        else
        {
            Main::Verify_After_Download_Choice->Disable();
            Main::Allow_Debug_Choice->Disable();
        }
    }
    else
    {
        Main::Verify_After_Download_Choice->Disable();
        Main::Calib_When_Download_Choice->Disable();
        Main::Late_Remote_Pair_Choice->Disable();
        Main::Late_Serial_Config_Choice->Disable();
        Main::Late_Clock_Calib_Choice->Disable();
        Main::Allow_Debug_Choice->Disable();
    }

    /* Button states */
    switch(State)
    {
        case ACTION_NONE:
        {
            Main::File_Picker->Enable();
            Main::Auto_Download->Enable();
            Main::Detect_Hardware->Enable();
            Main::Calib_Clock->Enable();
            Main::Toggle_Remote_Detect->SetLabel(sT("Start Remote Detection"));
            Main::Toggle_Remote_Detect->Enable();
            /* The firmware must be loaded when we're downloading */
            Main::Toggle_Download->SetLabel(sT("Start Download"));
            if(Main::File_Picker->GetPath()!="")
                Main::Toggle_Download->Enable();
            else
                Main::Toggle_Download->Disable();
            break;
        }
        case ACTION_DOWNLOAD:
        {
            Main::File_Picker->Disable();
            Main::Auto_Download->Disable();
            Main::Detect_Hardware->Disable();
            Main::Calib_Clock->Disable();
            Main::Toggle_Remote_Detect->Disable();
            if(Main::Auto_Download->GetValue()==true)
            {
                Main::Toggle_Download->SetLabel(sT("Stop Download"));
                Main::Toggle_Download->Enable();
            }
            else
                Main::Toggle_Download->Disable();
            break;
        }
        case ACTION_REMOTE:
        {
            Main::File_Picker->Disable();
            Main::Auto_Download->Enable();
            Main::Detect_Hardware->Disable();
            Main::Calib_Clock->Disable();
            Main::Toggle_Remote_Detect->SetLabel(sT("Stop Remote Detection"));
            Main::Toggle_Remote_Detect->Enable();
            Main::Toggle_Download->Disable();
            break;
        }
        case ACTION_CLOCK:
        case ACTION_DETECT:
        {
            Main::File_Picker->Disable();
            Main::Auto_Download->Disable();
            Main::Detect_Hardware->Disable();
            Main::Calib_Clock->Disable();
            Main::Toggle_Remote_Detect->Disable();
            Main::Toggle_Download->Disable();
            break;
        }
    }
}
/* End Function:Main::State_Set **********************************************/

/* Begin Function:Main::On_Auto_Timer *****************************************
Description : wxEVT_TIMER handler.
Input       : class wxTimerEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Main::On_Auto_Timer(class wxTimerEvent& Event)
{
    wxLogDebug("Main::On_Auto_Timer");

    /* Activate only when the timer is there */
    if(Main::Auto_Download->GetValue()==true)
    {
        Main::Output_Clear(OUTPUT_STATUS);
        Main::Download_Daemon->Connect_Set();
        Main::Download_Daemon->Download();
        Main::State_Set(ACTION_DOWNLOAD);
    }
}
/* End Function:Main::On_Auto_Timer ******************************************/

/* Begin Function:Main::On_Remote_Timer ***************************************
Description : wxEVT_TIMER handler.
Input       : class wxTimerEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Main::On_Remote_Timer(class wxTimerEvent& Event)
{
    wxLogDebug("Main::On_Remote_Timer");

    if((Main::Action_State==ACTION_REMOTE)&&(Main::Download_Daemon->Active==0))
        Main::Download_Daemon->Remote();
}
/* End Function:Main::On_Remote_Timer ****************************************/

/* Begin Function:Main::On_File_Picker ****************************************
Description : wxEVT_FILEPICKER_CHANGED handler.
Input       : class wxFileDirPickerEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Main::On_File_Picker(class wxFileDirPickerEvent& Event)
{
    wxLogDebug("Main::On_File_Picker");

    Main::Output_Clear(OUTPUT_INFO);

    /* Load application on choice */
    if(Main::Download_Daemon->App_Load(std::string(Main::File_Picker->GetPath()))<0)
    {
        Main::File_Picker->SetPath("");
        Main::Msgbox_Show(RVM_CFG_App::Main,MSGBOX_ERROR, sT("Open File"),
                          sT("The application image file might be corrupted."));
        return;
    }

    /* Disable unavailable options if the target does not support them */
    if(Main::Download_Daemon->Target.Has_Remote==OPTION_NO)
        Main::Late_Remote_Pair_Choice->SetSelection(1);

    if((Main::Download_Daemon->Target.Has_Serial==OPTION_NO)||
       (Main::Download_Daemon->Target.Has_Flexcom==OPTION_NO))
        Main::Late_Serial_Config_Choice->SetSelection(1);

    if(Main::Download_Daemon->Target.Has_Clock==OPTION_NO)
    {
        Main::Late_Clock_Calib_Choice->SetSelection(1);
        Main::Calib_When_Download_Choice->SetSelection(1);
    }
    else
    {
        if(Main::Download_Daemon->Target.Has_Debug==OPTION_NO)
            Main::Calib_When_Download_Choice->SetSelection(1);
    }

    if(Main::Download_Daemon->Target.Has_Debug==OPTION_NO)
    {
        Main::Verify_After_Download_Choice->SetSelection(1);
        Main::Allow_Debug_Choice->SetSelection(1);
    }

    /* We have loaded the programming file */
    Main::State_Set(ACTION_NONE);
}
/* End Function:Main::On_File_Picker *****************************************/

/* Begin Function:Main::On_Calib_Clock ****************************************
Description : wxEVT_BUTTON handler.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Main::On_Calib_Clock(class wxCommandEvent& Event)
{
    wxLogDebug("Main::On_Calib_Clock");

    Main::Output_Clear(OUTPUT_STATUS);
    Main::Download_Daemon->Connect_Set();
    Main::Download_Daemon->Clock();
    Main::State_Set(ACTION_CLOCK);
}
/* End Function:Main::On_Calib_Clock *****************************************/

/* Begin Function:Main::On_Detect_Hardware ************************************
Description : wxEVT_BUTTON handler.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Main::On_Detect_Hardware(class wxCommandEvent& Event)
{
    wxLogDebug("Main::On_Detect_Hardware");

    Main::Output_Clear(OUTPUT_STATUS);
    Main::Download_Daemon->Connect_Set();
    Main::Download_Daemon->Detect();
    Main::State_Set(ACTION_DETECT);
}
/* End Function:Main::On_Detect_Hardware *************************************/

/* Begin Function:Main::On_Toggle_Remote_Detect *******************************
Description : wxEVT_BUTTON handler.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Main::On_Toggle_Remote_Detect(class wxCommandEvent& Event)
{
    wxLogDebug("Main::On_Toggle_Remote_Detect");

    if(Main::Action_State==ACTION_NONE)
    {
        Main::Output_Clear(OUTPUT_STATUS);
        Main::Download_Daemon->Connect_Set();
        Main::Download_Daemon->Test();
        Main::State_Set(ACTION_DETECT);
    }
    else
    {
        Main::Remote_Timer->Stop();
        Main::Download_Daemon->USBCDC_Close();
        Main::State_Set(ACTION_NONE);
    }
}
/* End Function:Main::On_Toggle_Remote_Detect ********************************/

/* Begin Function:Main::On_Toggle_Download ************************************
Description : wxEVT_BUTTON handler.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Main::On_Toggle_Download(class wxCommandEvent& Event)
{
    wxLogDebug("Main::On_Toggle_Download");

    /* See if we're quitting the auto download session */
    if((Main::Auto_Download->GetValue()==true)&&(Main::Action_State==ACTION_DOWNLOAD))
    {
        /* Clear the auto download flag */
        Main::Auto_Download->SetValue(false);
        /* Timer running, just quit this session */
        if(Main::Auto_Timer->IsRunning()==true)
        {
            Main::Auto_Timer->Stop();
            Main::State_Set(ACTION_NONE);
        }
        /* We must be downloading. We'll quit after this session ends */
        else
            Main::Toggle_Download->Disable();
    }
    else
    {
        Main::Output_Clear(OUTPUT_STATUS);
        Main::Download_Daemon->Connect_Set();
        Main::Download_Daemon->Download();
        Main::State_Set(ACTION_DOWNLOAD);
    }
}
/* End Function:Main::On_Toggle_Download *************************************/

/* Begin Function:Main::On_Close_Window ***************************************
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

    /* Mark exit */
    this->Exit_Flag=1;

    /* We don't exit directly here - wait for wxWidgets to clean everything up */
    Event.Skip();
}
/* End Function:Main::On_Close_Window ****************************************/

/* Begin Function:RVM_CFG_App::OnInit ******************************************
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

/* Begin Function:RVM_CFG_App::Locale_Set **************************************
Description : Set locale for the whole execution environment.
Input       : None.
Output      : None.
Return      : int - The language ID returned.
******************************************************************************/
int RVM_CFG_App::Locale_Set(void)
{
    /* See if we need to override language */
#ifndef LANGUAGE_OVERRIDE
    this->Locale=wxLocale::GetSystemLanguage();
#else
    this->Locale=LANGUAGE_OVERRIDE;
#endif

    return this->Locale;
}
/* End Function:RVM_CFG_App::Locale_Set ***************************************/

/* Begin Function:RVM_CFG_App::Translate ***************************************
Description : Perform translation.
Input       : const char* Input - The input string.
Output      : None.
Return      : class wxString - The translated string returned.
******************************************************************************/
class wxString RVM_CFG_App::Translate(const char* Input)
{
    return RVM_CFG_App::Translate(wxString(Input));
}
/* End Function:RVM_CFG_App::Translate ****************************************/

/* Begin Function:RVM_CFG_App::Translate ***************************************
Description : Perform translation.
Input       : const std::string& Input - The input string.
Output      : None.
Return      : class wxString - The translated string returned.
******************************************************************************/
class wxString RVM_CFG_App::Translate(const std::string& Input)
{
    return RVM_CFG_App::Translate(wxString(Input));
}
/* End Function:RVM_CFG_App::Translate ****************************************/

/* Begin Function:RVM_CFG_App::Translate ***************************************
Description : Perform translation.
Input       : const class wxString& Input - The input string.
Output      : None.
Return      : class wxString - The translated string returned.
******************************************************************************/
class wxString RVM_CFG_App::Translate(const class wxString& Input)
{
    if((RVM_CFG_App::Locale==wxLANGUAGE_CHINESE)||
       (RVM_CFG_App::Locale==wxLANGUAGE_CHINESE_SIMPLIFIED)||
       (RVM_CFG_App::Locale==wxLANGUAGE_CHINESE_TRADITIONAL)||
       (RVM_CFG_App::Locale==wxLANGUAGE_CHINESE_HONGKONG)||
       (RVM_CFG_App::Locale==wxLANGUAGE_CHINESE_MACAU)||
       (RVM_CFG_App::Locale==wxLANGUAGE_CHINESE_SINGAPORE)||
       (RVM_CFG_App::Locale==wxLANGUAGE_CHINESE_TAIWAN))
    {

        if(Input==wxT("(C) Copyright WindIOT Co.,Ltd. All rights reserved."))
            return wxT("（C）微风科技有限公司。所有权利保留。");
        else if(Input==wxT("(Incompatible)"))
            return wxT("（不兼容）");
        else if(Input==wxT("."))
            return wxT("。");
        else if(Input==wxT("About Program"))
            return wxT("关于本程序");
        else if(Input==wxT("About Program(&A)"))
            return wxT("关于本程序(&A)");
        else if(Input==wxT("Address"))
            return wxT("地址");
        else if(Input==wxT("Allow"))
            return wxT("允许");
        else if(Input==wxT("Allow Future Debugging"))
            return wxT("允许未来调试");
        else if(Input==wxT("Analog input number"))
            return wxT("模拟输入数量");
        else if(Input==wxT("Application Image"))
            return wxT("应用程序映像");
        else if(Input==wxT("Application image is valid."))
            return wxT("应用程序映像有效。");
        else if(Input==wxT("Attempting next download in 5s."))
            return wxT("5秒后尝试再次下载。");
        else if(Input==wxT("Auto Download"))
            return wxT("自动下载");
        else if(Input==wxT("Beep When Complete"))
            return wxT("完成后播放提示音");
        else if(Input==wxT("Bootloader version"))
            return wxT("自举程序版本");
        else if(Input==wxT("By using this software service, you agree to the EULA located at the root folder of the installation."))
            return wxT("使用本软件即表示您同意接受位于安装根目录的最终用户许可条款（EULA）。");
        else if(Input==wxT("Calibrate Clock"))
            return wxT("校准时钟");
        else if(Input==wxT("Calibrate Clock When Downloading"))
            return wxT("下载时校准时钟");
        else if(Input==wxT("Check for Updates(&U)"))
            return wxT("检查更新(&U)");
        else if(Input==wxT("Checking firmware consistency."))
            return wxT("正在检查固件完整性。");
        else if(Input==wxT("Checking hardware configuration."))
            return wxT("正在检查硬件配置。");
        else if(Input==wxT("Clock calibration failed."))
            return wxT("时钟校准失败。");
        else if(Input==wxT("Clock calibration successful."))
            return wxT("时钟校准成功。");
        else if(Input==wxT("Clock calibration timed out."))
            return wxT("时钟校准超时。");
        else if(Input==wxT("Clock input number"))
            return wxT("时钟输入数量");
        else if(Input==wxT("Confirm"))
            return wxT("确认");
        else if(Input==wxT("Connecting to module."))
            return wxT("正在连接到模块。");
        else if(Input==wxT("Data"))
            return wxT("数据");
        else if(Input==wxT("Detect Hardware"))
            return wxT("探测硬件型号");
        else if(Input==wxT("Detect timed out."))
            return wxT("探测超时。");
        else if(Input==wxT("Detection firmware load failed."))
            return wxT("探测固件下载失败。");
        else if(Input==wxT("Detection firmware load successful."))
            return wxT("探测固件下载成功。");
        else if(Input==wxT("Detection firmware load timed out."))
            return wxT("探测固件下载超时。");
        else if(Input==wxT("Disallow"))
            return wxT("不允许");
        else if(Input==wxT("Download Actions"))
            return wxT("下载动作");
        else if(Input==wxT("Download Options"))
            return wxT("下载选项");
        else if(Input==wxT("Download failed."))
            return wxT("下载失败。");
        else if(Input==wxT("Download successful."))
            return wxT("下载成功。");
        else if(Input==wxT("Download timed out."))
            return wxT("下载超时。");
        else if(Input==wxT("Download verification failed."))
            return wxT("下载校验失败。");
        else if(Input==wxT("Downloader Manual(&D)"))
            return wxT("下载器使用手册(&S)");
        else if(Input==wxT("Downloading detection firmware."))
            return wxT("正在下载探测固件。");
        else if(Input==wxT("Downloading program."))
            return wxT("正在下载程序。");
        else if(Input==wxT("Duty value number"))
            return wxT("占空比数量");
        else if(Input==wxT("Each key must be held for more than one second."))
            return wxT("每个按键都必须持续按下超过1秒。");
        else if(Input==wxT("Erasing flash."))
            return wxT("正在擦除闪存。");
        else if(Input==wxT("Error"))
            return wxT("错误");
        else if(Input==wxT("Execution abort."))
            return wxT("执行中止。");
        else if(Input==wxT("Fatal"))
            return wxT("致命");
        else if(Input==wxT("File Path"))
            return wxT("文件路径");
        else if(Input==wxT("Finishing download."))
            return wxT("正在完成下载。");
        else if(Input==wxT("Firmware version"))
            return wxT("固件版本");
        else if(Input==wxT("Floating point error."))
            return wxT("浮点错误。");
        else if(Input==wxT("Further downloads may render the device inoperable."))
            return wxT("进一步下载可能会导致设备无法运行。");
        else if(Input==wxT("Hardware Manual(&H)"))
            return wxT("硬件使用手册(&H)");
        else if(Input==wxT("Help(&H)"))
            return wxT("帮助(&H)");
        else if(Input==wxT("Illegal instruction."))
            return wxT("非法指令。");
        else if(Input==wxT("Image Information"))
            return wxT("映像信息");
        else if(Input==wxT("Late Clock Calibration"))
            return wxT("后时钟校准");
        else if(Input==wxT("Late Remote Pairing"))
            return wxT("后遥控对码");
        else if(Input==wxT("Late Serial Configuration"))
            return wxT("后串口配置");
        else if(Input==wxT("Loading application."))
            return wxT("正在加载应用程序。");
        else if(Input==wxT("Memory access error."))
            return wxT("内存访问错误。");
        else if(Input==wxT("Module detected"))
            return wxT("探测到模块");
        else if(Input==wxT("No"))
            return wxT("否");
        else if(Input==wxT("No project target module detected."))
            return wxT("没有探测到工程目标模块。");
        else if(Input==wxT("No valid module detected."))
            return wxT("没有探测到有效模块。");
        else if(Input==wxT("Open File"))
            return wxT("打开文件");
        else if(Input==wxT("Other names may be trademarks of their respective owners."))
            return wxT("其他名称可能是其各自所有者的商标。");
        else if(Input==wxT("Please press all keys on the remote control one by one."))
            return wxT("请依次按下遥控器上的所有按键。");
        else if(Input==wxT("Preparing programming operation."))
            return wxT("正在准备下载操作。");
        else if(Input==wxT("Preparing remote detection."))
            return wxT("正在准备遥控探测。");
        else if(Input==wxT("Product identifier"))
            return wxT("产品标识符");
        else if(Input==wxT("Programming flash."))
            return wxT("正在编程闪存。");
        else if(Input==wxT("Project target module detected on port"))
            return wxT("工程目标模块连接于端口");
        else if(Input==wxT("Remote address length"))
            return wxT("遥控地址长度");
        else if(Input==wxT("Remote data length"))
            return wxT("遥控数据长度");
        else if(Input==wxT("Remote input number"))
            return wxT("遥控输入数量");
        else if(Input==wxT("See EULA_XX.txt for details of terms."))
            return wxT("有关条款的详细信息，请参阅EULA_XX.txt。");
        else if(Input==wxT("Serial input number"))
            return wxT("串口输入数量");
        else if(Input==wxT("Serial output number"))
            return wxT("串口输出数量");
        else if(Input==wxT("SmarTock (TM) Smart Delay Module Standalone Downloader"))
            return wxT("SmarTock(TM) 智能延时模块独立下载器");
        else if(Input==wxT("Start Download"))
            return wxT("开始下载");
        else if(Input==wxT("Start Remote Detection"))
            return wxT("开始遥控探测");
        else if(Input==wxT("Starting remote detection."))
            return wxT("正在开始遥控探测。");
        else if(Input==wxT("Status Log"))
            return wxT("状态日志");
        else if(Input==wxT("Stop Download"))
            return wxT("停止下载");
        else if(Input==wxT("Stop Remote Detection"))
            return wxT("停止遥控探测");
        else if(Input==wxT("Target module"))
            return wxT("目标模块");
        else if(Input==wxT("The application image file might be corrupted."))
            return wxT("应用程序映像可能已经损坏。");
        else if(Input==wxT("This program and its components is protected by copyright law and international treaties."))
            return wxT("本程序及其组件受版权法和国际条约的保护。");
        else if(Input==wxT("Trigger Exception(&T)"))
            return wxT("触发异常(&T)");
        else if(Input==wxT("Unauthorized reproduction or distribution of this program, or any portion of it may result in severe civil and criminal penalties."))
            return wxT("未经授权复制或分发本程序或其任何部分可能会导致严重的民事和刑事处罚。");
        else if(Input==wxT("Unhandled exception"))
            return wxT("未处理异常");
        else if(Input==wxT("Unknown error."))
            return wxT("未知错误。");
        else if(Input==wxT("Unknown module"))
            return wxT("未知模块");
        else if(Input==wxT("Updating firmware."))
            return wxT("正在更新固件。");
        else if(Input==wxT("Verify After Downloading"))
            return wxT("下载后校验");
        else if(Input==wxT("Version:"))
            return wxT("版本：");
        else if(Input==wxT("WindIOT and the WindIOT logo are registered trademarks of the WindIOT Co.,Ltd."))
            return wxT("微风科技和微风科技标志是微风科技有限公司的注册商标。");
        else if(Input==wxT("Writing configuration data."))
            return wxT("正在写入配置数据。");
        else if(Input==wxT("Yes"))
            return wxT("是");
        else if(Input==wxT("You might be a victim of device counterfeiting."))
            return wxT("您可能是假冒设备的受害者。");
        else
            return Input;
    }
    else
        return Input;
}
/* End Function:RVM_CFG_App::Translate ****************************************/

/* Begin Function:RVM_CFG_App::OnExceptionInMainLoop ***************************
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
        ASSERT(0,(std::string(sT("Unhandled exception"))+": "+Exc.what()).c_str());
    }
    return false;
}
/* End Function:RVM_CFG_App::OnExceptionInMainLoop ****************************/

/* Begin Function:RVM_CFG_App::OnSystemFatalException **************************
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
                "addr2line -f -p -e C4G02_Writer.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e C4G02_Writer.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e C4G02_Writer.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e C4G02_Writer.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e C4G02_Writer.exe %08lX &&"
                "echo ------------------------------------- &&"
                "addr2line -f -p -e C4G02_Writer.exe %08lX &&"
                "echo ------------------------------------- && pause",
                Stack[0],Stack[1],Stack[2],Stack[3],Stack[4],Stack[5]);
    system(Buf);
#endif
    /* Don't fuss when we're exiting in the release mode */
#ifndef DEBUG
    if(Main::Exit_Flag!=0)
        return 0;
#endif
    /* Print stack trace */
    switch(Exc->ExceptionRecord->ExceptionCode)
    {
        case EXCEPTION_STACK_OVERFLOW:
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        case EXCEPTION_DATATYPE_MISALIGNMENT:
        case EXCEPTION_ACCESS_VIOLATION:ASSERT(0,std::string(sT("Memory access error.")+Info).c_str());
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
        case EXCEPTION_INT_OVERFLOW:
        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
        case EXCEPTION_FLT_UNDERFLOW:ASSERT(0,std::string(sT("Floating point error.")+Info).c_str());
        case EXCEPTION_PRIV_INSTRUCTION:
        case EXCEPTION_ILLEGAL_INSTRUCTION:ASSERT(0,std::string(sT("Illegal instruction.")+Info).c_str());
        case EXCEPTION_IN_PAGE_ERROR:
        case EXCEPTION_INVALID_DISPOSITION:
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:ASSERT(0,std::string(sT("Execution abort.")+Info).c_str());
        default:ASSERT(0,std::string(sT("Unknown error.")).c_str());
    }
#else
    /* Don't fuss when we're exiting in the release mode */
#ifndef DEBUG
    if(Main::Exit_Flag!=0)
        return;
#endif
    switch(Exc)
    {
        case SIGSEGV:ASSERT(0,std::string(sT("Memory access error.")).c_str());
        case SIGABRT:ASSERT(0,std::string(sT("Execution abort.")).c_str());
        case SIGFPE:ASSERT(0,std::string(sT("Floating point error.")).c_str());
        case SIGILL:ASSERT(0,std::string(sT("Illegal instruction.")).c_str());
        default:ASSERT(0,std::string(sT("Unknown error.")).c_str());
    }
#endif
}
/* End Function:RVM_CFG_App::OnSystemFatalException ***************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
