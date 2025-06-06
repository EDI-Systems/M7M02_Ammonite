/******************************************************************************
Filename    : memory_panel.hpp
Author      : lbc
Date        : 24/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : memory information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __MEMORY_PANEL_TYPE__
#define __MEMORY_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __MEMORY_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __MEMORY_PANEL_DEF__
#define __MEMORY_PANEL_DEF__
/*****************************************************************************/
/* Mode of the panel, declaring shared or private memory */
#define MEM_PANEL_PRIVATE   (0)
#define MEM_PANEL_SHARED    (1)
/*****************************************************************************/
/* __MEMORY_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __MEMORY_PANEL_CLASS__
#define __MEMORY_PANEL_CLASS__
/*****************************************************************************/
class Memory_Panel:public wxPanel
{
public:
    ptr_t Mode;

    class wxBoxSizer* Border_Sizer;
    class wxBoxSizer* Main_Sizer;

    class wxGrid* Grid;

    class wxBoxSizer* Command_Sizer;
    class wxButton* Add;
    class wxButton* Remove;
    class wxButton* Move_Up;
    class wxButton* Move_Down;

    /* Whether to allow the name to be empty */
    ptr_t Name_Check;
    std::string Location;

    /* void */ Memory_Panel(class wxWindow* Parent, ptr_t Mode);
    /* void */ ~Memory_Panel(void);

    ret_t Row_Add(void);

    void Load(const std::vector<std::unique_ptr<class Mem_Info>>&Memory);
    ret_t Check(void);
    void Save(std::vector<std::unique_ptr<class Mem_Info>>&Memory);

    void On_Add(class wxCommandEvent& Event);
    void On_Remove(class wxCommandEvent& Event);
    void On_Move_Up(class wxCommandEvent& Event);
    void On_Move_Down(class wxCommandEvent& Event);
    void On_Grid(class wxGridRangeSelectEvent& Event);
    void On_Change(class wxGridEvent& Event);
};
/*****************************************************************************/
/* __MEMORY_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
