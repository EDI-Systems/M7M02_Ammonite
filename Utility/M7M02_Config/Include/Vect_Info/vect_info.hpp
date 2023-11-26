/******************************************************************************
Filename    : vect_info.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Vector information class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __VECT_INFO_TYPE__
#define __VECT_INFO_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __VECT_INFO_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __VECT_INFO_DEF__
#define __VECT_INFO_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __VECT_INFO_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __VECT_INFO_CLASS__
#define __VECT_INFO_CLASS__
/*****************************************************************************/
class Vect_Info
{
public:
    /* Vector name */
    std::string Name;
    /* Vector number */
    ptr_t Number;

    /* void */ Vect_Info(const std::string& Name,ptr_t Number);
    /* void */ Vect_Info(class wxXmlNode* Node);
    /* void */ ~Vect_Info(void);

    void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __VECT_INFO_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
