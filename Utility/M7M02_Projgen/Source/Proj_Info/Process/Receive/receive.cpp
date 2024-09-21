/******************************************************************************
Filename    : receive.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The receive endpoint class.
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
#include "Proj_Info/Process/Receive/receive.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Receive::Receive **************************************************
Description : Constructor for receive class.
Input       : xml_node_t* Root - The node containing the receive endpoint.
              class Process* Owner - The owner process of this kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Receive::Receive(xml_node_t* Root, class Process* Owner):
Kobj(Owner)
{
    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Name_Gen(this);
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Receive: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Receive: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Receive::Receive *********************************************/

/* Function:Receive::Report ***************************************************
Description : Report kernel object details.
Input       : None.
Output      : None.
Return      : std::string - The report string.
******************************************************************************/
std::string Receive::Report(void)
{
    std::string Temp;

    Temp="Receive "+this->Kobj::Report();

    return Temp;
}
/* End Function:Receive::Report **********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
