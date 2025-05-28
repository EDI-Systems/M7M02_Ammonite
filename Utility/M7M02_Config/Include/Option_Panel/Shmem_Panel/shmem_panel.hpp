/******************************************************************************
Filename    : shmem_panel.hpp
Author      : lbc
Date        : 25/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Shared memory information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __SHMEM_PANEL_TYPE__
#define __SHMEM_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __SHMEM_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __SHMEM_PANEL_DEF__
#define __SHMEM_PANEL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __SHMEM_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __SHMEM_PANEL_CLASS__
#define __SHMEM_PANEL_CLASS__
/*****************************************************************************/
class Shmem_Panel:public wxPanel
{
public:
    class wxBoxSizer* Main_Sizer;
    class wxBoxSizer* Border_Sizer;

    class wxGrid* Grid;

    class wxBoxSizer* Command_Sizer;
    class wxButton* Add;
    class wxButton* Remove;
    class wxButton* Move_Up;
    class wxButton* Move_Down;

    class wxArrayString Shmem_List;

    /* void */ Shmem_Panel(class wxWindow* Parent);
    /* void */ ~Shmem_Panel(void);

    ret_t Row_Add(void);
    void Grid_Editable(cnt_t Row, cnt_t Col, ptr_t Editable);

    void Load_Row(cnt_t Row);
    void Load(const std::vector<std::unique_ptr<class Shmem>>& Shmem);
    ret_t Check(void);
    void Save(std::vector<std::unique_ptr<class Shmem>>& Shmem);

    void On_Add(class wxCommandEvent& Event);
    void On_Remove(class wxCommandEvent& Event);
    void On_Move_Up(class wxCommandEvent& Event);
    void On_Move_Down(class wxCommandEvent& Event);
    void On_Grid(class wxGridRangeSelectEvent& Event);
    void On_Change(class wxGridEvent& Event);
};
/*****************************************************************************/
/* __SHMEM_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
