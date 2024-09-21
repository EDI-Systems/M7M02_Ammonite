/******************************************************************************
Filename    : captbl.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The capability table class.
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
#include "Proj_Info/Process/Captbl/captbl.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#include "Proj_Info/Process/Captbl/captbl.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Captbl::Captbl ****************************************************
Description : Constructor for Cpt class.
Input       : ptr_t Front - The final frontier.
              ptr_t Size - The final size.
              class Process* Owner - The owner process of this kernel object.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Captbl::Captbl(ptr_t Front, ptr_t Size, class Process* Owner):
Kobj(Owner)
{
    ASSERT((Front!=0)&&(Size!=0));

    this->Front=Front;
    this->Size=Size;
}
/* End Function:Captbl::Captbl ***********************************************/

/* Function:Captbl::Report ****************************************************
Description : Report kernel object details.
Input       : None.
Output      : None.
Return      : std::string - The report string.
******************************************************************************/
std::string Captbl::Report(void)
{
    return "Captbl "+this->Kobj::Report()+
            ", size "+std::to_string(this->Size)+" frontier "+std::to_string(this->Front);
}
/* End Function:Captbl::Report ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
