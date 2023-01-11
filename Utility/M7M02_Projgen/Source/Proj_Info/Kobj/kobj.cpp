/******************************************************************************
Filename    : kobj.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The kernel object base class.
******************************************************************************/

/* Includes ******************************************************************/
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

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Kobj/kobj.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Kobj::Kobj **************************************************
Description : Constructor for Kobj class.
Input       : class Process* Owner - The owner process of this kernel object.
                                     If this is NULL, then this is just a declaration.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kobj::Kobj(class Process* Owner)
{
    this->Owner=Owner;
}
/* End Function:Kobj::Kobj ***************************************************/

/* Begin Function:Kobj::~Kobj *************************************************
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
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
