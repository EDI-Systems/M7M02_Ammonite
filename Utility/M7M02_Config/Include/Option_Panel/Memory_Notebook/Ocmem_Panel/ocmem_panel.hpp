/******************************************************************************
Filename    : ocmem_panel.hpp
Author      : lbc
Date        : 16/05/2025
Licence     : The Unlicense; see LICENSE for details.
Description : On-chip memory information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __OCMEM_PANEL_TYPE__
#define __OCMEM_PANEL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __OCMEM_PANEL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __OCMEM_PANEL_DEF__
#define __OCMEM_PANEL_DEF__
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
/* __OCMEM_PANEL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __OCMEM_PANEL_CLASS__
#define __OCMEM_PANEL_CLASS__
/*****************************************************************************/
class Ocmem_Panel:public wxPanel
{
public:
    class wxBoxSizer* Border_Sizer;
    class wxBoxSizer* Main_Sizer;

    class wxGrid* Grid;

    /* void */ Ocmem_Panel(class wxWindow* Parent);
    /* void */ ~Ocmem_Panel(void);

    void Load(const std::vector<std::unique_ptr<class Mem_Info>>& Chip_Mem);
};
/*****************************************************************************/
/* __OCMEM_PANEL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
