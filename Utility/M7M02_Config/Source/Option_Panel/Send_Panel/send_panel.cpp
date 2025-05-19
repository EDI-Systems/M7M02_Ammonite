/******************************************************************************
Filename    : send_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Send panel implementation.
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
#include "Option_Panel/Send_Panel/send_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Send_Panel/send_panel.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#undef __HDR_CLASS__
/* End Include *************************************************************/
namespace RVM_CFG
{
/* Function:Send_Panel::Send_Panel ******************************************
Description : Constructor for send information.
Input       : class wxWindow* Parent - The parent window.
              const std::string& _Loction - The location where the error occurred.
Output      : None.
Return      : None.
****************************************************************************/
/* void */ Send_Panel::Send_Panel(class wxWindow* Parent, const std::string& _Location):
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
        this->Bind(wxEVT_BUTTON,&Send_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Send_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Send_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Send_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,3,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,_(""));
        this->Grid->SetColLabelValue(1,_("Process"));
        this->Grid->SetColLabelValue(2,_("Name"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(385));
        this->Grid->SetColSize(2,I2P(385));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Send_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED, &Send_Panel::On_Change, this);

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
/* End Function:Send_Panel::Send_Panel ***************************************/

/* Function:Send_Panel::~Send_Panel *******************************************
Description : Destructor for send panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Send_Panel::~Send_Panel(void)
{

}
/* End Function:Send_Panel::Send_Panel ***************************************/

/* Function:Send_Panel::Check *************************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Send_Panel::Check(void)
{
    cnt_t Row;
    std::string Name;
    std::string Process;
    std::pair<std::string,std::string> Check_Pair;
    std::set<std::pair<std::string,std::string>> Unique;

    /* TODO */
    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        if(this->Grid->GetCellBackgroundColour(Row,1)==*wxRED||this->Grid->GetCellBackgroundColour(Row,2)==*wxRED)
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _(this->Location),
                              _("There is a invalid row, row "+std::to_string(Row+1)));
            return -1;
        }

        /* Process */
        Process=this->Grid->GetCellValue(Row,1);
        if(Process=="")
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _(this->Location),
                              _("Process is invalid, row "+std::to_string(Row+1)));
            return -1;
        }

        /* Name */
        Name=this->Grid->GetCellValue(Row,2);
        if(Name=="")
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _(this->Location),
                              _("Name is invalid, row "+std::to_string(Row+1)));
            return -1;
        }

        /* The combination of process and name cannot be exactly the same */
        Check_Pair=std::make_pair(Process,Name);
        if(Unique.find(Check_Pair)==Unique.end())
            Unique.insert(Check_Pair);
        else
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _(this->Location),
                              _(Process+" and "+Name+" is repeated, row "+std::to_string(Row+1)));
            return -1;
        }

    }

    return 0;
}
/* End Function:Send_Panel::Check ********************************************/

/* Function:Send_Panel::Load **************************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Send>>&Send - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Send_Panel::Load(const std::vector<std::unique_ptr<class Send>>&Send)
{
    cnt_t Row;
    std::string Name;
    std::string Process;
    class wxArrayString Rec;
    std::map<std::string,wxArrayString>::iterator Iter;

    /* Update the information of "Native" and its "Receive" */
    this->Pro_Option.Clear();
    this->Pro_Rec.clear();
    for(const std::unique_ptr<class Native>&Native : Main::Proj_Info->Native)
    {
        Rec.Clear();
        for(const std::unique_ptr<class Receive>& Receive : Native->Receive)
            Rec.push_back(Receive->Name);
        this->Pro_Rec.insert(std::make_pair(Native->Name,Rec));
        this->Pro_Option.push_back(Native->Name);
    }

    /* Clear the grid */
    Main::Gird_Clear_Content(this->Grid);

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Send.size();Row++)
    {
        this->Add_Func();
        /* Process */
        Process=Send[Row]->Process;
        this->Grid->SetCellValue(Row,1,Process);

        /* Name will be fill in, whether valid or not */
        Name=Send[Row]->Name;
        this->Grid->SetCellValue(Row,2,Name);

        Iter=this->Pro_Rec.find(Process);
        if(Iter!=this->Pro_Rec.end())
            this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor(Iter->second));

        if(Iter==this->Pro_Rec.end())
        {
            this->Grid->SetCellValue(Row,1,Process+"(Invalid)");
            this->Grid->SetCellValue(Row,2,Name+"(Invalid)");
            this->Grid->SetCellBackgroundColour(Row,1,*wxRED);
            this->Grid->SetCellBackgroundColour(Row,2,*wxRED);
        }
        else if(Iter!=this->Pro_Rec.end()&&Iter->second.Index(Name)==wxNOT_FOUND)
        {
            this->Grid->SetCellValue(Row,2,Name+"(Invalid)");
            this->Grid->SetCellBackgroundColour(Row,2,*wxRED);
        }
    }
    Main::Row_Reorder(this->Grid);
}
/* End Function:Send_Panel::Load *********************************************/

