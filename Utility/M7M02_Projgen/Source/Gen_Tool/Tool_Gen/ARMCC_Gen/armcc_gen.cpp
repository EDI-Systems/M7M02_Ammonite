/******************************************************************************
Filename    : armcc_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for ARMCC, both v5 and v6; generates ARMCC linker scripts.
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
#include "Gen_Tool/Tool_Gen/ARMCC_Gen/armcc_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Gen_Tool/Tool_Gen/tool_gen.hpp"
#include "Gen_Tool/Tool_Gen/ARMCC_Gen/armcc_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:ARMCC_Gen::ARMCC_Gen ****************************************
Description : Generator for the ARMv7-M platform.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ ARMCC_Gen::ARMCC_Gen(void):
Tool_Gen("ARMCC")
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ARMCC generator:\n")+Exc.what());
    }
}
/* End Function:ARMCC_Gen::ARMCC_Gen *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/