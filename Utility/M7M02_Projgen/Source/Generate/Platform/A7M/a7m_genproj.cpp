/******************************************************************************
Filename    : rme_genproj.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The project folder generation class for ARMv7-M. This will be called
              only once for the whole project.
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
#include "Gen/rme_genproj.hpp"

#include "A7M/rme_a7m_tc_gcc.hpp"
#include "A7M/rme_a7m_tc_armc5.hpp"
#include "A7M/rme_a7m_ide_keil.hpp"
#include "A7M/rme_a7m_ide_eclipse.hpp"
#include "A7M/rme_a7m_ide_makefile.hpp"
#include "Generate/Platform/A7M/rme_a7m_genproj.hpp"
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
#include "Gen/rme_genproj.hpp"

#include "A7M/rme_a7m_tc_gcc.hpp"
#include "A7M/rme_a7m_tc_armc5.hpp"
#include "A7M/rme_a7m_ide_keil.hpp"
#include "A7M/rme_a7m_ide_eclipse.hpp"
#include "A7M/rme_a7m_ide_makefile.hpp"
#include "Generate/Platform/A7M/rme_a7m_genproj.hpp"
#include "A7M/rme_a7m.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace RVM_GEN
{
/* Begin Function:A7M_Proj_Gen::Plat_Gen **************************************
Description : Generate platform-related portion of the whole project.
              This usually only includes the outer project itself.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_Proj_Gen::Plat_Gen(void)
{
    if(*(this->Main->Format)=="keil")
        A7M_IDE_Keil::Workspace_Proj(this->Main);
    else if(*(this->Main->Format)=="eclipse")
        A7M_IDE_Eclipse::Workspace_Proj(this->Main);
    else if(*(this->Main->Format)=="makefile")
        A7M_IDE_Makefile::Workspace_Proj(this->Main);
    else
        throw std::runtime_error("A7M:\nA0600: This output format is not supported.");
}
/* End Function:A7M_Proj_Gen::Plat_Gen ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
