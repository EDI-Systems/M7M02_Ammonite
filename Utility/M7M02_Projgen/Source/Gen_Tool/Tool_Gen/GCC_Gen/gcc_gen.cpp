/******************************************************************************
Filename    : gcc_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for GCC; generates GCC linker scripts.
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
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/GCC_Gen/gcc_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:GCC_Gen::GCC_Gen ********************************************
Description : Generator for the ARMv7-M platform.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ GCC_Gen::GCC_Gen(void):
Tool_Gen("GCC")
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("GCC generator:\n")+Exc.what());
    }
}
/* End Function:GCC_Gen::GCC_Gen *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
