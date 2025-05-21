/******************************************************************************
Filename    : virtual.hpp
Author      : lbc
Date        : 21/04/2025
License     : Proprietary; confidential.
Description : Virtual process class header.
******************************************************************************/

/* Define ********************************************************************/
#ifdef __HDR_DEF__
#ifndef __VIRTUAL_TYPE__
#define __VIRTUAL_TYPE__
/*****************************************************************************/

/*****************************************************************************/
/* __VIRTUAL_TYPE__ */
#endif
/* __HDR_DEF__ */
#endif

namespace RVM_CFG
{
#ifdef __HDR_DEF__
#ifndef __VIRTUAL_DEF__
#define __VIRTUAL_DEF__
/*****************************************************************************/

/*****************************************************************************/
/* __VIRTUAL_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __VIRTUAL_CLASS__
#define __VIRTUAL_CLASS__
/*****************************************************************************/
class Virtual:public Process
{
public:
    /* Virtual only, begin */
    /* Stack_Size */
    ptr_t Vector_Stack_Size;
    ptr_t User_Stack_Size;

    /* Priority */
    ptr_t Priority;
    /* Timeslice */
    ptr_t Timeslice;
    /* Period */
    ptr_t Period;
    /* Watchdog */
    ptr_t Watchdog;
    /* Vector_Num */
    ptr_t Vector_Num;
    /* Guest_Type */
    std::string Guest_Type;

    /* Virtual header source output & overwrite */
    std::string Virtual_Header_Output;
    ptr_t Virtual_Header_Overwrite;

    /* Virtual source output & overwrite */
    std::string Virtual_Source_Output;
    ptr_t Virtual_Source_Overwrite;
	/* Virtual only, end */

    /* void */ Virtual(const std::string& Name,
                       const std::string& Root,
                       const class Plat_Info* Plat);
    /* void */ Virtual(class wxXmlNode* Node);
    /* change */
    /* void */ Virtual(const std::string& Name);
    /* void */ ~Virtual(void);

    virtual void Save(class wxXmlNode* Parent) final override;
    //void Save(class wxXmlNode* Parent);
};
/*****************************************************************************/
/* __VIRTUAL_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
