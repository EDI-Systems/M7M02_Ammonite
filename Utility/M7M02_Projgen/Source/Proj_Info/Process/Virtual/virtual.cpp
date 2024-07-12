/******************************************************************************
Filename    : virtual.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The virtual machine class.
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
#include "algorithm"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Virtual/virtual.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
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
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Virtual::Virtual **************************************************
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

        /* Virtual_Header_Output - relative to project */
        this->Virtual_Header_Output=Main::XML_Get_String(Root,"Virtual_Header_Output","DXXXX","DXXXX");
        this->Virtual_Header_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Virtual_Header_Output);
        this->Virtual_Header_Overwrite=Main::XML_Get_Yesno(Root,"Virtual_Header_Overwrite","DXXXX","DXXXX");
        /* Virtual_Source_Output - relative to project */
        this->Virtual_Source_Output=Main::XML_Get_String(Root,"Virtual_Source_Output","DXXXX","DXXXX");
        this->Virtual_Source_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Virtual_Source_Output);
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
        this->Vector_Num=Main::XML_Get_Number(Root,"Vector_Num","DXXXX","DXXXX");

        /* Handler & user thread for VM - VM does not have other threads */
        this->Thread.push_back(std::make_unique<class Thread>("Vct", 0, this->Vector_Stack_Size, 0, VIRT_VECTOR_PRIO, this));
        this->Thread.push_back(std::make_unique<class Thread>("Usr", 1, this->User_Stack_Size, 0, VIRT_USER_PRIO, this));
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

/* Function:Virtual::State_Alloc **********************************************
Description : Allocate the memory for state space. State space is like
              struct RVM_Param
              {
                  rvm_ptr_t Number;
                  rvm_ptr_t Param[4];
              };
              struct RVM_Vctf
              {
                  rvm_ptr_t Tim;
                  rvm_ptr_t Ctx;
                  rvm_u8_t Vct[16];
              };
              struct RVM_State
              {
                  rvm_ptr_t Vct_Act;
                  struct RVM_Param Vct;
                  struct RVM_Param Usr;
                  struct RVM_Vctf Flag;
              };
              Each byte corresponds to a single vector. The reason why we design
              this as a byte array is that some architectures lack atomics, so
              we cannot flip a bit atomically with Fetch_And. Even on architectures
              that have such atomics, this design makes porting easier.
Input       : ptr_t Source - The source number.
              ptr_t Wordlength - The processor wordlength.
              ptr_t Kom_Order - The kernel memory alignment order of 2.
Output      : None.
Return      : ptr_t - The vector flag sector size.
******************************************************************************/
ptr_t Virtual::State_Alloc(ptr_t Source, ptr_t Wordlength, ptr_t Kom_Order)
{
    ptr_t Raw;

    Raw=(1+5+5+2)*(Wordlength/8)+Source;

    return ROUND_UP_POW2(Raw, Kom_Order);
}
/* End Function:Virtual::State_Alloc *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
