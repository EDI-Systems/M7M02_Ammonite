/******************************************************************************
Filename    : option_panel.cpp
Author      : lbc
Date        : 24/04/2025
Licence     : The Unlicense; see LICENSE for details.
Description : option panel.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Option_Panel/option_panel.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Option_Panel/option_panel.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Option_Panel::Option_Panel ****************************************
Description : Constructor for option panel.
Input       : class wxWindow* parent - The parent window.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Option_Panel::Option_Panel(class wxWindow* parent):
wxPanel(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize)
{
    try
    {
        this->Main_Sizer=new class wxBoxSizer(wxVERTICAL);

        this->SetSizer(this->Main_Sizer);
        this->Layout();
    }
    catch(std::exception& Exc)
    {
        ASSERT(0,Exc.what());
    }
}
/* End Function:Option_Panel::Option_Panel ***********************************/

/* Function:Option_Panel::~Option_Panel ***************************************
Description : Destructor for option panel.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Option_Panel::~Option_Panel(void)
{

}
/* End Function:Option_Panel::~Option_Panel **********************************/

}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
