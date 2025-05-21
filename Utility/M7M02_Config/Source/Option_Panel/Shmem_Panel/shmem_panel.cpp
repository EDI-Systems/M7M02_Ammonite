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
              const std::string& _Loction - The loction where the error occurred.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Shmem_Panel::Shmem_Panel(class wxWindow* Parent, const std::string& _Location):
wxPanel(Parent,wxID_ANY),Location(_Location)
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
        this->Grid->SetColLabelValue(0,_(""));
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
        this->Grid->SetColSize(1,I2P(110));
        this->Grid->SetColSize(2,I2P(120));
        this->Grid->SetColSize(3,I2P(130));
        this->Grid->SetColSize(4,I2P(130));
        this->Grid->SetColSize(5,I2P(150));
        this->Grid->SetColSize(6,I2P(20));
        this->Grid->SetColSize(7,I2P(20));
        this->Grid->SetColSize(8,I2P(20));
        this->Grid->SetColSize(9,I2P(20));
        this->Grid->SetColSize(10,I2P(20));
        this->Grid->SetColSize(11,I2P(20));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Shmem_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED, &Shmem_Panel::On_Change, this);


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

}
/* End Function:Shmem_Panel::Shmem_Panel *************************************/

/* Function:Shmem_Panel::Check ************************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Shmem_Panel::Check(void)
{
    cnt_t Row;

    if(Main::Row_Name_Check(this->Grid,this->Location,BLANK_NAME_FORBID,1)!=0)
        return -1;

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* 'Type' is a read-only cell, so if it is empty or 'invalid', that means
         *  this memory is not exist in shared memory */

        /* But since we are not performing cross-checks at the moment, this error will remain */
//        if(this->Grid->GetCellValue(Row,2)==""||this->Grid->GetCellValue(Row,2)=="Invalid")
//        {
//            Main::Msgbox_Show(this, MSGBOX_ERROR,
//                              _(this->Location),
//                              _("There is a invalid row, row "+std::to_string(Row+1)));
//            return -1;
//        }
    }
    return 0;
}
/* End Function:Shmem_Panel::Check *******************************************/

/* Function:Shmem_Panel::Load *************************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Shmem>>&Shmem - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Shmem_Panel::Load(const std::vector<std::unique_ptr<class Shmem>>&Shmem)
{
    cnt_t Row;
    char Buf[32];
    cnt_t AttrCnt;
    std::string Name;
    std::vector<std::unique_ptr<class Mem_Info>>::iterator Mem_It;

    wxLogDebug("Shmem_Panel::Load %d",Shmem.size());

    /* Fill in the information */
    this->Name_Option.Clear();
    for(const std::unique_ptr<class Mem_Info>&Mem : Main::Proj_Info->Shmem)
        this->Name_Option.push_back(Mem->Name);
    /* Clear the grid */
    Main::Gird_Clear_Content(this->Grid);

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Shmem.size();Row++)
    {
        /* When I add a new row, I do not know the state of "Attribute",
         * which one can be write, which one is read-only, so the state
         * of "Attribute" should be set when I know the specific "Name". */
        this->Add_Func();

        /* Name */
        Name=Shmem[Row]->Name;
        this->Grid->SetCellValue(Row, 1, Name);

        /* The information is come from shared memory */
        for(const std::unique_ptr<class Mem_Info>&Mem : Main::Proj_Info->Shmem)
        {
            if(Mem->Name==Name)
            {
                /* Type */
                this->Grid->SetCellValue(Row, 2, this->Type_Option[Mem->Type]);
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

                /* The state of "Attribute" - which one is read-only. */
                for(AttrCnt=0;AttrCnt<6;++AttrCnt)
                {
                    if((Mem->Attr>>(AttrCnt))&1)
                    {
                        /* Can be selected */
                        this->Grid->SetCellValue(Row,6+AttrCnt,"1");
                        this->Grid->SetReadOnly(Row,6+AttrCnt,false);
                        this->Grid->SetCellBackgroundColour(Row,6+AttrCnt,*wxWHITE);
                    }
                    else
                    {
                        this->Grid->SetCellValue(Row,6+AttrCnt,"");
                        this->Grid->SetReadOnly(Row,6+AttrCnt,true);
                        this->Grid->SetCellBackgroundColour(Row,6+AttrCnt,*wxLIGHT_GREY);

                    }
                }
                /* Attribute */
                for(AttrCnt=0;AttrCnt<6;++AttrCnt)
                {
                    /* However, this "Attribute" must be a SUBSET of the "Attribute" of shared memory of this project  */
                    if(((Shmem[Row].get()->Attr>>(AttrCnt))&1)&&!this->Grid->IsReadOnly(Row,6+AttrCnt))
                        this->Grid->SetCellValue(Row,6+AttrCnt,"1");
                    else
                        this->Grid->SetCellValue(Row,6+AttrCnt,"");
                }
                break;
            }
            else
            {
                /* Type */
                this->Grid->SetCellValue(Row, 2, "Invalid");
                /* Base */
                this->Grid->SetCellValue(Row, 3, "Invalid");
                /* Size */
                this->Grid->SetCellValue(Row, 4, "Invalid");
                /* Align */
                this->Grid->SetCellValue(Row, 5, "Invalid");
                /* Attribute */
                for(AttrCnt=0;AttrCnt<6;++AttrCnt)
                {
                    this->Grid->SetCellValue(Row,6+AttrCnt,"");
                    this->Grid->SetReadOnly(Row,6+AttrCnt,true);
                }
            }
        }
    }
    Main::Row_Reorder(this->Grid);
