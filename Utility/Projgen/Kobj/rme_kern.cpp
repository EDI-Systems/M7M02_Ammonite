/******************************************************************************
Filename    : rme_kern.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The kernel function class.
******************************************************************************/

/* Includes ******************************************************************/
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "Main/rme_mcu.hpp"

#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_kern.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_kern.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace rme_mcu
{
/* Begin Function:Kern::Kern **************************************************
Description : Constructor for Kern class.
Input       : xml_node_t* Node - The node containing the kernel function.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kern::Kern(xml_node_t* Node)
{
    xml_node_t* Temp;

    try
    {
        /* Name */
        if((XML_Child(Node,"Name",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P1300: Name section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("P1301: Name section is empty.");
        this->Name=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        /* Start */
        if((XML_Child(Node,"Start",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P1302: Starting kernel function number section is missing.");
        if(XML_Get_Hex(Temp,&(this->Start))<0)
            throw std::invalid_argument("P1303: Starting kernel function number is not a valid hex integer.");

        /* End */
        if((XML_Child(Node,"End",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P1304: Ending kernel function number section is missing.");
        if(XML_Get_Hex(Temp,&(this->End))<0)
            throw std::invalid_argument("P1305: Ending kernel function number is not a valid hex integer.");

        if(End<Start)
            throw std::invalid_argument("P1306: Ending kernel function number is smaller than the starting kernel function number.");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!=nullptr)
            throw std::runtime_error(std::string("Kernel function: ")+*(this->Name)+"\n"+Exc.what());
        else
            throw std::runtime_error(std::string("Kernel function: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Kern::Kern ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
