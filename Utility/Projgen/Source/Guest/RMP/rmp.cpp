/******************************************************************************
Filename    : rme_guest_rmp.cpp
Author      : pry
Date        : 20/04/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The guest operating system configuring steps for RMP.
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

#include "Guest/rme_guest_rmp.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace rme_mcu
{
/* Begin Function:Guest_RMP::Generate *****************************************
Description : Generate guest operating system RMP.
Input       : class Main* Main - The main class instance.
              class Virt* Virt - The virtual machine instance.
              std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc - Include paths.
              std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src - Source files.
Output      : None.
Return      : None.
******************************************************************************/
void Guest_RMP::Generate(class Main* Main, class Virt* Virt,
                         std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc,
                         std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src)
{
    s8_t* From;
    s8_t* To;
    FILE* File;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;

    From=new s8_t[BUF_SIZE];
    To=new s8_t[BUF_SIZE];

    /* Make necessary directories */
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/Documents", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Kernel", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Platform", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Platform/%s_RVM", 
                          Virt->Name->c_str(), Main->Proj->Plat_Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Platform/%s_RVM/Chips",
                          Virt->Name->c_str(), Main->Proj->Plat_Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Platform/%s_RVM/Chips/%s",
                          Virt->Name->c_str(), Main->Proj->Plat_Name->c_str(), Main->Proj->Chip_Class->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Kernel", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/User", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Platform", Virt->Name->c_str());
    Main->Dstfs->Make_Dir("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Platform/%s_RVM",
                          Virt->Name->c_str(), Main->Proj->Plat_Name->c_str());

    /* Create configuration header files */
    Doc=std::make_unique<class Doc>();
    Doc->Csrc_Desc("rmp_platform.h", "The platform selection header.");
    Para=std::make_unique<class Para>("Doc:Platform Includes");
    Para->Add("/* Platform Includes *********************************************************/");
    Para->Add("#include \"Platform/%s_RVM/rmp_platform_%s_rvm.h\"",
              Main->Proj->Plat_Name->c_str(),
              Main->Proj->Plat_Lower->c_str());
    Para->Add("/* End Platform Includes *****************************************************/");
    Doc->Add(std::move(Para));
    Doc->Csrc_Foot();
    /* Generate rmp_platform.h */
    File=Main->Dstfs->Open_File("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Platform/rmp_platform.h", Virt->Name->c_str());
    Doc->Write(File);
    fclose(File);

    Doc=std::make_unique<class Doc>();
    sprintf(To,"rmp_platform_%s_rvm_conf.h", Main->Proj->Plat_Lower->c_str());
    Doc->Csrc_Desc(To, "The chip selection header.");
    Para=std::make_unique<class Para>("Doc:Platform Includes");
    Para->Add("/* Platform Includes *********************************************************/");
    Para->Add("#include \"Platform/%s_RVM/Chips/%s/rmp_platform_%s_rvm.h\"",
              Main->Proj->Plat_Name->c_str(),
              Main->Proj->Chip_Class->c_str(),
              Main->Proj->Chip_Class->c_str());
    Para->Add("/* End Platform Includes *****************************************************/");
    Doc->Add(std::move(Para));
    Doc->Csrc_Foot();
    /* Generate rmp_platform_xxx_conf.h */
    File=Main->Dstfs->Open_File("Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Platform/%s_RVM/rmp_platform_%s_rvm_conf.h",
                                Virt->Name->c_str(), Main->Proj->Plat_Name->c_str(), Main->Proj->Plat_Lower->c_str());
    Doc->Write(File);
    fclose(File);

    /* Copy whatever is needed to the destination folder */
    sprintf(From, "M5P1_MuProkaron/Documents/CN_M5P1_Light-Weight-RTOS-User-Manual.pdf");
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/Documents/CN_M5P1_Light-Weight-RTOS-User-Manual.pdf", Virt->Name->c_str());
    Main->Srcfs->Copy_File(From, To);

    sprintf(From, "M5P1_MuProkaron/Documents/EN_M5P1_Light-Weight-RTOS-User-Manual.pdf");
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/Documents/EN_M5P1_Light-Weight-RTOS-User-Manual.pdf", Virt->Name->c_str());
    Main->Srcfs->Copy_File(From, To);
    
    sprintf(From, "M5P1_MuProkaron/MProkaron/Include/rmp.h");
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/rmp.h", Virt->Name->c_str());
    Main->Srcfs->Copy_File(From, To);

    sprintf(From, "M5P1_MuProkaron/MProkaron/Include/Kernel/rmp_kernel.h");
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Kernel/rmp_kernel.h", Virt->Name->c_str());
    Main->Srcfs->Copy_File(From, To);

    sprintf(From, "M5P1_MuProkaron/MProkaron/Include/Platform/%s_RVM/rmp_platform_%s_rvm.h",
            Main->Proj->Plat_Name->c_str(), Main->Proj->Plat_Lower->c_str());
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Platform/%s_RVM/rmp_platform_%s_rvm.h",
            Virt->Name->c_str(), Main->Proj->Plat_Name->c_str(), Main->Proj->Plat_Lower->c_str());
    Main->Srcfs->Copy_File(From, To);

    sprintf(From, "M5P1_MuProkaron/MProkaron/Include/Platform/%s_RVM/Chips/%s/rmp_platform_%s_rvm.h",
            Main->Proj->Plat_Name->c_str(), Main->Proj->Chip_Class->c_str(), Main->Proj->Chip_Class->c_str());
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Include/Platform/%s_RVM/Chips/%s/rmp_platform_%s_rvm.h",
            Virt->Name->c_str(), Main->Proj->Plat_Name->c_str(), Main->Proj->Chip_Class->c_str(), Main->Proj->Chip_Class->c_str());
    Main->Srcfs->Copy_File(From, To);

    sprintf(From, "M5P1_MuProkaron/MProkaron/Kernel/rmp_kernel.c");
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Kernel/rmp_kernel.c", Virt->Name->c_str());
    Main->Srcfs->Copy_File(From, To);

    sprintf(From, "M5P1_MuProkaron/MProkaron/Platform/%s_RVM/rmp_platform_%s_rvm.c", 
            Main->Proj->Plat_Name->c_str(), Main->Proj->Plat_Lower->c_str());
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Platform/%s_RVM/rmp_platform_%s_rvm.c",
            Virt->Name->c_str(), Main->Proj->Plat_Name->c_str(), Main->Proj->Plat_Lower->c_str());
    Main->Srcfs->Copy_File(From, To);

    sprintf(From, "M5P1_MuProkaron/MProkaron/Benchmark/rmp_blank_rvm.c");
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/MProkaron/User/rmp_blank.c", Virt->Name->c_str());
    Main->Srcfs->Copy_File(From, To);

    /* Add whatever should be added to project */
    /* Includes */
    Inc->push_back(std::make_unique<std::string>("../Guest/M5P1_MuProkaron/MProkaron/Include"));
    /* Source files */
    Src->push_back(std::make_unique<std::string>("../Guest/M5P1_MuProkaron/MProkaron/Kernel/rmp_kernel.c"));
    sprintf(To, "../Guest/M5P1_MuProkaron/MProkaron/Platform/%s_RVM/rmp_platform_%s_rvm.c",
            Main->Proj->Plat_Name->c_str(), Main->Proj->Plat_Lower->c_str());
    Src->push_back(std::make_unique<std::string>(To));

    /* What platform are we generating for? We currently only support keil */
    if(*(Main->Proj->Plat_Name)=="A7M")
        Guest_RMP::A7M(Main, Virt, Inc, Src);
    else
    {
        /* Generation not supported for this platform yet. */
    }

    Src->push_back(std::make_unique<std::string>("../Guest/M5P1_MuProkaron/MProkaron/User/rmp_blank.c"));

    delete[] From;
    delete[] To;
}
/* End Function:Guest::Generate **********************************************/