//    cnt_t Row;
//    char Buf[32];
//    cnt_t AttrCnt;
//    std::string Name;
//    std::vector<std::unique_ptr<class Mem_Info>>::iterator Mem_It;
//
//
//    wxLogDebug("Shmem_Panel::Load %d",Shmem.size());
//
//    /* Fill in the information */
//    this->Name_Option.Clear();
//    for(const std::unique_ptr<class Mem_Info>&Mem : Main::Proj_Info->Shmem)
//        this->Name_Option.push_back(Mem->Name);
//    /* Clear the grid */
//    Main::Gird_Clear_Content(this->Grid);
//
//    /* Fill in the grid*/
//    for(Row=0;Row<(cnt_t)Shmem.size();Row++)
//    {
//        /* When I add a new row, I do not know the state of "Attribute",
//         * which one can be write, which one is read-only, so the state
//         * of "Attribute" should be set when I know the specific "Name". */
//        this->Add_Func();
//
//        Name=Shmem[Row]->Name;
//        for(const std::unique_ptr<class Mem_Info>&Mem : Main::Proj_Info->Shmem)
//        {
//            if(Mem->Name==Name)
//            {
//                /* Name */
//                this->Grid->SetCellValue(Row, 1, Name);
//                /* Type */
//                this->Grid->SetCellValue(Row, 2, this->Type_Option[Mem->Type]);
//                /* Base */
//                if(Mem->Base==MEM_AUTO)
//                    this->Grid->SetCellValue(Row, 3, "Auto");
//                else
//                {
//                    std::sprintf(Buf, "0x%llX", Mem->Base);
//                    this->Grid->SetCellValue(Row, 3, Buf);
//                }
//
//                /* Size */
//                std::sprintf(Buf, "0x%llX", Mem->Size);
//                this->Grid->SetCellValue(Row, 4, Buf);
//
//                /* Align */
//                if(Mem->Align==MEM_AUTO)
//                    this->Grid->SetCellValue(Row, 5, "Auto");
//                else
//                {
//                    std::sprintf(Buf, "%lld", Mem->Align);
//                    this->Grid->SetCellValue(Row, 5, Buf);
//                }
//
//                /* The state of "Attribute", not the value of "Attribute".
//                 * Now I can know which one is read-only. */
//                for(AttrCnt=0;AttrCnt<6;++AttrCnt)
//                {
//                    /* It is the "Attribute* of shared memory of this project,
//                     * instead of the shared memory of any "Native" or "Virtual". */
//                    if((Mem->Attr>>(AttrCnt))&1)
//                    {
//                        /* Can be selected */
//                        this->Grid->SetCellValue(Row,6+AttrCnt,"1");
//                        this->Grid->SetReadOnly(Row,6+AttrCnt,false);
//                        this->Grid->SetCellBackgroundColour(Row,6+AttrCnt,*wxWHITE);
//                    }
//                    else
//                    {
//                        this->Grid->SetCellValue(Row,6+AttrCnt,"");
//                        this->Grid->SetReadOnly(Row,6+AttrCnt,true);
//                        this->Grid->SetCellBackgroundColour(Row,6+AttrCnt,*wxLIGHT_GREY);
//
//                    }
//                }
//                /* Attribute */
//                for(AttrCnt=0;AttrCnt<6;++AttrCnt)
//                {
//                    /* However, this "Attribute" must be a SUBSET of the "Attribute" of shared memory of this project  */
//                    if(((Shmem[Row].get()->Attr>>(AttrCnt))&1)&&!this->Grid->IsReadOnly(Row,6+AttrCnt))
//                        this->Grid->SetCellValue(Row,6+AttrCnt,"1");
//                    else
//                        this->Grid->SetCellValue(Row,6+AttrCnt,"");
//                }
//                break;
//            }
//
//
//        }
//        if(this->Grid->GetCellValue(Row, 1)=="")
//        {
//            /* Name always be filled in, whether valid or not */
//            this->Grid->SetCellValue(Row,1,Name);
//            this->Grid->SetCellBackgroundColour(Row,1,*wxRED);
//            this->Grid->SetCellValue(Row, 2, "Invalid");
//            this->Grid->SetCellValue(Row, 3, "Invalid");
//            this->Grid->SetCellValue(Row, 4, "Invalid");
//            this->Grid->SetCellValue(Row, 5, "Invalid");
//            for(AttrCnt=0;AttrCnt<6;++AttrCnt)
//            {
//                this->Grid->SetCellValue(Row,6+AttrCnt,"");
//                this->Grid->SetReadOnly(Row,6+AttrCnt,true);
//                this->Grid->SetCellBackgroundColour(Row,6+AttrCnt,*wxLIGHT_GREY);
//            }
//        }
//    }
//    Main::Row_Reorder(this->Grid);
}
/* End Function:Shmem_Panel::Load ********************************************/

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
    this->Add_Func();
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
    Main::Row_Reorder(this->Grid);
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
    char Buf[64];
    cnt_t AttrCnt;
    std::string Name;

    Row=Event.GetRow();
    Col=Event.GetCol();

    switch (Col)
    {
        case 1:
        {
            Name=this->Grid->GetCellValue(Row,Col);
            for(const std::unique_ptr<class Mem_Info>&Mem : Main::Proj_Info->Shmem)
            {
                if(Mem->Name==Name)
                {
//                    /* Maybe a invalid shared memory translate a valid one */
//                    this->Grid->SetCellBackgroundColour(Row, 1, *wxWHITE);

                    /* Type */
                    switch(Mem->Type)
                    {
                        case MEM_CODE:this->Grid->SetCellValue(Row,2,"Code");break;
                        case MEM_DATA:this->Grid->SetCellValue(Row,2,"Data");break;
                        case MEM_DEVICE:this->Grid->SetCellValue(Row,2,"Device");break;
                        default:ASSERT(0,"Memory type is invalid.");
                    }

                    /* Base */
                    if(Mem->Base==MEM_AUTO)
                        this->Grid->SetCellValue(Row,3,"Auto");
                    else
                    {
                        std::sprintf(Buf, "0x%llx",Mem->Base);
                        this->Grid->SetCellValue(Row, 3,Buf);
                    }

                    /* Size */
                    std::sprintf(Buf, "0x%llx",Mem->Size);
                    this->Grid->SetCellValue(Row,4,Buf);

                    /* Align */
                    if(Mem->Align==MEM_AUTO)
                        this->Grid->SetCellValue(Row,5,"Auto");
                    else
                    {
                        std::sprintf(Buf, "%llu",Mem->Align);
                        this->Grid->SetCellValue(Row,5,Buf);
                    }

                    /* Attribute */
                    for(AttrCnt=0;AttrCnt<6;++AttrCnt)
                    {
                        if((Mem->Attr>>(AttrCnt))&1)
                        {
                            /* Can be selected */
                            this->Grid->SetCellValue(Row,6+AttrCnt,"1");
                            this->Grid->SetReadOnly(Row,6+AttrCnt,false);
                            this->Grid->SetCellBackgroundColour(Row,6+AttrCnt,*wxWHITE);
                        }
                        else
                        {
                            this->Grid->SetCellValue(Row,6+AttrCnt,"");
                            this->Grid->SetReadOnly(Row,6+AttrCnt,true);
                            this->Grid->SetCellBackgroundColour(Row,6+AttrCnt,*wxLIGHT_GREY);

                        }
                    }
                    break;
                }
            }
            break;
        }
        default:break;
    }
    Event.Skip();
}
/* End Function:Shmem_Panel::On_Change *************************************/

