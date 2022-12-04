/******************************************************************************
Filename    : plat_info.cpp
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
#include "Plat_Info/plat_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Conf_Info/conf_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Plat_Info::Plat_Info ****************************************
Description : Constructor for platform description class.
Input       : xml_node_t* Root - The node containing the chip information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Plat_Info::Plat_Info(xml_node_t* Root)
{
    try
    {
        /* Platform */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Name_Gen(this);
        /* Wordlength */
        this->Wordlength=Main::XML_Get_Number(Root,"Wordlength","DXXXX","DXXXX");
        /* Captbl_Max */
        this->Captbl_Max=POW2((this->Wordlength>>2)-1);
        /* Kfunc_Max */
        this->Kfunc_Max=POW2(this->Wordlength>>1);
        /* Version */
        this->Version=Main::XML_Get_String(Root,"Version","DXXXX","DXXXX");
        /* Guest */
        Main::XML_Get_CSV(Root,"Guest",this->Guest,"DXXXX","DXXXX");
        /* Build */
        Main::XML_Get_CSV(Root,"Buildsystem",this->Buildsystem,"DXXXX","DXXXX");
        /* Toolchain */
        Main::XML_Get_CSV(Root,"Toolchain",this->Toolchain,"DXXXX","DXXXX");
        /* Config */
        Trunk_Parse<class Conf_Info,class Conf_Info>(Root,"Config",this->Config,"DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Platform: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Platform: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Plat_Info::Plat_Info *****************************************/

/* Begin Function:Plat_Info::Check ********************************************
Description : Check whether the project configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Plat_Info::Check(void)
{
    try
    {
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
            Main::Error(std::string("Platform: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Platform: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Plat_Info::Check *********************************************/

/* Begin Function:Plat_Info::Project_Config_Mark_Check ************************
Description : Check whether all configs have been set.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Plat_Info::Project_Config_Mark_Check(void)
{
    try
    {
        for(std::unique_ptr<class Conf_Info>& Conf:this->Config)
        {
            if(Conf->Is_Configured==0)
                Main::Error("PXXXX: Config '"+Conf->Name+"' in project is not configured.");
        }
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Platform: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Platform: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Plat_Info::Project_Config_Mark_Check *************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
