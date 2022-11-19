/******************************************************************************
Filename    : rme_a7m_ide_makefile.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The makefile "IDE" port.
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

#include "A7M/rme_a7m_ide_makefile.hpp"
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

#include "A7M/rme_a7m_ide_makefile.hpp"
#include "A7M/rme_a7m.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:A7M_IDE_Makefile::RME_Proj **********************************
Description : Generate the RME keil project for ARMv7-M.
Input       : class Main* Main - The main structure.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_IDE_Makefile::RME_Proj(class Main* Main)
{

}
/* End Function:A7M_IDE_Makefile::RME_Proj ***********************************/

/* Begin Function:A7M_IDE_Makefile::RVM_Proj **********************************
Description : Generate the RVM keil project for ARMv7-M.
Input       : class Main* Main - The main structure.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_IDE_Makefile::RVM_Proj(class Main* Main)
{

}
/* End Function:A7M_IDE_Makefile::RVM_Proj ***********************************/

/* Begin Function:A7M_IDE_Makefile::Proc_Proj *********************************
Description : Generate the process keil project for ARMv7-M.
Input       : class Main* Main - The main structure.
              class Proc* Proc - The process we are generating for.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_IDE_Makefile::Proc_Proj(class Main* Main, class Proc* Proc)
{

}
/* End Function:A7M_IDE_Makefile::Proc_Proj **********************************/

/* Begin Function:A7M_IDE_Makefile::Workspace_Proj ****************************
Description : Generate the makefile workspace for ARMv7-M.
Input       : class Main* Main - The main structure.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_IDE_Makefile::Workspace_Proj(class Main* Main)
{

}
/* End Function:A7M_IDE_Makefile::Workspace_Proj *****************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