/* Function:Shmem_Panel::Add_Func *******************************************
Description : Add a new row to the grid and set the cells to appropriate controls.
Input       : None.
Output      : None.
Return      : None.
****************************************************************************/
void Shmem_Panel::Add_Func()
{

    ret_t Row;
    cnt_t Cnt;

    wxLogDebug("Shmem_Panel::On_Add");

    Row=Main::Row_Add(this->Grid);

    this->Grid->SetCellEditor(Row,1,new class wxGridCellChoiceEditor(this->Name_Option));

    /* Attribution */
    for(Cnt=6;Cnt<12;Cnt++)
    {
        this->Grid->SetCellEditor(Row,Cnt,new wxGridCellBoolEditor());
        this->Grid->SetCellRenderer(Row, Cnt, new wxGridCellBoolRenderer());
    }

    /* Type, base, size and align are read only */
    this->Grid->SetReadOnly(Row, 0, true);
    this->Grid->SetReadOnly(Row, 2, true);
    this->Grid->SetReadOnly(Row, 3, true);
    this->Grid->SetReadOnly(Row, 4, true);
    this->Grid->SetReadOnly(Row, 5, true);
    this->Grid->SetCellBackgroundColour(Row, 0, *wxLIGHT_GREY);
    this->Grid->SetCellBackgroundColour(Row, 2, *wxLIGHT_GREY);
    this->Grid->SetCellBackgroundColour(Row, 3, *wxLIGHT_GREY);
    this->Grid->SetCellBackgroundColour(Row, 4, *wxLIGHT_GREY);
    this->Grid->SetCellBackgroundColour(Row, 5, *wxLIGHT_GREY);
}
/* End Function:Shmem_Panel::Add_Func ****************************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
