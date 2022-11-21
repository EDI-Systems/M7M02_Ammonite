/******************************************************************************
Filename    : chip.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The chip class.
******************************************************************************/

/* Includes ******************************************************************/
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/Chip/chip.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Chip/chip.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Chip::Chip **************************************************
Description : Constructor for chip class.
Input       : xml_node_t* Root - The node containing the chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip::Chip(xml_node_t* Root)
{
    xml_node_t* Temp;
    xml_node_t* Trunk;
    xml_node_t* Mem_Type;
    std::unique_ptr<std::string> Str;

    try
    {
        /* Platform */
        this->Platform=Main::XML_Get_String(Root,"Platform","DXXXX","DXXXX");
        /* Class */
        this->Class=Main::XML_Get_String(Root,"Class","DXXXX","DXXXX");
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        /* Config */
        Main::XML_Get_KVP(Root,"Config",this->Config,"DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Chip: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Chip: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Chip::Chip ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
