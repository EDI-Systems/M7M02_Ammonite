/******************************************************************************
Filename    : memory_panel.cpp
Author      : lbc
Date        : 24/04/2025
License     : Proprietary; confidential.
Description : Memory panel implementation.
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
#include "Mem_Info/mem_info.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Option_Panel/Memory_Panel/memory_panel.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Memory_Panel::Memory_Panel ****************************************
Description : Constructor for memory information.
Input       : class wxWindow* Parent - The parent window.
              ptr_t Mode - The mode, shared or private.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Memory_Panel::Memory_Panel(class wxWindow* Parent, ptr_t Mode):
wxPanel(Parent,wxID_ANY)
{
    try
    {
        this->SetBackgroundColour(Parent->GetBackgroundColour());
        this->Mode=Mode;

        this->Border_Sizer=new class wxBoxSizer(wxVERTICAL);
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        this->Command_Sizer=new class wxBoxSizer(wxHORIZONTAL);

        this->Command_Sizer->AddStretchSpacer(1);
        this->Add=new class wxButton(this,wxID_ANY,_("Add"));
        this->Bind(wxEVT_BUTTON,&Memory_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Memory_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Memory_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Memory_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,12,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));

        this->Grid->SetColLabelValue(0,"#");
        /* Name is optional if we're declaring private memory */
        if(this->Mode==MEM_PANEL_PRIVATE)
            this->Grid->SetColLabelValue(1,_("Optional Name"));
        else
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
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Memory_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED, &Memory_Panel::On_Change, this);

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
/* End Function:Memory_Panel::Memory_Panel ***********************************/

/* Function:Memory_Panel::~Memory_Panel ***************************************
Description : Destructor for memory panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Memory_Panel::~Memory_Panel(void)
{

}
/* End Function:Memory_Panel::Memory_Panel ***********************************/

/* Function:Memory_Panel::Row_Add *********************************************
Description : Add a new line to this panel, setting the controls of the grids.
              The values still needs to be filled in later.
Input       : None.
Output      : None.
Return      : ret_t - The position the row is at.
******************************************************************************/
ret_t Memory_Panel::Row_Add(void)
{
    ret_t Row;
    cnt_t Cnt;
    class wxArrayString Type;
    class wxArrayString Base;
    class wxArrayString Align;

    wxLogDebug("Memory_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    this->Grid->SetReadOnly(Row, 0, true);

    /* Type include "Code", "Data" and "Device" */
    Type.Add(_("Code"));
    Type.Add(_("Data"));
    Type.Add(_("Device"));
    this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor(Type));

    /* Base can be a number or "Auto" */
    Base.Add(_("Auto"));
    this->Grid->SetCellEditor(Row,3,new class wxGridCellChoiceEditor(Base,true));

    /* Alignment could be any number > 8 that is a power of two up to 1MB at the moment */
    Align.Add("Auto");
    Align.Add("8 (256B)");
    Align.Add("9 (512B)");
    Align.Add("10 (1KiB)");
    Align.Add("11 (2KiB)");
    Align.Add("12 (4KiB)");
    Align.Add("13 (8KiB)");
    Align.Add("14 (16KiB)");
    Align.Add("15 (32KiB)");
    Align.Add("16 (64KiB)");
    Align.Add("17 (128KiB)");
    Align.Add("18 (256KiB)");
    Align.Add("19 (512KiB)");
    Align.Add("20 (1MiB)");
    Align.Add("21 (2MiB)");
    Align.Add("22 (4MiB)");
    Align.Add("23 (8MiB)");
    Align.Add("24 (16MiB)");
    Align.Add("25 (32MiB)");
    Align.Add("26 (64MiB)");
    Align.Add("27 (128MiB)");
    Align.Add("28 (256MiB)");
    Align.Add("29 (512MiB)");
    Align.Add("30 (1GiB)");
    this->Grid->SetCellEditor(Row,5,new class wxGridCellChoiceEditor(Align,true));

    /* Attributes */
    for(Cnt=6;Cnt<12;Cnt++)
    {
        this->Grid->SetCellEditor(Row,Cnt,new wxGridCellBoolEditor());
        this->Grid->SetCellRenderer(Row, Cnt, new wxGridCellBoolRenderer());
    }

    return Row;
}
/* End Function:Memory_Panel::Row_Add ****************************************/

