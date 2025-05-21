/******************************************************************************
Filename    : extmem_panel.cpp
Author      : lbc
Date        : 09/05/2025
License     : Proprietary; confidential.
Description : Extmem panel implementation.
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
#include "Option_Panel/Memory_Notebook/Extmem_Panel/extmem_panel.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Memory_Notebook/Extmem_Panel/extmem_panel.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Extmem_Panel::Extmem_Panel ****************************************
Description : Constructor for extmem panel.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Extmem_Panel::Extmem_Panel(class wxWindow* Parent):
wxPanel(Parent,wxID_ANY)
{
    try
    {
        this->Type_Option.Add("Code");
        this->Type_Option.Add("Data");
        this->Type_Option.Add("Device");

        this->SetBackgroundColour(Parent->GetBackgroundColour());

        this->Border_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        this->Command_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Command_Sizer->AddStretchSpacer(1);
        this->Add=new class wxButton(this,wxID_ANY,_("Add"));
        this->Bind(wxEVT_BUTTON,&Extmem_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Extmem_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Extmem_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Extmem_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,10,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,_(""));
        this->Grid->SetColLabelValue(1,_("Type"));
        this->Grid->SetColLabelValue(2,_("Base"));
        this->Grid->SetColLabelValue(3,_("Size"));
        this->Grid->SetColLabelValue(4,_("R"));
        this->Grid->SetColLabelValue(5,_("W"));
        this->Grid->SetColLabelValue(6,_("X"));
        this->Grid->SetColLabelValue(7,_("B"));
        this->Grid->SetColLabelValue(8,_("C"));
        this->Grid->SetColLabelValue(9,_("S"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(180));
        this->Grid->SetColSize(2,I2P(230));
        this->Grid->SetColSize(3,I2P(230));
        this->Grid->SetColSize(4,I2P(20));
        this->Grid->SetColSize(5,I2P(20));
        this->Grid->SetColSize(6,I2P(20));
        this->Grid->SetColSize(7,I2P(20));
        this->Grid->SetColSize(8,I2P(20));
        this->Grid->SetColSize(9,I2P(20));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Extmem_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED, &Extmem_Panel::On_Change, this);

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
/* End Function:Extmem_Panel::Extmem_Panel ***********************************/

/* Function:Extmem_Panel::~Extmem_Panel ***************************************
Description : Destructor for extmem panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Extmem_Panel::~Extmem_Panel(void)
{

}
/* End Function:Extmem_Panel::Extmem_Panel ***********************************/

/* Function:Extmem_Panel::Check ***********************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Extmem_Panel::Check(void)
{
    cnt_t Row;
    std::string Base;
    std::string Size;

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Base */
        Base=this->Grid->GetCellValue(Row,2);
        if(Main::Num_GEZ_Hex_Check(Base)!=0)
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _("Extra Memory"),
                              _("Base is not a valid hexadecimal nonnegative integer"));
            return -1;
        }

        /* Size */
        Size=this->Grid->GetCellValue(Row,3);
        if(Main::Num_GEZ_Hex_Check(Size)!=0)
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _("Extra Memory"),
                              _("Size is not a valid hexadecimal nonnegative integer"));
            return -1;
        }

    }

    return 0;
}
/* End Function:Extmem_Panel::Check *****************************************/

/* Function:Extmem_Panel::Load ***********************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Mem_Info>>&Memory - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Extmem_Panel::Load(const std::vector<std::unique_ptr<class Mem_Info>>&Memory)
{
    cnt_t Row;
    cnt_t AttrCnt;
    char Buf[32];

    wxLogDebug("Extmem_Panel::Load %d",Memory.size());

    /* Clear the grid */
    Main::Gird_Clear_Content(this->Grid);

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Memory.size();Row++)
    {
        this->Add_Func();

        /* Type */
        this->Grid->SetCellValue(Row, 1, this->Type_Option[Memory[Row]->Type]);

        /* Base */
        std::sprintf(Buf, "0x%llX", Memory[Row].get()->Base);
        this->Grid->SetCellValue(Row, 2, Buf);

        /* Size */
        std::sprintf(Buf, "0x%llX", Memory[Row].get()->Size);
        this->Grid->SetCellValue(Row, 3, Buf);

        /* Attribute */
        for(AttrCnt=0;AttrCnt<6;++AttrCnt)
        {
            if((Memory[Row].get()->Attr>>(AttrCnt))&1)
                this->Grid->SetCellValue(Row,4+AttrCnt,"1");
            else
                this->Grid->SetCellValue(Row,4+AttrCnt,wxEmptyString);
        }
    }
    Main::Row_Reorder(this->Grid);
}
/* End Function:Extmem_Panel::Load *******************************************/

