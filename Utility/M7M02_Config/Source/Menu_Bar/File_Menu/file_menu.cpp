/******************************************************************************
Filename    : file_menu.cpp
Author      : pry
Date        : 10/01/2023
License     : Proprietary; confidential.
Description : File menu class implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/splitter.h"

#include "Image/new_proj_small.xpm"
#include "Image/open_proj_small.xpm"
#include "Image/save_proj_small.xpm"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEF__
#include "Menu_Bar/File_Menu/file_menu.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Menu_Bar/File_Menu/file_menu.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:File_Menu::File_Menu *********************************************
Description : Constructor for file menu.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ File_Menu::File_Menu(void)
{
    try
    {
        this->New_Proj=new wxMenuItem(this,wxID_ANY,_("New Project(&N)\tCtrl+N"));
        this->New_Proj->SetBitmap(wxBitmap(New_Proj_Icon_Small));
        this->Append(this->New_Proj);
        this->Bind(wxEVT_MENU,&File_Menu::On_New_Proj,this,this->New_Proj->GetId());

        this->Open_Proj=new wxMenuItem(this,wxID_ANY,_("Open Project(&O)\tCtrl+O"));
        this->Open_Proj->SetBitmap(wxBitmap(Open_Proj_Icon_Small));
        this->Append(this->Open_Proj);
        this->Bind(wxEVT_MENU,&File_Menu::On_Open_Proj,this,this->Open_Proj->GetId());

        this->Close_Proj=new wxMenuItem(this,wxID_ANY,_("Close Project(&X)\tCtrl+W"));
        this->Append(this->Close_Proj);
        this->Bind(wxEVT_MENU,&File_Menu::On_Close_Proj,this,this->Close_Proj->GetId());

        this->AppendSeparator();

        this->Save_Proj=new wxMenuItem(this,wxID_ANY,_("Save Project(&S)\tCtrl+S"));
        this->Save_Proj->SetBitmap(wxBitmap(Save_Proj_Icon_Small));
        this->Append(this->Save_Proj);
        this->Bind(wxEVT_MENU,&File_Menu::On_Save_Proj,this,this->Save_Proj->GetId());

        this->Save_As=new wxMenuItem(this,wxID_ANY,_("Save As\tF12"));
        this->Append(this->Save_As);
        this->Bind(wxEVT_MENU,&File_Menu::On_Save_As,this,this->Save_As->GetId());
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:File_Menu::File_Menu *****************************************/

/* Function:File_Menu::~File_Menu *********************************************
Description : Destructor for file menu.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ File_Menu::~File_Menu(void)
{
    this->Unbind(wxEVT_MENU,&File_Menu::On_New_Proj,this);
    this->Unbind(wxEVT_MENU,&File_Menu::On_Open_Proj,this);
    this->Unbind(wxEVT_MENU,&File_Menu::On_Close_Proj,this);
    this->Unbind(wxEVT_MENU,&File_Menu::On_Save_Proj,this);
    this->Unbind(wxEVT_MENU,&File_Menu::On_Save_As,this);
}
/* End Function:File_Menu::~File_Menu ****************************************/

/* Function:File_Menu::State_Set **********************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void File_Menu::State_Set(ptr_t Type)
{
    if(Type==STATE_UI)
    {
        if(Main::UI_State==UI_NONE)
        {
            this->New_Proj->Enable(true);
            this->Open_Proj->Enable(true);
            this->Close_Proj->Enable(false);
            this->Save_Proj->Enable(false);
            this->Save_As->Enable(false);
        }
        else
        {
            this->New_Proj->Enable(false);
            this->Open_Proj->Enable(false);
            this->Close_Proj->Enable(true);
            this->Save_Proj->Enable(true);
            this->Save_As->Enable(true);
        }
    }
    else if(Type==STATE_SAVE)
    {
        if(Main::Save_State==SAVE_NONE)
            this->Save_Proj->Enable(false);
        else
            this->Save_Proj->Enable(true);
    }
}
/* End Function:File_Menu::State_Set *****************************************/

/* Function:File_Menu::On_New_Proj ********************************************
Description : wxEVT_MENU handler for 'New Project'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void File_Menu::On_New_Proj(class wxCommandEvent& Event)
{
    std::string Path;
    std::unique_ptr<wxFileDialog> File;

    wxLogDebug("File_Menu::On_New_Proj");

    /* Let the user choose a filename */
    File=std::make_unique<wxFileDialog>(RVM_CFG_App::Main,_("New Project"),wxT(""),wxT(""),
                                        _("Project File")+wxT(" (*.rvp)|*.rvp"),
                                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if(File->ShowModal()!=wxID_OK)
        return;
    Path=File->GetPath();

    Main::Proj_New(Path);
}
/* End Function:File_Menu::On_New_Proj ***************************************/

/* Function:File_Menu::On_Open_Proj *******************************************
Description : wxEVT_MENU handler for 'Open Project'. double click to open, etc.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void File_Menu::On_Open_Proj(class wxCommandEvent& Event)
{
    std::string Path;
    std::unique_ptr<class wxFileDialog> File;

    wxLogDebug("File_Menu::On_Open_Proj");

    /* Let the user choose a filename */
    File=std::make_unique<class wxFileDialog>(RVM_CFG_App::Main,_("Open File"),wxT(""),wxT(""),
                                              _("Project File")+wxT(" (*.rvp)|*.rvp"),
                                              wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if(File->ShowModal()!=wxID_OK)
        return;
    Path=File->GetPath();
    Main::Proj_Open(Path);
}
/* End Function:File_Menu::On_Open_Proj **************************************/

/* Function:File_Menu::On_Close_Proj ******************************************
Description : wxEVT_MENU handler for 'Close Project'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void File_Menu::On_Close_Proj(class wxCommandEvent& Event)
{
    wxLogDebug("File_Menu::On_Close_Proj");
    //Main::Proj_Close();
}
/* End Function:File_Menu::On_Close_File *************************************/

/* Function:File_Menu::On_Save_File *******************************************
Description : wxEVT_MENU handler for 'Save Project'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void File_Menu::On_Save_Proj(class wxCommandEvent& Event)
{
    wxLogDebug("File_Menu::On_Save_Proj");
    Main::Proj_Save();
}
/* End Function:File_Menu::On_Save_File **************************************/

/* Function:File_Menu::On_Save_As *********************************************
Description : wxEVT_MENU handler for 'Save As'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void File_Menu::On_Save_As(class wxCommandEvent& Event)
{
    std::string Path;
    std::unique_ptr<wxFileDialog> File;

    wxLogDebug("File_Menu::On_Save_As");
    if(Main::Check_And_Save_Current_Edit()!=0)
        return;
//    if(Main::Check_And_Save_All()!=0)
//        return;

    /* Let the user choose a filename and location */
    File=std::make_unique<wxFileDialog>(RVM_CFG_App::Main,_("Save File As"),wxT(""),wxT(""),
                                        _("Project File")+wxT(" (*.rvp)|*.rvp"),
                                        wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if(File->ShowModal()!=wxID_OK)
        return;
    Path=File->GetPath();
    Main::Proj_Save_As(Path);
}
/* End Function:File_Menu::On_Save_As ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