/* Function:Memory_Panel::Load ************************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Mem_Info>>&Memory - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Panel::Load(const std::vector<std::unique_ptr<class Mem_Info>>&Memory)
{
    cnt_t Row;
    char Buf[32];

    wxLogDebug("Memory_Panel::Load %d",Memory.size());

    /* Clean up the grid */
    if(this->Grid->GetNumberRows()!=0)
        this->Grid->DeleteRows(0,this->Grid->GetNumberRows());

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Memory.size();Row++)
    {
        this->Row_Add();

        /* Name */
        this->Grid->SetCellValue(Row, 1, Memory[Row]->Name);

        /* Type */
        Main::Mem_Type_Set(Memory[Row]->Type,this->Grid,Row,2);

        /* Base */
        if(Memory[Row].get()->Base==MEM_AUTO)
            this->Grid->SetCellValue(Row, 3, _("Auto"));
        else
        {
            sprintf(Buf, "0x%llX", Memory[Row]->Base);
            this->Grid->SetCellValue(Row, 3, Buf);
        }

        /* Size */
        sprintf(Buf, "0x%llX", Memory[Row]->Size);
        this->Grid->SetCellValue(Row, 4, Buf);

        /* Align */
        if(Memory[Row]->Align==MEM_AUTO)
            this->Grid->SetCellValue(Row, 5, "Auto");
        /* Original alignment less than 256 bytes, always align to 256 bytes */
        else if(Memory[Row]->Align<=8)
            this->Grid->SetCellValue(Row, 5, "8 (256B)");
        /* Original alignment more than 1M bytes, always align to 256M bytes */
        else if(Memory[Row]->Align>=30)
            this->Grid->SetCellValue(Row, 5, "30 (1GiB)");
        else
        {
            switch(Memory[Row]->Align)
            {
                case 9:this->Grid->SetCellValue(Row, 5, "9 (512B)");break;
                case 10:this->Grid->SetCellValue(Row, 5, "10 (1KiB)");break;
                case 11:this->Grid->SetCellValue(Row, 5, "11 (2KiB)");break;
                case 12:this->Grid->SetCellValue(Row, 5, "12 (4KiB)");break;
                case 13:this->Grid->SetCellValue(Row, 5, "13 (8KiB)");break;
                case 14:this->Grid->SetCellValue(Row, 5, "14 (16KiB)");break;
                case 15:this->Grid->SetCellValue(Row, 5, "15 (32KiB)");break;
                case 16:this->Grid->SetCellValue(Row, 5, "16 (64KiB)");break;
                case 17:this->Grid->SetCellValue(Row, 5, "17 (128KiB)");break;
                case 18:this->Grid->SetCellValue(Row, 5, "18 (256KiB)");break;
                case 19:this->Grid->SetCellValue(Row, 5, "19 (512KiB)");break;
                case 20:this->Grid->SetCellValue(Row, 5, "20 (1MiB)");break;
                case 21:this->Grid->SetCellValue(Row, 5, "21 (2MiB)");break;
                case 22:this->Grid->SetCellValue(Row, 5, "22 (4MiB)");break;
                case 23:this->Grid->SetCellValue(Row, 5, "23 (8MiB)");break;
                case 24:this->Grid->SetCellValue(Row, 5, "24 (16MiB)");break;
                case 25:this->Grid->SetCellValue(Row, 5, "25 (32MiB)");break;
                case 26:this->Grid->SetCellValue(Row, 5, "26 (64MiB)");break;
                case 27:this->Grid->SetCellValue(Row, 5, "27 (128MiB)");break;
                case 28:this->Grid->SetCellValue(Row, 5, "28 (256MiB)");break;
                case 29:this->Grid->SetCellValue(Row, 5, "29 (512MiB)");break;
                default:ASSERT(0,"Impossible memory block alignment.");
            }
        }

        /* Attribute */
        Main::Mem_Attr_Set(Memory[Row]->Attr,this->Grid,Row,6);
    }

    Main::Row_Reorder(this->Grid);
}
/* End Function:Memory_Panel::Load *******************************************/