/* Begin Function:Guest_RMP::A7M **********************************************
Description : Generate guest operating system RMP on A7M platform.
Input       : class Main* Main - The main class instance.
              class Virt* Virt - The virtual machine instance.
              std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc - Include paths.
              std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src - Source files.
Output      : None.
Return      : None.
******************************************************************************/
void Guest_RMP::A7M(class Main* Main, class Virt* Virt,
                    std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc,
                    std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src)
{
    /* What platform are we generating for? We currently only support keil */
    if(*(Main->Format)=="keil")
    {
        Guest_RMP::A7M_Keil(Main, Virt, Inc, Src);
    }
    else
    {
        /* Generation not supported for this format yet. */
    }
}
/* End Function:Guest::Generate **********************************************/

/* Begin Function:Guest_RMP::A7M **********************************************
Description : Generate guest operating system RMP on A7M platform, with keil format.
Input       : class Main* Main - The main class instance.
              class Virt* Virt - The virtual machine instance.
              std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc - Include paths.
              std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src - Source files.
Output      : None.
Return      : None.
******************************************************************************/
void Guest_RMP::A7M_Keil(class Main* Main, class Virt* Virt,
                         std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Inc,
                         std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& Src)
{
    s8_t* From;
    s8_t* To;

    From=new s8_t[BUF_SIZE];
    To=new s8_t[BUF_SIZE];

    /* Copy whatever is needed to the destination folder, and generate selection headers */
    sprintf(From, "M5P1_MuProkaron/MProkaron/Platform/A7M_RVM/rmp_platform_a7m_rvm_asm.s");
    sprintf(To, "Proc_%s/Guest/M5P1_MuProkaron/MProkaron/Platform/A7M_RVM/rmp_platform_a7m_rvm_asm.s", Virt->Name->c_str());
    Main->Srcfs->Copy_File(From, To);
    
    /* Finally add whatever should be added to project */
    Src->push_back(std::make_unique<std::string>("../Guest/M5P1_MuProkaron/MProkaron/Platform/A7M_RVM/rmp_platform_a7m_rvm_asm.s"));

    delete[] From;
    delete[] To;
}
/* End Function:Guest::Generate **********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
