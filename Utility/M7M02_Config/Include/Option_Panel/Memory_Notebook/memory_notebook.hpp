/******************************************************************************
Filename    : basic_notebook.hpp
Author      : lbc
Date        : 16/05/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Memory notebook class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __MEMORY_NOTEBOOK_TYPE__
#define __MEMORY_NOTEBOOK_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __MEMORY_NOTEBOOK_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __MEMORY_NOTEBOOK_DEF__
#define __MEMORY_NOTEBOOK_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __MEMORY_NOTEBOOK_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __MEMORY_NOTEBOOK_CLASS__
#define __MEMORY_NOTEBOOK_CLASS__
/*****************************************************************************/
class Memory_Notebook:public wxNotebook
{
public:
    class Extmem_Panel*Extmem_Panel;
    class Memory_Panel*Shmem_Panel;
    class Chip_Memory_Panel*Chip_Memory_Panel;

    ret_t Has_Been_Shown;

    /* void */ Memory_Notebook(class wxWindow*parent);
    /* void */ ~Memory_Notebook(void);

    void Load(void);
    void Save(void);
    ret_t Check(void);
    ret_t Check(ptr_t Page);
    void On_Config(class wxBookCtrlEvent& Event);
};
/*****************************************************************************/
/* __MEMORY_NOTEBOOK_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
