/******************************************************************************
Filename    : chip_info.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : The chip information class. This class is responsible for reading
              chip parameters.
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
#include "Chip_Info/chip_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Chip_Info::Chip_Info ****************************************
Description : Constructor for chip class.
Input       : xml_node_t* Root - The node containing the chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip_Info::Chip_Info(xml_node_t* Root)
{
    xml_node_t* Temp;
    xml_node_t* Trunk;
    xml_node_t* Mem_Type;
    std::unique_ptr<std::string> Str;

    try
    {
        /* Platform */
        this->Platform=Main::XML_Get_String(Root,"Platform","DXXXX","DXXXX");
        /* Version */
        this->Version=Main::XML_Get_String(Root,"Version","DXXXX","DXXXX");
        /* Class */
        this->Class=Main::XML_Get_String(Root,"Class","DXXXX","DXXXX");
        /* Compatible */
        Main::XML_Get_CSV(Root,"Compatible",this->Compatible,"DXXXX","DXXXX");
        /* Vendor */
        this->Vendor=Main::XML_Get_String(Root,"Class","DXXXX","DXXXX");
        /* Region */
        this->Region=std::stoul(Main::XML_Get_String(Root,"Region","DXXXX","DXXXX"));
        /* Attribute */
        Main::XML_Get_KVP(Root,"Attribute",this->Attribute,"DXXXX","DXXXX");
        /* Memory */
        Parse_Trunk<class Mem_Info,class Mem_Info>(Root,"Memory",this->Memory,"DXXXX","DXXXX");
        /* Config */
        Parse_Trunk<class Config,class Config>(Root,"Config",this->Config,"DXXXX","DXXXX");
        /* Vector */
        Parse_Trunk<class Vector,class Vector>(Root,"Vector",this->Vector,"DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Class!="")
            Main::Error(std::string("Chip: ")+this->Class+"\n"+Exc.what());
        else
            Main::Error(std::string("Chip: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Chip::Chip ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
