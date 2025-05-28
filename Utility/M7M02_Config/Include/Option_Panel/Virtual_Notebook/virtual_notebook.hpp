/******************************************************************************
Filename    : virtual_notebook.hpp
Author      : lbc
Date        : 25/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Virtual notebook class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __VIRTUAL_NOTEBOOK_TYPE__
#define __VIRTUAL_NOTEBOOK_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __VIRTUAL_NOTEBOOK_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __VIRTUAL_NOTEBOOK_DEF__
#define __VIRTUAL_NOTEBOOK_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __VIRTUAL_NOTEBOOK_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __VIRTUAL_NOTEBOOK_CLASS__
#define __VIRTUAL_NOTEBOOK_CLASS__
/*****************************************************************************/
class Virtual_Notebook:public wxNotebook
{
public:
    class Virtual_Panel* Basic;
    class Memory_Panel* Memory;
    class Shmem_Panel* Shmem;
    class Send_Panel* Send;
    class Kfunc_Panel* Kfunc;

    /* void */ Virtual_Notebook(class wxWindow* Parent);
    /* void */ ~Virtual_Notebook(void);

    void Load(const std::string& Virtual_Name);
    void Save(void);
    ret_t Check(void);
    ret_t Check(ptr_t Page);
    void On_Config(class wxBookCtrlEvent& Event);
};
/*****************************************************************************/
/* __VIRTUAL_NOTEBOOK_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
