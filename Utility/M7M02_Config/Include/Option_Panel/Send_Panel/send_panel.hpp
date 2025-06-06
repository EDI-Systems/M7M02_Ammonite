/******************************************************************************
Filename    : send_panel.hpp
Author      : lbc
Date        : 25/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Send information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __SEND_PANEL_TYPE__
#define __SEND_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __SEND_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __SEND_PANEL_DEF__
#define __SEND_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __SEND_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __SEND_PANEL_CLASS__
#define __SEND_PANEL_CLASS__
/*****************************************************************************/
class Send_Panel:public wxPanel
{
public:
    class wxBoxSizer* Border_Sizer;
    class wxBoxSizer* Main_Sizer;

    class wxGrid* Grid;

    class wxBoxSizer* Command_Sizer;
    class wxButton* Add;
    class wxButton* Remove;
    class wxButton* Move_Up;
    class wxButton* Move_Down;

    /* List holding all possible choices - using wx types for simplicity */
    class wxArrayString Proc_List;
    std::map<std::string,class wxArrayString> Recv_List;

    /* void */ Send_Panel(class wxWindow* Parent);
    /* void */ ~Send_Panel(void);

    ret_t Row_Add(void);

    void Load(const std::vector<std::unique_ptr<class Send>>&Send);
    ret_t Check(void);
    void Save(std::vector<std::unique_ptr<class Send>>&Send);

    void On_Add(class wxCommandEvent& Event);
    void On_Remove(class wxCommandEvent& Event);
    void On_Move_Up(class wxCommandEvent& Event);
    void On_Move_Down(class wxCommandEvent& Event);
    void On_Grid(class wxGridRangeSelectEvent& Event);
    void On_Change(class wxGridEvent& Event);
};
/*****************************************************************************/
/* __SEND_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
