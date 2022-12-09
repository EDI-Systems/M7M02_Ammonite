/******************************************************************************
Filename    : kernel.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The kernel configuration class.
******************************************************************************/

/* Includes ******************************************************************/
#include "map"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"
#include "algorithm"

extern "C"
{
#include "xml.h"
}

#define __HDR_DEFS__
#include "rvm_gen.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/proj_info.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
#include "Mem_Info/mem_info.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:Kernel::Kernel **********************************************
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
        this->Extra_Kmem=Main::XML_Get_Number(Root,"Extra_Kmem","DXXXX","DXXXX");
        /* Kmem_Order */
        this->Kmem_Order=Main::XML_Get_Number(Root,"Kmem_Order","DXXXX","DXXXX");
        /* Kern_Prio */
        this->Kern_Prio=Main::XML_Get_Number(Root,"Kern_Prio","DXXXX","DXXXX");

        /* Buildsystem */
        this->Buildsystem=Main::XML_Get_String(Root,"Buildsystem","DXXXX","DXXXX");
        /* Toolchain */
        this->Toolchain=Main::XML_Get_String(Root,"Toolchain","DXXXX","DXXXX");
        /* Optimization */
        this->Optimization=Main::XML_Get_String(Root,"Optimization","DXXXX","DXXXX");
        /* Kernel_Root */
        this->Kernel_Root=Main::XML_Get_String(Root,"Kernel_Root","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Kernel_Root);
        /* Project_Output */
        this->Project_Output=Main::XML_Get_String(Root,"Project_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Project_Output);
        /* Project_Overwrite */
        this->Project_Overwrite=Main::XML_Get_Yesno(Root,"Project_Overwrite","DXXXX","DXXXX");
        /* Linker_Output */
        this->Linker_Output=Main::XML_Get_String(Root,"Linker_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Linker_Output);
        /* Config_Header_Output */
        this->Config_Header_Output=Main::XML_Get_String(Root,"Config_Header_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Config_Header_Output);
        /* Boot_Header_Output */
        this->Boot_Header_Output=Main::XML_Get_String(Root,"Boot_Header_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Boot_Header_Output);
        /* Boot_Source_Output */
        this->Boot_Source_Output=Main::XML_Get_String(Root,"Boot_Source_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Boot_Source_Output);
        /* Hook_Source_Output */
        this->Hook_Source_Output=Main::XML_Get_String(Root,"Hook_Source_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Hook_Source_Output);
        /* Hook_Source_Output */
        this->Hook_Source_Overwrite=Main::XML_Get_Yesno(Root,"Hook_Source_Overwrite","DXXXX","DXXXX");
        /* Handler_Source_Output */
        this->Handler_Source_Output=Main::XML_Get_String(Root,"Handler_Source_Output","DXXXX","DXXXX");
        Main::Dir_Fixup(this->Handler_Source_Output);
        /* Handler_Source_Overwrite */
        this->Handler_Source_Overwrite=Main::XML_Get_Yesno(Root,"Handler_Source_Overwrite","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Kernel:\n")+Exc.what());
    }
}
/* End Function:Kernel::Kernel ***********************************************/

/* Begin Function:Kernel::Mem_Alloc *******************************************
Description : Allocate the memory for kernel itself.
Input       : ptr_t Kmem_Front - The current kernel memory frontier.
Output      : None.
Return      : None.
******************************************************************************/
void Kernel::Mem_Alloc(ptr_t Kmem_Front, ptr_t Vector_Num, ptr_t Event_Num, ptr_t Wordlength)
{
    /* Vector flag section - cut out from the data section */
    this->Vctf_Size=Proj_Info::Flag_Alloc(Vector_Num, Wordlength, this->Kmem_Order);
    this->Vctf_Base=this->Data_Base+this->Data_Size-this->Vctf_Size;
    Main::Info("> Vector flag base 0x%llX size 0x%llX.", this->Vctf_Base, this->Vctf_Size);
    if(this->Vctf_Base<=this->Data_Base)
        Main::Error("M2100: Kernel data section is not big enough, unable to allocate vector flags.");
    this->Data_Size=this->Vctf_Base-this->Data_Base;

    /* Event flag section - cut out from the data section */
    this->Evtf_Size=Proj_Info::Flag_Alloc(Event_Num, Wordlength, this->Kmem_Order);
    this->Evtf_Base=this->Data_Base+this->Data_Size-this->Evtf_Size;
    Main::Info("> Event flag base 0x%llX size 0x%llX.", this->Evtf_Base, this->Evtf_Size);
    if(this->Evtf_Base<=this->Data_Base)
        Main::Error("M2101: Kernel data section is not big enough, unable to allocate event flags.");
    this->Data_Size=this->Evtf_Base-this->Data_Base;

    /* Stack section - cut out from the data section */
    this->Stack_Size=ROUND_UP_POW2(this->Stack_Size,Kmem_Order);
    this->Stack_Base=this->Data_Base+this->Data_Size-this->Stack_Size;
    Main::Info("> Stack base 0x%llX size 0x%llX.", this->Stack_Base, this->Stack_Size);
    if(this->Stack_Base<=this->Data_Base)
        Main::Error("M2102: Kernel data section is not big enough, unable to allocate kernel stack.");
    this->Data_Size=this->Stack_Base-this->Data_Base;

    /* Kernel memory section - cut out from the data section */
    this->Kmem_Size=Kmem_Front+this->Extra_Kmem;
    this->Kmem_Size=ROUND_UP_POW2(this->Kmem_Size,this->Kmem_Order);
    this->Kmem_Base=this->Data_Base+this->Data_Size-this->Kmem_Size;
    this->Kmem_Base=ROUND_DOWN_POW2(this->Kmem_Base,this->Kmem_Order);
    Main::Info("> Kmem base 0x%llX size 0x%llX.", this->Kmem_Base, this->Kmem_Size);
    if(this->Kmem_Base<=this->Data_Base)
        Main::Error("M2103: Kernel data section is not big enough, unable to allocate kernel object memory.");

    /* Kernel data section - whatever is left */
    this->Data_Size=this->Kmem_Base-this->Data_Base;
    Main::Info("> Data base 0x%llX size 0x%llX.", this->Data_Base, this->Data_Size);
}
/* End Function:Kernel::Mem_Alloc ********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
