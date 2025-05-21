/******************************************************************************
Filename    : kfunc_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Kfunc panel implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/xml/xml.h"
#include "wx/grid.h"
#include "wx/regex.h"

#include "set"
#include "map"
#include "memory"

#define __HDR_DEF__
#include "Option_Panel/Kfunc_Panel/kfunc_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Kfunc_Panel/kfunc_panel.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Kfunc_Panel::Kfunc_Panel ******************************************
Description : Constructor for Kfunc panel.
Input       : class wxWindow* Parent - The parent window.
              const std::string& _Loction - The location where the error occurred.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kfunc_Panel::Kfunc_Panel(class wxWindow* Parent,const std::string& _Location):
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
        this->Bind(wxEVT_BUTTON,&Kfunc_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Kfunc_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Kfunc_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Kfunc_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,4,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,_(""));
        this->Grid->SetColLabelValue(1,_("Name"));
        this->Grid->SetColLabelValue(2,_("Begin"));
        this->Grid->SetColLabelValue(3,_("End"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(255));
        this->Grid->SetColSize(2,I2P(255));
        this->Grid->SetColSize(3,I2P(255));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Kfunc_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED, &Kfunc_Panel::On_Change, this);

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
/* End Function:Kfunc_Panel::Kfunc_Panel *************************************/

/* Function:Kfunc_Panel::~Kfunc_Panel *****************************************
Description : Destructor for Kfunc panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kfunc_Panel::~Kfunc_Panel(void)
{

}
/* End Function:Kfunc_Panel::Kfunc_Panel *************************************/

/* Function:Kfunc_Panel::Check ************************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Kfunc_Panel::Check()
{
    cnt_t Row;
    ptr_t End_Val;
    ptr_t Begin_Val;
    std::string End;
    std::string Begin;

    if(Main::Row_Name_Check(this->Grid,this->Location,BLANK_NAME_FORBID,1)!=0)
        return -1;

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Begin */
        Begin=this->Grid->GetCellValue(Row,2);
        if(Main::Num_GEZ_Hex_Check(Begin)!=0)
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              _(this->Location),
                              _("Begin a valid hexadecimal nonnegative integer, row "+std::to_string(Row+1)));
            return -1;
        }

        /* End */
        End=this->Grid->GetCellValue(Row,3);
        if(Main::Num_GEZ_Hex_Check(End)!=0)
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              _(this->Location),
                              _("End a valid hexadecimal nonnegative integer, row "+std::to_string(Row+1)));
            return -1;
        }

        /* 'End' must greater than 'Begin' */
        Begin_Val=std::stoll(Begin,0,0);
        End_Val=std::stoll(End,0,0);
        if(Begin_Val>End_Val)
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              _(this->Location),
                              _("'End' must be greater than or equal to 'Begin', row "+std::to_string(Row+1)));
            return -1;
        }
    }
    return 0;
}
/* End Function:Kfunc_Panel::Check *******************************************/

/* Function:Kfunc_Panel::Load *************************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Kfunc>>&Kfunc - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc_Panel::Load(const std::vector<std::unique_ptr<class Kfunc>>&Kfunc)
{
    cnt_t Row;
    char Buf[32];

    wxLogDebug("Kfunc_Panel::Load %d",Kfunc.size());

    /* Clear the grid */
    Main::Gird_Clear_Content(this->Grid);

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Kfunc.size();Row++)
    {
        this->Add_Func();

        /* Name */
        this->Grid->SetCellValue(Row, 1, Kfunc[Row].get()->Name);

        /* Begin */
        std::sprintf(Buf, "0x%llX", Kfunc[Row].get()->Begin);
        this->Grid->SetCellValue(Row, 2, Buf);

        /* End */
        std::sprintf(Buf, "0x%llX", Kfunc[Row].get()->End);
        this->Grid->SetCellValue(Row, 3, Buf);
    }
    Main::Row_Reorder(this->Grid);
}
/* End Function:Kfunc_Panel::Load ********************************************/

