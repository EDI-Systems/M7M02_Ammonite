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
              const ptr_t& _Name_Check - Whether to allow the name to be empty,
              this type can be set to BLANK_NAME_PERMIT or BLANK_NAME_FORBID.
              const std::string& _Loction - The location where the error occurred.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Memory_Panel::Memory_Panel(class wxWindow* Parent, const std::string& _Location, const ptr_t& _Name_Check):
wxPanel(Parent,wxID_ANY),Name_Check(_Name_Check),Location(_Location)
{
    try
    {
        this->Type_Option.Add("Code");
        this->Type_Option.Add("Data");
        this->Type_Option.Add("Device");

        this->Base_Option.Add("Auto");

        this->Align_Option.Add("Auto");
        this->Align_Option.Add("16");
        this->Align_Option.Add("17");
        this->Align_Option.Add("18");
        this->Align_Option.Add("19");
        this->Align_Option.Add("20");

        this->SetBackgroundColour(Parent->GetBackgroundColour());

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

        this->Grid->SetColLabelValue(0,_(""));
        if(this->Name_Check==BLANK_NAME_FORBID)
            this->Grid->SetColLabelValue(1,_("Name"));
        else
            this->Grid->SetColLabelValue(1,_("Name(optional)"));
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
        this->Grid->SetColSize(1,I2P(100));
        this->Grid->SetColSize(2,I2P(120));
        this->Grid->SetColSize(3,I2P(120));
        this->Grid->SetColSize(4,I2P(150));
        this->Grid->SetColSize(5,I2P(150));
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
    class wxString Size;
    class wxString Attr;
    class wxString Align;

    /* The name must be a legal C identifier and cannot be repeated */
    if(Main::Row_Name_Check(this->Grid,this->Location,this->Name_Check,1)!=0)
        return -1;


    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Base */
        Base=this->Grid->GetCellValue(Row,3);
        if(Base!="Auto"&&Main::Num_GEZ_Hex_Check(Base.ToStdString())!=0)
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              _(this->Location),
                              _("Base must be 'Auto' or a valid hexadecimal nonnegative integer"));
            return -1;
        }

        /* Size */
        Size=this->Grid->GetCellValue(Row,4);
        if(Main::Num_GEZ_Hex_Check(Size.ToStdString())!=0)
        {
            Main::Msgbox_Show(this, MSGBOX_ERROR,
                              _(this->Location),
                              _("Size is not a valid hexadecimal nonnegative integer"));
            return -1;
        }

        /* Align */
        Align=this->Grid->GetCellValue(Row,5);
        if(Align!="Auto"&&Main::Num_GZ_Check(Align.ToStdString())!=0)
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              _(this->Location),
                              _("Align must be 'Auto' or a valid decimal positive integer"));
            return -1;
        }
        /* Align must be set to 'Auto' when the segment base address is fixed */
        if(Align!="Auto"&&Base!="Auto")
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                              _(this->Location),
                              _("Align must be set to 'Auto' when the base address is fixed"));
            return -1;
        }
    }
    return 0;
}
/* End Function:Memory_Panel::Check ******************************************/

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
    cnt_t AttrCnt;
    char Buf[32];

    wxLogDebug("Memory_Panel::Load %d",Memory.size());

    /* Clear the grid */
    Main::Gird_Clear_Content(this->Grid);

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Memory.size();Row++)
    {
        //Row=Main::Row_Add(this->Grid);
        this->Add_Func();

        /* Name */
        this->Grid->SetCellValue(Row, 1, Memory[Row]->Name);

        /* Type */
        this->Grid->SetCellValue(Row, 2, this->Type_Option[Memory[Row]->Type]);

        /* Base */
        if(Memory[Row].get()->Base==MEM_AUTO)
            this->Grid->SetCellValue(Row, 3, "Auto");
        else
        {
            std::sprintf(Buf, "0x%llX", Memory[Row]->Base);
            this->Grid->SetCellValue(Row, 3, Buf);
        }

        /* Size */
        std::sprintf(Buf, "0x%llX", Memory[Row]->Size);
        this->Grid->SetCellValue(Row, 4, Buf);

        /* Align */
        if(Memory[Row].get()->Align==MEM_AUTO)
            this->Grid->SetCellValue(Row, 5, "Auto");
        else
        {
            std::sprintf(Buf, "%lld", Memory[Row]->Align);
            this->Grid->SetCellValue(Row, 5, Buf);
        }
        /* Attribute */
        for(AttrCnt=0;AttrCnt<6;++AttrCnt)
        {
            if((Memory[Row].get()->Attr>>(AttrCnt))&1)
                this->Grid->SetCellValue(Row,6+AttrCnt,"1");
            else
                this->Grid->SetCellValue(Row,6+AttrCnt,wxEmptyString);
        }
    }
    Main::Row_Reorder(this->Grid);
}
/* End Function:Memory_Panel::Load *******************************************/

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
        Name=this->Grid->GetCellValue(Row,1).ToStdString();

        /* Type */
        if(this->Grid->GetCellValue(Row,2)=="Code")
            Type=MEM_CODE;
        else if(this->Grid->GetCellValue(Row,2)=="Data")
            Type=MEM_DATA;
        else if(this->Grid->GetCellValue(Row,2)=="Device")
            Type=MEM_DEVICE;
        else
            throw std::runtime_error("Memory Type is malformed.");

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
        else
            Align=std::stoull(this->Grid->GetCellValue(Row,5).ToStdString(),0,0);

        /* Attribute */
        Attr=0;
        if(this->Grid->GetCellValue(Row,6)=="1")
            Attr|=MEM_READ;;
        if(this->Grid->GetCellValue(Row,7)=="1")
            Attr|=MEM_WRITE;
        if(this->Grid->GetCellValue(Row,8)=="1")
            Attr|=MEM_EXECUTE;
        if(this->Grid->GetCellValue(Row,9)=="1")
            Attr|=MEM_BUFFER;
        if(this->Grid->GetCellValue(Row,10)=="1")
            Attr|=MEM_CACHE;
        if(this->Grid->GetCellValue(Row,11)=="1")
            Attr|=MEM_STATIC;
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
    this->Add_Func();
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
    std::string Type;
    class wxString Num;

    wxLogDebug("Extmem_Panel::On_Change");

    Row=Event.GetRow();
    Col=Event.GetCol();

    switch (Col)
    {
        case 2:
        {
            Type=this->Grid->GetCellValue(Row,Col);
            if(Type=="Code")
            {
                this->Grid->SetCellValue(Row,3,"1");
                this->Grid->SetCellValue(Row,4,"");
                this->Grid->SetCellValue(Row,5,"1");
                this->Grid->SetCellValue(Row,6,"1");
                this->Grid->SetCellValue(Row,7,"1");
                this->Grid->SetCellValue(Row,8,"1");
            }
            else if(Type=="Data")
            {
                this->Grid->SetCellValue(Row,3,"1");
                this->Grid->SetCellValue(Row,4,"1");
                this->Grid->SetCellValue(Row,5,"");
                this->Grid->SetCellValue(Row,6,"1");
                this->Grid->SetCellValue(Row,7,"1");
                this->Grid->SetCellValue(Row,8,"1");
            }
            else if(Type=="Device")
            {
                this->Grid->SetCellValue(Row,3,"1");
                this->Grid->SetCellValue(Row,4,"1");
                this->Grid->SetCellValue(Row,5,"");
                this->Grid->SetCellValue(Row,6,"");
                this->Grid->SetCellValue(Row,7,"");
                this->Grid->SetCellValue(Row,8,"1");
            }
            break;
        }
        case 3:
        {
            Num=this->Grid->GetCellValue(Row,Col).Upper();
            if(Num=="AUTO")
                break;

            if (Num.starts_with("0X"))
                Num[1]='x';
            else
                Num="0x"+Num;
            this->Grid->SetCellValue(Row,Col,Num);
            break;
        }
        case 4:
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
/* End Memory_Panel::On_Change ***********************************************/

/* Function:Memory_Panel::Add_Func ********************************************
Description : Add a new row to the grid and set the cells to appropriate
              controls.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Memory_Panel::Add_Func()
{
    ret_t Row;
    cnt_t Cnt;

    wxLogDebug("Memory_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    /* Type include "Code", "Data" and "Device" */
    this->Grid->SetCellEditor(Row,2,new class wxGridCellChoiceEditor(this->Type_Option));
    this->Grid->SetCellEditor(Row,3,new class wxGridCellChoiceEditor(this->Base_Option,true));
    this->Grid->SetCellEditor(Row,5,new class wxGridCellChoiceEditor(this->Align_Option,true));

    /* Attribution */
    for(Cnt=6;Cnt<12;Cnt++)
    {
        this->Grid->SetCellEditor(Row,Cnt,new wxGridCellBoolEditor());
        this->Grid->SetCellRenderer(Row, Cnt, new wxGridCellBoolRenderer());
    }

    /* Default value */
    this->Grid->SetReadOnly(Row, 0, true);
    this->Grid->SetCellBackgroundColour(Row, 0, *wxLIGHT_GREY);

    this->Grid->SetCellValue(Row, 2, this->Type_Option[0]);
    this->Grid->SetCellValue(Row, 3, "Auto");
    this->Grid->SetCellValue(Row, 4, "0xFFFF");
    this->Grid->SetCellValue(Row, 5, "Auto");
    this->Grid->SetCellValue(Row,6,"1");
    this->Grid->SetCellValue(Row,7,"");
    this->Grid->SetCellValue(Row,8,"1");
    this->Grid->SetCellValue(Row,9,"1");
    this->Grid->SetCellValue(Row,10,"1");
    this->Grid->SetCellValue(Row,11,"1");
}
/* End Function:Memory_Panel::Add_Func ****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
