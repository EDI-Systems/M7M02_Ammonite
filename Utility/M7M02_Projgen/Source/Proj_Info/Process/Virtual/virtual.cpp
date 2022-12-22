/******************************************************************************
Filename    : virtual.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The virtual machine class.
******************************************************************************/

/* Includes ******************************************************************/
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Captbl/captbl.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Virtual::Virtual ********************************************
Description : Constructor for virtual machine class. The virtual machine is a special
              type of process, in which we populate the kernel objects autonomously.
Input       : xml_node_t* Root - The node containing the virtual machine description.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual::Virtual(xml_node_t* Root):
Process(Root,PROCESS_VIRTUAL)
{
    try
    {
        /* Guest_Type */
        this->Guest_Type=Main::XML_Get_String(Root,"Guest_Type","DXXXX","DXXXX");
        /* Guest_Root */
        this->Guest_Root=Main::XML_Get_String(Root,"Guest_Root","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Guest_Root);
        /* Virtual_Header_Output */
        this->Virtual_Header_Output=Main::XML_Get_String(Root,"Virtual_Header_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Virtual_Header_Output);
        this->Virtual_Header_Overwrite=Main::XML_Get_Yesno(Root,"Virtual_Header_Overwrite","DXXXX","DXXXX");
        /* Virtual_Source_Output */
        this->Virtual_Source_Output=Main::XML_Get_String(Root,"Virtual_Source_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Virtual_Source_Output);
        this->Virtual_Source_Overwrite=Main::XML_Get_Yesno(Root,"Virtual_Source_Overwrite","DXXXX","DXXXX");

        /* Stack_Size */
        this->Vector_Stack_Size=Main::XML_Get_Number(Root,"Vector_Stack_Size","DXXXX","DXXXX");
        this->User_Stack_Size=Main::XML_Get_Number(Root,"User_Stack_Size","DXXXX","DXXXX");
        /* Priority */
        this->Priority=Main::XML_Get_Number(Root,"Priority","DXXXX","DXXXX");
        /* Timeslice */
        this->Timeslice=Main::XML_Get_Number(Root,"Timeslice","DXXXX","DXXXX");
        /* Period */
        this->Period=Main::XML_Get_Number(Root,"Period","DXXXX","DXXXX");
        /* Watchdog */
        this->Watchdog=Main::XML_Get_Number(Root,"Watchdog","DXXXX","DXXXX");
        /* Number of virtual interrupts */
        this->Vect_Num=Main::XML_Get_Number(Root,"Vect_Num","DXXXX","DXXXX");

        /* Handler & user thread for VM - VM does not have other threads */
        this->Thread.push_back(std::make_unique<class Thread>("Vect", this->Vector_Stack_Size, 0, VIRT_VECTOR_PRIO, this));
        this->Thread.push_back(std::make_unique<class Thread>("User", this->User_Stack_Size, 0, VIRT_USER_PRIO, this));
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Virtual: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Virtual: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Virtual::Virtual *********************************************/

/* Begin Function:Virtual::State_Alloc ****************************************
Description : Allocate the memory for state space. State space is like
              struct RVM_Param
              {
                  rvm_ptr_t Number;
                  rvm_ptr_t Param[4];
              };
              struct RVM_Vctf
              {
                  rvm_ptr_t Timer;
                  rvm_ptr_t Ctxsw;
                  rvm_ptr_t Vect[1];
              };
              struct RVM_State
              {
                  rvm_ptr_t Vect_Act;
                  struct RVM_Param Vect;
                  struct RVM_Param User;
                  struct RVM_Vctf Flag;
              };
              Each bit corresponds to a single vector. The whole struct will repeat twice.
Input       : ptr_t Source - The source number.
              ptr_t Wordlength - The processor wordlength.
              ptr_t Kom_Order - The kernel memory alignment order of 2.
Output      : None.
Return      : ptr_t - The vector flag sector size.
******************************************************************************/
ptr_t Virtual::State_Alloc(ptr_t Source, ptr_t Wordlength, ptr_t Kom_Order)
{
    ptr_t Raw;

    Raw=(1+5+5+2+ROUND_DIV(Source, Wordlength))*(Wordlength/8);

    return ROUND_UP_POW2(Raw, Kom_Order);
}
/* End Function:Virtual::State_Alloc *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
