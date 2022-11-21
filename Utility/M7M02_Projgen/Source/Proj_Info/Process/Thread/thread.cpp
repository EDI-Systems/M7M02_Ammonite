/******************************************************************************
Filename    : thread.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The thread class.
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
#include "Proj_Info/Process/Thread/thread.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Thread::Thread **********************************************
Description : Constructor for thread class.
Input       : xml_node_t* Root - The node containing the thread.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thread::Thread(xml_node_t* Root)
{
    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        /* Stack Size */
        this->Stack_Size=Main::XML_Get_Number(Root,"Stack_Size","DXXXX","DXXXX");
        /* Parameter */
        this->Parameter=Main::XML_Get_Number(Root,"Parameter","DXXXX","DXXXX");
        /* Priority */
        this->Priority=Main::XML_Get_Number(Root,"Priority","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Thread: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Thread: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Thread::Thread ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
