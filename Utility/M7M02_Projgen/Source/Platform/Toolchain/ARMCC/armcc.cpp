/******************************************************************************
Filename    : rme_a7m_tc_armc5.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The ARM toolchain v5 port.
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

#include "A7M/rme_a7m_tc_armc5.hpp"
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

#include "A7M/rme_a7m_tc_armc5.hpp"
#include "A7M/rme_a7m.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace rme_mcu
{
/* Begin Function:A7M_TC_Armc5::RME_Lds ***************************************
Description : Generate the RME linker script for A7M port.
Input       : class Main* Main - The main class.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Armc5::RME_Lds(class Main* Main)
{
    FILE* File;
    std::unique_ptr<std::string> Date;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;

    Date=Doc::Date();

    Doc=std::make_unique<class Doc>();
    Para=std::make_unique<class Para>("Scatter");
    Para->Add(";******************************************************************************");
    Para->Add(";Filename    : rme_platform_%s.sct", Main->Chip->Chip_Class->c_str());
    Para->Add(";Author      : " CODE_AUTHOR);
    Para->Add(";Date        : %s",Date->c_str());
    Para->Add(";Licence     : " CODE_LICENSE);
    Para->Add(";Description : The scatter file for ARMv7-M layout. This file is intended");
    Para->Add(";              to be used with %s.", Main->Chip->Chip_Class->c_str());
    Para->Add(";******************************************************************************");
    Para->Add("");
    Para->Add("; Begin Segment:KERNEL ********************************************************");
    Para->Add("; Description : The kernel segment, this is where the kernel of the operating");
    Para->Add(";               system is located at. This segment also contains the .bss(ZI)");
    Para->Add(";               for processes.");
    Para->Add("; *****************************************************************************");
    Para->Add("KERNEL 0x%llX 0x%llX", Main->Proj->RME->Map->Code_Base, Main->Proj->RME->Map->Code_Size);
    Para->Add("{");
    Para->Add("    ; Kernel code segment");
    Para->Add("    KERNEL_CODE 0x%llX 0x%llX", Main->Proj->RME->Map->Code_Base, Main->Proj->RME->Map->Code_Size);
    Para->Add("    {");
    Para->Add("        ; The reset vectors");
    Para->Add("        *.o                            (RESET, +First)");
    Para->Add("        ; The lib code copying code");
    Para->Add("        *                              (InRoot$$Sections)");
    Para->Add("        ; The ARM C library code - all of them is here");
    Para->Add("        *armlib*                       (+RO)");
    Para->Add("        ; The kernel code section");
    Para->Add("        .ANY                           (+RO)");
    Para->Add("    }");
    Para->Add("");
    Para->Add("    ; Kernel stack segment");
    Para->Add("    KERNEL_STACK 0x%llX 0x%llX", Main->Proj->RME->Map->Stack_Base, Main->Proj->RME->Map->Stack_Size);
    Para->Add("    {");
    Para->Add("        rme_platform_a7m_asm.o             (HEAP)");
    Para->Add("        rme_platform_a7m_asm.o             (STACK)");
    Para->Add("    }");
    Para->Add("");
    Para->Add("    ; Initial kernel data segment");
    Para->Add("    KERNEL_INIT 0x%llX 0x%llX", Main->Proj->RME->Map->Data_Base, Main->Proj->RME->Map->Data_Size);
    Para->Add("    {");
    Para->Add("        .ANY                           (+RW +ZI)");
    Para->Add("    }");
    Para->Add("");
    Para->Add("    ; Dynamically managed kernel data segment");
    Para->Add("    KERNEL_DATA 0x%llX EMPTY 0x%llX", Main->Proj->RME->Map->Kmem_Base, Main->Proj->RME->Map->Kmem_Size);
    Para->Add("    {");
    Para->Add("");
    Para->Add("    }");
    Para->Add("}");
    Para->Add("; End Segment:KERNEL **********************************************************");
    Para->Add("");
    Para->Add("; Begin Segment:INIT **********************************************************");
    Para->Add("; Description : The .text segment of process INIT. Init is the first process");
    Para->Add(";               in the system, and should act as a memory manager itself.");
    Para->Add("; *****************************************************************************");
    Para->Add("INIT 0x%llX 0x%llX", Main->Proj->RVM->Map->Code_Base, Main->Proj->RVM->Map->Code_Size);
    Para->Add("{");
    Para->Add("    ; Init process code segment");
    Para->Add("    INIT_CODE 0x%llX 0x%llX", Main->Proj->RVM->Map->Code_Base, Main->Proj->RVM->Map->Code_Size);
    Para->Add("    {");
    Para->Add("        RVM.o                          (+RO)");
    Para->Add("    }");
    Para->Add("}");
    Para->Add("; End Segment:INIT ************************************************************");

    for(std::unique_ptr<class Proc>& Proc:Main->Proj->Proc)
    {
        Para->Add("");
        Para->Add("; Begin Segment:Proc **********************************************************");
        Para->Add("; Description : The .text segment of the virtual machines. All virtual machines");
        Para->Add("; needs to be placed here; If you don't want to convert them to C and compile to");
        Para->Add("; a whole image, flash them to this address.");
        Para->Add("; *****************************************************************************");
        Para->Add("%s 0x%llX 0x%llX", Proc->Name->c_str(), Proc->Map->Code_Base, Proc->Map->Code_Size);
        Para->Add("{");
        Para->Add("    ; Init process code segment");
        Para->Add("    %s_CODE 0x%llX 0x%llX", Proc->Name->c_str(), Proc->Map->Code_Base, Proc->Map->Code_Size);
        Para->Add("    {");
        Para->Add("        %s.o                         (+RO)", Proc->Name->c_str());
        Para->Add("    }");
        Para->Add("}");
        Para->Add("; End Segment:VM **************************************************************");
    }
    Para->Add("");
    Para->Add("; End Of File *****************************************************************");
    Para->Add("");
    Para->Add("; Copyright (C) Evo-Devo Instrum. All rights reserved *************************");
    Para->Add("");
    Doc->Add(std::move(Para));

    /* After we finish all these, we go back and populate the re-read file functionality */
    File=Main->Dstfs->Open_File("M7M1_MuEukaron/MEukaron/Include/Platform/%s/Chips/%s/rme_platform_%s.sct",
                                Main->Proj->Plat_Name->c_str(),
                                Main->Chip->Chip_Class->c_str(),
                                Main->Chip->Chip_Class->c_str());
    Doc->Write(File);
    fclose(File);
}
/* End Function:A7M_TC_Armc5::RME_Lds ****************************************/

