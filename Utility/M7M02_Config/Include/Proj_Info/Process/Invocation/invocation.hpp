/******************************************************************************
Filename    : invocation.hpp
Author      : pry
Date        : 15/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Invocation class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __INVOCATION_TYPE__
#define __INVOCATION_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __INVOCATION_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __INVOCATION_DEF__
#define __INVOCATION_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __INVOCATION_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __INVOCATION_CLASS__
#define __INVOCATION_CLASS__
/*****************************************************************************/
class Invocation
{
public:

    /* void */ Invocation(class wxXmlNode* Node);
    /* void */ ~Invocation(void);

    virtual void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __INVOCATION_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
