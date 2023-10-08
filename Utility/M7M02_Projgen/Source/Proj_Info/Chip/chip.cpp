/******************************************************************************
Filename    : chip.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The chip class.
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
    try
    {
        /* Platform */
        this->Platform=Main::XML_Get_String(Root,"Platform","DXXXX","DXXXX");
        /* Class */
        this->Class=Main::XML_Get_String(Root,"Class","DXXXX","DXXXX");
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        /* Coprocessor */
        Main::XML_Get_CSV(Root,"Coprocessor",this->Coprocessor,"DXXXX","DXXXX");
        None_Filter(this->Coprocessor);
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

/* Begin Function:Chip::Check *************************************************
Description : Check whether the project chip section makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Chip::Check(void)
{
    try
    {
        /* Check coprocessor list */
        Duplicate_Check<std::string>(this->Coprocessor, this->Coprocessor_Set, "PXXXX", "name", "Coprocessor");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Chip: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Chip: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Chip::Check **************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
