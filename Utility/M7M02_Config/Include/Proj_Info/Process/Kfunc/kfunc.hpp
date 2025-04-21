/******************************************************************************
Filename    : kfunc.hpp
Author      : pry
Date        : 15/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Kernel function class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __KFUNC_TYPE__
#define __KFUNC_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __KFUNC_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __KFUNC_DEF__
#define __KFUNC_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __KFUNC_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __KFUNC_CLASS__
#define __KFUNC_CLASS__
/*****************************************************************************/
class Kfunc
{
public:
	/* Name */
	std::string Name;
	/* Begin */
	ptr_t Begin;
	/* End */
	ptr_t End;

    /* void */ Kfunc(class wxXmlNode* Node);
    /* void */ ~Kfunc(void);

    virtual void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __KFUNC_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