/* Function:Send_Panel::Save **************************************************
Description : Save information to Proj_Info.
Input       : None.
Output      : std::vector<std::unique_ptr<class Send>>&Send - The corresponding
              data structure, which will be modified.
Return      : None.
******************************************************************************/
void Send_Panel::Save(std::vector<std::unique_ptr<class Send>>&Send)
{
    cnt_t Row;
    std::string Name;
    std::string Process;

    Send.clear();
    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Process */
        Process=this->Grid->GetCellValue(Row,1).ToStdString();

        /* Name */
        Name=this->Grid->GetCellValue(Row,2).ToStdString();

        Send.push_back(std::make_unique<class Send>(Name,Process));
    }
    wxLogDebug("Send_Panel::Save: %d block",this->Grid->GetNumberRows());

}
/* End Function:Send_Panel::Save ***********************************/

/* Function:Send_Panel::On_Add **********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
****************************************************************************/
void Send_Panel::On_Add(class wxCommandEvent& Event)
{
    this->Add_Func();
    Main::Row_Reorder(this->Grid);
}
/* End Function:Send_Panel::On_Add ******************************************/

/* Function:Send_Panel::On_Remove ********************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Send_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Send_Panel::On_Remove");
    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Send_Panel::On_Remove ***************************************/

/* Function:Send_Panel::On_Move_Up *******************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Send_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Send_Panel::On_Move_Up");
    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Send_Panel::On_Move_Up **************************************/

/* Function:Send_Panel::On_Move_Down *****************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Send_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Send_Panel::On_Move_Down");
    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Send_Panel::On_Move_Down ************************************/

/* Function:Send_Panel::On_Grid **********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Send_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Send_Panel::On_Grid");
    Main::Row_Pick(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Send_Panel::On_Grid ******************************************/

/* Function:Send_Panel::On_Change *********************************************
Description : wxEVT_GRID_CELL_CHANGED handler for 'Grid'.
Input       : class wxGridEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Send_Panel::On_Change(class wxGridEvent& Event)
{
    ret_t Row;
    ret_t Col;
    std::map<std::string,class wxArrayString>::iterator Iter;

    wxLogDebug("Extmem_Panel::On_Change");

    Row=Event.GetRow();
    Col=Event.GetCol();

    switch (Col)
    {
        case 1:
        {
            Iter=this->Pro_Rec.find(this->Grid->GetCellValue(Row,Col).ToStdString());
            if(Iter!=this->Pro_Rec.end())
            {
                this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor(Iter->second));
                if(Iter->second.size()>0)
                    this->Grid->SetCellValue(Row,2,Iter->second[0]);
                else
                    this->Grid->SetCellValue(Row,2,"");
                this->Grid->SetCellBackgroundColour(Row,1,*wxWHITE);
                this->Grid->SetCellBackgroundColour(Row,2,*wxWHITE);
            }
            break;
        }
        case 2:
        {
            this->Grid->SetCellBackgroundColour(Row,1,*wxWHITE);
            this->Grid->SetCellBackgroundColour(Row,2,*wxWHITE);
            break;
        }
        default:break;
    }
    Event.Skip();
}
/* End Function:Send_Panel::On_Change **************************************/

/* Function:Send_Panel::Add_Func ********************************************
Description : Add a new row to the grid and set the cells to appropriate controls.
Input       : None.
Output      : None.
Return      : None.
****************************************************************************/
void Send_Panel::Add_Func()
{
    ret_t Row;
    std::string Default_Process;

    wxLogDebug("Send_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    /* Default value */
    this->Grid->SetReadOnly(Row,0,true);
    this->Grid->SetCellBackgroundColour(Row,0,*wxLIGHT_GREY);

    /* Try to load the default choice */
    this->Grid->SetCellEditor(Row,1,new class wxGridCellChoiceEditor(this->Pro_Option));
    this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor());
    if(this->Pro_Option.size()>0)
    {
        Default_Process=this->Pro_Option[0];
        this->Grid->SetCellValue(Row,1,this->Pro_Option[0]);
        this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor(this->Pro_Rec.find(Default_Process)->second));
        if(this->Pro_Rec.find(Default_Process)->second.size()>0)
            this->Grid->SetCellValue(Row,2,this->Pro_Rec.find(Default_Process)->second[0]);
    }
}
/* End Function:Send_Panel::Add_Func *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
