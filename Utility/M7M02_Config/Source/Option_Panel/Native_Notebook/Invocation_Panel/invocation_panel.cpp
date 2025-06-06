/******************************************************************************
Filename    : invocation_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Invocation panel implementation.
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
#include "Option_Panel/Native_Notebook/Invocation_Panel/invocation_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"

#include "Option_Panel/Native_Notebook/Invocation_Panel/invocation_panel.hpp"

#include "Proj_Info/Process/Invocation/invocation.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Invocation_Panel::Invocation_Panel ********************************
Description : Constructor for invocation panel.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Invocation_Panel::Invocation_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY)
{
    try
    {
        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Border_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        this->Command_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Command_Sizer->AddStretchSpacer(1);
        this->Add=new class wxButton(this,wxID_ANY,_("Add"));
        this->Bind(wxEVT_BUTTON,&Invocation_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Invocation_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Invocation_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Invocation_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,3,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,"#");
        this->Grid->SetColLabelValue(1,_("Name"));
        this->Grid->SetColLabelValue(2,_("Stack Size"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(380));
        this->Grid->SetColSize(2,I2P(380));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Invocation_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED,&Invocation_Panel::On_Change,this);

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
/* End Function:Invocation_Panel::Invocation_Panel ***************************/

/* Function:Invocation_Panel::~Invocation_Panel *******************************
Description : Destructor for invocation panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Invocation_Panel::~Invocation_Panel(void)
{
    this->Unbind(wxEVT_GRID_RANGE_SELECT,&Invocation_Panel::On_Grid,this,this->Grid->GetId());
    this->Unbind(wxEVT_GRID_CELL_CHANGED,&Invocation_Panel::On_Change,this);
}
/* End Function:Invocation_Panel::Invocation_Panel ***************************/

/* Function:Invocation_Panel::Row_Add *****************************************
Description : Add a new line to this panel, setting the controls of the grids.
              The values still needs to be filled in later.
Input       : None.
Output      : None.
Return      : ret_t - The position the row is at.
******************************************************************************/
ret_t Invocation_Panel::Row_Add(void)
{
    ret_t Row;

    wxLogDebug("Invocation_Panel::Row_Add");

    Row=Main::Row_Add(this->Grid);

    /* Default value */
    this->Grid->SetReadOnly(Row, 0, true);

    return Row;
}
/* End Function:Invocation_Panel::Row_Add ************************************/

/* Function:Invocation_Panel::Load ********************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Invocation>>&Invocation -
              The corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Invocation_Panel::Load(const std::vector<std::unique_ptr<class Invocation>>&Invocation)
{
    cnt_t Row;
    char Buf[32];

    wxLogDebug("Invocation_Panel::Load %d",Invocation.size());

    /* Clean up the grid */
    if(this->Grid->GetNumberRows()!=0)
        this->Grid->DeleteRows(0,this->Grid->GetNumberRows());

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Invocation.size();Row++)
    {
        this->Row_Add();

        /* Name */
        this->Grid->SetCellValue(Row, 1, Invocation[Row]->Name);

        /* Stack Size */
        std::sprintf(Buf, "0x%llX", Invocation[Row]->Stack_Size);
        this->Grid->SetCellValue(Row, 2, Buf);
    }

    Main::Row_Reorder(this->Grid);
}
/* End Function:Invocation_Panel::Load ***************************************/

/* Function:Invocation_Panel::Check *******************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Invocation_Panel::Check(void)
{
    cnt_t Row;

    if(Main::Name_Check(this->Grid,1,_("Invocation"),BLANK_FORBID))
        return -1;

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Stack Size */
        if(Main::Hex_Check(this,this->Grid->GetCellValue(Row,2),
                           _("Invocation"),_("Stack Size")+_(" at row ")+std::to_string(Row+1))!=0)
            return -1;
    }

    return 0;
}
/* End Function:Invocation_Panel::Check **************************************/

/* Function:Invocation_Panel::Save ********************************************
Description : Save information to Proj_Info.
Input       : std::vector<std::unique_ptr<class Invocation>>&Invocation - The
              corresponding data structure.
Output      : std::vector<std::unique_ptr<class Invocation>>&Invocation - The
              corresponding data structure, which will be modified.
Return      : None.
******************************************************************************/
void Invocation_Panel::Save(std::vector<std::unique_ptr<class Invocation>>&Invocation)
{
    cnt_t Row;
    ptr_t Stack_Size;
    std::string Name;

    wxLogDebug("Invocation_Panel::Save");

    Invocation.clear();
    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Name */
        Name=this->Grid->GetCellValue(Row,1);

        /* Stack_Size */
        Stack_Size=std::stoull(std::string(this->Grid->GetCellValue(Row,2)),0,0);

        Invocation.push_back(std::make_unique<class Invocation>(Name,Stack_Size));
    }
}
/* End Function:Invocation_Panel::Save ***************************************/

/* Function:Invocation_Panel::On_Add ******************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Invocation_Panel::On_Add(class wxCommandEvent& Event)
{
    ret_t Row;

    Row=this->Row_Add();
    this->Grid->SetCellValue(Row, 2, "0x1000");

    Main::Row_Reorder(this->Grid);
}
/* End Function:Invocation_Panel::On_Add *************************************/

/* Function:Invocation_Panel::On_Remove ***************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Invocation_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Invocation_Panel::On_Remove");

    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Invocation_Panel::On_Remove **********************************/

/* Function:Invocation_Panel::On_Move_Up **************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Invocation_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Invocation_Panel::On_Move_Up");

    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Invocation_Panel::On_Move_Up *********************************/

/* Function:Invocation_Panel::On_Move_Down ************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Invocation_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Invocation_Panel::On_Move_Down");

    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Invocation_Panel::On_Move_Down *******************************/

/* Function:Invocation_Panel::On_Grid *****************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Invocation_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Invocation_Panel::On_Grid");

    Main::Row_Pick(this->Grid);
}
/* End Function:Invocation_Panel::On_Grid ************************************/

/* Function:Invocation_Panel::On_Change ***************************************
Description : wxEVT_GRID_CELL_CHANGED handler for name selection.
Input       : class wxGridEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Invocation_Panel::On_Change(class wxGridEvent& Event)
{
    ret_t Row;
    ret_t Col;
    class wxString Num;

    wxLogDebug("Extmem_Panel::On_Change");

    Row=Event.GetRow();
    Col=Event.GetCol();

    if(Col==2)
        this->Grid->SetCellValue(Row,2,Main::Num2Hex(std::string(this->Grid->GetCellValue(Row,2))));

    Event.Skip();
}
/* End Function:Invocation_Panel::On_Change **********************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
