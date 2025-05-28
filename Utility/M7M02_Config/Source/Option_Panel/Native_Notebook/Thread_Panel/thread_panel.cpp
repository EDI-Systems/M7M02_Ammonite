/******************************************************************************
Filename    : thread_panel.cpp
Author      : lbc
Date        : 24/04/2025
License     : Proprietary; confidential.
Description : Thread panel implementation.
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
#include "Option_Panel/Native_Notebook/Thread_Panel/thread_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/Native_Notebook/Thread_Panel/thread_panel.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Thread_Panel::Thread_Panel ****************************************
Description : Constructor for thread panel.
Input       : class wxWindow* Parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thread_Panel::Thread_Panel(class wxWindow* Parent):
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
        this->Bind(wxEVT_BUTTON,&Thread_Panel::On_Add,this,this->Add->GetId());
        this->Command_Sizer->Add(this->Add,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Remove=new class wxButton(this,wxID_ANY,_("Remove"));
        this->Bind(wxEVT_BUTTON,&Thread_Panel::On_Remove,this,this->Remove->GetId());
        this->Command_Sizer->Add(this->Remove,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Up=new class wxButton(this,wxID_ANY,_("Move Up"));
        this->Bind(wxEVT_BUTTON,&Thread_Panel::On_Move_Up,this,this->Move_Up->GetId());
        this->Command_Sizer->Add(this->Move_Up,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Move_Down=new class wxButton(this,wxID_ANY,_("Move Down"));
        this->Bind(wxEVT_BUTTON,&Thread_Panel::On_Move_Down,this,this->Move_Down->GetId());
        this->Command_Sizer->Add(this->Move_Down,2,wxALIGN_CENTER);
        this->Command_Sizer->AddStretchSpacer(1);

        this->Grid=new class wxGrid(this,wxID_ANY);
        this->Grid->CreateGrid(0,5,wxGrid::wxGridSelectRows);
        this->Grid->HideRowLabels();
        this->Grid->SetColLabelSize(I2P(32));
        this->Grid->SetColLabelValue(0,"#");
        this->Grid->SetColLabelValue(1,_("Name"));
        this->Grid->SetColLabelValue(2,_("Stack Size"));
        this->Grid->SetColLabelValue(3,_("Parameter"));
        this->Grid->SetColLabelValue(4,_("Priority"));
        this->Grid->SetColSize(0,I2P(30));
        this->Grid->SetColSize(1,I2P(130));
        this->Grid->SetColSize(2,I2P(215));
        this->Grid->SetColSize(3,I2P(215));
        this->Grid->SetColSize(4,I2P(200));
        this->Grid->DisableDragRowSize();
        this->Grid->DisableDragColSize();
        this->Bind(wxEVT_GRID_RANGE_SELECT,&Thread_Panel::On_Grid,this,this->Grid->GetId());
        this->Bind(wxEVT_GRID_CELL_CHANGED,&Thread_Panel::On_Change,this);

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
/* End Function:Thread_Panel::Thread_Panel ***********************************/

/* Function:Thread_Panel::~Thread_Panel ***************************************
Description : Destructor for thread panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thread_Panel::~Thread_Panel(void)
{

}
/* End Function:Thread_Panel::Thread_Panel ***********************************/

/* Function:Thread_Panel::Row_Add *********************************************
Description : Add a new row to the grid and set the cells to appropriate controls.
Input       : None.
Output      : None.
Return      : ret_t - The row added.
******************************************************************************/
ret_t Thread_Panel::Row_Add(void)
{
    ret_t Row;

    wxLogDebug("Thread_Panel::Row_Add");
    Row=Main::Row_Add(this->Grid);

    /* Default value */
    this->Grid->SetReadOnly(Row, 0, true);

    return Row;
}
/* End Function:Thread_Panel::Row_Add ****************************************/

