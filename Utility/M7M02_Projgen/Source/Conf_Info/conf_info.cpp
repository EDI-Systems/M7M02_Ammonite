/******************************************************************************
Filename    : conf_info.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : The configuration option info class. This class is responsible for
                 keeping configurations of particular chips.
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
#include "Conf_Info/conf_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Conf_Info/conf_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Conf_Info::Conf_Info ****************************************
Description : Parse the option section of a particular chip.
Input       : xml_node_t* Root - The config section's XML node.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Conf_Info::Conf_Info(xml_node_t* Root)
{
    std::string Temp;

    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Main::Idtfr_Check(this->Name,"Name","DXXXX","DXXXX");

        /* Type */
        Temp=Main::XML_Get_String(Root,"Type","DXXXX","DXXXX");
        if(Temp=="Range")
            this->Type=CONFIG_RANGE;
        else if(Temp=="Choice")
            this->Type=CONFIG_CHOICE;
        else
            Main::Error("D0104: Type is malformed.");

        /* Macro */
        this->Macro=Main::XML_Get_String(Root,"Macro","DXXXX","DXXXX");
        Main::Idtfr_Check(this->Macro,"Name","DXXXX","DXXXX");

        /* Range */
        Main::XML_Get_CSV(Root,"Range",this->Range,"DXXXX","DXXXX");

        /* Haven't been configured */
        this->Is_Configured=0;
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Config: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Config: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Conf_Info::Conf_Info *****************************************/

/* Begin Function:Conf_Info::Check ********************************************
Description : Check whether the chip description configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Conf_Info::Check(void)
{
    try
    {
        /* Check range validity */
        if(this->Type==CONFIG_RANGE)
        {
            if(this->Range.size()!=2)
                Main::Error("D0110: Range typed option cannot have more or less than two ends specified.");
            try
            {
                if(std::stoull(this->Range[0],0,0)>=std::stoull(this->Range[1],0,0))
                    Main::Error("D0112: No room for value.");
            }
            catch(std::exception& Exc)
            {
                Main::Error(std::string("D0111: Starting point or ending point conversion failure.\n")+Exc.what());
            }
        }
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Config: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Config: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Conf_Info::Check *********************************************/

/* Begin Function:Conf_Info::Project_Config_Mark ******************************
Description : Confirm the the project correctly configured the config.
Input       : const std::string& Name - The name passed in.
              const std::string& Value - The value passed in.
Output      : None.
Return      : None.
******************************************************************************/
void Conf_Info::Project_Config_Mark(const std::string & Name, const std::string& Value)
{
    try
    {
        /* Check range validity */
        if(this->Type==CONFIG_RANGE)
        {
            if(std::stoull(Value,0,0)<std::stoull(this->Range[0],0,0))
                Main::Error("D0112: Range type config is less than the range lower bound.");
            if(std::stoull(Value,0,0)>std::stoull(this->Range[1],0,0))
                Main::Error("D0112: Range type config is greater than the range upper bound.");
        }
        else
        {
            if(std::find(this->Range.begin(),this->Range.end(),Value)==this->Range.end())
                Main::Error("D0112: Choice type config is not in the allowed list.");
        }

        /* Mark as configured */
        if(this->Is_Configured!=0)
            Main::Error("D0112: Duplicate configs exist in the project config file.");
        this->Is_Configured=1;
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Config: ")+Name+"\n"+Exc.what());
    }
}
/* End Function:Conf_Info::Project_Config_Mark *******************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
