/******************************************************************************
Filename    : compatible.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : The compatibility information class. For each compartment, make
              sure the generation is supported by the buildsystem/toolchain/guest
              of choice. We only support makefile with open-source toolchains,
              such as GCC and LLVM, and one most popular proprietary toolchain
              for any platform. Support list is usually: 1. One proprietary
              toolchain (i.e. Keil), 2. LLVM/GCC+Makefile. IAR is out of scope.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "xml.h"
}

#include "set"
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Plat_Info/compatible/compatible.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Plat_Info/compatible/compatible.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Compatible::Compatible **************************************
Description : Constructor for compatible combination class.
Input       : xml_node_t* Root - The node containing the compatibility information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Compatible::Compatible(xml_node_t* Root)
{
    std::vector<std::string> List;

    try
    {
        Main::XML_Get_CSV(Root,"",List,"DXXXX","DXXXX");

        if(List.size()!=3)
            Main::Error("XXXXX: Compatibility entry malformed.");

        this->Buildsystem=List[0];
        this->Toolchain=List[1];
        this->Guest=List[2];
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Compatible:\n")+Exc.what());
    }
}
/* End Function:Compatible::Compatible ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
