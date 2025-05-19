/******************************************************************************
Filename    : memory_panel.hpp
Author      : lbc
Date        : 09/05/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Extra memory information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __EXTMEM_PANEL_TYPE__
#define __EXTMEM_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __EXTMEM_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __EXTMEM_PANEL_DEF__
#define __EXTMEM_PANEL_DEF__
/*****************************************************************************/
/* Memory type - Code, Data, Device */
#define MEM_CODE            0
#define MEM_DATA            1
#define MEM_DEVICE          2

/* Memory access permissions */
#define MEM_READ            POW2(0)
#define MEM_WRITE           POW2(1)
#define MEM_EXECUTE         POW2(2)
#define MEM_BUFFER          POW2(3)
#define MEM_CACHE           POW2(4)
#define MEM_STATIC          POW2(5)

/* Memory placement */
#define MEM_AUTO            ((ptr_t)(-1LL))
/*****************************************************************************/
/* __EXTMEM_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __EXTMEM_PANEL_CLASS__
#define __EXTMEM_PANEL_CLASS__
/*****************************************************************************/
class Extmem_Panel:public wxPanel
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

    class wxArrayString Type_Option;

    /* void */ Extmem_Panel(class wxWindow*parent);
    /* void */ ~Extmem_Panel(void);

    ret_t Check(void);

    void Load(const std::vector<std::unique_ptr<class Mem_Info>>&Memory);
    void Save(std::vector<std::unique_ptr<class Mem_Info>>&Memory);
    void On_Add(class wxCommandEvent& Event);
    void On_Remove(class wxCommandEvent& Event);
    void On_Move_Up(class wxCommandEvent& Event);
    void On_Move_Down(class wxCommandEvent& Event);
    void On_Grid(class wxGridRangeSelectEvent& Event);
    void On_Change(class wxGridEvent& Event);
    void Add_Func(void);
};
/*****************************************************************************/
/* __EXTMEM_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
