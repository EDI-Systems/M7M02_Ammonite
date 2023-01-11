/******************************************************************************
Filename    : status_bar.hpp
Author      : pry
Date        : 12/08/2021
Licence     : Proprietary; confidential.
Description : Status bar class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __STATUS_BAR_HPP_TYPES__
#define __STATUS_BAR_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __STATUS_BAR_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __STATUS_BAR_HPP_DEFS__
#define __STATUS_BAR_HPP_DEFS__
/*****************************************************************************/
#define STATUS_BAR_SIZE                     (2)

/* Status bar second box size */
#define STATUS_FIRST_WIDTH                  (-1)
#define STATUS_SECOND_WIDTH                 (100)
/*****************************************************************************/
/* __STATUS_BAR_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __STATUS_BAR_HPP_CLASSES__
#define __STATUS_BAR_HPP_CLASSES__
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
/* __STATUS_BAR_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
