/******************************************************************************
Filename    : ocmem_panel.cpp
Author      : lbc
Date        : 24/04/2025
License     : Proprietary; confidential.
Description : On-chip memory information implementation.
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
#include "Option_Panel/Memory_Notebook/Ocmem_Panel/ocmem_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Memory_Notebook/Ocmem_Panel/ocmem_panel.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Ocmem_Panel::Ocmem_Panel ******************************************
Description : Constructor for memory information. This panel maybe include
              more than one memory segments.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Ocmem_Panel::Ocmem_Panel(class wxWindow* Parent):
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
        this->Grid->SetColSize(0,I2P(190));
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
/* End Function:Ocmem_Panel::Ocmem_Panel *************************************/

/* Function:Ocmem_Panel::~Ocmem_Panel *****************************************
Description : Destructor for memory panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Ocmem_Panel::~Ocmem_Panel(void)
{

}
/* End Function:Ocmem_Panel::Ocmem_Panel *************************************/

/* Function:Ocmem_Panel::Load *************************************************
Description : Load.
Input       : None.
Output      : None.
Return      : ret_t - 0 for OK, -1 for error.
******************************************************************************/
void Ocmem_Panel::Load(const std::vector<std::unique_ptr<class Mem_Info>>& Chip_Mem)
{
    cnt_t Row;
    cnt_t Count;
    char Buf[32];

    wxLogDebug("Ocmem_Panel::Load %d",Chip_Mem.size());

    /* Clean up the grid */
    if(this->Grid->GetNumberRows()!=0)
        this->Grid->DeleteRows(0,this->Grid->GetNumberRows());

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Chip_Mem.size();Row++)
    {
        Main::Row_Add(this->Grid);

        /* The values are pulled from the chip configuration files and are read only */
        for(Count=0;Count<9;Count++)
            this->Grid->SetReadOnly(Row, Count, true);

        /* Attributes use bool renderer */
        for(Count=3;Count<9;Count++)
        {
            this->Grid->SetCellEditor(Row, Count, new wxGridCellBoolEditor());
            this->Grid->SetCellRenderer(Row, Count, new wxGridCellBoolRenderer());
        }

        /* Type */
        Main::Mem_Type_Set(Chip_Mem[Row]->Type, this->Grid, Row, 0);

        /* Base */
        sprintf(Buf, "0x%llX",Chip_Mem[Row]->Base);
        this->Grid->SetCellValue(Row, 1, Buf);

        /* Size */
        sprintf(Buf, "0x%llX", Chip_Mem[Row]->Size);
        this->Grid->SetCellValue(Row, 2, Buf);

        /* Attributes */
        Main::Mem_Attr_Set(Chip_Mem[Row]->Attr,this->Grid,Row,3);
    }
}
/* End Function:Ocmem_Panel::Load ********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
