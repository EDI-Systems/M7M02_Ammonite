/******************************************************************************
Filename    : port_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Port panel implementation.
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
#include "Option_Panel/Native_Notebook/Port_Panel/port_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Native_Notebook/Port_Panel/port_panel.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#undef __HDR_CLASS__
/* End Include *************************************************************/
namespace RVM_CFG
{
/* Function:Port_Panel::Port_Panel ******************************************
Description : Constructor for port information.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
****************************************************************************/
/* void */ Port_Panel::Port_Panel(class wxWindow* Parent):
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
        this->Bind(wxEVT_BUTTON,&Port_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Port_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Port_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Port_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,3,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,"#");
        this->Grid->SetColLabelValue(1,_("Process"));
        this->Grid->SetColLabelValue(2,_("Name"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(380));
        this->Grid->SetColSize(2,I2P(380));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Port_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED, &Port_Panel::On_Change, this);

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
/* End Function:Port_Panel::Port_Panel *************************************/

/* Function:Port_Panel::~Port_Panel *****************************************
Description : Destructor for port panel.
Input       : None.
Output      : None.
Return      : None.
****************************************************************************/
/* void */ Port_Panel::~Port_Panel(void)
{
    this->Bind(wxEVT_GRID_RANGE_SELECT,&Port_Panel::On_Grid,this,this->Grid->GetId());
    this->Bind(wxEVT_GRID_CELL_CHANGED, &Port_Panel::On_Change, this);
}
/* End Function:Port_Panel::Port_Panel *************************************/

/* Function:Port_Panel::Add_Func ********************************************
Description : Add a new row to the grid and set the cells to appropriate
              controls.
Input       : None.
Output      : None.
Return      : ret_t - The row added.
****************************************************************************/
ret_t Port_Panel::Row_Add(void)
{
    ret_t Row;

    wxLogDebug("Port_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    /* Default value */
    this->Grid->SetReadOnly(Row,0,true);

    /* Try to load the default choice */
    this->Grid->SetCellEditor(Row,1,new class wxGridCellChoiceEditor(this->Proc_List));
    this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor());

    return Row;
}
/* End Function:Port_Panel::On_Add ******************************************/

/* Function:Port_Panel::Check ***********************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
*****************************************************************************/
ret_t Port_Panel::Check(void)
{
    cnt_t Row;
    std::string Name;
    std::string Process;
    std::pair<std::string,std::string> Check_Pair;
    std::set<std::pair<std::string,std::string>> Unique;

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Process */
        Process=this->Grid->GetCellValue(Row,1);
        if(Process=="")
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _("Port"),
                              _("Process is invalid, row "+std::to_string(Row+1)));
            return -1;
        }

        /* Name */
        Name=this->Grid->GetCellValue(Row,2);
        if(Name=="")
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _("Port"),
                              _("Name is invalid, row "+std::to_string(Row+1)));
            return -1;
        }

        /* Can't declare the same pair again */
        Check_Pair=std::make_pair(Process,Name);
        if(Unique.find(Check_Pair)==Unique.end())
            Unique.insert(Check_Pair);
        else
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _("Port"),
                              _(Process+" and "+Name+" is repeated, row "+std::to_string(Row+1)));
            return -1;
        }

    }

    return 0;
}
/* End Function:Port_Panel::Check ********************************************/

/* Function:Port_Panel::Load **************************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Port>>&Port - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Port_Panel::Load(const std::vector<std::unique_ptr<class Port>>&Port)
{
    cnt_t Row;
    class wxArrayString Inv;
    std::map<std::string, class wxArrayString>::iterator Iter;

    /* The invocation targets could only exist in native processes */
    this->Inv_List.clear();
    this->Proc_List.Clear();
    for(const std::unique_ptr<class Native>&Native:Main::Proj_Info->Native)
    {
        Inv.clear();

        for(const std::unique_ptr<class Invocation>& Invocation:Native->Invocation)
            Inv.push_back(Invocation->Name);

        this->Proc_List.push_back(Native->Name);
        this->Inv_List.insert(std::make_pair(Native->Name,Inv));
    }

    /* Clean up the grid */
    if(this->Grid->GetNumberRows()!=0)
        this->Grid->DeleteRows(0,this->Grid->GetNumberRows());

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Port.size();Row++)
    {
        this->Row_Add();

        /* Process */
        this->Grid->SetCellValue(Row,1,Port[Row]->Process);

        /* Name will be fill in, whether valid or not */
        this->Grid->SetCellValue(Row,2,Port[Row]->Name);

        /* Set the wxChoice available invocation choices */
        Iter=this->Inv_List.find(Port[Row]->Process);
        if(Iter!=this->Inv_List.end())
            this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor(Iter->second));
    }

    Main::Row_Reorder(this->Grid);
}
/* End Function:Port_Panel::Load *********************************************/

