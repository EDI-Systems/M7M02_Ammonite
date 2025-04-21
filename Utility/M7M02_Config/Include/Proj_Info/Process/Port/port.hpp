/******************************************************************************
Filename    : port.hpp
Author      : pry
Date        : 15/01/2023
Licence     : The Unlicense; see LICENSE for details.
Description : Port class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __PORT_TYPE__
#define __PORT_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __PORT_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __PORT_DEF__
#define __PORT_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __PORT_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PORT_CLASS__
#define __PORT_CLASS__
/*****************************************************************************/
class Port
{
public:
	/* Name */
	std::string Name;
	/* Process */
	std::string Process;

    /* void */ Port(class wxXmlNode* Node);
    /* void */ ~Port(void);

    virtual void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __PORT_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
