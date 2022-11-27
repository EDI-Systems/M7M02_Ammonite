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

/* Begin Function:Kobj::Upper *************************************************
Description : Convert the string to uppercase.
Input       : std::string& Str - The string to convert.
Output      : std::string& Str - The converted string.
Return      : None.
******************************************************************************/
void Kobj::Upper(std::string& Str)
{
    std::transform(Str.begin(), Str.end(), Str.begin(), (int(*)(int))std::toupper);
}
/* End Function:Kobj::Upper **************************************************/

/* Begin Function:Kobj::Lower *************************************************
Description : Convert the string to lowercase.
Input       : std::string& Str - The string to convert.
Output      : std::string& Str - The converted string.
Return      : None.
******************************************************************************/
void Kobj::Lower(std::string& Str)
{
    std::transform(Str.begin(), Str.end(), Str.begin(), (int(*)(int))std::tolower);
}
/* End Function:Kobj::Lower **************************************************/

/* Begin Function:Kobj::Strcicmp **********************************************
Description : Compare two strings in a case insensitive way.
Input       : const std::string& Str1 - The first string.
              const std::string& Str2 - The second string.
Output      : None.
Return      : ret_t - If two strings are equal, then 0; if not, -1.
******************************************************************************/
ret_t Kobj::Strcicmp(const std::string& Str1, const std::string& Str2)
{
    bool Equal;

    Equal=std::equal(Str1.begin(), Str1.end(), Str2.begin(), Str2.end(),
                     [](s8_t Char1, s8_t Char2)
                     {
                         return std::tolower(Char1)==std::tolower(Char2);
                     });

    if(Equal==true)
        return 0;

    return -1;
}
/* End Function:Kobj::Strcicmp ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
