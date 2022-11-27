/******************************************************************************
Filename    : vect_info.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : The configuration vector info class. This class is responsible for
                 keeping vectors of particular chips.
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
#include "Vect_Info/vect_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Vect_Info/vect_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Vect_Info::Vect_Info ****************************************
Description : Parse the vector section of a particular chip.
Input       : xml_node_t* Root - The option section's XML node.
              class Process* Owner - The owner process of this kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Vect_Info::Vect_Info(xml_node_t* Root, class Process* Owner):
Kobj(Owner)
{
    std::string Temp;

    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Main::Idtfr_Check(this->Name,"Name","DXXXX","DXXXX");

        /* Number */
        this->Number=Main::XML_Get_Number(Root,"Number","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Vector: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Vector: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Vect_Info::Vect_Info *****************************************/

/* Begin Function:Vect_Info::Check ********************************************
Description : Check whether the chip description vector list makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Vect_Info::Check(void)
{
    try
    {
        /* Do nothing */
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Vector: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Vector: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Vect_Info::Check *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
