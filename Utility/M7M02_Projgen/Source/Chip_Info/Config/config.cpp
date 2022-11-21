/******************************************************************************
Filename    : config.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : The configuration option info class. This class is responsible for
                 keeping configurations of particular chips.
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
#include "rvm_gen.hpp"
#include "Chip_Info/Config/config.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Chip_Info/Config/config.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Config::Config **********************************************
Description : Parse the option section of a particular chip.
Input       : xml_node_t* Root - The config section's XML node.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Config::Config(xml_node_t* Root)
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
        else if(Temp=="Select")
            this->Type=CONFIG_SELECT;
        else
            Main::Error("D0104: Type is malformed.");

        /* Macro */
        this->Macro=Main::XML_Get_String(Root,"Macro","DXXXX","DXXXX");
        Main::Idtfr_Check(this->Macro,"Name","DXXXX","DXXXX");

        /* Range */
        Main::XML_Get_CSV(Root,"Range",this->Range,"DXXXX","DXXXX");
        /* Check range validity */
        if(this->Type==CONFIG_RANGE)
        {
            if(this->Range.size()!=2)
                throw std::invalid_argument("D0110: Range typed option cannot have more or less than two ends specified.");
            try
            {
                if(std::stoull(this->Range[0])>=std::stoull(this->Range[1]))
                    throw std::runtime_error("D0112: No room for value.");
            }
            catch(std::exception& Exc)
            {
                throw std::runtime_error(std::string("D0111: Starting point or ending point conversion failure.\n")+Exc.what());
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
/* End Function:Chip::Chip ***************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