/* Function:Kfunc_Panel::Save *************************************************
Description : Save information to Proj_Info.
Input       : None.
Output      : std::vector<std::unique_ptr<class Kfunc>>&Kfunc - The corresponding
              data structure, which will be modified.
Return      : None.
******************************************************************************/
void Kfunc_Panel::Save(std::vector<std::unique_ptr<class Kfunc>>&Kfunc)
{
    cnt_t Row;
    ptr_t End;
    ptr_t Begin;
    std::string Name;

    Kfunc.clear();

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Name */
        Name=this->Grid->GetCellValue(Row,1).ToStdString();

        /* Begin */
        Begin=std::stoull(this->Grid->GetCellValue(Row,2).ToStdString(),0,0);

        /* End */
        End=std::stoull(this->Grid->GetCellValue(Row,3).ToStdString(),0,0);

        Kfunc.push_back(std::make_unique<class Kfunc>(Name,Begin,End));
    }
    wxLogDebug("Memory_Panel::Save: %d block",this->Grid->GetNumberRows());

}
/* End Function:Kfunc_Panel::Save ********************************************/

/* Function:Kfunc_Panel::On_Add ***********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc_Panel::On_Add(class wxCommandEvent& Event)
{
    this->Add_Func();
    Main::Row_Reorder(this->Grid);
}
/* End Function:Kfunc_Panel::On_Add ******************************************/

/* Function:Kfunc_Panel::On_Remove ********************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc_Panel::On_Remove(class wxCommandEvent& Event)
{
    //wxLogDebug("Kfunc_Panel::On_Remove");
    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Kfunc_Panel::On_Remove ***************************************/

/* Function:Kfunc_Panel::On_Move_Up *******************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    //wxLogDebug("Kfunc_Panel::On_Move_Up");
    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Kfunc_Panel::On_Move_Up **************************************/

/* Function:Kfunc_Panel::On_Move_Down *****************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    //wxLogDebug("Kfunc_Panel::On_Move_Down");
    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Kfunc_Panel::On_Move_Down ************************************/

/* Function:Kfunc_Panel::On_Grid **********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    //wxLogDebug("Kfunc_Panel::On_Grid");
    Main::Row_Pick(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Kfunc_Panel::On_Grid *****************************************/

/* Function:Kfunc_Panel::On_Change ********************************************
Description : wxEVT_GRID_CELL_CHANGED handler for 'Grid'.
Input       : class wxGridEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc_Panel::On_Change(class wxGridEvent& Event)
{
    ret_t Row;
    ret_t Col;
    class wxString Num;

    //wxLogDebug("Extmem_Panel::On_Change");

    Row=Event.GetRow();
    Col=Event.GetCol();

    switch (Col)
    {
        case 2:
        case 3:
        {
            Num=this->Grid->GetCellValue(Row,Col).Upper();
            if (Num.starts_with("0X"))
                Num[1]='x';
            else
                Num="0x"+Num;
            this->Grid->SetCellValue(Row,Col,Num);
            break;
        }
        default:break;
    }
    Event.Skip();
}
/* End Function:Kfunc_Panel::On_Change ***************************************/

/* Function:Kfunc_Panel::Add_Func *********************************************
Description : Add a new row to the grid and set the cells to appropriate
              controls.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Kfunc_Panel::Add_Func()
{
    ret_t Row;

    //wxLogDebug("Kfunc_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    /* Default value */
    this->Grid->SetReadOnly(Row, 0, true);
    this->Grid->SetCellBackgroundColour(Row,0,*wxLIGHT_GREY);

    this->Grid->SetCellValue(Row, 2, "0xFFFF");
    this->Grid->SetCellValue(Row, 3, "0xFFFF");
}
/* End Function:Kfunc_Panel::Add_Func ******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
