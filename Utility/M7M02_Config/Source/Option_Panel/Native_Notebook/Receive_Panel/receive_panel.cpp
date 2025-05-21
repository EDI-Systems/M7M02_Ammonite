/******************************************************************************
Filename    : receive_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Receive panel implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/xml/xml.h"
#include "wx/grid.h"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEF__
#include "Option_Panel/Native_Notebook/Receive_Panel/receive_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Native_Notebook/Receive_Panel/receive_panel.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Receive_Panel::Receive_Panel **************************************
Description : Constructor for receive panel.
Input       : class wxWindow* Parent - The parent window.
              const std::string& _Loction - The location where the error occurred.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Receive_Panel::Receive_Panel(class wxWindow* Parent,  const std::string& _Location):
wxPanel(Parent,wxID_ANY),Location(_Location)
{
    try
    {
        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Border_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        this->Command_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Command_Sizer->AddStretchSpacer(1);
        this->Add=new class wxButton(this,wxID_ANY,_("Add"));
        this->Bind(wxEVT_BUTTON,&Receive_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Receive_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Receive_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Receive_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,2,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,_(""));
        this->Grid->SetColLabelValue(1,_("Name"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(770));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Receive_Panel::On_Grid,this,this->Grid->GetId());

        this->Main_Sizer->Add(this->Grid,100,wxEXPAND);
        this->Main_Sizer->AddStretchSpacer(1);
        this->Main_Sizer->Add(this->Command_Sizer,0,wxEXPAND);

        this->Border_Sizer->Add(this->Main_Sizer,1,wxEXPAND|wxALL,I2P(5));
        this->SetSizer(this->Border_Sizer);
        this->Border_Sizer->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Receive_Panel::Receive_Panel *********************************/

/* Function:Receive_Panel::~Receive_Panel *************************************
Description : Destructor for receive panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Receive_Panel::~Receive_Panel(void)
{

}
/* End Function:Receive_Panel::Receive_Panel *********************************/

/* Function:Receive_Panel::Check **********************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Receive_Panel::Check(void)
{
    if(Main::Row_Name_Check(this->Grid,this->Location,BLANK_NAME_FORBID,1))
        return -1;
    return 0;
}
/* End Function:Receive_Panel::Check *****************************************/

/* Function:Receive_Panel::Load ***********************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Receive>>&Receive - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Receive_Panel::Load(const std::vector<std::unique_ptr<class Receive>>&Receive)
{
    cnt_t Row;

    wxLogDebug("Receive_Panel::Load %d",Receive.size());

    /* Clear the grid */
    Main::Gird_Clear_Content(this->Grid);

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Receive.size();Row++)
    {
        this->Add_Func();
        /* Name */
        this->Grid->SetCellValue(Row, 1, Receive[Row]->Name);
    }
    Main::Row_Reorder(this->Grid);
}
/* End Function:Receive_Panel::Load ******************************************/

/* Function:Receive_Panel::Save ***********************************************
Description : Save information to Proj_Info.
Input       : std::vector<std::unique_ptr<class Receive>>&Receive - The
              corresponding data structure.
Output      : std::vector<std::unique_ptr<class Receive>>&Receive - The
              corresponding data structure, which will be modified.
Return      : None.
******************************************************************************/
void Receive_Panel::Save(std::vector<std::unique_ptr<class Receive>>&Receive)
{
    cnt_t Row;
    std::string Name;

    Receive.clear();

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Name */
        Name=this->Grid->GetCellValue(Row,1).ToStdString();

        Receive.push_back(std::make_unique<class Receive>(Name));
    }
    wxLogDebug("Receive_Panel::Save: %d block",this->Grid->GetNumberRows());

}
/* End Function:Receive_Panel::Save ******************************************/

/* Function:Receive_Panel::On_Add *********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Receive_Panel::On_Add(class wxCommandEvent& Event)
{
    this->Add_Func();
    Main::Row_Reorder(this->Grid);
}
/* End Function:Receive_Panel::On_Add ****************************************/

/* Function:Receive_Panel::On_Remove ******************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Receive_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Receive_Panel::On_Remove");
    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Receive_Panel::On_Remove *************************************/

/* Function:Receive_Panel::On_Move_Up *****************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Receive_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Receive_Panel::On_Move_Up");
    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Receive_Panel::On_Move_Up ************************************/

/* Function:Receive_Panel::On_Move_Down ***************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Receive_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Receive_Panel::On_Move_Down");
    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Receive_Panel::On_Move_Down **********************************/

/* Function:Receive_Panel::On_Grid ********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Receive_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Receive_Panel::On_Grid");
    Main::Row_Pick(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Receive_Panel::On_Grid ***************************************/

/* Function:Receive_Panel::Add_Func *******************************************
Description : Add a new row to the grid and set the cells to appropriate controls.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Receive_Panel::Add_Func()
{
    ret_t Row;

    wxLogDebug("Receive_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    /* Default value */
    this->Grid->SetReadOnly(Row, 0, true);
    this->Grid->SetCellBackgroundColour(Row, 0, *wxLIGHT_GREY);
}
/* End Function:Receive_Panel::Add_Func **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
