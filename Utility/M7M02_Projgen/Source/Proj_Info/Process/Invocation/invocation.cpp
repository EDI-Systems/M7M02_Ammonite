/******************************************************************************
Filename    : invocation.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The invocation class.
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
#include "Proj_Info/Process/Invocation/invocation.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Invocation::Invocation ********************************************
Description : Constructor for invocation class.
Input       : xml_node_t* Root - The node containing the invocation.
              class Process* Owner - The owner process of this kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Invocation::Invocation(xml_node_t* Root, class Process* Owner):
Kobj(Owner)
{
    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Name_Gen(this);
        /* Stack Size */
        this->Stack_Size=Main::XML_Get_Number(Root,"Stack_Size","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Invocation: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Invocation: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Invocation::Invocation ***************************************/

/* Function:Invocation::Report ************************************************
Description : Report kernel object details.
Input       : None.
Output      : None.
Return      : std::string - The report string.
******************************************************************************/
std::string Invocation::Report(void)
{
    return "Invocation "+this->Kobj::Report()+", "+
           "stack 0x"+Main::Hex(this->Stack_Base)+"(0x"+Main::Hex(this->Stack_Size)+")";
}
/* End Function:Invocation::Report *******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