/* Function:Memory_Panel::Check ***********************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Memory_Panel::Check(void)
{
    cnt_t Row;
    class wxString Base;
    class wxString Align;
    class wxString Caption;

    if(this->Mode==MEM_PANEL_PRIVATE)
    {
        Caption=_("Private Memory");
        if(Main::Name_Check(this->Grid,1,Caption,BLANK_FORBID)!=0)
            return -1;
    }
    else
    {
        Caption=_("Shared Memory");
        if(Main::Name_Check(this->Grid,1,Caption,BLANK_ALLOW)!=0)
            return -1;
    }

    /* Check names for duplication first */
    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Base */
        Base=this->Grid->GetCellValue(Row,3);
        if((Base!=_("Auto"))&&
           (Main::Hex_Check(this,Base,Caption,_("Base")+_(" at row ")+std::to_string(Row+1))!=0))
            return -1;

        if((Base==_("Auto"))&&(Main::Mem_Type_Get(this->Grid,Row,2)==MEM_DEVICE))
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              Caption,
                              _("Device memory")+_(" at row ")+std::to_string(Row+1)+_(" must have a fixed base address and cannot be 'Auto'."));
            return -1;
        }

        /* Size */
        if(Main::Hex_Pos_Check(this,this->Grid->GetCellValue(Row,4),
                               Caption,_("Size")+_(" at row ")+std::to_string(Row+1))!=0)
            return -1;

        /* Align must be set to 'Auto' when the segment base address is fixed */
        Align=this->Grid->GetCellValue(Row,5);
        if((Align!=_("Auto"))&&(Base!=_("Auto")))
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              Caption,
                              _("Align")+_(" at row ")+std::to_string(Row+1)+_(" must be 'Auto' when the base address is fixed."));
            return -1;
        }
    }

    return 0;
}
/* End Function:Memory_Panel::Check ******************************************/

/* Function:Memory_Panel::Save ************************************************
Description : Save information to Proj_Info.
Input       : None.
Output      : std::vector<std::unique_ptr<class Mem_Info>>&Memory - The corresponding
              data structure, which will be modified.
Return      : None.
******************************************************************************/
void Memory_Panel::Save(std::vector<std::unique_ptr<class Mem_Info>>&Memory)
{
    cnt_t Row;
    ptr_t Type;
    ptr_t Base;
    ptr_t Size;
    ptr_t Align;
    ptr_t Attr;
    std::string Name;

    wxLogDebug("Memory_Panel::Save");

    Memory.clear();

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Name */
        Name=this->Grid->GetCellValue(Row,1);

        /* Type */
        Type=Main::Mem_Type_Get(this->Grid,Row,2);

        /* Base */
        if(this->Grid->GetCellValue(Row,3)=="Auto")
            Base=MEM_AUTO;
        else
            Base=std::stoull(this->Grid->GetCellValue(Row,3).ToStdString(),0,0);

        /* Size */
        Size=std::stoull(this->Grid->GetCellValue(Row,4).ToStdString(),0,0);

        /* Align */
        if(this->Grid->GetCellValue(Row,5)=="Auto")
            Align=MEM_AUTO;
        /* There's a space between the number and the parenthesis so substr(0,2) is okay */
        else
            Align=std::stoull(std::string(this->Grid->GetCellValue(Row,5)).substr(0,2),0,0);

        /* Attributes */
        Attr=Main::Mem_Attr_Get(this->Grid,Row,6);

        Memory.push_back(std::make_unique<class Mem_Info>(Name,Base,Size,Type,Attr,Align));
    }
    wxLogDebug("Memory_Panel::Save: %d block",this->Grid->GetNumberRows());

}
/* End Function:Memory_Panel::Save *******************************************/

/* Function:Memory_Panel::On_Add **********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Panel::On_Add(class wxCommandEvent& Event)
{
    ret_t Row;

    Row=this->Row_Add();

    /* Default value */
    Main::Mem_Type_Set(MEM_DATA,this->Grid,Row,2);
    this->Grid->SetCellValue(Row, 3, _("Auto"));
    this->Grid->SetCellValue(Row, 4, "0x1000");
    this->Grid->SetCellValue(Row, 5, _("Auto"));
    Main::Mem_Attr_Set(MEM_READ|MEM_WRITE|MEM_CACHE|MEM_BUFFER|MEM_STATIC,this->Grid,Row,6);

    Main::Row_Reorder(this->Grid);
}
/* End Function:Memory_Panel::On_Add ****************************************/

/* Function:Memory_Panel::On_Remove ******************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Memory_Panel::On_Remove");

    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Memory_Panel::On_Remove **************************************/

/* Function:Memory_Panel::On_Move_Up ******************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Memory_Panel::On_Move_Up");

    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Memory_Panel::On_Move_Up *************************************/

/* Function:Memory_Panel::On_Move_Down ****************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Memory_Panel::On_Move_Down");

    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Memory_Panel::On_Move_Down ***********************************/

/* Function:Memory_Panel::On_Grid *********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Memory_Panel::On_Grid");

    Main::Row_Pick(this->Grid);
}
/* End Function:Memory_Panel::On_Grid ****************************************/

