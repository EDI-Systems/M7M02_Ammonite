/******************************************************************************
Filename    : native.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The native process class.
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
#include "Proj_Info/Process/Native/native.hpp"
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
#include "Proj_Info/Process/Native/native.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Native::Native ***************************************************
Description : Constructor for native process class.
Input       : xml_node_t* Root - The node containing the virtual machine description.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native::Native(xml_node_t* Root):
Process(Root,PROCESS_NATIVE)
{
    try
    {
        /* Entry_Source_Output - relative to project */
        this->Entry_Source_Output=Main::XML_Get_String(Root, "Entry_Source_Output", "DXXXX", "DXXXX");
        this->Entry_Source_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Entry_Source_Output);
        this->Entry_Source_Overwrite=Main::XML_Get_Yesno(Root, "Entry_Source_Overwrite", "DXXXX", "DXXXX");
        /* Thread */
        Trunk_Parse_Param<class Thread, class Thread>(Root,"Thread",this->Thread,this,"DXXXX","DXXXX");
        /* Invocation */
        Trunk_Parse_Param<class Invocation, class Invocation>(Root,"Invocation",this->Invocation,this,"DXXXX","DXXXX");
        /* Port */
        Trunk_Parse_Param<class Port, class Port>(Root,"Port",this->Port,this,"DXXXX","DXXXX");
        /* Receive */
        Trunk_Parse_Param<class Receive, class Receive>(Root,"Receive",this->Receive,this,"DXXXX","DXXXX");
        /* Vector */
        Trunk_Parse_Param<class Vect_Info, class Vect_Info>(Root,"Vector",this->Vector,this,"DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Native: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Native: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Native::Native ***********************************************/

/* Function:Native::Report ****************************************************
Description : Report kernel object details.
Input       : None.
Output      : None.
Return      : std::string - The report string.
******************************************************************************/
std::string Native::Report(void)
{
    return "Native "+this->Kobj::Report();
}
/* End Function:Native::Report ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