/* Begin Function:A7M_TC_Armc5::RME_Asm ***************************************
Description : Generate the RME files for keil uvision. 
              This includes the platform-specific assembly file and the scatter.
Input       : class Main* Main - The main class.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Armc5::RME_Asm(class Main* Main)
{
    class A7M* A7M;

    A7M=static_cast<class A7M*>(Main->Plat.get());

    if(A7M->CPU_Type==A7M_CPU_CM0P)
        Main->Srcfs->Copy_File("M7M1_MuEukaron/MEukaron/Platform/A7M/rme_platform_a7m0p_asm.s");
    else
        Main->Srcfs->Copy_File("M7M1_MuEukaron/MEukaron/Platform/A7M/rme_platform_a7m_asm.s");
}
/* End Function:A7M_TC_Armc5::RME_Asm ****************************************/

/* Begin Function:A7M_TC_Armc5::RVM_Lds ***************************************
Description : Generate the RVM linker script for A7M port.
Input       : class Main* Main - The main class.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Armc5::RVM_Lds(class Main* Main)
{
    FILE* File;
    std::unique_ptr<std::string> Date;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;

    Date=Doc::Date();

    Doc=std::make_unique<class Doc>();
    Para=std::make_unique<class Para>("Scatter");
    Para->Add(";******************************************************************************");
    Para->Add(";Filename    : rvm_platform_%s.sct", Main->Chip->Chip_Class->c_str());
    Para->Add(";Author      : " CODE_AUTHOR);
    Para->Add(";Date        : %s",Date->c_str());
    Para->Add(";Licence     : " CODE_LICENSE);
    Para->Add(";Description : The scatter file for ARMv7-M layout. This file is intended");
    Para->Add(";              to be used with %s.", Main->Chip->Chip_Class->c_str());
    Para->Add(";******************************************************************************");
    Para->Add("");
    Para->Add("; Begin Segment:INIT **********************************************************");
    Para->Add("; Description : The init segment, which contains the core of the user-level library.");
    Para->Add("; *****************************************************************************");
    Para->Add("INIT 0x%llX 0x%llX", Main->Proj->RVM->Map->Code_Base, Main->Proj->RVM->Map->Code_Size);
    Para->Add("{");
    Para->Add("    ; The code segment of the INIT process");
    Para->Add("    INIT_CODE 0x%llX 0x%llX", Main->Proj->RVM->Map->Code_Base, Main->Proj->RVM->Map->Code_Size);
    Para->Add("    {");
    Para->Add("        ; The entry will be the first instruction");
    Para->Add("        *.o                    (ARCH, +First)");
    Para->Add("        ; The lib code copying code");
    Para->Add("        *                      (InRoot$$Sections)");
    Para->Add("        ; The init code section");
    Para->Add("        .ANY                   (+RO)");
    Para->Add("    }");
    Para->Add("");
    Para->Add("    ; The data section of the INIT process");
    Para->Add("    INIT_DATA 0x%llX 0x%llX", Main->Proj->RVM->Map->Data_Base, Main->Proj->RVM->Map->Data_Size);
    Para->Add("    {");
    Para->Add("        .ANY                   (+RW +ZI)");
    Para->Add("    }");
    Para->Add("}");
    Para->Add("; End Segment:INIT ************************************************************");
    Para->Add("");
    Para->Add("; End Of File *****************************************************************");
    Para->Add("");
    Para->Add("; Copyright (C) Evo-Devo Instrum. All rights reserved *************************");
    Para->Add("");
    Doc->Add(std::move(Para));

    /* After we finish all these, we go back and populate the re-read file functionality */
    File=Main->Dstfs->Open_File("M7M2_MuAmmonite/MAmmonite/Include/Platform/%s/Chips/%s/rvm_platform_%s.sct",
                                Main->Proj->Plat_Name->c_str(),
                                Main->Chip->Chip_Class->c_str(),
                                Main->Chip->Chip_Class->c_str());
    Doc->Write(File);
    fclose(File);
}
/* End Function:A7M_TC_Armc5::RVM_Lds ****************************************/

