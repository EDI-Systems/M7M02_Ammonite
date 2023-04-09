/******************************************************************************
Filename    : native.hpp
Author      : pry
Date        : 15/01/2023
License     : Proprietary; confidential.
Description : Native process class header.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __NATIVE_HPP_TYPES__
#define __NATIVE_HPP_TYPES__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEFS__
#ifndef __NATIVE_HPP_DEFS__
#define __NATIVE_HPP_DEFS__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __NATIVE_HPP_CLASSES__
#define __NATIVE_HPP_CLASSES__
/*****************************************************************************/
class Native:public Process
{
public:
    /* Entry source & overwrite - native only */
    std::string Entry_Source_Output;
    ptr_t Entry_Source_Overwrite;

    /* Threads */
    std::vector<std::unique_ptr<class Thread>> Thread;

    /* Invocations */
    std::vector<std::unique_ptr<class Invocation>> Invocation;

    /* Ports */
    std::vector<std::unique_ptr<class Port>> Port;

    /* Receive endpoints */
    std::vector<std::unique_ptr<class Receive>> Receive;

    /* Vector endpoints */
    std::vector<std::unique_ptr<class Vect_Info>> Vector;

    /* void */ Native(const std::string& Name,
                      const std::string& Root,
                      const class Plat_Info* Plat);
    /* void */ Native(class wxXmlNode* Node);
    /* void */ ~Native(void);

    virtual void Save(class wxXmlNode* Parent) final override;
};
/*****************************************************************************/
/* __NATIVE_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
