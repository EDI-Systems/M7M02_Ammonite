/******************************************************************************
Filename    : plat_info.cpp
Author      : pry
Date        : 2022/11/19
Licence     : LGPL v3+; see COPYING for details.
Description : The chip information class. This class is responsible for reading
              chip parameters.
******************************************************************************/

/* Include *******************************************************************/
#include "set"
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Plat_Info/plat_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Plat_Info/Compatible/compatible.hpp"
#include "Plat_Info/plat_info.hpp"
#include "Conf_Info/conf_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Plat_Info::Plat_Info **********************************************
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
        /* Coprocessor */
        Main::XML_Get_CSV(Root,"Coprocessor",this->Coprocessor,"DXXXX","DXXXX");
        None_Filter(this->Coprocessor);
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
        /* Compatible */
        Trunk_Parse<class Compatible,class Compatible>(Root,"Compatible",this->Compatible,"DXXXX","DXXXX");
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

/* Function:Plat_Info::Check **************************************************
Description : Check whether the project configuration makes sense.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Plat_Info::Check(void)
{
    try
    {
        /* Check buildsystem list */
        if(this->Buildsystem.empty())
            Main::Error("XXXXX: No buildsystem declarations exist.");
        Duplicate_Check<std::string>(this->Buildsystem, this->Buildsystem_Set, "PXXXX", "name", "Buildsystem");
        if(this->Toolchain.empty())
            Main::Error("XXXXX: No toolchain declarations exist.");
        Duplicate_Check<std::string>(this->Toolchain, this->Toolchain_Set, "PXXXX", "name", "Toolchain");
        if(this->Guest.empty())
            Main::Error("XXXXX: No guest OS declarations exist.");
        Duplicate_Check<std::string>(this->Guest, this->Guest_Set, "PXXXX", "name", "Guest");

        /* Check coprocessor list */
        Duplicate_Check<std::string>(this->Coprocessor, this->Coprocessor_Set, "PXXXX", "name", "Coprocessor");

        /* Check compatibility data - all entries shall exist */
        for(std::unique_ptr<class Compatible>& Compat:this->Compatible)
        {
            if(this->Buildsystem_Set.find(Compat->Buildsystem)==this->Buildsystem_Set.end())
                Main::Error(std::string("XXXXX: Buildsystem '")+Compat->Buildsystem+"' in compatibility list cannot be found in declaration list.");
            if(this->Toolchain_Set.find(Compat->Toolchain)==this->Toolchain_Set.end())
                Main::Error(std::string("XXXXX: Toolchain '")+Compat->Toolchain+"' in compatibility list cannot be found in declaration list.");
            if(Compat->Guest!="Native")
            {
                if(this->Guest_Set.find(Compat->Guest)==this->Guest_Set.end())
                    Main::Error(std::string("XXXXX: Guest '")+Compat->Guest+"' in compatibility list cannot be found in declaration list.");

            }
        }

        /* Check configs - neither the name nor the macro can be the same */
        for(std::unique_ptr<class Conf_Info>& Conf:this->Config)
            Conf->Check();
        Duplicate_Check<class Conf_Info,std::string>(this->Config, this->Config_Map,
                                                     [](std::unique_ptr<class Conf_Info>& Conf)->std::string{return Conf->Name;},
                                                     "PXXXX", "name", "Config");
        Duplicate_Check<class Conf_Info,std::string>(this->Config, this->Config_Macro_Map,
                                                     [](std::unique_ptr<class Conf_Info>& Conf)->std::string{return Conf->Macro;},
                                                     "PXXXX", "macro", "Config");
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

/* Function:Plat_Info::Compatible_Check ***************************************
Description : Check whether all configs have been set.
Input       : const std::string& Name - The name in the error info.
              const std::string& Buildsystem - The buildsystem to check for.
              const std::string& Toolchain - The toolchain to check for.
              const std::string& Guest - The guest OS to check for.
Output      : None.
Return      : None.
******************************************************************************/
void Plat_Info::Compatible_Check(const std::string& Name,
                                 const std::string& Buildsystem,
                                 const std::string& Toolchain,
                                 const std::string& Guest)
{
    for(std::unique_ptr<class Compatible>& Compat:this->Compatible)
    {
        if((Compat->Buildsystem==Buildsystem)&&
           (Compat->Toolchain==Toolchain)&&
           (Compat->Guest==Guest))
            return;
    }

    Main::Error(Name+":\n"+"Combination '"+Buildsystem+"/"+Toolchain+"/"+Guest+"' is not found in compatible list.");
}
/* End Function:Plat_Info::Compatible_Check **********************************/

/* Function:Plat_Info::Project_Config_Mark_Check ******************************
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
                Main::Error("XXXXX: Config '"+Conf->Name+"' in project is not configured.");
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
