/******************************************************************************
Filename    : native.cpp
Author      : pry
Date        : 13/01/2023
License     : Proprietary; confidential.
Description : Native process information implementation.
******************************************************************************/

/* Include *******************************************************************/
#define __HDR_DEF__
#include "rvm_cfg.hpp"
#undef __HDR_DEF__

#include "wx/wx.h"
#include "wx/xml/xml.h"
#include "wx/filename.h"

#include "map"
#include "string"
#include "memory"

#define __HDR_DEF__
#include "Proj_Info/Process/process.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_cfg.hpp"
#include "Mem_Info/mem_info.hpp"
#include "Proj_Info/Process/Vector/vector.hpp"
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
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Native::Native ****************************************************
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
    Trunk_Load<class Vector>(Main::Simple_Load(Node,"Vector"),"V",this->Vector);
}
/* End Function:Native::Native ***********************************************/


/* Function:Native::Native ***************************************************
Description : Constructor for process information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native::Native(const std::string& Name):
Process(Name, PROCESS_NATIVE)
{
    /* Set default value */
    this->Entry_Source_Output="../Source/";
    this->Entry_Source_Overwrite=0;

    /* Each process must contain at least one thread. */
    this->Thread.push_back(std::make_unique<class Thread>("Default_Thread",0,0,5));
}
/* End Function:Native::Native **********************************************/

/* Function:Native::~Native ***************************************************
Description : Destructor for process information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Native::~Native(void)
{

}
/* End Function:Native::~Native **********************************************/

/* Function:Native::Save ******************************************************
Description : Save process information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Native::Save(class wxXmlNode* Parent)
{
    /* Save the common portion */
    //static_cast<class Process*>(this)->Save(Parent);

    /* Name */
    Main::Text_Save(Parent,"Name",this->Name);
    /* Extra_Captbl */
    Main::Num_Save(Parent,"Extra_Captbl",this->Extra_Captbl);
    /* Coprocessor */
    Main::CSV_Save(Parent,"Coprocessor",this->Coprocessor);
    /* Buildsystem */
    Main::Text_Save(Parent,"Buildsystem",this->Buildsystem);
    /* Toolchain */
    Main::Text_Save(Parent,"Toolchain",this->Toolchain);
    /* Optimization */
    Main::Opt_Save(Parent,"Optimization",this->Optimization);
    /* roject_Output */
    Main::Text_Save(Parent,"Project_Output",this->Project_Output);
    /* Project_Overwrite */
    Main::Yesno_Save(Parent,"Project_Overwrite",this->Project_Overwrite);
    /* Linker_Output */
    Main::Text_Save(Parent,"Linker_Output",this->Linker_Output);
    /* Main_Header_Output */
    Main::Text_Save(Parent,"Main_Header_Output",this->Main_Header_Output);
    /* Main_Source_Output */
    Main::Text_Save(Parent,"Main_Source_Output",this->Main_Source_Output);
    /* Entry_Source_Output (Native only) */
    Main::Text_Save(Parent,"Entry_Source_Output",this->Entry_Source_Output);
    /* Entry_Source_Overwrite (Native only)*/
    Main::Yesno_Save(Parent,"Entry_Source_Overwrite",this->Entry_Source_Overwrite);

    /* Memory */
    Trunk_Save<class Mem_Info>(Main::Simple_Save(Parent,"Memory"),"M",this->Memory);
    /* Shmem */
    Trunk_Save<class Shmem>(Main::Simple_Save(Parent,"Shmem"),"S",this->Shmem);
    /* Thread (Native only) */
    Trunk_Save<class Thread>(Main::Simple_Save(Parent,"Thread"),"T",this->Thread);
    /* Invocation (Native only) */
    Trunk_Save<class Invocation>(Main::Simple_Save(Parent,"Invocation"),"I",this->Invocation);
    /* Port (Native only) */
    Trunk_Save<class Port>(Main::Simple_Save(Parent,"Port"),"P",this->Port);
    /* Receive (Native only) */
    Trunk_Save<class Receive>(Main::Simple_Save(Parent,"Receive"),"R",this->Receive);
    /* Send */
    Trunk_Save<class Send>(Main::Simple_Save(Parent,"Send"),"S",this->Send);
    /* Kfunc */
    Trunk_Save<class Kfunc>(Main::Simple_Save(Parent,"Kfunc"),"K",this->Kfunc);
    /* Vector (Native only) */
    Trunk_Save<class Vector>(Main::Simple_Save(Parent,"Vector"),"V",this->Vector);
}
/* End Function:Native::Save *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
