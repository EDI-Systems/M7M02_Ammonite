/******************************************************************************
Filename    : send.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The send endpoint class.
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
#include "Proj_Info/Process/Send/send.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Send::Send **************************************************
Description : Constructor for Send class.
Input       : xml_node_t* Root - The node containing the receive endpoint.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Send::Send(xml_node_t* Root)
{
    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        /* Process */
        this->Process=Main::XML_Get_String(Root,"Process","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Send: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Send: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Send::Send ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
