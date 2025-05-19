/******************************************************************************
Filename    : vector.hpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the vector class.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __VECTOR_DEF__
#define __VECTOR_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __VECTOR_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __VECTOR_CLASS__
#define __VECTOR_CLASS__
/*****************************************************************************/
/* The vector information */
class Vector
{
public:
    /* Vector name */
    std::string Name;
    /* Vector number */
    ptr_t Number;

    /* void */ Vector(class wxXmlNode* Node);
    /* void */ Vector(const std::string& Name,const ptr_t& Number);
    /* void */ ~Vector(void);

    void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __VECTOR_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
