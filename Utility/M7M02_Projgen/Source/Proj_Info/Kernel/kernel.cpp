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
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "rvm_gen.hpp"
#include "Proj_Info/Kernel/kernel.hpp"
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
        /* Code base */
        this->Code_Base=Main::XML_Get_Number(Root,"Code_Base","DXXXX","DXXXX");
        /* Code size */
        this->Code_Size=Main::XML_Get_Number(Root,"Code_Size","DXXXX","DXXXX");
        /* Data base */
        this->Data_Base=Main::XML_Get_Number(Root,"Data_Base","DXXXX","DXXXX");
        /* Data size */
        this->Data_Size=Main::XML_Get_Number(Root,"Data_Size","DXXXX","DXXXX");
        /* Stack size */
        this->Stack_Size=Main::XML_Get_Number(Root,"Stack_Size","DXXXX","DXXXX");
        /* Extra kernel memory */
        this->Extra_Kmem=Main::XML_Get_Number(Root,"Extra_Kmem","DXXXX","DXXXX");
        /* Kmem_Order */
        this->Kmem_Order=Main::XML_Get_Number(Root,"Kmem_Order","DXXXX","DXXXX");
        /* Kern_Prio */
        this->Kern_Prio=Main::XML_Get_Number(Root,"Kern_Prio","DXXXX","DXXXX");

        /* Build */
        this->Build=Main::XML_Get_String(Root,"Build","DXXXX","DXXXX");
        /* Toolchain */
        this->Toolchain=Main::XML_Get_String(Root,"Toolchain","DXXXX","DXXXX");
        /* Optimization */
        this->Optimization=Main::XML_Get_String(Root,"Optimization","DXXXX","DXXXX");
        /* Kernel_Root */
        this->Kernel_Root=Main::XML_Get_String(Root,"Kernel_Root","DXXXX","DXXXX");
        /* Project_Output */
        this->Project_Output=Main::XML_Get_String(Root,"Project_Output","DXXXX","DXXXX");
        /* Project_Overwrite */
        this->Project_Overwrite=Main::XML_Get_Yesno(Root,"Project_Overwrite","DXXXX","DXXXX");
        /* Linker_Output */
        this->Linker_Output=Main::XML_Get_String(Root,"Linker_Output","DXXXX","DXXXX");
        /* Config_Header_Output */
        this->Config_Header_Output=Main::XML_Get_String(Root,"Config_Header_Output","DXXXX","DXXXX");
        /* Boot_Header_Output */
        this->Boot_Header_Output=Main::XML_Get_String(Root,"Boot_Header_Output","DXXXX","DXXXX");
        /* Boot_Source_Output */
        this->Boot_Source_Output=Main::XML_Get_String(Root,"Boot_Source_Output","DXXXX","DXXXX");
        /* Init_Source_Output */
        this->Init_Source_Output=Main::XML_Get_String(Root,"Init_Source_Output","DXXXX","DXXXX");
        /* Init_Source_Output */
        this->Init_Source_Overwrite=Main::XML_Get_Yesno(Root,"Init_Source_Overwrite","DXXXX","DXXXX");
        /* Handler_Source_Output */
        this->Handler_Source_Output=Main::XML_Get_String(Root,"Handler_Source_Output","DXXXX","DXXXX");
        /* Handler_Source_Overwrite */
        this->Handler_Source_Overwrite=Main::XML_Get_Yesno(Root,"Handler_Source_Overwrite","DXXXX","DXXXX");
    }
    catch(std::exception& Exc)
    {
        Main::Error(std::string("Kernel:\n")+Exc.what());
    }
}
/* End Function:Kernel::Kernel ***********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/