/* Function:Port_Panel::Save **************************************************
Description : Save information to Proj_Info.
Input       : std::vector<std::unique_ptr<class Port>>&Port - The corresponding
              data structure.
Output      : std::vector<std::unique_ptr<class Port>>&Port - The corresponding
              data structure, which will be modified.
Return      : None.
******************************************************************************/
void Port_Panel::Save(std::vector<std::unique_ptr<class Port>>&Port)
{
    cnt_t Row;
    std::string Name;
    std::string Process;

    Port.clear();
    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Process */
        Process=this->Grid->GetCellValue(Row,1);

        /* Name */
        Name=this->Grid->GetCellValue(Row,2);

        Port.push_back(std::make_unique<class Port>(Name,Process));
    }

    wxLogDebug("Port_Panel::Save: %d block",this->Grid->GetNumberRows());
}
/* End Function:Port_Panel::Save *******************************************/

/* Function:Port_Panel::On_Add **********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
****************************************************************************/
void Port_Panel::On_Add(class wxCommandEvent& Event)
{
    ret_t Row;
    std::string Default;

    wxLogDebug("Port_Panel::On_Add");

    Row=this->Row_Add();

    /* Try to supply default value */
    if(this->Proc_List.size()>0)
    {
        Default=this->Proc_List[0];
        this->Grid->SetCellValue(Row,1,Default);
        this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor(this->Inv_List.find(Default)->second));
        if(this->Inv_List.find(Default)->second.size()>0)
            this->Grid->SetCellValue(Row,2,this->Inv_List.find(Default)->second[0]);
    }

    Main::Row_Reorder(this->Grid);
}
/* End Function:Port_Panel::On_Add ******************************************/

/* Function:Port_Panel::On_Remove ********************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Port_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Port_Panel::On_Remove");

    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Port_Panel::On_Remove ***************************************/

/* Function:Port_Panel::On_Move_Up *******************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Port_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Port_Panel::On_Move_Up");

    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Port_Panel::On_Move_Up **************************************/

/* Function:Port_Panel::On_Move_Down *****************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Port_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Port_Panel::On_Move_Down");

    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Port_Panel::On_Move_Down ************************************/

/* Function:Port_Panel::On_Grid **********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Port_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Port_Panel::On_Grid");

    Main::Row_Pick(this->Grid);
}
/* End Function:Port_Panel::On_Grid ******************************************/

/* Function:Port_Panel::On_Change *********************************************
Description : wxEVT_GRID_CELL_CHANGED handler for name selection.
Input       : class wxGridEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Port_Panel::On_Change(class wxGridEvent& Event)
{
    ret_t Row;
    ret_t Col;
    std::map<std::string,class wxArrayString>::iterator Iter;

    wxLogDebug("Port_Panel::On_Change");

    Row=Event.GetRow();
    Col=Event.GetCol();

    if(Col==1)
    {
        Iter=this->Inv_List.find(std::string(this->Grid->GetCellValue(Row,1)));
        if(Iter!=this->Inv_List.end())
        {
            this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor(Iter->second));
            /* If there is any 'Invocation' in current native process, load the
             * first one as the default value, or we clean the cell of 'Name' */
            if(Iter->second.size()>0)
                this->Grid->SetCellValue(Row,2,Iter->second[0]);
            else
                this->Grid->SetCellValue(Row,2,"");
        }
    }

    Event.Skip();
}
/* End Function:Port_Panel::On_Change ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
