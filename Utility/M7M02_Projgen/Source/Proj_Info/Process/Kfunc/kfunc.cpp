/******************************************************************************
Filename    : kfunc.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The kernel function class.
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
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Kfunc::Kfunc ******************************************************
Description : Constructor for kernel function class.
Input       : xml_node_t* Root - The node containing the kernel function.
              class Process* Owner - The owner process of this kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kfunc::Kfunc(xml_node_t* Root, class Process* Owner):
Kobj(Owner)
{
    try
    {
        /* Name */
        this->Name=Main::XML_Get_String(Root,"Name","DXXXX","DXXXX");
        Name_Gen(this);
        /* Start */
        this->Start=Main::XML_Get_Number(Root,"Start","DXXXX","DXXXX");
        /* End */
        this->End=Main::XML_Get_Number(Root,"End","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        if(this->Name!="")
            Main::Error(std::string("Kfunc: ")+this->Name+"\n"+Exc.what());
        else
            Main::Error(std::string("Kfunc: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Kfunc::Kfunc *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
