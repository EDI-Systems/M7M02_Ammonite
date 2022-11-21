/******************************************************************************
Filename    : kernel.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The kernel configuration class.
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
#include "Proj_Info/Kernel/kernel.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Kernel::Kernel **********************************************
Description : Constructor for RME class.
Input       : xml_node_t* Node - The node containing the whole project.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kernel::Kernel(xml_node_t* Node)
{
    xml_node_t* Temp;
    xml_node_t* Trunk;

    try
    {
        /* Code base */
        if((XML_Child(Node,"Code_Base",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0100: Code base section is missing.");
        if(XML_Get_Hex(Temp,&(this->Code_Base))<0)
            throw std::invalid_argument("P0101: Code base is not a valid hex integer.");

        /* Code size */
        if((XML_Child(Node,"Code_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0102: Code size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Code_Size))<0)
            throw std::invalid_argument("P0103: Code size is not a valid hex integer.");

        /* Data base */
        if((XML_Child(Node,"Data_Base",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0104: Data base section is missing.");
        if(XML_Get_Hex(Temp,&(this->Data_Base))<0)
            throw std::invalid_argument("P0105: Data base is not a valid hex integer.");

        /* Data size */
        if((XML_Child(Node,"Data_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0106: Data size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Data_Size))<0)
            throw std::invalid_argument("P0107: Data size is not a valid hex integer.");

        /* Stack size */
        if((XML_Child(Node,"Stack_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0108: Stack size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Stack_Size))<0)
            throw std::invalid_argument("P0109: Stack size is not a valid hex integer.");

        /* Extra kernel memory */
        if((XML_Child(Node,"Extra_Kmem",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0110: Extra kernel memory section is missing.");
        if(XML_Get_Hex(Temp,&(this->Extra_Kmem))<0)
            throw std::invalid_argument("P0111: Extra kernel memory is not a valid hex integer.");

        /* Kmem_Order */
        if((XML_Child(Node,"Kmem_Order",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0112: Kernel memory order section is missing.");
        if(XML_Get_Uint(Temp,&(this->Kmem_Order))<0)
            throw std::invalid_argument("P0113: Kernel memory order is not a valid unsigned integer.");

        /* Priorities */
        if((XML_Child(Node,"Kern_Prios",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0114: Priority number section is missing.");
        if(XML_Get_Uint(Temp,&(this->Kern_Prios))<0)
            throw std::invalid_argument("P0115: Priority number is not a valid unsigned integer.");

        /* Compiler */
        if((XML_Child(Node,"Compiler",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0116: Compiler option section is missing.");
        this->Comp=std::make_unique<class Comp>(Temp);

        /* Platform */
        if((XML_Child(Node,"Platform",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0117: Platform option section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0118: Platform option section parsing internal error.");
        while(Trunk!=0)
        {
            this->Plat.push_back(std::make_unique<class Raw>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0118: Platform option section parsing internal error.");
        }

        /* Chip */
        if((XML_Child(Node,"Chip",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0119: Chip option section is missing.");
        if(XML_Child(Temp,0,&Trunk)<0)
            throw std::invalid_argument("P0120: Chip option section parsing internal error.");
        while(Trunk!=0)
        {
            this->Chip.push_back(std::make_unique<class Raw>(Trunk));

            if(XML_Child(Temp,"",&Trunk)<0)
                throw std::invalid_argument("P0120: Chip option section parsing internal error.");
        }

        this->Map=std::make_unique<class RME_Memmap>();
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("RME:\n")+Exc.what());
    }
}
/* End Function:Kernel::Kernel ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