/* Function:Thread_Panel::Load ************************************************
Description : Load information from Proj_Info into the this panel.
Input       : const std::vector<std::unique_ptr<class Thread>>&Thread - The
              corresponding data structure.
Output      : None.
Return      : None.
******************************************************************************/
void Thread_Panel::Load(const std::vector<std::unique_ptr<class Thread>>& Thread)
{
    cnt_t Row;
    char Buf[32];

    wxLogDebug("Thread_Panel::Load %d",Thread.size());

    /* Clean up the grid */
    if(this->Grid->GetNumberRows()!=0)
        this->Grid->DeleteRows(0,this->Grid->GetNumberRows());

    /* Fill in the grid*/
    for(Row=0;Row<(cnt_t)Thread.size();Row++)
    {
        this->Row_Add();

        /* Name */
        this->Grid->SetCellValue(Row, 1, Thread[Row]->Name);

        /* Stack_Size */
        std::sprintf(Buf, "0x%llX", Thread[Row]->Stack_Size);
        this->Grid->SetCellValue(Row, 2, Buf);

        /* Parameter */
        std::sprintf(Buf, "0x%llX", Thread[Row]->Parameter);
        this->Grid->SetCellValue(Row, 3, Buf);

        /* Priority */
        std::sprintf(Buf, "%lld", Thread[Row]->Priority);
        this->Grid->SetCellValue(Row, 4, Buf);
    }

    Main::Row_Reorder(this->Grid);
}
/* End Function:Thread_Panel::Load *******************************************/

/* Function:Thread_Panel::Check ***********************************************
Description : Check whether the current panel contains any errors.
Input       : None.
Output      : None.
Return      : ret_t - if 0, no error exists; else -1.
******************************************************************************/
ret_t Thread_Panel::Check(void)
{
    cnt_t Row;
    ret_t Kern_Prio;
    ret_t Priority_Val;

    std::string Name;
    std::string Priority;
    std::string Parameter;
    std::string Stack_Size;

    if(Main::Name_Check(this->Grid,1,_("Thread"),BLANK_FORBID))
        return -1;

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Stack Size */
        Stack_Size=this->Grid->GetCellValue(Row,2);
//        if(Main::Num_GEZ_Hex_Check(Stack_Size)!=0)
//        {
//            Main::Msgbox_Show(this,MSGBOX_ERROR,
//                              _(this->Location),
//                              _("Stack size is not a valid hexadecimal nonnegative integer, row "+std::to_string(Row+1)));
//            return -1;
//        }

        /* Parameter */
        Parameter=this->Grid->GetCellValue(Row,3);
//        if(Main::Num_GEZ_Hex_Check(Parameter)!=0)
//        {
//            Main::Msgbox_Show(this,MSGBOX_ERROR,
//                              _(this->Location),
//                              _("Parameter is not a valid hexadecimal nonnegative integer, row "+std::to_string(Row+1)));
//            return -1;
//        }

        /* Priority */
        Priority=this->Grid->GetCellValue(Row,4);
        if(Priority=="")
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                    _("Thread"),
                              _("Priority is not a valid decimal positive integer, row "+std::to_string(Row+1)));
            return -1;
        }
        Priority_Val=std::stoll(Priority,0,0);
        Kern_Prio=Main::Proj_Info->Kernel->Kern_Prio;
//        if(Main::Num_GZ_Check(Priority)){
//            Main::Msgbox_Show(this,MSGBOX_ERROR,
//                                 _(this->Location),
//                                 _("Priority is not a valid decimal positive integer, row "+std::to_string(Row+1)));
//            return -1;
//        }
        if(Priority_Val<5)
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                    _("Thread"),
                                 _("Priority must be greater than or equal to 5, row "+std::to_string(Row+1)));
            return -1;
        }
        if(Priority_Val>Kern_Prio-2)
        {
            Main::Msgbox_Show(this,MSGBOX_ERROR,
                    _("Thread"),
                                 _("Priority must be less than or equal to "+std::to_string(Kern_Prio-2)+
                                   ", because Kernel Priority is "+std::to_string(Kern_Prio)+", row "+
                                   std::to_string(Row+1)));
            return -1;
        }
    }

    return 0;
}
/* End Function:Thread_Panel::Check ******************************************/

