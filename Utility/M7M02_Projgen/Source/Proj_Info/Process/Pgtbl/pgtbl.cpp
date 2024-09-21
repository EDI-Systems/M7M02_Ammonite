/******************************************************************************
Filename    : pgtbl.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The page table class.
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
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Pgtbl::Pgtbl ******************************************************
Description : Constructor for Pgtbl class.
Input       : ptr_t Base - The base address.
              ptr_t Size_Order - The size order.
              ptr_t Num_Order - The number order.
              ptr_t Attr - The attributes on the page directory itself.
              class Process* Owner - The owner process of this kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Pgtbl::Pgtbl(ptr_t Base, ptr_t Size_Order, ptr_t Num_Order, ptr_t Attr, class Process* Owner):
Kobj(Owner)
{
    try
    {
        this->Is_Top=0;
        this->Base=Base;
        this->Size_Order=Size_Order;
        this->Num_Order=Num_Order;
        this->Attr=Attr;

        this->Page.resize(POW2(Num_Order));
        std::fill(this->Page.begin(), this->Page.end(), 0);

        /* Automatically filled with nullptr */
        this->Pgdir.resize(POW2(Num_Order));
    }
    catch(std::exception& Exc)
    {
        throw std::runtime_error(std::string("Page table creation:\n")+Exc.what());
    }
}
/* End Function:Pgtbl::Pgtbl *************************************************/

/* Function:Pgtbl::Report *****************************************************
Description : Report kernel object details.
Input       : None.
Output      : None.
Return      : std::string - The report string.
******************************************************************************/
std::string Pgtbl::Report(void)
{
    std::string Temp;

    if(this->Is_Top!=0)
        Temp="Toplevel ";
    else
        Temp="Pgtbl ";

    Temp+=this->Kobj::Report()+", "+
          "base 0x"+Main::Hex(this->Base)+
          " size "+std::to_string(this->Size_Order)+
          " number "+std::to_string(this->Num_Order)+
          " "+Mem_Info::Attr2Str(this->Attr);

    return Temp;
}
/* End Function:Pgtbl::Report ************************************************/

/* Function:Pgtbl::Report_Tree ************************************************
Description : Report page table details alongside the entire tree, including self.
Input       : std::string - The prefix to use.
Output      : std::unique_ptr<std::vector<std::string>>& List - The text output.
Return      : None.
******************************************************************************/
void Pgtbl::Report_Tree(std::unique_ptr<std::vector<std::string>>& List, std::string Prefix)
{
    ptr_t Count;
    std::string Temp;

    if(Prefix=="")
        List->push_back(this->Report());
    else
    {
        List->push_back(Prefix+" "+this->Report());
        Prefix=Prefix.substr(0,Prefix.length()-3);
    }

    Prefix="    "+Prefix;
    for(Count=0;Count<this->Pgdir.size();Count++)
    {
        if(Pgdir[Count]!=nullptr)
            Pgdir[Count]->Report_Tree(List, Prefix+"["+std::to_string(Count)+"]");
    }
}
/* End Function:Pgtbl::Report_Tree *******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
