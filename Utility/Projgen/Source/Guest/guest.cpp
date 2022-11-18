/******************************************************************************
Filename    : rme_guest.cpp
Author      : pry
Date        : 20/04/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The guest operating system configuring interface, for guest operating
              system source installation into the folder. Guest operating systems
              may have their own configuration system, and for each guest operating
              system such configuration procedures may vary.
******************************************************************************/

/* Includes ******************************************************************/
extern "C"
{
#include "xml.h"
#include "pbfs.h"
}

#include "list"
#include "string"
#include "memory"
#include "vector"
#include "iostream"
#include "iterator"
#include "stdexcept"
#include "algorithm"

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
#include "Gen/rme_genrvm.hpp"
#include "Gen/rme_genproc.hpp"
#include "Gen/rme_genproj.hpp"

#include "Guest/rme_guest.hpp"
#include "Guest/rme_guest_rmp.hpp"
#undef __HDR_DEFS__

#define __HDR_CLASSES__
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
#include "Main/rme_mcu.hpp"

#include "Gen/rme_doc.hpp"
#include "Gen/rme_genrme.hpp"
#include "Gen/rme_genrvm.hpp"
#include "Gen/rme_genproc.hpp"
#include "Gen/rme_genproj.hpp"

#include "Guest/rme_guest.hpp"
#include "Guest/rme_guest_rmp.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace rme_mcu
{
/* Begin Function:Guest::Generate *********************************************
Description : Parse the files.
Input       : class Main* Main - The main class instance.
              class Virt* Virt - The virtual machine instance.
              std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc - Include paths.
              std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src - Source files.
Output      : None.
Return      : None.
******************************************************************************/
void Guest::Generate(class Main* Main, class Virt* Virt,
                     std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc,
                     std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src)
{
    if(*(Virt->Guest)=="RMP")
    {
        Guest_RMP::Generate(Main, Virt, Inc, Src);
    }
    else if(*(Virt->Guest)=="Other")
    {
        /* We don't do anything, because the user probably wants to supply OS... */
    }
    else
    {
        /* Can't figure out what operating system this is. Whatever. */
    }
}
/* End Function:Guest::Generate **********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
