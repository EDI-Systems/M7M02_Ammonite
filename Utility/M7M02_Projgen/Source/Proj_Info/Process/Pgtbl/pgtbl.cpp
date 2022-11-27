/******************************************************************************
Filename    : pgtbl.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The page table class.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "xml.h"
}

#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Pgtbl/pgtbl.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Pgtbl::Pgtbl ************************************************
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
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
