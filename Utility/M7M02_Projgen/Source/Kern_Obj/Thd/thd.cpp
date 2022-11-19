/******************************************************************************
Filename    : rme_thd.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The thread class.
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
#include "Kobj/rme_thd.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_thd.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Thd::Thd ****************************************************
Description : Constructor for Thd class.
Input       : xml_node_t* Node - The node containing the thread.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Thd::Thd(xml_node_t* Node)
{
    xml_node_t* Temp;

    try
    {
        /* Name */
        if((XML_Child(Node,"Name",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0700: Name section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("P0701: Name section is empty.");
        this->Name=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        /* Stack Size */
        if((XML_Child(Node,"Stack_Size",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0702: Stack size section is missing.");
        if(XML_Get_Hex(Temp,&(this->Stack_Size))<0)
            throw std::invalid_argument("P0703: Stack size is not a valid hex integer.");
        if(this->Stack_Size==0)
            throw std::invalid_argument("P0704: Stack size cannot be zero.");

        /* Parameter */
        if((XML_Child(Node,"Parameter",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0705: Parameter section is missing.");
        if(XML_Get_Hex(Temp,&(this->Param))<0)
            throw std::invalid_argument("P0706: Parameter is not a valid hex integer.");

        /* Priority */
        if((XML_Child(Node,"Priority",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("P0707: Priority section is missing.");
        if(XML_Get_Uint(Temp,&(this->Prio))<0)
            throw std::invalid_argument("P0708: Priority is not a valid unsigned integer.");

        this->Map=std::make_unique<class Thd_Memmap>();
    }
    catch(std::exception& Exc)
    {
        if(this->Name!=nullptr)
            throw std::runtime_error(std::string("Thread: ")+*(this->Name)+"\n"+Exc.what());
        else
            throw std::runtime_error(std::string("Thread: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Thd::Thd *****************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
