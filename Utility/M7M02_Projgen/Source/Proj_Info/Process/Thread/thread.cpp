/******************************************************************************
Filename    : thread.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The thread class.
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
#include "Proj_Info/Process/Thread/thread.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Thread::Thread ****************************************************
Description : Constructor for thread class.
Input       : xml_node_t* Root - The node containing the thread.
              class Process* Owner - The owner process of this kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thread::Thread(xml_node_t* Root, class Process* Owner):
Kobj(Owner)
{
    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Name_Gen(this);
        /* Definitely not hypervisor managed when created from XML */
        this->Is_Hyp=0;
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

/* Function:Thread::Thread ****************************************************
Description : Constructor for thread class.
Input       : const std::string& Name - The name of the thread.
              ptr_t Is_Hyp - Whether this thread is hypervisor managed.
              ptr_t Stack_Size - Thread stack size.
              ptr_t Parameter - Thread start parameter.
              ptr_t Priority - Thread priority.
              class Process* Owner - The owner process of this kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thread::Thread(const std::string& Name,
		                  ptr_t Is_Hyp, ptr_t Stack_Size, ptr_t Parameter, ptr_t Priority,
						  class Process* Owner):
Kobj(Owner)
{
    this->Name=Name;
    Name_Gen(this);
    this->Is_Hyp=Is_Hyp;
    this->Stack_Size=Stack_Size;
    this->Parameter=Parameter;
    this->Priority=Priority;
}
/* End Function:Thread::Thread ***********************************************/

/* Function:Thread::Report ****************************************************
Description : Report kernel object details.
Input       : None.
Output      : None.
Return      : std::string - The report string.
******************************************************************************/
std::string Thread::Report(void)
{
    std::string Temp;

    if(this->Is_Hyp!=0)
        Temp="Hypervisor ";
    else
        Temp="Thread ";

    Temp+=this->Kobj::Report()+", "+
          "stack 0x"+Main::Hex(this->Stack_Base)+"(0x"+Main::Hex(this->Stack_Size)+")"+
          " param 0x"+Main::Hex(this->Parameter)+
          " priority "+std::to_string(this->Priority);

    return Temp;
}
/* End Function:Thread::Report ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
