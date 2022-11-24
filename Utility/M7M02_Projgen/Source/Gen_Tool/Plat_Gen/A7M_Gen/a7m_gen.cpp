/******************************************************************************
Filename    : a7m_gen.cpp
Author      : pry
Date        : 12/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : This toolset is for ARMv7-M. Specifically, this suits Cortex-M0+,
              Cortex-M3, Cortex-M4, Cortex-M7. For ARMv8-M, please use A8M port.
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
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/A7M_Gen/a7m_gen.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Gen_Tool/Plat_Gen/plat_gen.hpp"
#include "Gen_Tool/Plat_Gen/A7M_Gen/a7m_gen.hpp"
#undef __HDR_STRUCTS__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:A7M_Gen::A7M_Gen ********************************************
Description : Generator for the ARMv7-M platform.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ A7M_Gen::A7M_Gen(void):
Plat_Gen("A7M")
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ARMv7-M (A7M) generator:\n")+Exc.what());
    }
}
/* End Function:A7M_Gen::A7M_Gen *********************************************/

/* Begin Function:A7M_Gen::Mem_Align ******************************************
Description : Memory aligner for the ARMv7-M platform.
Input       : class Mem_Info* Mem - The memory block to calculate alignment.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_Gen::Mem_Align(class Mem_Info* Mem)
{
    try
    {

    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("ARMv7-M (A7M) generator:\n")+Exc.what());
    }
}
/* End Function:A7M_Gen::Mem_Align *******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
