/******************************************************************************
Filename    : keil_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for Keil uVision build system.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "xml.h"
}

#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Keil_Gen/keil_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Gen_Tool/Build_Gen/build_gen.hpp"
#include "Gen_Tool/Build_Gen/Keil_Gen/keil_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Keil_Gen::Keil_Gen ******************************************
Description : Generator for the Keil uVision.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Keil_Gen::Keil_Gen(void):
Build_Gen("Keil")
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Keil uVision generator:\n")+Exc.what());
    }
}
/* End Function:Keil_Gen::Keil_Gen *******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/