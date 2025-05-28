/******************************************************************************
Filename    : native.hpp
Author      : pry
Date        : 15/01/2023
License     : Proprietary; confidential.
Description : Native process class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __NATIVE_TYPE__
#define __NATIVE_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __NATIVE_DEF__
#define __NATIVE_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __NATIVE_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __NATIVE_CLASS__
#define __NATIVE_CLASS__
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
    std::vector<std::unique_ptr<class Vector>> Vector;

    /* void */ Native(class wxXmlNode* Node);
    /* change */
    /* void */ Native(const std::string& Name);
    /* void */ ~Native(void);

    virtual void Save(class wxXmlNode* Parent) final override;
    //void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __NATIVE_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
