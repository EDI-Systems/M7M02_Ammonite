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
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Chip_Info/Config/config.hpp"
#include "Vect_Info/vect_info.hpp"
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
        this->Region=Main::XML_Get_Number(Root,"Region","DXXXX","DXXXX");
        /* Iregion */
        this->Iregion=Main::XML_Get_Number(Root,"Iregion","DXXXX","DXXXX");
        /* Dregion */
        this->Dregion=Main::XML_Get_Number(Root,"Dregion","DXXXX","DXXXX");
        /* Attribute */
        Main::XML_Get_KVP(Root,"Attribute",this->Attribute,"DXXXX","DXXXX");
        /* Memory */
        Trunk_Parse_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Memory",this->Memory,MEM_DECL,"DXXXX","DXXXX");
        /* Config */
        Trunk_Parse<class Config,class Config>(Root,"Config",this->Config,"DXXXX","DXXXX");
        /* Vector */
        Trunk_Parse<class Vect_Info,class Vect_Info>(Root,"Vector",this->Vector,"DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Class!="")
            Main::Error(std::string("Chip: ")+this->Class+"\n"+Exc.what());
        else
            Main::Error(std::string("Chip: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Chip_Info::Chip_Info *****************************************/

/* Begin Function:Chip_Info::Check ********************************************
Description : Check whether the project configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Chip_Info::Check(void)
{
    try
    {
        /* Check chip memory */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Memory)
        {
            Mem->Check();
            if(Mem->Base==MEM_AUTO)
                Main::Error("PXXXX: On-chip memory must have a concrete base address.");
        }

        /* Check configs - neither the name nor the macro can be the same */
        for(std::unique_ptr<class Config>& Conf:this->Config)
            Conf->Check();
        Duplicate_Check<class Config,std::string>(this->Config,this->Config_Map,
                                                  [](std::unique_ptr<class Config>& Conf)->std::string{return Conf->Name;},
                                                  "PXXXX","name","Config");
        Duplicate_Check<class Config,std::string>(this->Config,this->Config_Macro_Map,
                                                  [](std::unique_ptr<class Config>& Conf)->std::string{return Conf->Macro;},
                                                  "PXXXX","macro","Config");

        /* Check vectors - neither the name nor the number can be the same */
        for(std::unique_ptr<class Vect_Info>& Vect:this->Vector)
            Vect->Check();
        Duplicate_Check<class Vect_Info,std::string>(this->Vector,this->Vector_Map,
                                                     [](std::unique_ptr<class Vect_Info>& Vect)->std::string{return Vect->Name;},
                                                     "PXXXX","name","Vector");
        Duplicate_Check<class Vect_Info,ptr_t>(this->Vector,this->Vector_Number_Map,
                                               [](std::unique_ptr<class Vect_Info>& Vect)->ptr_t{return Vect->Number;},
                                               "PXXXX","number","Vector");
    }
    catch(std::exception& Exc)
    {
        if(this->Class!="")
            Main::Error(std::string("Chip: ")+this->Class+"\n"+Exc.what());
        else
            Main::Error(std::string("Chip: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Chip_Info::Check *********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/