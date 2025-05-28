/******************************************************************************
Filename    : shmem_panel.cpp
Author      : lbc
Date        : 25/04/2025
License     : Proprietary; confidential.
Description : Shmem panel implementation.
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
#include "string"

#define __HDR_DEF__
#include "Mem_Info/mem_info.hpp"
#include "Option_Panel/Shmem_Panel/shmem_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Option_Panel/Shmem_Panel/shmem_panel.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Shmem_Panel::Shmem_Panel ******************************************
Description : Constructor for shared memory information.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem_Panel::Shmem_Panel(class wxWindow* Parent):
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
        this->Bind(wxEVT_BUTTON,&Shmem_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Shmem_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Shmem_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Shmem_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,12,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,"#");
        this->Grid->SetColLabelValue(1,_("Name"));
        this->Grid->SetColLabelValue(2,_("Type"));
        this->Grid->SetColLabelValue(3,_("Base"));
        this->Grid->SetColLabelValue(4,_("Size"));
        this->Grid->SetColLabelValue(5,_("Align"));
        this->Grid->SetColLabelValue(6,_("R"));
        this->Grid->SetColLabelValue(7,_("W"));
        this->Grid->SetColLabelValue(8,_("X"));
        this->Grid->SetColLabelValue(9,_("B"));
        this->Grid->SetColLabelValue(10,_("C"));
        this->Grid->SetColLabelValue(11,_("S"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(160));
        this->Grid->SetColSize(2,I2P(120));
        this->Grid->SetColSize(3,I2P(120));
        this->Grid->SetColSize(4,I2P(120));
        this->Grid->SetColSize(5,I2P(120));
        this->Grid->SetColSize(6,I2P(20));
        this->Grid->SetColSize(7,I2P(20));
        this->Grid->SetColSize(8,I2P(20));
        this->Grid->SetColSize(9,I2P(20));
        this->Grid->SetColSize(10,I2P(20));
        this->Grid->SetColSize(11,I2P(20));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Shmem_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED,&Shmem_Panel::On_Change,this);

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
/* End Function:Shmem_Panel::Shmem_Panel *************************************/

/* Function:Shmem_Panel::~Shmem_Panel *****************************************
Description : Destructor for shmem panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem_Panel::~Shmem_Panel(void)
{
    this->Unbind(wxEVT_GRID_RANGE_SELECT,&Shmem_Panel::On_Grid,this,this->Grid->GetId());
    this->Unbind(wxEVT_GRID_CELL_CHANGED, &Shmem_Panel::On_Change, this);
}
/* End Function:Shmem_Panel::Shmem_Panel *************************************/

