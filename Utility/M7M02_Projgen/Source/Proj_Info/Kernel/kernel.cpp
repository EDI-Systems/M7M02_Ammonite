/******************************************************************************
Filename    : kernel.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The kernel configuration class.
******************************************************************************/

/* Include *******************************************************************/
extern "C"
{
#include "xml.h"
}

#include "set"
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

#define __HDR_DEF__
#include "rvm_gen.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEF__

#define __HDR_CLASS__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASS__
/* End Include ***************************************************************/
namespace RVM_GEN
{
/* Function:Kernel::Kernel ****************************************************
Description : Constructor for RME class.
Input       : xml_node_t* Root - The node containing the kernel section.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Kernel::Kernel(xml_node_t* Root)
{
    try
    {
        /* Code base/size */
        this->Code_Base=Main::XML_Get_Number(Root,"Code_Base","DXXXX","DXXXX");
        this->Code_Size=Main::XML_Get_Number(Root,"Code_Size","DXXXX","DXXXX");
        this->Code=std::make_unique<class Mem_Info>("Kernel_Code",this->Code_Base,this->Code_Size,MEM_CODE,MEM_CODE_KERNEL);
        /* Data base/size */
        this->Data_Base=Main::XML_Get_Number(Root,"Data_Base","DXXXX","DXXXX");
        this->Data_Size=Main::XML_Get_Number(Root,"Data_Size","DXXXX","DXXXX");
        this->Data=std::make_unique<class Mem_Info>("Kernel_Data",this->Data_Base,this->Data_Size,MEM_DATA,MEM_DATA_KERNEL);
        /* Stack size */
        this->Stack_Size=Main::XML_Get_Number(Root,"Stack_Size","DXXXX","DXXXX");
        /* Extra kernel memory */
        this->Extra_Kom=Main::XML_Get_Number(Root,"Extra_Kom","DXXXX","DXXXX");
        /* Kom_Order */
        this->Kom_Order=Main::XML_Get_Number(Root,"Kom_Order","DXXXX","DXXXX");
        /* Kern_Prio */
        this->Kern_Prio=Main::XML_Get_Number(Root,"Kern_Prio","DXXXX","DXXXX");

        /* Buildsystem */
        this->Buildsystem=Main::XML_Get_String(Root,"Buildsystem","DXXXX","DXXXX");
        /* Toolchain */
        this->Toolchain=Main::XML_Get_String(Root,"Toolchain","DXXXX","DXXXX");
        /* Optimization */
        this->Optimization=Main::XML_Get_String(Root,"Optimization","DXXXX","DXXXX");
        /* Full_Image */
        this->Full_Image=Main::XML_Get_Yesno(Root,"Full_Image","DXXXX","DXXXX");
        /* Project_Output - relative to workspace */
        this->Project_Output=Main::XML_Get_String(Root,"Project_Output","DXXXX","DXXXX");
        this->Project_Output=Main::Path_Absolute(PATH_DIR, Main::Workspace_Output, this->Project_Output);
        this->Project_Overwrite=Main::XML_Get_Yesno(Root,"Project_Overwrite","DXXXX","DXXXX");
        /* Linker_Output - relative to project */
        this->Linker_Output=Main::XML_Get_String(Root,"Linker_Output","DXXXX","DXXXX");
        this->Linker_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Linker_Output);
        /* Config_Header_Output - relative to project */
        this->Config_Header_Output=Main::XML_Get_String(Root,"Config_Header_Output","DXXXX","DXXXX");
        this->Config_Header_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Config_Header_Output);
        /* Boot_Header_Output - relative to project */
        this->Boot_Header_Output=Main::XML_Get_String(Root,"Boot_Header_Output","DXXXX","DXXXX");
        this->Boot_Header_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Boot_Header_Output);
        /* Boot_Source_Output - relative to project */
        this->Boot_Source_Output=Main::XML_Get_String(Root,"Boot_Source_Output","DXXXX","DXXXX");
        this->Boot_Source_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Boot_Source_Output);
        /* Hook_Source_Output - relative to project */
        this->Hook_Source_Output=Main::XML_Get_String(Root,"Hook_Source_Output","DXXXX","DXXXX");
        this->Hook_Source_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Hook_Source_Output);
        /* Hook_Source_Output - relative to project */
        this->Hook_Source_Overwrite=Main::XML_Get_Yesno(Root,"Hook_Source_Overwrite","DXXXX","DXXXX");
        /* Handler_Source_Output - relative to project */
        this->Handler_Source_Output=Main::XML_Get_String(Root,"Handler_Source_Output","DXXXX","DXXXX");
        this->Handler_Source_Output=Main::Path_Absolute(PATH_DIR, this->Project_Output, this->Handler_Source_Output);
        /* Handler_Source_Overwrite */
        this->Handler_Source_Overwrite=Main::XML_Get_Yesno(Root,"Handler_Source_Overwrite","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Kernel:\n")+Exc.what());
    }
}
/* End Function:Kernel::Kernel ***********************************************/

