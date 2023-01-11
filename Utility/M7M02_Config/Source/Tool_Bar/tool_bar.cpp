/******************************************************************************
Filename    : tool_bar.cpp
Author      : zjx
Date        : 16/04/2021
License     : Proprietary; confidential.
Description : Build toolbar class implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/grid.h"

#include "Image/new_proj.xpm"
#include "Image/open_proj.xpm"
#include "Image/save_proj.xpm"
#include "Image/generate.xpm"
#include "Image/validate.xpm"

#include "memory"
#include "vector"

#define __HDR_DEFS__
#include "Tool_Bar/tool_bar.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Tool_Bar/tool_bar.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Tool_Bar::Tool_Bar ******************************************
Description : Constructor for Tool_Bar.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Tool_Bar::Tool_Bar(class wxWindow* Parent):
wxToolBar(Parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxTB_HORIZONTAL|wxTB_FLAT|wxTB_NODIVIDER)
{
    try
    {
        this->New_Proj=this->AddTool(wxID_ANY,_("New Project"),wxBitmap(New_Proj_Icon),_("New Project"));
        this->Bind(wxEVT_TOOL,&Tool_Bar::On_New_Proj,this,this->New_Proj->GetId());

        this->Open_Proj=this->AddTool(wxID_ANY,_("Open Project"),wxBitmap(Open_Proj_Icon),_("Open Project"));
        this->Bind(wxEVT_TOOL,&Tool_Bar::On_Open_Proj,this,this->Open_Proj->GetId());

        this->Save_Proj=this->AddTool(wxID_ANY,_("Save Project"),wxBitmap(Save_Proj_Icon),_("Save Project"));
        this->Bind(wxEVT_TOOL,&Tool_Bar::On_Save_Proj,this,this->Save_Proj->GetId());

        this->AddSeparator();

        this->Generate=this->AddTool(wxID_ANY,_("Generate"),wxBitmap(Generate_Icon),_("Generate"));
        this->Bind(wxEVT_TOOL,&Tool_Bar::On_Generate,this,this->Generate->GetId());

        this->Validate=this->AddTool(wxID_ANY,_("Validate"),wxBitmap(Validate_Icon),_("Validate"));
        this->Bind(wxEVT_TOOL,&Tool_Bar::On_Validate,this,this->Validate->GetId());

        this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        this->Realize();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Tool_Bar::Tool_Bar *******************************************/

/* Begin Function:Tool_Bar::~Tool_Bar *****************************************
Description : Destructor for build tool bar.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Tool_Bar::~Tool_Bar(void)
{
    this->Unbind(wxEVT_TOOL,&Tool_Bar::On_New_Proj,this);
    this->Unbind(wxEVT_TOOL,&Tool_Bar::On_Open_Proj,this);
    this->Unbind(wxEVT_TOOL,&Tool_Bar::On_Save_Proj,this);
    this->Unbind(wxEVT_TOOL,&Tool_Bar::On_Generate,this);
    this->Unbind(wxEVT_TOOL,&Tool_Bar::On_Validate,this);
}
/* End Function:Tool_Bar::~Tool_Bar ******************************************/

/* Begin Function:Tool_Bar::State_Set *****************************************
Description : Set the current UI state, and decide what controls are usable.
Input       : ptr_t Type - The state type.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Bar::State_Set(ptr_t Type)
{
    if(Type==STATE_UI)
    {
        if(Main::UI_State==UI_NONE)
        {
            this->EnableTool(this->New_Proj->GetId(),true);
            this->EnableTool(this->Open_Proj->GetId(),true);
            this->EnableTool(this->Generate->GetId(),false);
            this->EnableTool(this->Validate->GetId(),false);
        }
        else
        {
            this->EnableTool(this->New_Proj->GetId(),false);
            this->EnableTool(this->Open_Proj->GetId(),false);
            this->EnableTool(this->Generate->GetId(),true);
            this->EnableTool(this->Validate->GetId(),true);
        }
    }
    else if(Type==STATE_SAVE)
    {
        if(Main::Save_State==SAVE_NONE)
            this->EnableTool(this->Save_Proj->GetId(),false);
        else
            this->EnableTool(this->Save_Proj->GetId(),true);
    }
}
/* End Function:Tool_Bar::State_Set ******************************************/

/* Begin Function:Tool_Bar::On_New_Proj ***************************************
Description : wxEVT_TOOL handler for 'New Project'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Bar::On_New_Proj(class wxCommandEvent& Event)
{
    std::string Path;
    class wxFileDialog* File;

    wxLogDebug("Tool_Bar::On_New_Proj");

    /* Let the user choose a filename */
    File=new class wxFileDialog(RVM_CFG_App::Main,_("New Project"),wxT(""),wxT(""),
                                _("Project File")+wxT(" (*.rvp)|*.rvp"),
                                wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if(File->ShowModal()!=wxID_OK)
        return;
    Path=File->GetPath();
    File->Destroy();

    //Main::File_New(Path);
}
/* End Function:Tool_Bar::On_New_Proj ****************************************/

/* Begin Function:Tool_Bar::On_Open_Proj **************************************
Description : wxEVT_TOOL handler for 'Open Project'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Bar::On_Open_Proj(class wxCommandEvent& Event)
{
    std::string Path;
    class wxFileDialog* File;

    wxLogDebug("Tool_Bar::On_Open_Proj");

    /* Let the user choose a filename */
    File=new class wxFileDialog(RVM_CFG_App::Main,_("Open File"),wxT(""),wxT(""),
                                _("Project File")+wxT(" (*.rvp)|*.rvp"),
                                wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if(File->ShowModal()!=wxID_OK)
        return;
    Path=File->GetPath();
    File->Destroy();

    //Main::File_Open(Path);
}
/* End Function:Tool_Bar::On_Open_Proj ***************************************/

/* Begin Function:Tool_Bar::On_Save_Proj **************************************
Description : wxEVT_TOOL handler for 'Save Project'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Bar::On_Save_Proj(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Bar::On_Save_Proj");
    //Main::File_Save();
}
/* End Function:Tool_Bar::On_Save_Proj ***************************************/

/* Begin Function:Tool_Bar::On_Validate ***************************************
Description : wxEVT_TOOL handler for 'Generate'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Bar::On_Generate(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Bar::On_Generate");
    //Main::Compile_Begin();
}
/* End Function:Tool_Bar::On_Generate *****************************************/

/* Begin Function:Tool_Bar::On_Validate ***************************************
Description : wxEVT_TOOL handler for 'Debug'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Tool_Bar::On_Validate(class wxCommandEvent& Event)
{
    wxLogDebug("Tool_Bar::On_Validate");
    //Main::Validate_Begin();
}
/* End Function:Tool_Bar::On_Validate ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
