/******************************************************************************
Filename    : kobj.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The kernel object base class.
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
#include "algorithm"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Kobj::Kobj ********************************************************
Description : Constructor for Kobj class.
Input       : class Process* Owner - The owner process of this kernel object.
                                     If this is NULL, then this is just a declaration.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kobj::Kobj(class Process* Owner)
{
    this->Owner=Owner;
    this->Kom_Base=0;
    this->Kom_Size=0;
}
/* End Function:Kobj::Kobj ***************************************************/

/* Function:Kobj::~Kobj *******************************************************
Description : Pure virtual destructor (cannot be omitted).
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kobj::~Kobj(void)
{
    /* Do nothing */
}
/* End Function:Kobj::~Kobj **************************************************/

/* Function:Kobj::Report ******************************************************
Description : Report kernel object details.
Input       : None.
Output      : None.
Return      : std::string - The report string.
******************************************************************************/
std::string Kobj::Report(void)
{
    std::string Temp;

    if(this->Name!="")
        Temp=this->Name+" ";

    /* This only happens for Kfunc */
    if(this->Macro_Global!="")
        Temp+=this->Macro_Global+"(0x"+Main::Hex(this->Cid_Global)+") ";

    /* This is more prevalent */
    if(this->Macro_Local!="")
        Temp+=this->Macro_Local+"(0x"+Main::Hex(this->Cid_Local)+") ";

    if(this->Kom_Base!=0)
        Temp+="@ 0x"+Main::Hex(this->Kom_Base)+"(0x"+Main::Hex(this->Kom_Size)+")";

    return Temp;
}
/* End Function:Kobj::Report *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
