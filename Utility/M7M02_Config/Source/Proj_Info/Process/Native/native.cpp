/******************************************************************************
Filename    : native.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Native process information implementation.
******************************************************************************/

/* Includes ******************************************************************/
#define __HDR_DEFS__
#include "rvm_cfg.hpp"
#undef __HDR_DEFS__

#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEFS__
#include "Proj_Info/Process/process.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_cfg.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Vect_Info/vect_info.hpp"
#include "Proj_Info/Process/Shmem/shmem.hpp"
#include "Proj_Info/Process/Thread/thread.hpp"
#include "Proj_Info/Process/Invocation/invocation.hpp"
#include "Proj_Info/Process/Port/port.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Send/send.hpp"
#include "Proj_Info/Process/Receive/receive.hpp"
#include "Proj_Info/Process/Kfunc/kfunc.hpp"
#include "Proj_Info/Process/process.hpp"
#include "Proj_Info/Process/Native/native.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_CFG
{
/* Begin Function:Native::Process *********************************************
Description : Default constructor for process information.
Input       : const std::string& Name - The exact chip name.
              const std::string& Root - The root folder path.
              const class Plat_Info* Plat - The platform information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native::Native(const std::string& Name,
                          const std::string& Root,
                          const class Plat_Info* Plat):
Process(Name,PROCESS_NATIVE,Root,Plat)
{
    this->Type=Type;
    this->Name=Name;
    /* Populate later */
}
/* End Function:Native::Native ***********************************************/

/* Begin Function:Native::Native **********************************************
Description : Constructor for process information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native::Native(class wxXmlNode* Node):
Process(Node, PROCESS_NATIVE)
{
    /* Entry_Source_Output */
    this->Entry_Source_Output=Main::Text_Load(Node,"Entry_Source_Output");
    /* Entry_Source_Overwrite */
    this->Entry_Source_Overwrite=Main::Yesno_Load(Node,"Entry_Source_Overwrite");

    /* Thread */
    Trunk_Load<class Thread>(Main::Simple_Load(Node,"Thread"),"T",this->Thread);
    /* Invocation */
    Trunk_Load<class Invocation>(Main::Simple_Load(Node,"Invocation"),"I",this->Invocation);
    /* Port */
    Trunk_Load<class Port>(Main::Simple_Load(Node,"Port"),"P",this->Port);
    /* Receive */
    Trunk_Load<class Receive>(Main::Simple_Load(Node,"Receive"),"R",this->Receive);

    /* Vector */
    Trunk_Load<class Vect_Info>(Main::Simple_Load(Node,"Vector"),"V",this->Vector);
}
/* End Function:Native::Native ***********************************************/

/* Begin Function:Native::~Native *********************************************
Description : Destructor for process information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native::~Native(void)
{

}
/* End Function:Native::~Native **********************************************/

/* Begin Function:Native::Save ************************************************
Description : Save process information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Native::Save(class wxXmlNode* Parent)
{
    /* Save the common portion */
    static_cast<class Process*>(this)->Save(Parent);

    /* Entry_Source_Output */
    Main::Text_Save(Parent,"Entry_Source_Output",this->Entry_Source_Output);
    /* Entry_Source_Overwrite */
    Main::Yesno_Save(Parent,"Entry_Source_Overwrite",this->Entry_Source_Overwrite);

    /* Thread */
    Trunk_Save<class Thread>(Main::Simple_Save(Parent,"Thread"),"T",this->Thread);
    /* Invocation */
    Trunk_Save<class Invocation>(Main::Simple_Save(Parent,"Invocation"),"I",this->Invocation);
    /* Port */
    Trunk_Save<class Port>(Main::Simple_Save(Parent,"Port"),"P",this->Port);
    /* Receive */
    Trunk_Save<class Receive>(Main::Simple_Save(Parent,"Receive"),"R",this->Receive);

    /* Vector */
    Trunk_Save<class Vect_Info>(Main::Simple_Save(Parent,"Vector"),"V",this->Vector);
}
/* End Function:Native::Save *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
