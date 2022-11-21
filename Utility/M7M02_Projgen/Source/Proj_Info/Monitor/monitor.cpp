/******************************************************************************
Filename    : monitor.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The virtual machine monitor class.
******************************************************************************/

/* Includes ******************************************************************/
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
#include "Proj_Info/Monitor/monitor.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Monitor/monitor.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace rme_mcu
{
/* Begin Function:Monitor::Monitor ********************************************
Description : Constructor for RVM class.
Input       : xml_node_t* Node - The node containing the whole project.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Monitor::Monitor(xml_node_t* Node)
{
    xml_node_t* Temp;
    std::unique_ptr<std::string> Str;

    try
    {
        /* Code size */
        if((XML_Child(Node,"Code_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0200: Code size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Code_Size))<0)
            throw std::invalid_argument("P0201: Code size is not a valid hex integer.");

        /* Data size */
        if((XML_Child(Node,"Data_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0202: Data size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Data_Size))<0)
            throw std::invalid_argument("P0203: Data size is not a valid hex integer.");

        /* Stack size */
        if((XML_Child(Node,"Stack_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0204: Stack size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Stack_Size))<0)
            throw std::invalid_argument("P0205: Stack size is not a valid hex integer.");

        /* Extra Captbl */
        if((XML_Child(Node,"Extra_Captbl",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0206: Extra capability table size section is missing.");
        if(XML_Get_Uint(Temp,&(this->Extra_Captbl))<0)
            throw std::invalid_argument("P0207: Extra capability table size is not a valid unsigned integer.");

        /* Compiler */
        if((XML_Child(Node,"Compiler",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0208: Compiler section is missing.");
        this->Comp=std::make_unique<class Comp>(Temp);

        /* Number of virtual machine priorities */
        if((XML_Child(Node,"Virt_Prios",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0209: Virtual machine priorities section is missing.");
        if(XML_Get_Uint(Temp,&(this->Virt_Prios))<0)
            throw std::invalid_argument("P0210: Virtual machine priorities is not a valid unsigned integer.");

        /* Number of virtual machine events from native processes */
        if((XML_Child(Node,"Virt_Evts",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0211: Virtual machine event number section is missing.");
        if(XML_Get_Uint(Temp,&(this->Virt_Evts))<0)
            throw std::invalid_argument("P0212: Virtual machine event number is not a valid unsigned integer.");

        /* Number of virtual machine events from native processes */
        if((XML_Child(Node,"Virt_Maps",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0213: Virtual machine mapping total number section is missing.");
        if(XML_Get_Uint(Temp,&(this->Virt_Maps))<0)
            throw std::invalid_argument("P0214: Virtual machine mapping total number is not a valid unsigned integer.");

        this->Map=std::make_unique<class RVM_Memmap>();
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("RVM:\n")+Exc.what());
    }
}
/* End Function:Monitor::Monitor *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
