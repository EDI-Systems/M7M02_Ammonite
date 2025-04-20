/******************************************************************************
Filename    : process.hpp
Author      : pry
Date        : 15/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Generic process class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __PROCESS_TYPE__
#define __PROCESS_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __PROCESS_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __PROCESS_DEF__
#define __PROCESS_DEF__
/*****************************************************************************/
#define PROCESS_NATIVE      0
#define PROCESS_VIRTUAL     1
/*****************************************************************************/
/* __PROCESS_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PROCESS_CLASS__
#define __PROCESS_CLASS__
/*****************************************************************************/
class Process
{
public:


    /* void */ Process(const std::string& Name,
                       ptr_t Type,
                       const std::string& Root,
                       const class Plat_Info* Plat);
    /* void */ Process(class wxXmlNode* Node, ptr_t Type);
    /* void */ ~Process(void);

    virtual void Save(class wxXmlNode* Parent)=0;
};
/*****************************************************************************/
/* __PROCESS_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
