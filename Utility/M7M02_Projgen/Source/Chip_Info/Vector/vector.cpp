/******************************************************************************
Filename    : vector.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : The configuration vector info class. This class is responsible for
                 keeping vectors of particular chips.
******************************************************************************/

/* Includes ******************************************************************/
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
#include "Chip_Info/Vector/vector.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Chip_Info/Vector/vector.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Vector::Vector **********************************************
Description : Parse the option section of a particular chip.
Input       : xml_node_t* Node - The option section's XML node.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Vector::Vector(xml_node_t* Node)
{
    std::string Temp;

    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Node,"Name","DXXXX","DXXXX");
        Main::Idtfr_Check(this->Name,"Name","DXXXX","DXXXX");

        /* Number */
        Temp=Main::XML_Get_String(Node,"Number","DXXXX","DXXXX");

    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Vector: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Vector: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Vector::Vector ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