/* Function:Thread_Panel::Save ************************************************
Description : Save information to Proj_Info.
Input       : std::vector<std::unique_ptr<class Thread>>&Thread - The corresponding
              data structure.
Output      : std::vector<std::unique_ptr<class Thread>>&Thread - The corresponding
              data structure, which will be modified.
Return      : None.
******************************************************************************/
void Thread_Panel::Save(std::vector<std::unique_ptr<class Thread>>& Thread)
{
    cnt_t Row;
    ptr_t Priority;
    ptr_t Parameter;
    ptr_t Stack_Size;
    std::string Name;


    wxLogDebug("Thread_Panel::Save");
    Thread.clear();

    for(Row=0;Row<(cnt_t)this->Grid->GetNumberRows();Row++)
    {
        /* Name */
        Name=this->Grid->GetCellValue(Row,1).ToStdString();
        /* Stack_Size */
        Stack_Size=std::stoull(this->Grid->GetCellValue(Row,2).ToStdString(),0,0);
        /* Parameter */
        Parameter=std::stoull(this->Grid->GetCellValue(Row,3).ToStdString(),0,0);
        /* Priority */
        Priority=std::stoull(this->Grid->GetCellValue(Row,4).ToStdString(),0,0);

        Thread.push_back(std::make_unique<class Thread>(Name,Stack_Size,Parameter,Priority));
    }

    wxLogDebug("Thread_Panel::Save: %d block",this->Grid->GetNumberRows());
}
/* End Function:Thread_Panel::Save ******************************************/

/* Function:Thread_Panel::On_Add *********************************************
Description : wxEVT_BUTTON handler for 'Add'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
*****************************************************************************/
void Thread_Panel::On_Add(class wxCommandEvent& Event)
{
    ret_t Row;

    wxLogDebug("Thread_Panel::On_Add");

    Row=this->Row_Add();

    this->Grid->SetCellValue(Row, 2, "0xFFFF");
    this->Grid->SetCellValue(Row, 3, "0xFFFF");
    this->Grid->SetCellValue(Row, 4, "5");

    Main::Row_Reorder(this->Grid);
}
/* End Function:Thread_Panel::On_Add *****************************************/

/* Function:Thread_Panel::On_Remove *******************************************
Description : wxEVT_BUTTON handler for 'Remove'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Thread_Panel::On_Remove(class wxCommandEvent& Event)
{
    wxLogDebug("Thread_Panel::On_Remove");

    Main::Row_Remove(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Thread_Panel::On_Remove **************************************/

/* Function:Thread_Panel::On_Move_Up ******************************************
Description : wxEVT_BUTTON handler for 'Move Up'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Thread_Panel::On_Move_Up(class wxCommandEvent& Event)
{
    wxLogDebug("Thread_Panel::On_Move_Up");

    Main::Row_Move_Up(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Thread_Panel::On_Move_Up *************************************/

/* Function:Thread_Panel::On_Move_Down ****************************************
Description : wxEVT_BUTTON handler for 'Restore'.
Input       : class wxCommandEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Thread_Panel::On_Move_Down(class wxCommandEvent& Event)
{
    wxLogDebug("Thread_Panel::On_Move_Down");

    Main::Row_Move_Down(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Thread_Panel::On_Move_Down ***********************************/

/* Function:Thread_Panel::On_Grid *********************************************
Description : wxEVT_GRID_RANGE_SELECT handler for 'Grid'.
Input       : class wxGridRangeSelectEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Thread_Panel::On_Grid(class wxGridRangeSelectEvent& Event)
{
    wxLogDebug("Thread_Panel::On_Grid");

    Main::Row_Pick(this->Grid);
    Main::Row_Reorder(this->Grid);
}
/* End Function:Thread_Panel::On_Grid ****************************************/

/* Function:Thread_Panel::On_Change *******************************************
Description : wxEVT_GRID_CELL_CHANGED handler for name selection.
Input       : class wxGridEvent& Event - The event.
Output      : None.
Return      : None.
******************************************************************************/
void Thread_Panel::On_Change(class wxGridEvent& Event)
{
    ret_t Row;
    ret_t Col;

    wxLogDebug("Thread_Panel::On_Change");

    Row=Event.GetRow();
    Col=Event.GetCol();

    if((Col==2)||(Col==3))
        this->Grid->SetCellValue(Row,Col,Main::Num2Hex(std::string(this->Grid->GetCellValue(Row,Col))));

    Event.Skip();
}
/* End Function:Thread_Panel::On_Change **************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