/* Function:Shmem_Panel::Row_Add *********************************************
Description : Add a new row to the grid and set the cells to appropriate controls.
Input       : None.
Output      : None.
Return      : ret_t - The row returned.
******************************************************************************/
ret_t Shmem_Panel::Row_Add(void)
{
    ret_t Row;
    cnt_t Cnt;

    wxLogDebug("Shmem_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    this->Grid->SetCellEditor(Row,1,new class wxGridCellChoiceEditor(this->Shmem_List));

    /* Attribution */
    for(Cnt=6;Cnt<12;Cnt++)
    {
        this->Grid->SetCellEditor(Row,Cnt,new class wxGridCellBoolEditor());
        this->Grid->SetCellRenderer(Row,Cnt,new class wxGridCellBoolRenderer());
    }

    /* Type, base, size and align are read only */
    this->Grid->SetReadOnly(Row, 0, true);
    this->Grid->SetReadOnly(Row, 2, true);
    this->Grid->SetReadOnly(Row, 3, true);
    this->Grid->SetReadOnly(Row, 4, true);
    this->Grid->SetReadOnly(Row, 5, true);

    /* Which attributes are editable are decided later */

    return Row;
}
/* End Function:Shmem_Panel::Row_Add ****************************************/

/* Function:Shmem_Panel::Grid_Editable ***************************************
Description : Set a grid editable.
Input       : cnt_t Row - The row.
              cnt_t Col - The column.
              ptr_t Editable - Whether to set the grid as read-only.
Output      : None.
Return      : ret_t - The row returned.
******************************************************************************/
void Shmem_Panel::Grid_Editable(cnt_t Row, cnt_t Col, ptr_t Editable)
{
    if(Editable==0)
    {
        this->Grid->SetCellValue(Row,Col,"0");
        this->Grid->SetReadOnly(Row,Col,true);
        this->Grid->SetCellBackgroundColour(Row,Col,*wxLIGHT_GREY);
    }
    else
    {
        /* We won't set ones, but definitely clears zeros to filter attr out */
        this->Grid->SetReadOnly(Row,Col,false);
        this->Grid->SetCellBackgroundColour(Row,Col,*wxWHITE);
    }
}
/* End Function:Shmem_Panel::Grid_Editable ***********************************/

/* Function:Shmem_Panel::Load_Row *********************************************
Description : Load the rest of the row, assuming that the name has been loaded.
Input       : cnt_t Row - The row.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::Load_Row(cnt_t Row)
{
    char Buf[32];
    std::string Name;

    Name=this->Grid->GetCellValue(Row, 1);

    /* Set grids to invalid beforehand */
    this->Grid->SetCellValue(Row, 2, _("Invalid"));
    this->Grid->SetCellValue(Row, 3, _("Invalid"));
    this->Grid->SetCellValue(Row, 4, _("Invalid"));
    this->Grid->SetCellValue(Row, 5, _("Invalid"));

    /* Find the corresponding shared memory block */
    for(const std::unique_ptr<class Mem_Info>&Mem:Main::Proj_Info->Shmem)
    {
        /* If we can find it, set attributes accordingly, and if we can't, just leave it alone */
        if(Mem->Name==Name)
        {
            /* Type */
            Main::Mem_Type_Set(Mem->Type,this->Grid,Row,2);

            /* Base */
            if(Mem->Base==MEM_AUTO)
                this->Grid->SetCellValue(Row, 3, "Auto");
            else
            {
                std::sprintf(Buf, "0x%llX", Mem->Base);
                this->Grid->SetCellValue(Row, 3, Buf);
            }

            /* Size */
            std::sprintf(Buf, "0x%llX", Mem->Size);
            this->Grid->SetCellValue(Row, 4, Buf);

            /* Align */
            if(Mem->Align==MEM_AUTO)
                this->Grid->SetCellValue(Row, 5, "Auto");
            else
            {
                std::sprintf(Buf, "%lld", Mem->Align);
                this->Grid->SetCellValue(Row, 5, Buf);
            }

            /* Attributes must be a subset of the original */
            this->Grid_Editable(Row,6,Mem->Attr&MEM_READ);
            this->Grid_Editable(Row,7,Mem->Attr&MEM_WRITE);
            this->Grid_Editable(Row,8,Mem->Attr&MEM_EXECUTE);
            this->Grid_Editable(Row,9,Mem->Attr&MEM_BUFFER);
            this->Grid_Editable(Row,10,Mem->Attr&MEM_CACHE);
            this->Grid_Editable(Row,11,Mem->Attr&MEM_STATIC);

            break;
        }
    }
}
/* End Function:Shmem_Panel::Load_Row ****************************************/

