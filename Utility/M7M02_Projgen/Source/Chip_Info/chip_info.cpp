/******************************************************************************
Filename    : chip_info.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : The chip information class. This class is responsible for reading
              chip parameters.
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
#include "Chip_Info/chip_info.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Chip_Info/chip_info.hpp"
#include "Conf_Info/conf_info.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Vect_Info/vect_info.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Chip_Info::Chip_Info **********************************************
Description : Constructor for chip class.
Input       : xml_node_t* Root - The node containing the chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Chip_Info::Chip_Info(xml_node_t* Root)
{
    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Name_Gen(this);
        /* Version */
        this->Version=Main::XML_Get_String(Root,"Version","DXXXX","DXXXX");
        /* Platform */
        this->Platform=Main::XML_Get_String(Root,"Platform","DXXXX","DXXXX");
        /* Compatible */
        Main::XML_Get_CSV(Root,"Compatible",this->Compatible,"DXXXX","DXXXX");
        /* Vendor */
        this->Vendor=Main::XML_Get_String(Root,"Vendor","DXXXX","DXXXX");
        /* Vector */
        this->Vector=Main::XML_Get_Number(Root,"Vector","DXXXX","DXXXX");
        /* Region */
        this->Region=Main::XML_Get_Number(Root,"Region","DXXXX","DXXXX");
        /* Iregion */
        this->Iregion=Main::XML_Get_Number(Root,"Iregion","DXXXX","DXXXX");
        /* Dregion */
        this->Dregion=Main::XML_Get_Number(Root,"Dregion","DXXXX","DXXXX");
        /* Coprocessor */
        Main::XML_Get_CSV(Root,"Coprocessor",this->Coprocessor,"DXXXX","DXXXX");
        None_Filter(this->Coprocessor);
        /* Attribute */
        Main::XML_Get_KVP(Root,"Attribute",this->Attribute,"DXXXX","DXXXX");
        /* Memory */
        Trunk_Parse_Param<class Mem_Info,class Mem_Info,ptr_t>(Root,"Memory",this->Memory,MEM_PHYS,"DXXXX","DXXXX");
        /* Config */
        Trunk_Parse<class Conf_Info,class Conf_Info>(Root,"Config",this->Config,"DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Chip: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Chip: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Chip_Info::Chip_Info *****************************************/

/* Function:Chip_Info::Check **************************************************
Description : Check whether the chip configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Chip_Info::Check(void)
{
    try
    {
    	/* Check compatible models */
        Duplicate_Check<std::string>(this->Compatible, this->Compatible_Set, "PXXXX", "name", "Compatible");

        /* Check chip regions */
        if((this->Region==0)&&((this->Iregion==0)||(this->Dregion==0)))
            Main::Error("PXXXX: Chip must have more than one MPU region for instruction and data.");

        /* Check coprocessor list */
        Duplicate_Check<std::string>(this->Coprocessor, this->Coprocessor_Set, "PXXXX", "name", "Coprocessor");

        /* Check chip memory */
        for(std::unique_ptr<class Mem_Info>& Mem:this->Memory)
            Mem->Check();

        /* Check configs - neither the name nor the macro can be the same */
        for(std::unique_ptr<class Conf_Info>& Conf:this->Config)
            Conf->Check();
        Duplicate_Check<class Conf_Info,std::string>(this->Config,this->Config_Map,
                                                     [](std::unique_ptr<class Conf_Info>& Conf)->std::string{return Conf->Name;},
                                                     "PXXXX","name","Config");
        Duplicate_Check<class Conf_Info,std::string>(this->Config,this->Config_Macro_Map,
                                                     [](std::unique_ptr<class Conf_Info>& Conf)->std::string{return Conf->Macro;},
                                                     "PXXXX","macro","Config");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Chip: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Chip: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Chip_Info::Check *********************************************/

/* Function:Chip_Info::Project_Config_Mark_Check ******************************
Description : Check whether all configs have been set.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Chip_Info::Project_Config_Mark_Check(void)
{
    try
    {
        for(std::unique_ptr<class Conf_Info>& Conf:this->Config)
        {
            if(Conf->Is_Configured==0)
                Main::Error("XXXXX: Config '"+Conf->Name+"' in project is not configured.");
        }
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Chip: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Chip: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Chip_Info::Project_Config_Mark_Check *************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
