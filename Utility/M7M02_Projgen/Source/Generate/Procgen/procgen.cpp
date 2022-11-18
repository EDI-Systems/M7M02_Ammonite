/******************************************************************************
Filename    : rme_genproc.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The project folder generation class.
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
#include "Gen/rme_genproc.hpp"
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
#include "Gen/rme_genproc.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace rme_mcu
{
/* Begin Function:Proc_Gen::Folder ********************************************
Description : Setup the generic folder contents for process.
Input       : class Proc* Proc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proc_Gen::Folder(class Proc* Proc)
{
    s8_t* From;
    s8_t* To;

    /* Process directory */
    this->Main->Dstfs->Make_Dir("Proc_%s", Proc->Name->c_str());
    this->Main->Dstfs->Make_Dir("Proc_%s/Include", Proc->Name->c_str());
    this->Main->Dstfs->Make_Dir("Proc_%s/Source", Proc->Name->c_str());
    this->Main->Dstfs->Make_Dir("Proc_%s/Project", Proc->Name->c_str());

    /* If this process is a virtual machine, generate a folder for guest operating system */
    if(Proc->Type==PROC_TYPE_VIRT)
        this->Main->Dstfs->Make_Dir("Proc_%s/Guest", Proc->Name->c_str());

    From=new s8_t[BUF_SIZE];
    To=new s8_t[BUF_SIZE];
    
    /* Copy kernel interface source and header - The toolchain specific assembly be
     * created when we are playing with toolchains */
    sprintf(From, "M7M2_MuAmmonite/MAmmonite/Guest/%s/rvm_guest_%s.c", 
            this->Main->Proj->Plat_Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    sprintf(To, "Proc_%s/Source/rvm_guest_%s.c",
            Proc->Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    this->Main->Srcfs->Copy_File(From, To);
    
    sprintf(From, "M7M2_MuAmmonite/MAmmonite/Guest/%s/rvm_guest_%s.h", 
            this->Main->Proj->Plat_Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    sprintf(To, "Proc_%s/Include/rvm_guest_%s.h",
            Proc->Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    this->Main->Srcfs->Copy_File(From, To);

    /* Copy generic system call header */
    sprintf(To, "Proc_%s/Include/rvm.h", Proc->Name->c_str());
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Include/rvm.h", To);

    /* Copy generic system library interface */
    sprintf(To, "Proc_%s/Include/rvm_guest.h", Proc->Name->c_str());
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Guest/rvm_guest.h", To);
    sprintf(To, "Proc_%s/Source/rvm_guest.c", Proc->Name->c_str());
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Guest/rvm_guest.c", To);


    delete[] From;
    delete[] To;
}
/* End Function:Proc_Gen::Folder *********************************************/

/* Begin Function:Proc_Gen::Proc_Hdr ******************************************
Description : Generate the proc.h. These files are mainly user stubs, which
              the user may use to call kernel objects that this process is allowed
              to call.
Input       : class Proc* Proc - The process to generate for.
Output      : None.
Return      : None.
******************************************************************************/
void Proc_Gen::Proc_Hdr(class Proc* Proc)
{
    s8_t* Buf;
    FILE* File;
    std::unique_ptr<std::string> Macro;
    std::unique_ptr<std::string> Base;
    std::unique_ptr<std::string> Size;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;
    std::unique_ptr<std::string> Proc_Upper;
    std::unique_ptr<std::string> Proc_Lower;

    Proc_Upper=std::make_unique<std::string>(*(Proc->Name));
    Kobj::To_Upper(Proc_Upper);
    Proc_Lower=std::make_unique<std::string>(*(Proc->Name));
    Kobj::To_Lower(Proc_Lower);

    Buf=new s8_t[BUF_SIZE];
    Doc=std::make_unique<class Doc>();

    sprintf(Buf, "proc_%s.h", Proc_Lower->c_str());
    Doc->Csrc_Desc(Buf, "The user kernel object header.");

    Para=std::make_unique<class Para>("Doc:Defines");
    Para->Add("/* Defines *******************************************************************/");
    Para->Add("#ifndef __PROC_%s_H__", Proc_Upper->c_str());
    Para->Add("#define __PROC_%s_H__", Proc_Upper->c_str());
    /* Ports */
    Para->Add("/* Ports */");
    for(std::unique_ptr<class Port>& Port: Proc->Port)
        Para->Cdef(Port->Loc_Macro, (ret_t)(Port->Loc_Capid));
    Para->Add("");

    /* Receive endpoints */
    Para->Add("/* Receive endpoints */");
    for(std::unique_ptr<class Recv>& Recv: Proc->Recv)
        Para->Cdef(Recv->Loc_Macro, (ret_t)(Recv->Loc_Capid));
    Para->Add("");

    /* Send endpoints */
    Para->Add("/* Send endpoints */");
    for(std::unique_ptr<class Send>& Send: Proc->Send)
        Para->Cdef(Send->Loc_Macro, (ret_t)(Send->Loc_Capid));
    Para->Add("");

    /* Vector endpoints */
    Para->Add("/* Vector endpoints */");
    for(std::unique_ptr<class Vect>& Vect: Proc->Vect)
        Para->Cdef(Vect->Loc_Macro, (ret_t)(Vect->Loc_Capid));
    Para->Add("");

    /* Kernel function */
    Para->Add("/* Kernel function */");
    for(std::unique_ptr<class Kern>& Kern: Proc->Kern)
        Para->Cdef(Kern->Loc_Macro, (ret_t)(Kern->Loc_Capid));
    Para->Add("");

    /* Memory blocks */
    Para->Add("/* Code memory blocks */");
    for(std::unique_ptr<class Mem>& Mem: Proc->Code)
    {
        /* Is this memory segment shared or not? */
        if(Mem->Is_Shared==0)
        {
            if(Mem->Name!=nullptr)
            {
                Macro=std::make_unique<std::string>("CODE_");
                *Macro+=*(Mem->Name);
            }
            else
            {
                Macro=std::make_unique<std::string>("ACODE_");
                sprintf(Buf,"0X%llX",Mem->Base);
                *Macro+=Buf;
            }
        }
        else
        {
            Macro=std::make_unique<std::string>("SCODE_");
            *Macro+=*(Mem->Name);
        }

        Kobj::To_Upper(Macro);
        
        Base=std::make_unique<std::string>((*Macro)+"_BASE");
        Para->Cdef(Base, Mem->Base);
        Size=std::make_unique<std::string>((*Macro)+"_SIZE");
        Para->Cdef(Size, Mem->Size);
    }
    Para->Add("");

    Para->Add("/* Data memory blocks */");
    for(std::unique_ptr<class Mem>& Mem: Proc->Data)
    {
        /* Is this memory segment shared or not? */
        if(Mem->Is_Shared==0)
        {
            if(Mem->Name!=nullptr)
            {
                Macro=std::make_unique<std::string>("DATA_");
                *Macro+=*(Mem->Name);
            }
            else
            {
                Macro=std::make_unique<std::string>("ADATA_");
                sprintf(Buf,"0X%llX",Mem->Base);
                *Macro+=Buf;
            }
        }
        else
        {
            Macro=std::make_unique<std::string>("SDATA_");
            *Macro+=*(Mem->Name);
        }

        Kobj::To_Upper(Macro);
        
        Base=std::make_unique<std::string>((*Macro)+"_BASE");
        Para->Cdef(Base, Mem->Base);
        Size=std::make_unique<std::string>((*Macro)+"_SIZE");
        Para->Cdef(Size, Mem->Size);
    }
    Para->Add("");

    Para->Add("/* Device memory blocks */");
    for(std::unique_ptr<class Mem>& Mem: Proc->Device)
    {
        /* Is this memory segment shared or not? */
        if(Mem->Is_Shared==0)
        {
            if(Mem->Name!=nullptr)
            {
                Macro=std::make_unique<std::string>("DEVICE_");
                *Macro+=*(Mem->Name);
            }
            else
            {
                Macro=std::make_unique<std::string>("ADEVICE_");
                sprintf(Buf,"0X%llX",Mem->Base);
                *Macro+=Buf;
            }
        }
        else
        {
            Macro=std::make_unique<std::string>("SDEVICE_");
            *Macro+=*(Mem->Name);
        }

        Kobj::To_Upper(Macro);
        
        Base=std::make_unique<std::string>((*Macro)+"_BASE");
        Para->Cdef(Base, Mem->Base);
        Size=std::make_unique<std::string>((*Macro)+"_SIZE");
        Para->Cdef(Size, Mem->Size);
    }
    Para->Add("");

    /* If this is a virtual machine, define:
     * 1. the number of vectors, 
     * 2. the vector bitmap address, and
     * 3. the user register set address. */
    if(Proc->Type==PROC_TYPE_VIRT)
    {
        Para->Add("/* Virtual vector total number */");
        Para->Cdef("RVM_VIRT_VECT_NUM", (ret_t)(static_cast<class Virt*>(Proc)->Vect_Num));
        Para->Add("/* Virtual vector bitmap base address */");
        Para->Cdef("RVM_VIRT_VCTF_BASE", static_cast<class Virt*>(Proc)->Virt_Map->Vctf_Base);
        Para->Add("/* Virtual register base address */");
        Para->Cdef("RVM_VIRT_REG_BASE", static_cast<class Virt*>(Proc)->Virt_Map->Reg_Base);
        Para->Add("/* Hypercall parameter base address */");
        Para->Cdef("RVM_VIRT_PARAM_BASE", static_cast<class Virt*>(Proc)->Virt_Map->Param_Base);
    }

    /* The number of MPU regions */
    Para->Add("/* The number of MPU regions */");
    Para->Cdef("RVM_MPU_REGIONS", (ret_t)(this->Main->Chip->Regions));
    /* The kernel memory slot order */
    Para->Add("/* The kernel memory allocation granularity order */");
    Para->Cdef("RVM_KMEM_SLOT_ORDER", (ret_t)(this->Main->Proj->RME->Kmem_Order));
    
    Para->Add("#endif /* __PROC_%s_H__ */", Proc_Upper->c_str());
    Para->Add("/* End Defines ***************************************************************/");

    Doc->Add(std::move(Para));
    Doc->Csrc_Foot();

    /* Generate proc_xxx.h */
    File=this->Main->Dstfs->Open_File("Proc_%s/Include/proc_%s.h",Proc->Name->c_str(),Proc_Lower->c_str());
    Doc->Write(File);
    fclose(File);

    /* Generate platform selection header */
    Doc=std::make_unique<class Doc>();
    Doc->Csrc_Desc("rvm_guest_conf.h", "The guest library configuration header.");
    Para=std::make_unique<class Para>("Doc:Includes");
    Para->Add("/* Includes ******************************************************************/");
    Para->Add("#include \"proc_%s.h\"", Proc_Lower->c_str());
    Para->Add("#include \"rvm_guest_%s.h\"", this->Main->Proj->Plat_Lower->c_str());
    Para->Add("/* End Includes **************************************************************/");
    Doc->Add(std::move(Para));
    Doc->Csrc_Foot();
    File=this->Main->Dstfs->Open_File("Proc_%s/Include/rvm_guest_conf.h",Proc->Name->c_str());
    Doc->Write(File);
    fclose(File);

    delete[] Buf;
}
/* End Function:Proc_Gen::Proc_Hdr *******************************************/

/* Begin Function:Proc_Gen::Proc_Src ******************************************
Description : Generate the proc_xxx.c. This file is mainly responsible for user stubs.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void Proc_Gen::Proc_Src(class Proc* Proc)
{
    s8_t* Buf;
    FILE* File;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;
    std::unique_ptr<std::string> Proc_Lower;

    Proc_Lower=std::make_unique<std::string>(*(Proc->Name));
    Kobj::To_Lower(Proc_Lower);

    Buf=new s8_t[BUF_SIZE];
    Doc=std::make_unique<class Doc>();
    
    sprintf(Buf, "proc_%s.c", Proc_Lower->c_str());
    Doc->Csrc_Desc(Buf, "The user stub file.");

    /* Print all header includes */
    Para=std::make_unique<class Para>("Doc:Includes");
    Para->Add("/* Includes ******************************************************************/");
    Para->Add("#include \"rvm.h\"");
    Para->Add("#include \"proc_%s.h\"", Proc_Lower->c_str());
    Para->Add("#include \"rvm_guest.h\"");
    Para->Add("/* End Includes **************************************************************/");
    Doc->Add(std::move(Para));

    /* Private prototypes */
    Para=std::make_unique<class Para>("Doc:Private C Function Prototypes");
    Para->Add("/* Private C Function Prototypes *********************************************/");
    /* All threads */
    Para->Add("/* Threads */");
    for(std::unique_ptr<class Thd>& Thd:Proc->Thd)
        Para->Add("rvm_ret_t Thd_%s(rvm_ret_t Param);", Thd->Name->c_str());

    /* All ports */
    Para->Add("/* Invocations */");
    for(std::unique_ptr<class Inv>& Inv:Proc->Inv)
        Para->Add("rvm_ret_t Inv_%s(rvm_ret_t Param);", Inv->Name->c_str());
    Para->Add("");
    Para->Add("/* End Private C Function Prototypes *****************************************/");
    Doc->Add(std::move(Para));

    /* Thread functions themselves */
    for(std::unique_ptr<class Thd>& Thd:Proc->Thd)
    {
        Para=std::make_unique<class Para>("Func:Thd_%s", Thd->Name->c_str());
        Input.push_back(std::make_unique<std::string>("rvm_ret_t Param - The parameter, which is supplied by the caller."));
        sprintf(Buf, "Thd_%s", Thd->Name->c_str());
        Para->Cfunc_Desc(Buf, "The function body for thread.", Input, Output, "rvm_ret_t - Should never return.");
        Input.clear();
        Output.clear();
        Para->Add("rvm_ret_t Thd_%s(rvm_ret_t Param)", Thd->Name->c_str());
        Para->Add("{");
        /* If this process is a virtual machine, we generate something different */
        if(Proc->Type==PROC_TYPE_VIRT)
        {
            if(*(Thd->Name)=="Vect")
            {
                Para->Add("    /* DO NOT MODIFY - ESSENTIAL FOR VIRTUAL MACHINE */");
                Para->Add("    RVM_Virt_Init();");
                Para->Add("    RVM_Vect_Loop();");
            }
            else if(*(Thd->Name)=="User")
            {
                Para->Add("    /* USELESS STUB - MAIN FUNCTION PROVIDED BY ORIGINAL OPERATING SYSTEM */");
            }
            else
                throw std::runtime_error("G2000: Virtual machine should not have threads other than Vect and User.");
        }
        else
            Para->Add("    /* Add your code here - Threads shall never return */");
        Para->Add("    while(1);");
        Para->Add("}");
        Para->Cfunc_Foot(std::make_unique<std::string>(Buf));
        Doc->Add(std::move(Para));
    }

    /* Invocation functions themselves */
    for(std::unique_ptr<class Inv>& Inv:Proc->Inv)
    {
        Para=std::make_unique<class Para>("Func:Inv_%s", Inv->Name->c_str());
        Input.push_back(std::make_unique<std::string>("rvm_ret_t Param - The parameter, which is supplied by the caller."));
        sprintf(Buf, "Inv_%s", Inv->Name->c_str());
        Para->Cfunc_Desc(Buf, "The function body for port.", Input, Output, "rvm_ret_t - The return value returned to the caller.");
        Input.clear();
        Output.clear();
        Para->Add("rvm_ret_t Inv_%s(rvm_ret_t Param)", Inv->Name->c_str());
        Para->Add("{");
        Para->Add("    /* Add your code here */");
        Para->Add("    return 0;");
        Para->Add("}");
        Para->Cfunc_Foot(std::make_unique<std::string>(Buf));
        Doc->Add(std::move(Para));
    }

    /* The main function - call the thread with the highest priority. 
     * If we are using a virtual machine, main function should be provided by the virtual machine */
    if(Proc->Type==PROC_TYPE_NATIVE)
    {
        Para=std::make_unique<class Para>("Func:main");
        Para->Cfunc_Desc("main", "The function body for thread.", Input, Output, "rvm_ret_t - Should never return.");
        Input.clear();
        Output.clear();
        Para->Add("rvm_ret_t main(void)");
        Para->Add("{");
        Para->Add("    RVM_Entry((rvm_ptr_t)Thd_%s, 0x%llx, 0x%llx, 0x%llx);", 
                  Proc->Thd[0]->Name->c_str(), Proc->Thd[0]->Map->Stack_Base, Proc->Thd[0]->Map->Stack_Size, Proc->Thd[0]->Param);
        Para->Add("}");
        Para->Cfunc_Foot(std::make_unique<std::string>("main"));
        Doc->Add(std::move(Para));
    }

    /* The RVM library putchar function - print character to console */
    Para=std::make_unique<class Para>("Func:RVM_Putchar");
    Input.push_back(std::make_unique<std::string>("char Char - The character to print to console."));
    Para->Cfunc_Desc("RVM_Putchar", "The character printing function for debugging.", Input, Output, "None.");
    Input.clear();
    Output.clear();
    Para->Add("void RVM_Putchar(char Char)");
    Para->Add("{");
    Para->Add("    /* Add your own code */");
    Para->Add("}");
    Para->Cfunc_Foot(std::make_unique<std::string>("RVM_Putchar"));
    Doc->Add(std::move(Para));
    
    Doc->Csrc_Foot();

    /* Generate rme_boot.c */
    File=this->Main->Dstfs->Open_File("Proc_%s/Source/proc_%s.c",Proc->Name->c_str(),Proc_Lower->c_str());
    Doc->Write(File);
    fclose(File);

    delete[] Buf;
}
/* End Function:Proc_Gen::Proc_Src *******************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