/* Function:Memory_Panel::On_Change *******************************************
Description : wxEVT_GRID_CELL_CHANGED handler for 'Grid'.
Input       : class wxGridEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Panel::On_Change(class wxGridEvent& Event)
{
    ret_t Row;
    ret_t Col;
    std::string Temp;
    ptr_t Align;

    wxLogDebug("Extmem_Panel::On_Change");

    Row=Event.GetRow();
    Col=Event.GetCol();

    switch(Col)
    {
        /* If type ever changes, default attributes to that type */
        case 2:
        {
            Temp=this->Grid->GetCellValue(Row,2);
            if(Temp==_("Code"))
                Main::Mem_Attr_Set(MEM_READ|MEM_EXECUTE|MEM_CACHE|MEM_BUFFER|MEM_STATIC,this->Grid,Row,2);
            else if(Temp=="Data")
                Main::Mem_Attr_Set(MEM_READ|MEM_WRITE|MEM_CACHE|MEM_BUFFER|MEM_STATIC,this->Grid,Row,2);
            else if(Temp=="Device")
                Main::Mem_Attr_Set(MEM_READ|MEM_WRITE|MEM_STATIC,this->Grid,Row,2);

            break;
        }
        /* Base address - if not auto, we need to make sure it is hex */
        case 3:
        {
            Temp=this->Grid->GetCellValue(Row,3);
            if(Temp!="Auto")
                this->Grid->SetCellValue(Row,3,Main::Num2Hex(Temp));

            break;
        }
        /* Size */
        case 4:
        {
            this->Grid->SetCellValue(Row,4,Main::Num2Hex(std::string(this->Grid->GetCellValue(Row,4))));
            break;
        }
        /* Align - need to fix it if it is out of bound, if yes, fix accordingly */
        case 5:
        {
            Temp=this->Grid->GetCellValue(Row,5);
            if(Temp!="Auto")
            {
                Align=std::stoull(Main::Num2Dec(std::string(Temp.substr(0,2))));
                if(Align==0)
                    this->Grid->SetCellValue(Row,5,"Auto");
                else if(Align<=8)
                    this->Grid->SetCellValue(Row,5,"8 (256B)");
                else if(Align>=30)
                    this->Grid->SetCellValue(Row,5,"30 (1GiB)");
                else
                {
                    switch(Align)
                    {
                        case 9:this->Grid->SetCellValue(Row, 5, "9 (512B)");break;
                        case 10:this->Grid->SetCellValue(Row, 5, "10 (1KiB)");break;
                        case 11:this->Grid->SetCellValue(Row, 5, "11 (2KiB)");break;
                        case 12:this->Grid->SetCellValue(Row, 5, "12 (4KiB)");break;
                        case 13:this->Grid->SetCellValue(Row, 5, "13 (8KiB)");break;
                        case 14:this->Grid->SetCellValue(Row, 5, "14 (16KiB)");break;
                        case 15:this->Grid->SetCellValue(Row, 5, "15 (32KiB)");break;
                        case 16:this->Grid->SetCellValue(Row, 5, "16 (64KiB)");break;
                        case 17:this->Grid->SetCellValue(Row, 5, "17 (128KiB)");break;
                        case 18:this->Grid->SetCellValue(Row, 5, "18 (256KiB)");break;
                        case 19:this->Grid->SetCellValue(Row, 5, "19 (512KiB)");break;
                        case 20:this->Grid->SetCellValue(Row, 5, "20 (1MiB)");break;
                        case 21:this->Grid->SetCellValue(Row, 5, "21 (2MiB)");break;
                        case 22:this->Grid->SetCellValue(Row, 5, "22 (4MiB)");break;
                        case 23:this->Grid->SetCellValue(Row, 5, "23 (8MiB)");break;
                        case 24:this->Grid->SetCellValue(Row, 5, "24 (16MiB)");break;
                        case 25:this->Grid->SetCellValue(Row, 5, "25 (32MiB)");break;
                        case 26:this->Grid->SetCellValue(Row, 5, "26 (64MiB)");break;
                        case 27:this->Grid->SetCellValue(Row, 5, "27 (128MiB)");break;
                        case 28:this->Grid->SetCellValue(Row, 5, "28 (256MiB)");break;
                        case 29:this->Grid->SetCellValue(Row, 5, "29 (512MiB)");break;
                        default:ASSERT(0,"Impossible memory block alignment.");
                    }
                }
            }
            break;
        }

        default:break;
    }

    Event.Skip();
}
/* End Memory_Panel::On_Change ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
