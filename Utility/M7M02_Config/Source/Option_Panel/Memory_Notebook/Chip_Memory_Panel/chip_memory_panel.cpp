/******************************************************************************
Filename    : chip_memory_panel.cpp
Author      : lbc
Date        : 24/04/2025
License     : Proprietary; confidential.
Description : Chip memory information implementation.
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
#include "Option_Panel/Memory_Notebook/Chip_Memory_Panel/chip_memory_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Memory_Notebook/Chip_Memory_Panel/chip_memory_panel.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Chip_Memory_Panel::Chip_Memory_Panel ******************************
Description : Constructor for memory information. This panel maybe include
              more than one memory segments.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip_Memory_Panel::Chip_Memory_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY)
{
    try
    {

        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Border_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,9,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,_("Type"));
        this->Grid->SetColLabelValue(1,_("Base"));
        this->Grid->SetColLabelValue(2,_("Size"));
        this->Grid->SetColLabelValue(3,_("R"));
        this->Grid->SetColLabelValue(4,_("W"));
        this->Grid->SetColLabelValue(5,_("X"));
        this->Grid->SetColLabelValue(6,_("B"));
        this->Grid->SetColLabelValue(7,_("C"));
        this->Grid->SetColLabelValue(8,_("S"));
        this->Grid->SetColSize(0,I2P(180));
        this->Grid->SetColSize(1,I2P(240));
        this->Grid->SetColSize(2,I2P(240));
        this->Grid->SetColSize(3,I2P(20));
        this->Grid->SetColSize(4,I2P(20));
        this->Grid->SetColSize(5,I2P(20));
        this->Grid->SetColSize(6,I2P(20));
        this->Grid->SetColSize(7,I2P(20));
        this->Grid->SetColSize(8,I2P(20));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Chip_Memory_Panel::On_Grid,this,this->Grid->GetId());

        this->Main_Sizer->Add(this->Grid,100,wxEXPAND);
        this->Main_Sizer->AddStretchSpacer(1);

        this->Border_Sizer->Add(this->Main_Sizer,1,wxEXPAND|wxALL,I2P(5));
        this->SetSizer(this->Border_Sizer);
        this->Border_Sizer->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Chip_Memory_Panel::Chip_Memory_Panel *************************/

/* Function:Chip_Memory_Panel::~Chip_Memory_Panel *****************************
Description : Destructor for memory panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip_Memory_Panel::~Chip_Memory_Panel(void)
{

}
/* End Function:Chip_Memory_Panel::Chip_Memory_Panel *************************/

/* Function:Chip_Memory_Panel::Load *******************************************
Description : Load.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
void Chip_Memory_Panel::Load(const std::vector<std::unique_ptr<class Mem_Info>>& Chip_Mem)
{
    cnt_t Row;
    cnt_t AttrCnt;

    ptr_t Type;
    ptr_t Base;
    ptr_t Size;
    ptr_t Attr;
    char Buf[32];
    class wxArrayString Type_Option;

    Type_Option.Add("Code");
    Type_Option.Add("Data");
    Type_Option.Add("Device");

    wxLogDebug("Chip_Memory_Panel::Load %d",Chip_Mem.size());

    /* Clear the grid */
    Main::Gird_Clear_Content(this->Grid);

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Chip_Mem.size();Row++)
    {
        this->Add_Func();

        /* Type */
        Type=Chip_Mem[Row]->Type;
        this->Grid->SetCellValue(Row, 0, Type_Option[Type]);

        /* Base */
        Base=Chip_Mem[Row]->Base;
        if(Base==MEM_AUTO)
            this->Grid->SetCellValue(Row, 1, "Auto");
        else
        {
            std::sprintf(Buf, "0x%llX",Base);
            this->Grid->SetCellValue(Row, 1, Buf);
        }

        /* Size */
        Size=Chip_Mem[Row]->Size;
        std::sprintf(Buf, "0x%llX", Size);
        this->Grid->SetCellValue(Row, 2, Buf);

        /* Attribute */
        Attr=Chip_Mem[Row]->Attr;
        for(AttrCnt=0;AttrCnt<6;++AttrCnt)
        {
            if((Attr>>(AttrCnt))&1)
                this->Grid->SetCellValue(Row,3+AttrCnt,"1");
            else
                this->Grid->SetCellValue(Row,3+AttrCnt,wxEmptyString);
        }
    }
}
/* End Function:Chip_Memory_Panel::Load **************************************/

/* Function:Chip_Memory_Panel::On_Grid ****************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Chip_Memory_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Chip_Memory_Panel::On_Grid");
    Main::Row_Pick(this->Grid);
}
/* End Function:Chip_Memory_Panel::On_Grid ***********************************/

/* Function:Chip_Memory_Panel::Add_Func ***************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Chip_Memory_Panel::Add_Func()
{
    ret_t Row;
    cnt_t Cnt;

    wxLogDebug("Chip_Memory_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    /* Attribution */
    for(Cnt=3;Cnt<9;Cnt++)
    {
        this->Grid->SetCellEditor(Row,Cnt,new wxGridCellBoolEditor());
        this->Grid->SetCellRenderer(Row, Cnt, new wxGridCellBoolRenderer());
    }

    /* Default value */
    for(Cnt=0;Cnt<9;++Cnt)
    {
        this->Grid->SetReadOnly(Row, Cnt, true);
        this->Grid->SetCellBackgroundColour(Row, Cnt, *wxLIGHT_GREY);
    }
}
/* End Function:Chip_Memory_Panel::Add_Func ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
