/******************************************************************************
Filename    : native_notebook.hpp
Author      : lbc
Date        : 25/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : Native process notebook class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __NATIVE_NOTEBOOK_TYPE__
#define __NATIVE_NOTEBOOK_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_NOTEBOOK_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __NATIVE_NOTEBOOK_DEF__
#define __NATIVE_NOTEBOOK_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_NOTEBOOK_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __NATIVE_NOTEBOOK_CLASS__
#define __NATIVE_NOTEBOOK_CLASS__
/*****************************************************************************/
class Native_Notebook:public wxNotebook
{
public:
    class Native_Panel* Basic;
    class Memory_Panel* Memory;
    class Shmem_Panel* Shmem;
    class Thread_Panel* Thread;
    class Invocation_Panel* Invocation;
    class Port_Panel* Port;
    class Receive_Panel* Receive;
    class Send_Panel* Send;
    class Vector_Panel* Vector;
    class Kfunc_Panel* Kfunc;

    /* void */ Native_Notebook(class wxWindow* Parent);
    /* void */ ~Native_Notebook(void);

    void Load(const std::string& Native_Name);
    ret_t Check(void);
    ret_t Check(ptr_t Page);
    void Save(void);

    void On_Config(class wxBookCtrlEvent& Event);
};
/*****************************************************************************/
/* __NATIVE_NOTEBOOK_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