/* Begin Function:A7M_TC_Armc5::RVM_Asm ***************************************
Description : Generate the RVM files for keil uvision. 
              This includes the platform-specific assembly file and the scatter.
Input       : class Main* Main - The main class.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Armc5::RVM_Asm(class Main* Main)
{
    class A7M* A7M;

    A7M=static_cast<class A7M*>(Main->Plat.get());

    if(A7M->CPU_Type==A7M_CPU_CM0P)
        Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Platform/A7M/rvm_platform_a7m0p_asm.s");
    else
        Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Platform/A7M/rvm_platform_a7m_asm.s");
}
/* End Function:A7M_TC_Armc5::RVM_Asm ****************************************/

/* Begin Function:A7M_TC_Armc5::Proc_Lds **************************************
Description : Generate the process linker script for A7M port.
Input       : class Main* Main - The main class.
              class Proc* Proc - The process we are generating for.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Armc5::Proc_Lds(class Main* Main, class Proc* Proc)
{
    FILE* File;
    std::unique_ptr<std::string> Date;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;
    std::unique_ptr<std::string> Proc_Lower;

    Proc_Lower=std::make_unique<std::string>(*(Proc->Name));
    Kobj::To_Lower(Proc_Lower);

    Date=Doc::Date();

    Doc=std::make_unique<class Doc>();
    Para=std::make_unique<class Para>("Scatter");
    Para->Add(";******************************************************************************");
    Para->Add(";Filename    : proc_%s.sct", Proc_Lower->c_str());
    Para->Add(";Author      : " CODE_AUTHOR);
    Para->Add(";Date        : %s",Date->c_str());
    Para->Add(";Licence     : " CODE_LICENSE);
    Para->Add(";Description : The scatter file for the process.");
    Para->Add(";******************************************************************************");
    Para->Add("");
    Para->Add("; Begin Segment:PROC **********************************************************");
    Para->Add("; Description : The process segment, where the current process is located at.");
    Para->Add("; *****************************************************************************");
    Para->Add("PROC 0x%llX 0x%llX", Proc->Map->Code_Base, Proc->Map->Code_Size);
    Para->Add("{");
    Para->Add("    ; The code segment of the process");
    Para->Add("    PROC_CODE 0x%llX 0x%llX", Proc->Map->Code_Base, Proc->Map->Code_Size);
    Para->Add("    {");
    Para->Add("        ; The entry will be the first instruction");
    Para->Add("        rvm_proc_asm.o         (ARCH, +First)");
    Para->Add("        ; The lib code copying code");
    Para->Add("        *                      (InRoot$$Sections)");
    Para->Add("        ; The init code section");
    Para->Add("        .ANY                   (+RO)");
    Para->Add("    }");
    Para->Add("");
    Para->Add("    ; The data section of the process");
    Para->Add("    PROC_DATA 0x%llX 0x%llX", Proc->Map->Data_Base, Proc->Map->Data_Size);
    Para->Add("    {");
    Para->Add("        .ANY                   (+RW +ZI)");
    Para->Add("    }");
    Para->Add("}");
    Para->Add("; End Segment:PROC ************************************************************");
    Para->Add("");
    Para->Add("; End Of File *****************************************************************");
    Para->Add("");
    Para->Add("; Copyright (C) Evo-Devo Instrum. All rights reserved *************************");
    Para->Add("");
    Doc->Add(std::move(Para));

    /* After we finish all these, we go back and populate the re-read file functionality */
    File=Main->Dstfs->Open_File("Proc_%s/Source/proc_%s.sct", Proc->Name->c_str(), Proc_Lower->c_str());
    Doc->Write(File);
    fclose(File);
}
/* End Function:A7M_TC_Armc5::Proc_Lds ***************************************/

