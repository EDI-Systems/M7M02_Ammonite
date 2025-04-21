/******************************************************************************
Filename    : virtual.cpp
Author      : lbc
Date        : 21/04/2025
License     : Proprietary; confidential.
Description : Virtual process information implementation.
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
#include "Proj_Info/Process/Virtual/virtual.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_CFG
{
/* Function:Virutual::Process ***************************************************
Description : Default constructor for process information.
Input       : const std::string& Name - The exact chip name.
              const std::string& Root - The root folder path.
              const class Plat_Info* Plat - The platform information.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual::Virtual(const std::string& Name,
                          const std::string& Root,
                          const class Plat_Info* Plat):
Process(Name,PROCESS_NATIVE,Root,Plat)
{

}
/* End Function:Virtual::Virtual ***********************************************/

/* Function:Virtual::Virtual ****************************************************
Description : Constructor for process information.
Input       : class wxXmlNode* Node - The node containing information.
              ptr_t Type - Whether this is a native process or a VM.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual::Virtual(class wxXmlNode* Node):
Process(Node, PROCESS_NATIVE)
{
	/* Vector_Stack_Size */
	this->Vector_Stack_Size=Main::Num_Load(Node,"Vector_Stack_Size");
	/* User_Stack_Size */
	this->User_Stack_Size=Main::Num_Load(Node,"User_Stack_Size");
	/* Priority */
	this->Priority=Main::Num_Load(Node,"Priority");
	/* Timeslice */
	this->Timeslice=Main::Num_Load(Node,"Timeslice");
	/* Period */
	this->Period=Main::Num_Load(Node,"Period");
	/* Watchdog */
	this->Watchdog=Main::Num_Load(Node,"Watchdog");
	/* Vector_Num */
	this->Vector_Num=Main::Num_Load(Node,"Vector_Num");
	/* Guest_Type */
	this->Guest_Type=Main::Text_Load(Node,"Guest_Type");
    /* Virtual_Header_Output */
	this->Virtual_Header_Output=Main::Text_Load(Node,"Virtual_Header_Output");
    /* Virtual_Header_Overwrite */
	this->Virtual_Header_Overwrite=Main::Yesno_Load(Node,"Virtual_Header_Overwrite");
    /* Virtual_Source_Output */
	this->Virtual_Source_Output=Main::Text_Load(Node,"Virtual_Source_Output");
    /* Virtual_Source_Overwrite */
	this->Virtual_Source_Overwrite=Main::Yesno_Load(Node,"Virtual_Source_Overwrite");

    /* Memory */
    Trunk_Load<class Mem_Info>(Main::Simple_Load(Node,"Memory"),"M",this->Memory);
    /* Shmem */
    Trunk_Load<class Shmem>(Main::Simple_Load(Node,"Shmem"),"S",this->Shmem);
    /* Send */
    Trunk_Load<class Send>(Main::Simple_Load(Node,"Send"),"S",this->Send);
    /* Kfunc */
    Trunk_Load<class Kfunc>(Main::Simple_Load(Node,"Kfunc"),"K",this->Kfunc);
}
/* End Function:Virtual::Virtual ***********************************************/

/* Function:Virtual::~Virtual ***************************************************
Description : Destructor for process information.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Virtual::~Virtual(void)
{

}
/* End Function:Virtual::~Virtual **********************************************/

/* Function:Virtual::Save ******************************************************
Description : Save process information to XML file.
Input       : class wxXmlNode* Parent - The parent node.
Output      : None.
Return      : None.
******************************************************************************/
void Virtual::Save(class wxXmlNode* Parent)
{
    /* Save the common portion */
    static_cast<class Process*>(this)->Save(Parent);

	/* Vector_Stack_Size */
    Main::Hex_Save(Parent,"Vector_Stack_Size",6,this->Vector_Stack_Size);
	/* User_Stack_Size */
    Main::Hex_Save(Parent,"User_Stack_Size",6,this->User_Stack_Size);
	/* Priority */
    Main::Num_Save(Parent,"Priority",this->Priority);
	/* Timeslice */
    Main::Num_Save(Parent,"Timeslice",this->Timeslice);
	/* Period */
    Main::Num_Save(Parent,"Period",this->Period);
	/* Watchdog */
    Main::Num_Save(Parent,"Watchdog",this->Watchdog);
	/* Vector_Num */
    Main::Num_Save(Parent,"Vector_Num",this->Vector_Num);
	/* Guest_Type */
    Main::Text_Save(Parent,"Guest_Type",this->Guest_Type);
    /* Virtual_Header_Output */
    Main::Text_Save(Parent,"Virtual_Header_Output",this->Virtual_Header_Output);
    /* Virtual_Header_Overwrite */
    Main::Yesno_Save(Parent,"Virtual_Header_Overwrite",this->Virtual_Header_Overwrite);
    /* Virtual_Source_Output */
    Main::Text_Save(Parent,"Virtual_Source_Output",this->Virtual_Source_Output);
    /* Virtual_Source_Overwrite */
    Main::Yesno_Save(Parent,"Virtual_Source_Overwrite",this->Virtual_Source_Overwrite);

    /* Memory */
    Trunk_Save<class Mem_Info>(Main::Simple_Save(Parent,"Memory"),"T",this->Memory);
    /* Shmem */
    Trunk_Save<class Shmem>(Main::Simple_Save(Parent,"Shmem"),"T",this->Shmem);
    /* Send */
    Trunk_Save<class Send>(Main::Simple_Save(Parent,"Send"),"T",this->Send);
    /* Kfunc */
    Trunk_Save<class Kfunc>(Main::Simple_Save(Parent,"Kfunc"),"T",this->Kfunc);

}
/* End Function:Virutal::Save *************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
