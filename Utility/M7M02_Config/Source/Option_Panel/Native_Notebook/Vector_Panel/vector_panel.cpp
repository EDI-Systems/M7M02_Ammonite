/******************************************************************************
Filename    : vector_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Vector panel implementation.
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
#include "Option_Panel/Native_Notebook/Vector_Panel/vector_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Native_Notebook/native_notebook.hpp"
#include "Option_Panel/Native_Notebook/Vector_Panel/vector_panel.hpp"
#include "Option_Panel/Native_Notebook/Native_Panel/native_panel.hpp"
#include "Option_Panel/option_panel.hpp"

#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#include "Proj_Info/Process/Vector/vector.hpp"
#include "Chip_Info/chip_info.hpp"
#undef __HDR_CLASS__
/* End Include *************************************************************/
namespace RVM_CFG
{
/* Function:Vector_Panel::Vector_Panel **************************************
Description : Constructor for vector panel.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
****************************************************************************/
/* void */ Vector_Panel::Vector_Panel(class wxWindow* Parent):
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
        this->Bind(wxEVT_BUTTON,&Vector_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Vector_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Vector_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Vector_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,3,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,"#");
        this->Grid->SetColLabelValue(1,_("Name"));
        this->Grid->SetColLabelValue(2,_("Number"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(380));
        this->Grid->SetColSize(2,I2P(380));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Vector_Panel::On_Grid,this,this->Grid->GetId());

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
/* End Function:Vector_Panel::Vector_Panel **********************************/

/* Function:Vector_Panel::~Vector_Panel **************************************
Description : Destructor for vector panel.
Input       : None.
Output      : None.
Return      : None.
*****************************************************************************/
/* void */ Vector_Panel::~Vector_Panel(void)
{

}
/* End Function:Vector_Panel::Vector_Panel **********************************/

/* Function:Vector_Panel::Row_Add ********************************************
Description : Add a new row to the grid and set the cells to appropriate controls.
Input       : None.
Output      : None.
Return      : ret_t - The position that the row is at.
*****************************************************************************/
ret_t Vector_Panel::Row_Add(void)
{
    ret_t Row;

    wxLogDebug("Vector_Panel::Row_Add");

    Row=Main::Row_Add(this->Grid);

    /* First row is serial number */
    this->Grid->SetReadOnly(Row, 0, true);

    return Row;
}
/* End Function:Vector_Panel::Row_Add ****************************************/

/* Function:Vector_Panel::Load ************************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Vector>>&Vector - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Vector_Panel::Load(const std::vector<std::unique_ptr<class Vector>>&Vector)
{
    cnt_t Row;
    char Buf[32];

    wxLogDebug("Vector_Panel::Load %d",Vector.size());

    /* Clean up the grid */
    if(this->Grid->GetNumberRows()!=0)
        this->Grid->DeleteRows(0,this->Grid->GetNumberRows());

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Vector.size();Row++)
    {
        this->Row_Add();

        /* Name */
        this->Grid->SetCellValue(Row, 1, Vector[Row]->Name);

        /* Number */
        std::sprintf(Buf, "%lld", Vector[Row]->Number);
        this->Grid->SetCellValue(Row, 2, Buf);
    }

    Main::Row_Reorder(this->Grid);
}
/* End Function:Vector_Panel::Load *******************************************/

/* Function:Vector_Panel::Check **********************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Vector_Panel::Check(void)
{
    cnt_t Row;
    ptr_t Num_Val;
    std::string Num;
    std::set<std::string>Unique_Num;

    if(Main::Name_Check(this->Grid,1,_("Vector"),BLANK_FORBID))
        return -1;

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Number */
        Num=this->Grid->GetCellValue(Row,2);
        if(Main::Dec_Check(this,Num,_("Vector"),_("Number")+_(" at row ")+std::to_string(Row+1))!=0)
            return -1;

        Num_Val=std::stoll(Num,0,0);
        if(Num_Val>=Main::Chip_Info->Vector)
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              _("Vector"),
                              _("Number")+_(" at row ")+std::to_string(Row+1)+
                              " must less than "+std::to_string(Main::Chip_Info->Vector)+_("."));
            return -1;
        }

        /* Vector number should not be repeated in the current native process */
        if(Unique_Num.find(Num)==Unique_Num.end())
            Unique_Num.insert(Num);
        else
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              _("Vector"),
                              _("Number")+_(" at row ")+std::to_string(Row+1)+
                              " should not have duplicates in the current native process "+std::to_string(Main::Chip_Info->Vector)+_("."));
            return -1;
        }

        /* Vector number should not be repeated in the entire project */
        for(std::unique_ptr<class Native>&Native : Main::Proj_Info->Native)
        {
            /* Check all native process except the one which is currently being edited */
            if(Native->Name==Main::Option_Panel->Native_Notebook->Basic->Name->GetValue())
                continue;
            for(std::unique_ptr<class Vector>&Vec:Native->Vector)
            {
                if(Vec->Number==Num_Val)
                {
                    Main::Msgbox_Show(this,MSGBOX_ERROR,
                                      _("Vector"),
                                      _("Number")+_(" at row ")+std::to_string(Row+1)+
                                      " should not have duplicates throughout the whole project "+std::to_string(Main::Chip_Info->Vector)+_("."));
                    return -1;
                }
            }
        }
    }

    return 0;
}
/* End Function:Vector_Panel::Check ******************************************/

/* Function:Vector_Panel::Save ************************************************
Description : Save information to Proj_Info.
Input       : std::vector<std::unique_ptr<class Vector>>& Vector - The corresponding
              data structure.
Output      : std::vector<std::unique_ptr<class Vector>>& Vector - The corresponding
              data structure, which will be modified.
Return      : None.
******************************************************************************/
void Vector_Panel::Save(std::vector<std::unique_ptr<class Vector>>& Vector)
{
    cnt_t Row;
    ptr_t Number;
    std::string Name;

    Vector.clear();
    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Name */
        Name=this->Grid->GetCellValue(Row,1);

        /* Number */
        Number=std::stoull(std::string(this->Grid->GetCellValue(Row,2)),0,0);

        Vector.push_back(std::make_unique<class Vector>(Name,Number));
    }

    wxLogDebug("Vector_Panel::Save: %d block",this->Grid->GetNumberRows());
}
/* End Function:Vector_Panel::Save ******************************************/

/* Function:Vector_Panel::On_Add *********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
*****************************************************************************/
void Vector_Panel::On_Add(class wxCommandEvent& Event)
{
    wxLogDebug("Vector_Panel::On_Add");

    this->Row_Add();
    Main::Row_Reorder(this->Grid);
}
/* End Function:Vector_Panel::On_Add ****************************************/

/* Function:Vector_Panel::On_Remove ******************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
*****************************************************************************/
void Vector_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Vector_Panel::On_Remove");

    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Vector_Panel::On_Remove *************************************/

/* Function:Vector_Panel::On_Move_Up *****************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
****************************************************************************/
void Vector_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Vector_Panel::On_Move_Up");

    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Vector_Panel::On_Move_Up ************************************/

/* Function:Vector_Panel::On_Move_Down ***************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
*****************************************************************************/
void Vector_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Vector_Panel::On_Move_Down");

    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Vector_Panel::On_Move_Down **********************************/

/* Function:Vector_Panel::On_Grid ********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Vector_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Vector_Panel::On_Grid");

    Main::Row_Pick(this->Grid);
}
/* End Function:Vector_Panel::On_Grid ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
