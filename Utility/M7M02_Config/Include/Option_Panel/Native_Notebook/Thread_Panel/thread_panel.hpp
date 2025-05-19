/******************************************************************************
Filename    : thread_panel.hpp
Author      : lbc
Date        : 24/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Thread information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __THREAD_PANEL_TYPE__
#define __THREAD_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __THREAD_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __THREAD_PANEL_DEF__
#define __THREAD_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __THREAD_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __THREAD_PANEL_CLASS__
#define __THREAD_PANEL_CLASS__
/*****************************************************************************/
class Thread_Panel:public wxPanel
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

    std::string Location;

    /* void */ Thread_Panel(class wxWindow*parent, const std::string& Location);
    /* void */ ~Thread_Panel(void);

    ret_t Check(void);

    void Load(const std::vector<std::unique_ptr<class Thread>>&Thread);
    void Save(std::vector<std::unique_ptr<class Thread>>&Thread);
    void On_Add(class wxCommandEvent& Event);
    void On_Remove(class wxCommandEvent& Event);
    void On_Move_Up(class wxCommandEvent& Event);
    void On_Move_Down(class wxCommandEvent& Event);
    void On_Grid(class wxGridRangeSelectEvent& Event);
    void On_Change(class wxGridEvent& Event);
    void Add_Func(void);
};
/*****************************************************************************/
/* __THREAD_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
