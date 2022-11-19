/******************************************************************************
Filename    : rme_a7m_tc_gcc.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The gcc port.
******************************************************************************/

/* Includes ******************************************************************/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

extern "C"
{
#include "xml.h"
#include "pbfs.h"
}

#include "list"
#include "string"
#include "memory"
#include "vector"
#include "stdexcept"

#define __HDR_DEFS__
#include "Main/rme_mcu.hpp"
#include "Main/rme_fsys.hpp"
#include "Main/rme_chip.hpp"
#include "Main/rme_comp.hpp"
#include "Main/rme_raw.hpp"
#include "Main/rme_mem.hpp"

#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_captbl.hpp"
#include "Kobj/rme_pgtbl.hpp"
#include "Kobj/rme_thd.hpp"
#include "Kobj/rme_inv.hpp"
#include "Kobj/rme_port.hpp"
#include "Kobj/rme_recv.hpp"
#include "Kobj/rme_send.hpp"
#include "Kobj/rme_vect.hpp"
#include "Kobj/rme_kern.hpp"
#include "Kobj/rme_proc.hpp"

#include "Main/rme_proj.hpp"

#include "Gen/rme_doc.hpp"
#include "Gen/rme_genrme.hpp"

#include "A7M/rme_a7m_tc_gcc.hpp"
#include "A7M/rme_a7m.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
#include "Main/rme_mcu.hpp"
#include "Main/rme_fsys.hpp"
#include "Main/rme_chip.hpp"
#include "Main/rme_comp.hpp"
#include "Main/rme_raw.hpp"
#include "Main/rme_mem.hpp"

#include "Kobj/rme_kobj.hpp"
#include "Kobj/rme_captbl.hpp"
#include "Kobj/rme_pgtbl.hpp"
#include "Kobj/rme_thd.hpp"
#include "Kobj/rme_inv.hpp"
#include "Kobj/rme_port.hpp"
#include "Kobj/rme_recv.hpp"
#include "Kobj/rme_send.hpp"
#include "Kobj/rme_vect.hpp"
#include "Kobj/rme_kern.hpp"
#include "Kobj/rme_proc.hpp"

#include "Main/rme_proj.hpp"

#include "Gen/rme_doc.hpp"
#include "Gen/rme_genrme.hpp"

#include "A7M/rme_a7m_tc_gcc.hpp"
#include "A7M/rme_a7m.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:A7M_TC_Gcc::RME_Lds *****************************************
Description : Generate the RME linker script for A7M port.
Input       : class Main* Main - The main class.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Gcc::RME_Lds(class Main* Main)
{
        
}
/* End Function:A7M_TC_Gcc::RME_Lds ******************************************/

/* Begin Function:A7M_TC_Gcc::RME_Asm *****************************************
Description : Generate the RME files for keil uvision.
                This includes the platform-specific assembly file and the scatter.
Input       : class Main* Main - The main class.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Gcc::RME_Asm(class Main* Main)
{

}
/* End Function:A7M_TC_Gcc::RME_Asm ******************************************/

/* Begin Function:A7M_TC_Gcc::RVM_Lds *****************************************
Description : Generate the RVM linker script for A7M port.
Input       : class Main* Main - The main class.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Gcc::RVM_Lds(class Main* Main)
{

}
/* End Function:A7M_TC_Gcc::RVM_Lds ******************************************/

/* Begin Function:A7M_TC_Gcc::RVM_Asm *****************************************
Description : Generate the RVM files for keil uvision.
              This includes the platform-specific assembly file and the scatter.
Input       : class Main* Main - The main class.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Gcc::RVM_Asm(class Main* Main)
{

}
/* End Function:A7M_TC_Gcc::RVM_Asm ******************************************/

/* Begin Function:A7M_TC_Gcc::Proc_Lds ****************************************
Description : Generate the RVM linker script for A7M port.
Input       : class Main* Main - The main class.
                class Proc* Proc - The process we are generating for.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Gcc::Proc_Lds(class Main* Main, class Proc* Proc)
{

}
/* End Function:A7M_TC_Gcc::Proc_Lds *****************************************/

/* Begin Function:A7M_TC_Gcc::Proc_Asm **************************************
Description : Generate the RVM files for keil uvision.
              This includes the platform-specific assembly file and the scatter.
Input       : class Main* Main - The main class.
              class Proc* Proc - The process we are generating for.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Gcc::Proc_Asm(class Main* Main, class Proc* Proc)
{

}
/* End Function:A7M_TC_Gcc::Proc_Asm *****************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
