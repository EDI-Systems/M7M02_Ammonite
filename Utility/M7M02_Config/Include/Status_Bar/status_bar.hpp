/******************************************************************************
Filename    : status_bar.hpp
Author      : pry
Date        : 12/08/2021
Licence     : Proprietary; confidential.
Description : Status bar class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __STATUS_BAR_TYPE__
#define __STATUS_BAR_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __STATUS_BAR_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __STATUS_BAR_DEF__
#define __STATUS_BAR_DEF__
/*****************************************************************************/
#define STATUS_BAR_SIZE                     (2)

/* Status bar second box size */
#define STATUS_FIRST_WIDTH                  (-1)
#define STATUS_SECOND_WIDTH                 (100)
/*****************************************************************************/
/* __STATUS_BAR_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __STATUS_BAR_CLASS__
#define __STATUS_BAR_CLASS__
/*****************************************************************************/
class Status_Bar:public wxStatusBar
{
public:
    class wxWindow* Parent;
    class wxStaticText* Memory;
    class wxStaticText* Version;

    class wxTimer* Timer;

    Status_Bar(class wxWindow* Parent);
    ~Status_Bar(void);

    void State_Set(ptr_t Type);

    /* Event handlers */
    void On_Size(class wxSizeEvent& Event);
    void On_Timer(class wxTimerEvent& event);
};
/*****************************************************************************/
/* __STATUS_BAR_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