/* Function:Shmem_Panel::Load *************************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Shmem>>&Shmem - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::Load(const std::vector<std::unique_ptr<class Shmem>>& Shmem)
{
    cnt_t Row;

    wxLogDebug("Shmem_Panel::Load %d",Shmem.size());

    /* Gather all shared memory for reference */
    this->Shmem_List.Clear();
    for(const std::unique_ptr<class Mem_Info>&Mem : Main::Proj_Info->Shmem)
        this->Shmem_List.push_back(Mem->Name);

    /* Clean up the grid */
    if(this->Grid->GetNumberRows()!=0)
        this->Grid->DeleteRows(0,this->Grid->GetNumberRows());

    /* Fill in the grid */
    for(Row=0;Row<(cnt_t)Shmem.size();Row++)
    {
        this->Row_Add();

        /* Name */
        this->Grid->SetCellValue(Row,1,Shmem[Row]->Name);

        /* Original attributes - those not allowed will be filtered out */
        Main::Mem_Attr_Set(Shmem[Row]->Attr,this->Grid,Row,6);

        /* The rest */
        this->Load_Row(Row);
    }

    Main::Row_Reorder(this->Grid);
}
/* End Function:Shmem_Panel::Load ********************************************/

/* Function:Shmem_Panel::Check ************************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Shmem_Panel::Check(void)
{
    /* Only name needs to be checked because other stuff were checked in place */
    if(Main::Name_Check(this->Grid,1,_("Shared Memory"),BLANK_FORBID)!=0)
        return -1;

    return 0;
}
/* End Function:Shmem_Panel::Check *******************************************/

/* Function:Shmem_Panel::Save *************************************************
Description : Save information to Proj_Info.
Input       : std::vector<std::unique_ptr<class Shmem>>&Shmem - The corresponding
              data structure.
Output      : std::vector<std::unique_ptr<class Shmem>>&Shmem - The corresponding
              data structure, which will be modified.
Return      : None.
******************************************************************************/
void Shmem_Panel::Save(std::vector<std::unique_ptr<class Shmem>>&Shmem)
{
    cnt_t Row;
    ptr_t Attr;
    std::string Name;

    wxLogDebug("Memory_Panel::Save");

    Shmem.clear();
    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Name */
        Name=this->Grid->GetCellValue(Row,1).ToStdString();

        /* Attribute */
        Attr=Main::Mem_Attr_Get(this->Grid, Row, 6);

        Shmem.push_back(std::make_unique<class Shmem>(Name,Attr));
    }

    wxLogDebug("Shmem_Panel::Save: %d block",this->Grid->GetNumberRows());
}
/* End Function:Shmem_Panel::Save ********************************************/

/* Function:Shmem_Panel::On_Add ***********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::On_Add(class wxCommandEvent& Event)
{
    wxLogDebug("Shmem_Panel::On_Remove");

    this->Row_Add();
    Main::Row_Reorder(this->Grid);
}
/* End Function:Shmem_Panel::On_Add ******************************************/

/* Function:Shmem_Panel::On_Remove ********************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Shmem_Panel::On_Remove");

    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Shmem_Panel::On_Remove ***************************************/

/* Function:Shmem_Panel::On_Move_Up *******************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Shmem_Panel::On_Move_Up");

    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Shmem_Panel::On_Move_Up **************************************/

/* Function:Shmem_Panel::On_Move_Down *****************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Shmem_Panel::On_Move_Down");

    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Shmem_Panel::On_Move_Down ************************************/

/* Function:Shmem_Panel::On_Grid **********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Shmem_Panel::On_Grid");

    Main::Row_Pick(this->Grid);
}
/* End Function:Shmem_Panel::On_Grid *****************************************/

/* Function:Shmem_Panel::On_Change ********************************************
Description : wxEVT_GRID_CELL_CHANGED handler for name selection.
Input       : class wxGridEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::On_Change(class wxGridEvent& Event)
{
    ret_t Row;
    ret_t Col;

    Row=Event.GetRow();
    Col=Event.GetCol();

    /* Changing name, need to load the rest again */
    if(Col==1)
    {
        /* Default attributes is everything */
        Main::Mem_Attr_Set(MEM_ALL,this->Grid,Row,6);
        /* Filter out those allowed by the shared memory */
        this->Load_Row(Row);
    }

    Event.Skip();
}
/* End Function:Shmem_Panel::On_Change ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