/* Begin Function:A7M_TC_Armc5::Proc_Asm **************************************
Description : Generate the process files for keil uvision. 
              This includes the platform-specific assembly file and the generated
              stub for all threads/ports.
Input       : class Main* Main - The main class.
              class Proc* Proc - The process we are generating for.
Output      : None.
Return      : None.
******************************************************************************/
void A7M_TC_Armc5::Proc_Asm(class Main* Main, class Proc* Proc)
{
    class A7M* A7M;
    s8_t* From;
    s8_t* To;
    FILE* File;
    std::unique_ptr<std::string> Date;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;

    Date=Doc::Date();

    /* Copy the guest assembly */
    From=new s8_t[BUF_SIZE];
    To=new s8_t[BUF_SIZE];
    A7M=static_cast<class A7M*>(Main->Plat.get());

    if(A7M->CPU_Type==A7M_CPU_CM0P)
    {
        sprintf(From, "M7M2_MuAmmonite/MAmmonite/Guest/A7M/rvm_guest_a7m0p_asm.s");
        sprintf(To, "Proc_%s/Source/rvm_guest_a7m0p_asm.s", Proc->Name->c_str());
        Main->Srcfs->Copy_File(From, To);
    }
    else
    {
        sprintf(From, "M7M2_MuAmmonite/MAmmonite/Guest/A7M/rvm_guest_a7m_asm.s");
        sprintf(To, "Proc_%s/Source/rvm_guest_a7m_asm.s", Proc->Name->c_str());
        Main->Srcfs->Copy_File(From, To);
    }

    delete[] From;
    delete[] To;

    /* Construct the guest assembly stub file */
    Doc=std::make_unique<class Doc>();
    Para=std::make_unique<class Para>("Assembly");
    Para->Add(";/*****************************************************************************");
    Para->Add(";Filename    : rvm_proc_asm.s");
    Para->Add(";Author      : " CODE_AUTHOR);
    Para->Add(";Date        : %s",Date->c_str());
    Para->Add(";Licence     : " CODE_LICENSE);
    Para->Add(";Description : The assembly stub file for the process.");
    Para->Add(";*****************************************************************************/");
    Para->Add("");
    Para->Add(";/* Begin Header *************************************************************/");
    Para->Add("    ;The align is \"(2^3)/8=1(Byte).\" In fact it does not take effect.");
    Para->Add("    AREA                ARCH,CODE,READONLY,ALIGN=3");
    Para->Add("    THUMB");
    Para->Add("    REQUIRE8");
    Para->Add("    PRESERVE8");
    Para->Add(";/* End Header ***************************************************************/");
    Para->Add("");
    Para->Add(";/* Begin Imports ************************************************************/");
    Para->Add("    IMPORT              __main");
    /* Import all threads */
    Para->Add("    ;Threads");
    for(std::unique_ptr<class Thd>& Thd:Proc->Thd)
        Para->Add("    IMPORT              Thd_%s", Thd->Name->c_str());
    /* Import all ports */
    Para->Add("    ;Invocations");
    for(std::unique_ptr<class Inv>& Inv:Proc->Inv)
        Para->Add("    IMPORT              Inv_%s", Inv->Name->c_str());
    Para->Add(";/* End Imports **************************************************************/");
    Para->Add("");
    Para->Add(";/* Begin Exports ************************************************************/");
    Para->Add("    EXPORT              _RVM_Jmp_Stub");
    Para->Add("    EXPORT              RVM_Entry");
    Para->Add(";/* End Exports **************************************************************/");
    Para->Add("");
    Para->Add(";/* Begin Entry List *********************************************************/");
    for(std::unique_ptr<class Thd>& Thd:Proc->Thd)
    {
        /* If this is a virtual machine, we let the user thread (lower priority) jump to main */
        if(Proc->Type==PROC_TYPE_VIRT)
        {
            if(*(Thd->Name)=="User")
                Para->Add("    B                   __main");
            else
                Para->Add("    B                   Thd_%s", Thd->Name->c_str());
        }
        else
        {
            if(Thd.get()==Proc->Thd[0].get())
                Para->Add("    B                   __main");
            else
                Para->Add("    B                   Thd_%s", Thd->Name->c_str());
        }

        Para->Add("    SPACE               %d", 4*(ENTRY_SLOT_SIZE-1));
    }
    for(std::unique_ptr<class Inv>& Inv:Proc->Inv)
    {
        Para->Add("    B                   Inv_%s", Inv->Name->c_str());
        Para->Add("    SPACE               %d", 4*(ENTRY_SLOT_SIZE-1));
    }
    Para->Add(";/* End Entry List ***********************************************************/");
    Para->Add("");
    Para->Add(";/* Begin Function:_RVM_Jmp_Stub **********************************************");
    Para->Add(";Description : The user level stub for thread creation.");
    Para->Add(";Input       : R4 - rvm_ptr_t Entry - The entry address.");
    Para->Add(";              R5 - rvm_ptr_t Stack - The stack address that we are using now.");
    Para->Add(";Output      : None.");
    Para->Add(";Return      : None.");
    Para->Add(";*****************************************************************************/");
    Para->Add("_RVM_Jmp_Stub");
    Para->Add("    SUB                 SP,#0x40            ; In order not to destroy the stack");
    Para->Add("    MOV                 R0,R5");
    Para->Add("    BLX                 R4                  ; Branch to the actual entry address.");
    Para->Add(";/* End Function:_RVM_Jmp_Stub ***********************************************/");
    Para->Add("");
    Para->Add(";/* Begin Function:RVM_Entry **************************************************");
    Para->Add(";Description : The jump to a desired position to start initialization of the system,");
    Para->Add(";              using a certain stack address.");
    Para->Add(";Input       : rvm_ptr_t R0 - The entry.");
    Para->Add(";              rvm_ptr_t R1 - The stack base.");
    Para->Add(";              rvm_ptr_t R2 - The stack length.");
    Para->Add(";              rvm_ptr_r R3 - The parameter.");
    Para->Add(";Output      : None.");
    Para->Add(";Return      : None.");
    Para->Add(";*****************************************************************************/");
    Para->Add("RVM_Entry");
    Para->Add("    ADD                 R1,R1,R2");
    Para->Add("    SUB                 R1,#0x100");
    Para->Add("    MOV                 SP,R1");
    Para->Add("    MOV                 R4,R0");
    Para->Add("    MOV                 R0,R3");
    Para->Add("    BX                  R4");
    Para->Add(";/* End Function:RVM_Entry ***************************************************/");
    Para->Add("    END");
    Para->Add(";/* End Of File **************************************************************/");
    Para->Add("");
    Para->Add(";/* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/");
    Para->Add("");
    Doc->Add(std::move(Para));

    /* After we finish all these, we go back and populate the re-read file functionality */
    File=Main->Dstfs->Open_File("Proc_%s/Source/rvm_proc_asm.s", Proc->Name->c_str());
    Doc->Write(File);
    fclose(File);
}
/* End Function:A7M_TC_Armc5::Proc_Asm ***************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
