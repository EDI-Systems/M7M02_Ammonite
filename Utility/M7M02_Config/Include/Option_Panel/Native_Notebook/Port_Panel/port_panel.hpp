/******************************************************************************
Filename    : port_panel.hpp
Author      : lbc
Date        : 25/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Port information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __PORT_PANEL_TYPE__
#define __PORT_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __PORT_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __PORT_PANEL_DEF__
#define __PORT_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __PORT_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PORT_PANEL_CLASS__
#define __PORT_PANEL_CLASS__
/*****************************************************************************/
class Port_Panel:public wxPanel
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
    std::map<std::string,class wxArrayString> Inv_List;

    /* void */ Port_Panel(class wxWindow* Parent);
    /* void */ ~Port_Panel(void);

    ret_t Row_Add(void);

    void Load(const std::vector<std::unique_ptr<class Port>>&Port);
    ret_t Check(void);
    void Save(std::vector<std::unique_ptr<class Port>>&Port);

    void On_Add(class wxCommandEvent& Event);
    void On_Remove(class wxCommandEvent& Event);
    void On_Move_Up(class wxCommandEvent& Event);
    void On_Move_Down(class wxCommandEvent& Event);
    void On_Grid(class wxGridRangeSelectEvent& Event);
    void On_Change(class wxGridEvent& Event);
};
/*****************************************************************************/
/* __PORT_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
