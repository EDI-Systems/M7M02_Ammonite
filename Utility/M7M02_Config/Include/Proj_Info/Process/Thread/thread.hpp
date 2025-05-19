/******************************************************************************
Filename    : thread.hpp
Author      : pry
Date        : 15/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Thread class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __THREAD_TYPE__
#define __THREAD_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __THREAD_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __THREAD_DEF__
#define __THREAD_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __THREAD_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __THREAD_CLASS__
#define __THREAD_CLASS__
/*****************************************************************************/
class Thread
{
public:
    /* Name */
    std::string Name;
    /* Stack_Size */
    ptr_t Stack_Size;
    /* Parameter */
    ptr_t Parameter;
    /* Priority */
    ptr_t Priority;

    /* void */ Thread(class wxXmlNode* Node);
    /* void */ Thread(const std::string Name,const ptr_t& Stack_Size,
                      const ptr_t& Parameter,const ptr_t& Priority);
    /* void */ ~Thread(void);

    virtual void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __THREAD_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
