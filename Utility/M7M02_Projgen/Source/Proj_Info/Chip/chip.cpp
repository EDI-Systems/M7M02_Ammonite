/******************************************************************************
Filename    : chip.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The chip class.
******************************************************************************/

/* Include *******************************************************************/
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

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Info/Chip/chip.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Chip/chip.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Chip::Chip ********************************************************
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
        this->Platform_Lower=this->Platform;
        Main::Lower(this->Platform_Lower);
        /* Class */
        this->Class=Main::XML_Get_String(Root,"Class","DXXXX","DXXXX");
        this->Class_Lower=this->Class;
        Main::Lower(this->Class_Lower);
        /* Fullname */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        this->Name_Lower=Name;
        Main::Lower(this->Name_Lower);
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

/* Function:Chip::Check *******************************************************
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