/* Function:Extmem_Panel::Save ************************************************
Description : Save information to Proj_Info.
Input       : std::vector<std::unique_ptr<class Mem_Info>>&Memory - The corresponding
              data structure.
Output      : std::vector<std::unique_ptr<class Mem_Info>>&Memoryc - The corresponding
              data structure, which will be modified.
Return      : None.
******************************************************************************/
void Extmem_Panel::Save(std::vector<std::unique_ptr<class Mem_Info>>&Memory)
{
    cnt_t Row;
    ptr_t Type;
    ptr_t Base;
    ptr_t Size;
    ptr_t Attr;

    wxLogDebug("Extmem_Panel::Save");

    Memory.clear();

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Type */
        if(this->Grid->GetCellValue(Row,1)=="Code")
            Type=MEM_CODE;
        else if(this->Grid->GetCellValue(Row,1)=="Data")
            Type=MEM_DATA;
        else if(this->Grid->GetCellValue(Row,1)=="Device")
            Type=MEM_DEVICE;
        else
            throw std::runtime_error("Memory Type is malformed.");

        /* Base */
        Base=std::stoull(this->Grid->GetCellValue(Row,2).ToStdString(),0,0);

        /* Size */
        Size=std::stoull(this->Grid->GetCellValue(Row,3).ToStdString(),0,0);

        /* Attribute */
        Attr=0;
        if(this->Grid->GetCellValue(Row,4)=="1")
            Attr|=MEM_READ;;
        if(this->Grid->GetCellValue(Row,5)=="1")
            Attr|=MEM_WRITE;
        if(this->Grid->GetCellValue(Row,6)=="1")
            Attr|=MEM_EXECUTE;
        if(this->Grid->GetCellValue(Row,7)=="1")
            Attr|=MEM_BUFFER;
        if(this->Grid->GetCellValue(Row,8)=="1")
            Attr|=MEM_CACHE;
        if(this->Grid->GetCellValue(Row,9)=="1")
            Attr|=MEM_STATIC;

        /* Name and align do not exist */
        Memory.push_back(std::make_unique<class Mem_Info>("",Base,Size,Type,Attr,ALIGN_INVALID));
    }
    wxLogDebug("Extmem_Panel::Save: %d block",this->Grid->GetNumberRows());

}
/* End Function:Extmem_Panel::Save *******************************************/

/* Function:Extmem_Panel::On_Add **********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Extmem_Panel::On_Add(class wxCommandEvent& Event)
{
    this->Add_Func();
    Main::Row_Reorder(this->Grid);
}
/* End Function:Extmem_Panel::On_Add *****************************************/

/* Function:Extmem_Panel::On_Remove *******************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Extmem_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Extmem_Panel::On_Remove");
    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Extmem_Panel::On_Remove **************************************/

/* Function:Extmem_Panel::On_Move_Up ******************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Extmem_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Extmem_Panel::On_Move_Up");
    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Extmem_Panel::On_Move_Up *************************************/

/* Function:Extmem_Panel::On_Move_Down ****************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Extmem_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Extmem_Panel::On_Move_Down");
    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Extmem_Panel::On_Move_Down ***********************************/

/* Function:Extmem_Panel::On_Grid *********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Extmem_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Extmem_Panel::On_Grid");
    Main::Row_Pick(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Extmem_Panel::On_Grid ****************************************/

/* Function:Extmem_Panel::On_Change *******************************************
Description : wxEVT_GRID_CELL_CHANGED handler for name selection.
Input       : class wxGridEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Extmem_Panel::On_Change(class wxGridEvent& Event)
{
    ret_t Row;
    ret_t Col;
    std::string Type;
    class wxString Num;

    wxLogDebug("Extmem_Panel::On_Change");

    Row=Event.GetRow();
    Col=Event.GetCol();
    switch (Col)
    {
        case 1:
        {
            Type=this->Grid->GetCellValue(Row,Col);
            if(Type=="Code")
            {
                this->Grid->SetCellValue(Row,4,"1");
                this->Grid->SetCellValue(Row,5,"");
                this->Grid->SetCellValue(Row,6,"1");
                this->Grid->SetCellValue(Row,7,"1");
                this->Grid->SetCellValue(Row,8,"1");
                this->Grid->SetCellValue(Row,9,"1");
            }
            else if(Type=="Data")
            {
                this->Grid->SetCellValue(Row,4,"1");
                this->Grid->SetCellValue(Row,5,"1");
                this->Grid->SetCellValue(Row,6,"");
                this->Grid->SetCellValue(Row,7,"1");
                this->Grid->SetCellValue(Row,8,"1");
                this->Grid->SetCellValue(Row,9,"1");
            }
            else if(Type=="Device")
            {
                this->Grid->SetCellValue(Row,4,"1");
                this->Grid->SetCellValue(Row,5,"1");
                this->Grid->SetCellValue(Row,6,"");
                this->Grid->SetCellValue(Row,7,"");
                this->Grid->SetCellValue(Row,8,"");
                this->Grid->SetCellValue(Row,9,"1");
            }
            break;
        }
        case 2:
        case 3:
        {
            Num=this->Grid->GetCellValue(Row,Col);
            Num=Num.Upper();
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
/* End Function:Extmem_Panel::On_Change **************************************/

/* Function:Extmem_Panel::Add_Func ********************************************
Description : Add a new row to the grid and set the cells to appropriate
              controls.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Extmem_Panel::Add_Func()
{
    ret_t Row;
    cnt_t Cnt;

    wxLogDebug("Extmem_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    /* Type include "Code", "Data" and "Device" */
    this->Grid->SetCellEditor(Row,1,new class wxGridCellChoiceEditor(this->Type_Option));


    /* Attribution */
    for(Cnt=4;Cnt<10;Cnt++)
    {
        this->Grid->SetCellEditor(Row,Cnt,new wxGridCellBoolEditor());
        this->Grid->SetCellRenderer(Row, Cnt, new wxGridCellBoolRenderer());
    }

    /* Default value */

    this->Grid->SetReadOnly(Row,0,true);
    this->Grid->SetCellBackgroundColour(Row,0,*wxLIGHT_GREY);
    this->Grid->SetCellValue(Row, 1, this->Type_Option[0]);
    this->Grid->SetCellValue(Row, 2, "0xFFFF");
    this->Grid->SetCellValue(Row, 3, "0xFFFF");
    this->Grid->SetCellValue(Row,4,"1");
    this->Grid->SetCellValue(Row,5,"");
    this->Grid->SetCellValue(Row,6,"1");
    this->Grid->SetCellValue(Row,7,"1");
    this->Grid->SetCellValue(Row,8,"1");
    this->Grid->SetCellValue(Row,9,"1");
}
/* End Function:Extmem_Panel::Add_Func ***************************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
