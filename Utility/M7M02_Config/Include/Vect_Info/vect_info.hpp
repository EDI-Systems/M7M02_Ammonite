/******************************************************************************
Filename    : vect_info.hpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Vector information class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __VECT_INFO_HPP_TYPES__
#define __VECT_INFO_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __VECT_INFO_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __VECT_INFO_HPP_DEFS__
#define __VECT_INFO_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __VECT_INFO_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __VECT_INFO_HPP_CLASSES__
#define __VECT_INFO_HPP_CLASSES__
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
/* __VECT_INFO_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