/* Function:Kernel::Mem_Alloc *************************************************
Description : Allocate the memory for kernel itself.
Input       : ptr_t Kom_Front - The current kernel memory frontier.
              ptr_t Vector_Num - The number of physical vectors.
              ptr_t Event_Num - The number of event sources.
              ptr_t Wordlength - The processor wordlength.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel::Mem_Alloc(ptr_t Kom_Front, ptr_t Vector_Num, ptr_t Event_Num, ptr_t Wordlength)
{
    ptr_t Real_Kom_Order;

    /* Vector flag section - cut out from the data section */
    this->Vctf_Size=Proj_Info::Flag_Alloc(Vector_Num, Wordlength, this->Kom_Order);
    this->Vctf_Base=this->Data_Base+this->Data_Size-this->Vctf_Size;
    Main::Info("> Vector flag base 0x%llX size 0x%llX.", this->Vctf_Base, this->Vctf_Size);
    if(this->Vctf_Base<=this->Data_Base)
        Main::Error("XXXXX: Kernel data section is not big enough, unable to allocate vector flags.");
    this->Data_Size=this->Vctf_Base-this->Data_Base;

    /* Event flag section - cut out from the data section */
    this->Evtf_Size=Proj_Info::Flag_Alloc(Event_Num, Wordlength, this->Kom_Order);
    this->Evtf_Base=this->Data_Base+this->Data_Size-this->Evtf_Size;
    Main::Info("> Event flag base 0x%llX size 0x%llX.", this->Evtf_Base, this->Evtf_Size);
    if(this->Evtf_Base<=this->Data_Base)
        Main::Error("XXXXX: Kernel data section is not big enough, unable to allocate event flags.");
    this->Data_Size=this->Evtf_Base-this->Data_Base;

    /* Stack section - cut out from the data section */
    this->Stack_Size=ROUND_UP_POW2(this->Stack_Size,Kom_Order);
    this->Stack_Base=this->Data_Base+this->Data_Size-this->Stack_Size;
    Main::Info("> Stack base 0x%llX size 0x%llX.", this->Stack_Base, this->Stack_Size);
    if(this->Stack_Base<=this->Data_Base)
        Main::Error("XXXXX: Kernel data section is not big enough, unable to allocate kernel stack.");
    this->Data_Size=this->Stack_Base-this->Data_Base;

    /* Kernel memory section - cut out from the data section - alignment order at least 6 */
    if(this->Kom_Order>6)
        Real_Kom_Order=this->Kom_Order;
    else
        Real_Kom_Order=6;

    this->Kom_Size=Kom_Front+this->Extra_Kom;
    this->Kom_Size=ROUND_UP_POW2(this->Kom_Size, Real_Kom_Order);
    this->Kom_Base=this->Data_Base+this->Data_Size-this->Kom_Size;
    this->Kom_Base=ROUND_DOWN_POW2(this->Kom_Base, Real_Kom_Order);
    Main::Info("> Kom base 0x%llX size 0x%llX.", this->Kom_Base, this->Kom_Size);
    if(this->Kom_Base<=this->Data_Base)
        Main::Error("XXXXX: Kernel data section is not big enough, unable to allocate kernel object memory.");

    /* Kernel data section - whatever is left */
    this->Data_Size=this->Kom_Base-this->Data_Base;
    Main::Info("> Data base 0x%llX size 0x%llX.", this->Data_Base, this->Data_Size);
}
/* End Function:Kernel::Mem_Alloc ********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
