/******************************************************************************
Filename    : rme_genrvm.cpp
Author      : pry
Date        : 16/07/2019
Licence     : LGPL v3+; see COPYING for details.
Description : The rvm folder generation class.
			  This generates the rvm-related, platform-agnostic files.
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
#include "Gen/rme_genrvm.hpp"
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
#include "Gen/rme_genrvm.hpp"
#undef __HDR_CLASSES__
/* End Includes **************************************************************/
namespace rme_mcu
{
/* Begin Function:RVM_Gen::Include ********************************************
Description : Generate the RVM-related include section.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add these to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Include(std::unique_ptr<class Para>& Para)
{
    /* Print includes */
    Para->Add("#include \"rvm.h\"");
    Para->Add("");
    Para->Add("#define __HDR_DEFS__");
    Para->Add("#include \"Platform/%s/rvm_platform_%s.h\"",
              this->Main->Proj->Plat_Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    Para->Add("#include \"Init/rvm_syssvc.h\"");
    Para->Add("#include \"Init/rvm_init.h\"");
    Para->Add("#include \"Init/rvm_hyper.h\"");
    Para->Add("#undef __HDR_DEFS__");
    Para->Add("");

    Para->Add("#define __HDR_STRUCTS__");
    Para->Add("#include \"Platform/%s/rvm_platform_%s.h\"",
              this->Main->Proj->Plat_Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    Para->Add("#include \"Init/rvm_syssvc.h\"");
    Para->Add("#include \"Init/rvm_init.h\"");
    Para->Add("#include \"Init/rvm_hyper.h\"");
    Para->Add("#undef __HDR_STRUCTS__");
    Para->Add("");

    Para->Add("#define __HDR_PUBLIC_MEMBERS__");
    Para->Add("#include \"Platform/%s/rvm_platform_%s.h\"",
              this->Main->Proj->Plat_Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    Para->Add("#include \"Init/rvm_syssvc.h\"");
    Para->Add("#include \"Init/rvm_init.h\"");
    Para->Add("#include \"Init/rvm_hyper.h\"");
    Para->Add("#undef __HDR_PUBLIC_MEMBERS__");
    Para->Add("");
}
/* End Function:RVM_Gen::Include *********************************************/

/* Begin Function:RVM_Gen::Folder *********************************************
Description : Setup the folder contents for RVM.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Folder(void)
{
    /* RME directory */
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/Documents");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Include");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Include/Init");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Include/Platform");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Include/Platform/%s",this->Main->Proj->Plat_Name->c_str());
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Include/Platform/%s/Chips",this->Main->Proj->Plat_Name->c_str());
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Include/Platform/%s/Chips/%s",
                                this->Main->Proj->Plat_Name->c_str(),this->Main->Chip->Chip_Class->c_str());
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Init");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Platform");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/MAmmonite/Platform/%s",this->Main->Proj->Plat_Name->c_str());
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/Project");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/Project/Source");
    this->Main->Dstfs->Make_Dir("M7M2_MuAmmonite/Project/Include");

    /* Copy kernel file, kernel header, platform file, platform header, and chip headers */
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/Documents/EN_M7M2_RT-Runtime-User-Manual.pdf");
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/Documents/CN_M7M2_RT-Runtime-User-Manual.pdf");
    /* Currently the VMM and Posix is disabled, thus only the init is copied. */
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Init/rvm_init.c");
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Init/rvm_syssvc.c");
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Init/rvm_hyper.c");
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Include/rvm.h");
    /* The toolchain specific one will be created when we are playing with toolchains */
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Platform/%s/rvm_platform_%s.c",
                                 this->Main->Proj->Plat_Name->c_str(),this->Main->Proj->Plat_Lower->c_str());
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Include/Init/rvm_init.h");
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Include/Init/rvm_syssvc.h");
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Include/Init/rvm_hyper.h");
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Include/Platform/%s/rvm_platform_%s.h",
                                 this->Main->Proj->Plat_Name->c_str(),this->Main->Proj->Plat_Lower->c_str());
    this->Main->Srcfs->Copy_File("M7M2_MuAmmonite/MAmmonite/Include/Platform/%s/Chips/%s/rvm_platform_%s.h",
                                 this->Main->Proj->Plat_Name->c_str(),
                                 this->Main->Chip->Chip_Class->c_str(),
                                 this->Main->Chip->Chip_Class->c_str());
}
/* End Function:RVM_Gen::Folder **********************************************/

/* Begin Function:RVM_Gen::Conf_Hdr *******************************************
Description : Crank the platform configuration headers for RVM.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Conf_Hdr(void)
{
    FILE* File;
    s8_t Buf[1024];
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;

    Doc=std::make_unique<class Doc>();
    Doc->Csrc_Desc("rvm_platform.h", "The platform selection header.");
    Para=std::make_unique<class Para>("Doc:Platform Includes");
    Para->Add("/* Platform Includes *********************************************************/");
    Para->Add("#include \"Platform/%s/rvm_platform_%s.h\"", this->Main->Proj->Plat_Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    Para->Add("/* End Platform Includes *****************************************************/");
    Doc->Add(std::move(Para));
    Doc->Csrc_Foot();

    /* Generate rvm_platform.h */
    File=this->Main->Dstfs->Open_File("M7M2_MuAmmonite/MAmmonite/Include/Platform/rvm_platform.h");
    Doc->Write(File);
    fclose(File);

    Doc=std::make_unique<class Doc>();
    sprintf(Buf,"rvm_platform_%s_conf.h", this->Main->Proj->Plat_Lower->c_str());
    Doc->Csrc_Desc(Buf, "The chip selection header.");
    Para=std::make_unique<class Para>("Doc:Platform Includes");
    Para->Add("/* Platform Includes *********************************************************/");
    Para->Add("#include \"Platform/%s/Chips/%s/rvm_platform_%s.h\"",
              this->Main->Proj->Plat_Name->c_str(), this->Main->Proj->Chip_Class->c_str(), this->Main->Proj->Chip_Class->c_str());
    Para->Add("/* End Platform Includes *****************************************************/");
    Doc->Add(std::move(Para));
    Doc->Csrc_Foot();

    /* Generate rvm_platform_xxx_conf.h */
    File=this->Main->Dstfs->Open_File("M7M2_MuAmmonite/MAmmonite/Include/Platform/%s/rvm_platform_%s_conf.h",
                               this->Main->Proj->Plat_Name->c_str(), this->Main->Proj->Plat_Lower->c_str());
    Doc->Write(File);
    fclose(File);
}
/* End Function:RVM_Gen::Conf_Hdr ********************************************/

/* Begin Function:RVM_Gen::Macro_Vect *****************************************
Description : Generate the macros for vectors.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Macro_Vect(std::unique_ptr<class Para>& Para)
{
    s8_t Buf[512];
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    ptr_t Capacity;
    class Vect* Vect;

    Capacity=this->Main->Plat->Capacity;

    /* Vector capability tables & Vectors */
    Para->Add("/* Vector endpoint capability tables */");
    Cap_Front=this->Main->Proj->RME->Map->Vect_Cap_Front;
    Capacity=this->Main->Plat->Capacity;
    Para->Add("/* Vector capability table capability tables */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Vect.size();Obj_Cnt+=Capacity)
    {
        sprintf(Buf, "RVM_BOOT_CTVECT%lld",Obj_Cnt/Capacity);
        Para->Cdef(std::make_unique<std::string>(Buf), Cap_Front++);
    }
    Para->Add("");
    Para->Add("/* Vectors */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Vect)
    {
        Vect=static_cast<class Vect*>(Info->Kobj);
        sprintf(Buf, "RVM_CAPID(RVM_BOOT_CTVECT%lld,%lld)", 
                     Vect->RVM_Capid/Capacity, Vect->RVM_Capid%Capacity);
        Para->Cdef(Vect->RVM_Macro, std::make_unique<std::string>(Buf));
    }
    Para->Add("");
    if(Cap_Front!=this->Main->Proj->RVM->Map->Before_Cap_Front)
        throw std::runtime_error("Vector:\nG1000: Internal capability table computation failure.");
}
/* End Function:RVM_Gen::Macro_Vect ******************************************/

/* Begin Function:RVM_Gen::Macro_Virtep ***************************************
Description : Generate the macros for virtual machine endpoints.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Macro_Virtep(std::unique_ptr<class Para>& Para)
{
    s8_t Buf[512];
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    ptr_t Capacity;

    Capacity=this->Main->Plat->Capacity;

    /* Virtual machine endpoint capability tables & Captbls */
    Cap_Front=this->Main->Proj->RVM->Map->Virtep_Cap_Front;
    Para->Add("/* Virtual machine endpoint capability tables */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->Virt.size();Obj_Cnt+=Capacity)
    {
        sprintf(Buf, "RVM_BOOT_CTVIRTEP%lld",Obj_Cnt/Capacity);
        Para->Cdef(std::make_unique<std::string>(Buf), Cap_Front++);
    }
    Para->Add("");
    if(Cap_Front!=this->Main->Proj->RVM->Map->Captbl_Cap_Front)
        throw std::runtime_error("Virtual machine endpoints:\nG1000: Internal capability table computation failure.");
}
/* End Function:RVM_Gen::Macro_Virtep ****************************************/

/* Begin Function:RVM_Gen::Macro_Captbl ***************************************
Description : Generate the macros for capability tables.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Macro_Captbl(std::unique_ptr<class Para>& Para)
{
    s8_t Buf[512];
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    ptr_t Capacity;
    class Captbl* Captbl;

    Capacity=this->Main->Plat->Capacity;

    /* Captbl capability tables & Captbls */
    Cap_Front=this->Main->Proj->RVM->Map->Captbl_Cap_Front;
    Para->Add("/* Process capability table capability tables */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Captbl.size();Obj_Cnt+=Capacity)
    {
        sprintf(Buf, "RVM_BOOT_CTCAPTBL%lld",Obj_Cnt/Capacity);
        Para->Cdef(std::make_unique<std::string>(Buf), Cap_Front++);
    }
    Para->Add("");
    Para->Add("/* Process capability tables */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Captbl)
    {
        Captbl=static_cast<class Captbl*>(Info->Kobj);
        sprintf(Buf, "RVM_CAPID(RVM_BOOT_CTCAPTBL%lld,%lld)", 
                     Captbl->RVM_Capid/Capacity, Captbl->RVM_Capid%Capacity);
        Para->Cdef(Captbl->RVM_Macro, std::make_unique<std::string>(Buf));
    }
    Para->Add("");
    if(Cap_Front!=this->Main->Proj->RVM->Map->Pgtbl_Cap_Front)
        throw std::runtime_error("Capability table:\nG1000: Internal capability table computation failure.");
}
/* End Function:RVM_Gen::Macro_Captbl ****************************************/

/* Begin Function:RVM_Gen::Macro_Pgtbl ****************************************
Description : Generate the macros for page tables.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Macro_Pgtbl(std::unique_ptr<class Para>& Para)
{
    s8_t Buf[512];
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    ptr_t Capacity;
    class Pgtbl* Pgtbl;

    Capacity=this->Main->Plat->Capacity;

    /* Pgtbl capability tables & Pgtbls */
    Cap_Front=this->Main->Proj->RVM->Map->Pgtbl_Cap_Front;
    Para->Add("/* Process page table capability tables */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Pgtbl.size();Obj_Cnt+=Capacity)
    {
        sprintf(Buf, "RVM_BOOT_CTPGTBL%lld",Obj_Cnt/Capacity);
        Para->Cdef(std::make_unique<std::string>(Buf), Cap_Front++);
    }
    Para->Add("");
    Para->Add("/* Process page tables */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Pgtbl)
    {
        Pgtbl=static_cast<class Pgtbl*>(Info->Kobj);
        sprintf(Buf, "RVM_CAPID(RVM_BOOT_CTPGTBL%lld,%lld)", 
                     Pgtbl->RVM_Capid/Capacity, Pgtbl->RVM_Capid%Capacity);
        Para->Cdef(Pgtbl->RVM_Macro, std::make_unique<std::string>(Buf));
    }
    Para->Add("");
    if(Cap_Front!=this->Main->Proj->RVM->Map->Proc_Cap_Front)
        throw std::runtime_error("Page table:\nG1000: Internal capability table computation failure.");
}
/* End Function:RVM_Gen::Macro_Pgtbl *****************************************/

/* Begin Function:RVM_Gen::Macro_Proc ****************************************
Description : Generate the macros for processes.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Macro_Proc(std::unique_ptr<class Para>& Para)
{
    s8_t Buf[512];
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    ptr_t Capacity;
    class Proc* Proc;

    Capacity=this->Main->Plat->Capacity;

    /* Process capability tables & Processes */
    Cap_Front=this->Main->Proj->RVM->Map->Proc_Cap_Front;
    Para->Add("/* Process capability tables */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Proc.size();Obj_Cnt+=Capacity)
    {
        sprintf(Buf, "RVM_BOOT_CTPROC%lld",Obj_Cnt/Capacity);
        Para->Cdef(std::make_unique<std::string>(Buf), Cap_Front++);
    }
    Para->Add("");
    Para->Add("/* Processes */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Proc)
    {
        Proc=static_cast<class Proc*>(Info->Kobj);
        sprintf(Buf, "RVM_CAPID(RVM_BOOT_CTPROC%lld,%lld)",
                     Proc->RVM_Capid/Capacity, Proc->RVM_Capid%Capacity);
        Para->Cdef(Proc->RVM_Macro, std::make_unique<std::string>(Buf));
    }
    Para->Add("");
    if(Cap_Front!=this->Main->Proj->RVM->Map->Thd_Cap_Front)
        throw std::runtime_error("Process:\nG1000: Internal capability table computation failure.");
}
/* End Function:RVM_Gen::Macro_Proc ******************************************/

/* Begin Function:RVM_Gen::Macro_Thd ****************************************
Description : Generate the macros for threads.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Macro_Thd(std::unique_ptr<class Para>& Para)
{
    s8_t Buf[512];
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    ptr_t Capacity;
    class Thd* Thd;

    Capacity=this->Main->Plat->Capacity;

    /* Thread capability tables & Threads */
    Cap_Front=this->Main->Proj->RVM->Map->Thd_Cap_Front;
    Para->Add("/* Thread capability tables */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Thd.size();Obj_Cnt+=Capacity)
    {
        sprintf(Buf, "RVM_BOOT_CTTHD%lld",Obj_Cnt/Capacity);
        Para->Cdef(std::make_unique<std::string>(Buf), Cap_Front++);
    }
    Para->Add("");
    Para->Add("/* Threads */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Thd)
    {
        Thd=static_cast<class Thd*>(Info->Kobj);
        sprintf(Buf, "RVM_CAPID(RVM_BOOT_CTTHD%lld,%lld)",
                     Thd->RVM_Capid/Capacity, Thd->RVM_Capid%Capacity);
        Para->Cdef(Thd->RVM_Macro, std::make_unique<std::string>(Buf));
    }
    Para->Add("");
    if(Cap_Front!=this->Main->Proj->RVM->Map->Inv_Cap_Front)
        throw std::runtime_error("Thread:\nG1000: Internal capability table computation failure.");
}
/* End Function:RVM_Gen::Macro_Thd *******************************************/

/* Begin Function:RVM_Gen::Macro_Inv ****************************************
Description : Generate the macros for invocations.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Macro_Inv(std::unique_ptr<class Para>& Para)
{
    s8_t Buf[512];
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    ptr_t Capacity;
    class Inv* Inv;

    Capacity=this->Main->Plat->Capacity;

    /* Invocation capability tables & Invocations */
    Cap_Front=this->Main->Proj->RVM->Map->Inv_Cap_Front;
    Para->Add("/* Invocation capability tables */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Inv.size();Obj_Cnt+=Capacity)
    {
        sprintf(Buf, "RVM_BOOT_CTINV%lld",Obj_Cnt/Capacity);
        Para->Cdef(std::make_unique<std::string>(Buf), Cap_Front++);
    }
    Para->Add("");
    Para->Add("/* Invocations */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Inv)
    {
        Inv=static_cast<class Inv*>(Info->Kobj);
        sprintf(Buf, "RVM_CAPID(RVM_BOOT_CTINV%lld,%lld)",
                     Inv->RVM_Capid/Capacity, Inv->RVM_Capid%Capacity);
        Para->Cdef(Inv->RVM_Macro, std::make_unique<std::string>(Buf));
    }
    Para->Add("");
    if(Cap_Front!=this->Main->Proj->RVM->Map->Recv_Cap_Front)
        throw std::runtime_error("Invocation:\nG1000: Internal capability table computation failure.");
}
/* End Function:RVM_Gen::Macro_Inv *******************************************/

/* Begin Function:RVM_Gen::Macro_Recv ***************************************
Description : Generate the macros for receive endpoints.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Macro_Recv(std::unique_ptr<class Para>& Para)
{
    s8_t Buf[512];
    ptr_t Obj_Cnt;
    ret_t Cap_Front;
    ptr_t Capacity;
    class Recv* Recv;

    Capacity=this->Main->Plat->Capacity;

    /* Receive endpoint capability tables & Receive endpoints */
    Cap_Front=this->Main->Proj->RVM->Map->Recv_Cap_Front;
    Para->Add("/* Receive endpoint capability tables */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Recv.size();Obj_Cnt+=Capacity)
    {
        sprintf(Buf, "RVM_BOOT_CTRECV%lld",Obj_Cnt/Capacity);
        Para->Cdef(std::make_unique<std::string>(Buf), Cap_Front++);
    }
    Para->Add("");
    Para->Add("/* Receive endpoints */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Recv)
    {
        Recv=static_cast<class Recv*>(Info->Kobj);
        sprintf(Buf, "RVM_CAPID(RVM_BOOT_CTRECV%lld,%lld)",
                     Recv->RVM_Capid/Capacity, Recv->RVM_Capid%Capacity);
        Para->Cdef(Recv->RVM_Macro, std::make_unique<std::string>(Buf));
    }
    Para->Add("");
    if(Cap_Front!=this->Main->Proj->RVM->Map->After_Cap_Front)
        throw std::runtime_error("Receive endpoint:\nG1000: Internal capability table computation failure.");
}
/* End Function:RVM_Gen::Macro_Recv ******************************************/

/* Begin Function:RVM_Gen::Boot_Hdr *******************************************
Description : Generate the rvm_boot.h.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Boot_Hdr(void)
{
    FILE* File;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;

    Doc=std::make_unique<class Doc>();
    Doc->Csrc_Desc("rvm_boot.h", "The boot-time initialization file header.");

    Para=std::make_unique<class Para>("Doc:Defines");
    Para->Add("/* Defines *******************************************************************/");
    Macro_Vect(Para);
    /* RVM internal resource creation goes here - not created by the script */
    if(this->Main->Proj->Virt.size()!=0)
        Macro_Virtep(Para);
    Macro_Captbl(Para);
    Macro_Pgtbl(Para);
    Macro_Proc(Para);
    Macro_Thd(Para);
    Macro_Inv(Para);
    Macro_Recv(Para);
    
    /* Extra capability table frontier */
    Para->Add("/* Capability table frontier */");
    Para->Cdef(std::make_unique<std::string>("RVM_BOOT_CAP_FRONTIER"),
               (ret_t)(this->Main->Proj->RVM->Map->After_Cap_Front));
    /* Extra kernel memory frontier */
    Para->Add("/* Kernel memory frontier */");
    Para->Cdef(std::make_unique<std::string>("RVM_BOOT_KMEM_FRONTIER"),
               this->Main->Proj->RVM->Map->After_Kmem_Front);

    /* Finish file generation */
    Para->Add("/* End Defines ***************************************************************/");
    Doc->Add(std::move(Para));
    Doc->Csrc_Foot();

    /* Generate rvm_boot.h */
    File=this->Main->Dstfs->Open_File("M7M2_MuAmmonite/Project/Include/rvm_boot.h");
    Doc->Write(File);
    fclose(File);
}
/* End Function:RVM_Gen::Boot_Hdr ********************************************/

/* Begin Function:RVM_Gen::Virtmap_Crt ****************************************
Description : Create the virtual machine constant info array.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Virtmap_Crt(std::unique_ptr<class Para>& Para)
{
    ptr_t Obj_Cnt;
    ptr_t Capacity;
    
    Capacity=this->Main->Plat->Capacity;

    Obj_Cnt=0;
    for(class Virt* Virt:this->Main->Proj->Virt)
    {
        Para->Add("{(rvm_s8_t*)\"%s\", %lld, %lld, %lld, %lld, %lld,", Virt->Name->c_str(), Virt->Prio, Virt->Slices, Virt->Period, Virt->Watchdog, Virt->Vect_Num);
        Para->Add(" (struct RVM_Regs*)0x%llX, (struct RVM_Param_Area*)0x%llX,",
                  Virt->Virt_Map->Reg_Base, Virt->Virt_Map->Param_Base);
        Para->Add(" (struct RVM_Vect_Flag*)0x%llX, 0x%llX, RVM_CAPID(RVM_BOOT_CTVIRTEP%lld,%lld),",
                  Virt->Virt_Map->Vctf_Base, Virt->Map->Entry_Code_Front, Obj_Cnt/Capacity, Obj_Cnt%Capacity);
        Para->Add(" %s, %s, 0x%llX, 0x%llX, 0x%llX,", 
                  Virt->Thd[0]->RVM_Macro->c_str(), Virt->Thd[0]->RVM_Macro->c_str(), 
                  Virt->Thd[0]->Map->Entry_Addr, Virt->Thd[0]->Map->Stack_Base, Virt->Thd[0]->Map->Stack_Size);
        Para->Add(" %s, %s, 0x%llX, 0x%llX, 0x%llX},",
                  Virt->Thd[1]->RVM_Macro->c_str(), Virt->Thd[1]->RVM_Macro->c_str(), 
                  Virt->Thd[1]->Map->Entry_Addr, Virt->Thd[1]->Map->Stack_Base, Virt->Thd[1]->Map->Stack_Size);
        Obj_Cnt++;
    }
}
/* End Function:RVM_Gen::Virtmap_Crt *****************************************/

/* Begin Function:RVM_Gen::Virtep_Crt *****************************************
Description : Create the virtual machine vector endpoints.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Virtep_Crt(std::unique_ptr<class Doc>& Doc)
{
    ptr_t Obj_Cnt;
    ptr_t Capacity;
    ptr_t Captbl_Size;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Virtep_Crt");
    
    /* Capability table creation */
    Para->Cfunc_Desc("RVM_Boot_Virtep_Crt",
                     "Create all virtual machine vector endpoints at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Virtep_Crt(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Cur_Addr;");
    Para->Add("");
    Para->Add("    Cur_Addr=0x%llX;", this->Main->Proj->RVM->Map->Virtep_Kmem_Front);
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Creating virtual machine endpoints.\\r\\n\");");
    Para->Add("    /* Create all the virtual machine endpoint capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->Virt.size();Obj_Cnt+=Capacity)
    {
        if(this->Main->Proj->Virt.size()>=(Obj_Cnt+1)*Capacity)
            Captbl_Size=Capacity;
        else
            Captbl_Size=this->Main->Proj->Virt.size()%Capacity;

        Para->Add("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTVIRTEP%lld, Cur_Addr, %lld)==0);", 
                  Obj_Cnt/Capacity,Captbl_Size);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created virtual machine endpoint capability table CID \", RVM_BOOT_CTVIRTEP%lld, \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Obj_Cnt/Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(%lld));",Captbl_Size);
    }
    Para->Add("");
    Para->Add("    /* Then the endpoints themselves */");
    Obj_Cnt=0;
    for(class Virt* Virt:this->Main->Proj->Virt)
    {
        Para->Add("    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CTVIRTEP%lld, %lld)==0);",
                  Obj_Cnt/Capacity, Obj_Cnt%Capacity);
        Para->Add("    RVM_LOG_SIS(\"Init:Created virtual machine endpoint CID \", RVM_CAPID(RVM_BOOT_CTVIRTEP%lld,%lld), \".\\r\\n\");",
                  Obj_Cnt/Capacity, Obj_Cnt%Capacity);
        Obj_Cnt++;
    }
    Para->Add("");
    Para->Add("    RVM_ASSERT(Cur_Addr==0x%llX);", this->Main->Proj->RVM->Map->Captbl_Kmem_Front);
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Virtep_Crt");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Virtep_Crt ******************************************/

/* Begin Function:RVM_Gen::Captbl_Crt *****************************************
Description : Create the capability tables.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Captbl_Crt(std::unique_ptr<class Doc>& Doc)
{
    class Captbl* Captbl;
    ptr_t Obj_Cnt;
    ptr_t Capacity;
    ptr_t Captbl_Size;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Captbl_Crt");
    
    /* Capability table creation */
    Para->Cfunc_Desc("RVM_Boot_Captbl_Crt",
                     "Create all capability tables at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Captbl_Crt(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Cur_Addr;");
    Para->Add("");
    Para->Add("    Cur_Addr=0x%llX;", this->Main->Proj->RVM->Map->Captbl_Kmem_Front);
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Creating process capability tables.\\r\\n\");");
    Para->Add("    /* Create all the capability table capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Captbl.size();Obj_Cnt+=Capacity)
    {
        if(this->Main->Proj->RVM->Captbl.size()>=(Obj_Cnt+1)*Capacity)
            Captbl_Size=Capacity;
        else
            Captbl_Size=this->Main->Proj->RVM->Captbl.size()%Capacity;

        Para->Add("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTCAPTBL%lld, Cur_Addr, %lld)==0);", 
                  Obj_Cnt/Capacity,Captbl_Size);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created capability table capability table CID \", RVM_BOOT_CTCAPTBL%lld, \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Obj_Cnt/Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(%lld));",Captbl_Size);
    }
    Para->Add("");
    Para->Add("    /* Then the capability tables themselves */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Captbl)
    {
        Captbl=static_cast<class Captbl*>(Info->Kobj);

        Para->Add("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CTCAPTBL%lld, RVM_BOOT_INIT_KMEM, %lld, Cur_Addr, %lld)==0);",
                  Captbl->RVM_Capid/Capacity, Captbl->RVM_Capid%Capacity, Captbl->Size);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created capability table %s CID \", RVM_CAPID(RVM_BOOT_CTCAPTBL%lld,%lld), \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Captbl->RVM_Macro->c_str(), Captbl->RVM_Capid/Capacity, Captbl->RVM_Capid%Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(%lld));", Captbl->Size);
    }
    Para->Add("");
    Para->Add("    RVM_ASSERT(Cur_Addr==0x%llX);", this->Main->Proj->RVM->Map->Pgtbl_Kmem_Front);
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Captbl_Crt");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Captbl_Crt ******************************************/

/* Begin Function:RVM_Gen::Pgtbl_Crt ******************************************
Description : Create the capability tables.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Pgtbl_Crt(std::unique_ptr<class Doc>& Doc)
{
    class Pgtbl* Pgtbl;
    ptr_t Obj_Cnt;
    ptr_t Capacity;
    ptr_t Captbl_Size;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Pgtbl_Crt");

    /* Page table creation */
    Para->Cfunc_Desc("RVM_Boot_Pgtbl_Crt",
                     "Create all page tables at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Pgtbl_Crt(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Cur_Addr;");
    Para->Add("");
    Para->Add("    Cur_Addr=0x%llX;", this->Main->Proj->RVM->Map->Pgtbl_Kmem_Front);
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Creating process page tables.\\r\\n\");");
    Para->Add("    /* Create all the page tables capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Pgtbl.size();Obj_Cnt+=Capacity)
    {
        if(this->Main->Proj->RVM->Pgtbl.size()>=(Obj_Cnt+1)*Capacity)
            Captbl_Size=Capacity;
        else
            Captbl_Size=this->Main->Proj->RVM->Pgtbl.size()%Capacity;

        Para->Add("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTPGTBL%lld, Cur_Addr, %lld)==0);", 
                  Obj_Cnt/Capacity,Captbl_Size);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created page table capability table CID \", RVM_BOOT_CTPGTBL%lld, \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Obj_Cnt/Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(%lld));",Captbl_Size);
    }
    Para->Add("");
    Para->Add("    /* Then the page tables themselves */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Pgtbl)
    {
        Pgtbl=static_cast<class Pgtbl*>(Info->Kobj);

        Para->Add("    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL%lld, RVM_BOOT_INIT_KMEM, %lld, Cur_Addr, 0x%llX, %lld, %lld, %lld)==0);",
                  Pgtbl->RVM_Capid/Capacity, Pgtbl->RVM_Capid%Capacity,
                  Pgtbl->Base_Addr,(ptr_t)(Pgtbl->Is_Top!=0),Pgtbl->Size_Order, Pgtbl->Num_Order);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created page table %s CID \", RVM_CAPID(RVM_BOOT_CTPGTBL%lld,%lld), \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Pgtbl->RVM_Macro->c_str(), Pgtbl->RVM_Capid/Capacity, Pgtbl->RVM_Capid%Capacity);
        Para->Add("    Cur_Addr+=0x%llx;", this->Main->Plat->Pgtbl_Size(Pgtbl->Num_Order,Pgtbl->Is_Top));
    }
    Para->Add("");
    Para->Add("    RVM_ASSERT(Cur_Addr==0x%llX);", this->Main->Proj->RVM->Map->Proc_Kmem_Front);
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Pgtbl_Crt");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Pgtbl_Crt *******************************************/

/* Begin Function:RVM_Gen::Proc_Crt *******************************************
Description : Create the processes.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Proc_Crt(std::unique_ptr<class Doc>& Doc)
{
    class Proc* Proc;
    ptr_t Obj_Cnt;
    ptr_t Capacity;
    ptr_t Captbl_Size;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Proc_Crt");

    /* Process creation */
    Para->Cfunc_Desc("RVM_Boot_Proc_Crt",
                     "Create all processes at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Proc_Crt(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Cur_Addr;");
    Para->Add("");
    Para->Add("    Cur_Addr=0x%llX;", this->Main->Proj->RVM->Map->Proc_Kmem_Front);
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Creating processes.\\r\\n\");");
    Para->Add("    /* Create all the process capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Proc.size();Obj_Cnt+=Capacity)
    {
        if(this->Main->Proj->RVM->Proc.size()>=(Obj_Cnt+1)*Capacity)
            Captbl_Size=Capacity;
        else
            Captbl_Size=this->Main->Proj->RVM->Proc.size()%Capacity;

        Para->Add("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTPROC%lld, Cur_Addr, %lld)==0);", 
                  Obj_Cnt/Capacity,Captbl_Size);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created process capability table CID \", RVM_BOOT_CTPROC%lld, \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Obj_Cnt/Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(%lld));",Captbl_Size);
    }
    Para->Add("");
    Para->Add("    /* Then the processes themselves */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Proc)
    {
        Proc=static_cast<class Proc*>(Info->Kobj);

        Para->Add("    RVM_ASSERT(RVM_Proc_Crt(RVM_BOOT_CTPROC%lld, %lld, %s, %s)==0);",
                  Proc->RVM_Capid/Capacity, Proc->RVM_Capid%Capacity, Proc->Captbl->RVM_Macro->c_str(), Proc->Pgtbl->RVM_Macro->c_str());
        Para->Add("    RVM_LOG_SIS(\"Init:Created process %s CID \", RVM_CAPID(RVM_BOOT_CTPROC%lld,%lld), \".\\r\\n\");",
                  Proc->RVM_Macro->c_str(), Proc->RVM_Capid/Capacity, Proc->RVM_Capid%Capacity);
    }
    Para->Add("");
    Para->Add("    RVM_ASSERT(Cur_Addr==0x%llX);", this->Main->Proj->RVM->Map->Thd_Kmem_Front);
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Proc_Crt");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Proc_Crt ********************************************/

/* Begin Function:RVM_Gen::Thd_Crt ********************************************
Description : Create the threads.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Thd_Crt(std::unique_ptr<class Doc>& Doc)
{
    class Thd* Thd;
    class Proc* Proc;
    ptr_t Obj_Cnt;
    ptr_t Capacity;
    ptr_t Captbl_Size;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Thd_Crt");

    /* Thread creation */
    Para->Cfunc_Desc("RVM_Boot_Thd_Crt",
                     "Create all threads at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Thd_Crt(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Cur_Addr;");
    Para->Add("");
    Para->Add("    Cur_Addr=0x%llX;", this->Main->Proj->RVM->Map->Thd_Kmem_Front);
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Creating threads.\\r\\n\");");
    Para->Add("    /* Create all the thread capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Thd.size();Obj_Cnt+=Capacity)
    {
        if(this->Main->Proj->RVM->Thd.size()>=(Obj_Cnt+1)*Capacity)
            Captbl_Size=Capacity;
        else
            Captbl_Size=this->Main->Proj->RVM->Thd.size()%Capacity;

        Para->Add("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTTHD%lld, Cur_Addr, %lld)==0);", 
                  Obj_Cnt/Capacity,Captbl_Size);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created thread capability table CID \", RVM_BOOT_CTTHD%lld, \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Obj_Cnt/Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(%lld));",Captbl_Size);
    }
    Para->Add("");
    Para->Add("    /* Then the threads themselves */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Thd)
    {
        Thd=static_cast<class Thd*>(Info->Kobj);
        Proc=Info->Proc;

        Para->Add("    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CTTHD%lld, RVM_BOOT_INIT_KMEM, %lld, %s, %lld, Cur_Addr)==0);",
                  Thd->RVM_Capid/Capacity, Thd->RVM_Capid%Capacity, Proc->RVM_Macro->c_str(), Thd->Prio);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created thread %s CID \", RVM_CAPID(RVM_BOOT_CTTHD%lld,%lld), \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Thd->RVM_Macro->c_str(), Thd->RVM_Capid/Capacity, Thd->RVM_Capid%Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_THD_SIZE);");
    }
    Para->Add("");
    Para->Add("    RVM_ASSERT(Cur_Addr==0x%llX);", this->Main->Proj->RVM->Map->Inv_Kmem_Front);
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Thd_Crt");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Thd_Crt *********************************************/

/* Begin Function:RVM_Gen::Inv_Crt ********************************************
Description : Create the invocations.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Inv_Crt(std::unique_ptr<class Doc>& Doc)
{
    class Inv* Inv;
    class Proc* Proc;
    ptr_t Obj_Cnt;
    ptr_t Capacity;
    ptr_t Captbl_Size;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Inv_Crt");

    /* Invocation creation */
    Para->Cfunc_Desc("RVM_Boot_Inv_Crt",
                     "Create all invocations at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Inv_Crt(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Cur_Addr;");
    Para->Add("");
    Para->Add("    Cur_Addr=0x%llX;", this->Main->Proj->RVM->Map->Inv_Kmem_Front);
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Creating invocations.\\r\\n\");");
    Para->Add("    /* Create all the invocation capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Inv.size();Obj_Cnt+=Capacity)
    {
        if(this->Main->Proj->RVM->Inv.size()>=(Obj_Cnt+1)*Capacity)
            Captbl_Size=Capacity;
        else
            Captbl_Size=this->Main->Proj->RVM->Inv.size()%Capacity;

        Para->Add("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTINV%lld, Cur_Addr, %lld)==0);", 
                  Obj_Cnt/Capacity,Captbl_Size);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created invocation capability table CID \", RVM_BOOT_CTINV%lld, \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Obj_Cnt/Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(%lld));",Captbl_Size);
    }
    Para->Add("");
    Para->Add("    /* Then the invocations themselves */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Inv)
    {
        Inv=static_cast<class Inv*>(Info->Kobj);
        Proc=Info->Proc;
        Para->Add("    RVM_ASSERT(RVM_Inv_Crt(RVM_BOOT_CTINV%lld, RVM_BOOT_INIT_KMEM, %lld, %s, Cur_Addr)==0);",
                  Inv->RVM_Capid/Capacity, Inv->RVM_Capid%Capacity, Proc->RVM_Macro->c_str());
        Para->Add("    RVM_LOG_SISUS(\"Init:Created invocation %s CID \", RVM_CAPID(RVM_BOOT_CTINV%lld,%lld), \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Inv->RVM_Macro->c_str(), Inv->RVM_Capid/Capacity, Inv->RVM_Capid%Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_INV_SIZE);");
    }
    Para->Add("");
    Para->Add("    RVM_ASSERT(Cur_Addr==0x%llX);", this->Main->Proj->RVM->Map->Recv_Kmem_Front);
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Inv_Crt");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Inv_Crt *********************************************/

/* Begin Function:RVM_Gen::Recv_Crt *******************************************
Description : Create the receive endpoints.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Recv_Crt(std::unique_ptr<class Doc>& Doc)
{
    class Recv* Recv;
    ptr_t Obj_Cnt;
    ptr_t Capacity;
    ptr_t Captbl_Size;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Recv_Crt");
    
    /* Receive endpoint creation */
    Para->Cfunc_Desc("RVM_Boot_Recv_Crt",
                     "Create all receive endpoints at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Recv_Crt(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Cur_Addr;");
    Para->Add("");
    Para->Add("    Cur_Addr=0x%llX;", this->Main->Proj->RVM->Map->Recv_Kmem_Front);
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Creating receive endpoints.\\r\\n\");");
    Para->Add("    /* Create all the receive endpoint capability tables first */");
    for(Obj_Cnt=0;Obj_Cnt<this->Main->Proj->RVM->Recv.size();Obj_Cnt+=Capacity)
    {
        if(this->Main->Proj->RVM->Recv.size()>=(Obj_Cnt+1)*Capacity)
            Captbl_Size=Capacity;
        else
            Captbl_Size=this->Main->Proj->RVM->Recv.size()%Capacity;

        Para->Add("    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTRECV%lld, Cur_Addr, %lld)==0);", 
                  Obj_Cnt/Capacity,Captbl_Size);
        Para->Add("    RVM_LOG_SISUS(\"Init:Created receive endpoint capability table CID \", RVM_BOOT_CTRECV%lld, \" @ address 0x\", Cur_Addr, \".\\r\\n\");",
                  Obj_Cnt/Capacity);
        Para->Add("    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(%lld));",Captbl_Size);
    }
    Para->Add("");
    Para->Add("    /* Then the receive endpoints themselves */");
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Recv)
    {
        Recv=static_cast<class Recv*>(Info->Kobj);

        Para->Add("    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CTRECV%lld, %lld)==0);",
                  Recv->RVM_Capid/Capacity, Recv->RVM_Capid%Capacity);
        Para->Add("    RVM_LOG_SIS(\"Init:Created receive endpoint %s CID \", RVM_CAPID(RVM_BOOT_CTRECV%lld,%lld), \".\\r\\n\");",
                  Recv->RVM_Macro->c_str(), Recv->RVM_Capid/Capacity, Recv->RVM_Capid%Capacity);
    }
    Para->Add("");
    Para->Add("    RVM_ASSERT(Cur_Addr==0x%llX);", this->Main->Proj->RVM->Map->After_Kmem_Front);
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Recv_Crt");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Recv_Crt ********************************************/

/* Begin Function:RVM_Gen::Virtcap_Init ***************************************
Description : Initialize virtual machine capability table special contents.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Virtcap_Init(std::unique_ptr<class Doc>& Doc)
{
    ptr_t Capacity;
    ptr_t Obj_Cnt;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Virtcap_Init");

    /* Capability table initialization */
    Para->Cfunc_Desc("RVM_Boot_Virtcap_Init",
                     "Initialize all virtual machine capability table special contents.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Virtcap_Init(void)");
    Para->Add("{");
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Initializing virtual machine capability tables.\\r\\n\");");
    Obj_Cnt=0;
    for(class Virt* Virt:this->Main->Proj->Virt)
    {
        Para->Add("    /* Initializing virtual machine: %s */", Virt->Name->c_str());

        /* Setup system call send endpoint at capability table location 0 */
        Para->Add("    /* System call endpoint */");
        Para->Add("    RVM_ASSERT(RVM_Captbl_Add(%s, 0, RVM_BOOT_CAPTBL, RVM_Vmmd_Sig_Cap, %s)==0);",
                  Virt->Captbl->RVM_Macro->c_str(), "RVM_SIG_FLAG_SND");
        Para->Add("    RVM_LOG_S(\"Init:Delegated system call send endpoint to virtual machine %s.\\r\\n\");",
                  Virt->RVM_Macro->c_str());

        /* Setup virtual machine vector endpoint at capability table location 1 */
        Para->Add("    /* Vector endpoint - this is a special one */");
        Para->Add("    RVM_ASSERT(RVM_Captbl_Add(%s, 1, RVM_BOOT_CTVIRTEP%lld, %lld, %s)==0);",
                  Virt->Captbl->RVM_Macro->c_str(), Obj_Cnt/Capacity, Obj_Cnt%Capacity, "RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV");
        Para->Add("    RVM_LOG_S(\"Init:Delegated system call send endpoint to virtual machine %s.\\r\\n\");",
                  Virt->RVM_Macro->c_str());
        Para->Add("");

        /* Initialize virtual machine data structure */

        Obj_Cnt++;
    }
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Virtcap_Init");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Virtcap_Init ****************************************/

/* Begin Function:RVM_Gen::Captbl_Init ****************************************
Description : Initialize capability tables.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Captbl_Init(std::unique_ptr<class Doc>& Doc)
{
    ptr_t Capacity;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Captbl_Init");

    /* Capability table initialization */
    Para->Cfunc_Desc("RVM_Boot_Captbl_Init",
                     "Initialize the capability tables of all processes.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Captbl_Init(void)");
    Para->Add("{");
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Initializing process capability tables.\\r\\n\");");
    for(std::unique_ptr<class Proc>& Proc:this->Main->Proj->Proc)
    {
        Para->Add("    /* Initializing captbl for process: %s */", Proc->Name->c_str());

        /* If this is not a virtual machine, we intialize the slot 0 as kernel capability
         * for sending events to the RVM's Vctd */
        if(Proc->Type==PROC_TYPE_NATIVE)
        {
            Para->Add("    /* Event kernel capability */");
            Para->Add("    RVM_ASSERT(RVM_Captbl_Kern(%s, 0, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, RVM_KERN_EVT_LOCAL_TRIG, RVM_KERN_EVT_LOCAL_TRIG)==0);",
                      Proc->Captbl->RVM_Macro->c_str());
            Para->Add("    RVM_LOG_S(\"Init:Delegated kernel capability for event sending to process %s capability table position 0.\\r\\n\");",
                      Proc->RVM_Macro->c_str());
        }

        /* Ports */
        Para->Add("    /* Ports */");
        for(std::unique_ptr<class Port>& Port:Proc->Port)
        {
            Para->Add("    RVM_ASSERT(RVM_Captbl_Add(%s, %lld, RVM_BOOT_CTINV%lld, %lld, %s)==0);",
                      Proc->Captbl->RVM_Macro->c_str(), Port->Loc_Capid, Port->RVM_Capid/Capacity, Port->RVM_Capid%Capacity,
                      "RVM_INV_FLAG_ACT");
            Para->Add("    RVM_LOG_S(\"Init:Delegated invocation port %s to process %s capability table position %lld.\\r\\n\");",
                      Port->RVM_Macro->c_str(), Proc->RVM_Macro->c_str(), Port->Loc_Capid);
        }

        /* Receive endpoints */
        Para->Add("    /* Receive endpoints */");
        for(std::unique_ptr<class Recv>& Recv:Proc->Recv)
        {
            Para->Add("    RVM_ASSERT(RVM_Captbl_Add(%s, %lld, RVM_BOOT_CTRECV%lld, %lld, %s)==0);",
                      Proc->Captbl->RVM_Macro->c_str(), Recv->Loc_Capid, Recv->RVM_Capid/Capacity, Recv->RVM_Capid%Capacity,
                      "RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV");
            Para->Add("    RVM_LOG_S(\"Init:Delegated receive endpoint %s to process %s capability table position %lld.\\r\\n\");",
                      Recv->RVM_Macro->c_str(), Proc->RVM_Macro->c_str(), Recv->Loc_Capid);
        }

        /* Send endpoints */
        Para->Add("    /* Send endpoints */");
        for(std::unique_ptr<class Send>& Send:Proc->Send)
        {
            Para->Add("    RVM_ASSERT(RVM_Captbl_Add(%s, %lld, RVM_BOOT_CTRECV%lld, %lld, %s)==0);",
                      Proc->Captbl->RVM_Macro->c_str(), Send->Loc_Capid, Send->RVM_Capid/Capacity, Send->RVM_Capid%Capacity,
                      "RVM_SIG_FLAG_SND");
            Para->Add("    RVM_LOG_S(\"Init:Delegated send endpoint %s to process %s capability table position %lld.\\r\\n\");",
                      Send->RVM_Macro->c_str(), Proc->RVM_Macro->c_str(), Send->Loc_Capid);
        }

        /* Vector endpoints */
        Para->Add("    /* Vector endpoints */");
        for(std::unique_ptr<class Vect>& Vect:Proc->Vect)
        {
            Para->Add("    RVM_ASSERT(RVM_Captbl_Add(%s, %lld, RVM_BOOT_CTVECT%lld, %lld, %s)==0);",
                      Proc->Captbl->RVM_Macro->c_str(), Vect->Loc_Capid, Vect->RVM_Capid/Capacity, Vect->RVM_Capid%Capacity,
                      "RVM_SIG_FLAG_RCV");
            Para->Add("    RVM_LOG_S(\"Init:Delegated vector endpoint %s to process %s capability table position %lld.\\r\\n\");",
                      Vect->RVM_Macro->c_str(), Proc->RVM_Macro->c_str(), Vect->Loc_Capid);
        }

        /* Kernel functions */
        Para->Add("    /* Kernel functions */");
        for(std::unique_ptr<class Kern>& Kern:Proc->Kern)
        {
            Para->Add("    RVM_ASSERT(RVM_Captbl_Kern(%s, %lld, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, 0x%llX, 0x%llX)==0);",
                      Proc->Captbl->RVM_Macro->c_str(), Kern->Loc_Capid, Kern->Start, Kern->End);
            Para->Add("    RVM_LOG_S(\"Init:Delegated kernel function 0x%llX - 0x%llX to process %s capability table position %lld.\\r\\n\");",
                      Kern->Start, Kern->End, Proc->RVM_Macro->c_str(), Kern->Loc_Capid);
        }
    }
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Captbl_Init");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Captbl_Init *****************************************/

/* Begin Function:RVM_Gen::Pgtbl_Con ******************************************
Description : Construct the page table for RVM. This will produce the desired final
              page table tree, and is recursive.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add these to.
              class Pgtbl* Pgtbl - The page table structure.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Pgtbl_Con(std::unique_ptr<class Para>& Para, class Pgtbl* Pgtbl)
{
    ptr_t Count;
    class Pgtbl* Child;

    /* Construct whatever page table to this page table */
    for(Count=0;Count<POW2(Pgtbl->Num_Order);Count++)
    {
        Child=Pgtbl->Pgdir[(unsigned int)Count].get();
        if(Child==nullptr)
            continue;
        
        Para->Add("    RVM_ASSERT(RVM_Pgtbl_Con(%s, 0x%llX, %s, %s)==0);",
                  Pgtbl->RVM_Macro->c_str(), Count, Child->RVM_Macro->c_str(), "RVM_PGTBL_ALL_PERM");
        
        Para->Add("    RVM_LOG_S(\"Init:Constructed page table child %s into page table parent %s @ position 0x%llX.\\r\\n\");",
                  Child->RVM_Macro->c_str(), Pgtbl->RVM_Macro->c_str(), Count);

        /* Recursively call this for all the page tables */
        Pgtbl_Con(Para, Child);
    }
}
/* End Function:RVM_Gen::Pgtbl_Con *******************************************/

/* Begin Function:RVM_Gen::Pgtbl_Map ******************************************
Description : Map pages into a page table. This is not recursive.
Input       : std::unique_ptr<class Para>& Para - The paragraph to add these to.
              class Pgtbl* Pgtbl - The page table structure.
              ptr_t Init_Size_Ord - The initial page table's number order.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Pgtbl_Map(std::unique_ptr<class Para>& Para, class Pgtbl* Pgtbl, ptr_t Init_Size_Ord)
{
    ptr_t Count;
    ptr_t Attr;
    ptr_t Pos_Src;
    ptr_t Index;
    ptr_t Page_Start;
    ptr_t Page_Size;
    ptr_t Page_Num;
    ptr_t Init_Size;
    std::unique_ptr<std::string> Flags;

    Page_Size=POW2(Pgtbl->Size_Order);
    Page_Num=POW2(Pgtbl->Num_Order);
    Init_Size=POW2(Init_Size_Ord);

    /* Map whatever pages into this page table */
    for(Count=0;Count<Page_Num;Count++)
    {
        Attr=Pgtbl->Page[(unsigned int)Count];
        if(Attr==0)
            continue;

        /* Compute flags */
        Flags=std::make_unique<std::string>("");

        if((Attr&MEM_READ)!=0)
            *Flags+="RVM_PGTBL_READ|";
        if((Attr&MEM_WRITE)!=0)
            *Flags+="RVM_PGTBL_WRITE|";
        if((Attr&MEM_EXECUTE)!=0)
            *Flags+="RVM_PGTBL_EXECUTE|";
        if((Attr&MEM_CACHEABLE)!=0)
            *Flags+="RVM_PGTBL_CACHEABLE|";
        if((Attr&MEM_BUFFERABLE)!=0)
            *Flags+="RVM_PGTBL_BUFFERABLE|";
        if((Attr&MEM_STATIC)!=0)
            *Flags+="RVM_PGTBL_STATIC|";

        Flags->pop_back();

        /* Compute Pos_Src and Index */
        Page_Start=Pgtbl->Base_Addr+Count*Page_Size;
        Pos_Src=Page_Start/Init_Size;
        Index=(Page_Start-Pos_Src*Init_Size)/Page_Size;

        Para->Add("    RVM_ASSERT(RVM_Pgtbl_Add(%s, 0x%llX, \\\n"
                  "                             %s, \\\n"
                  "                             %s, 0x%llX, 0x%llX)==0);",
                  Pgtbl->RVM_Macro->c_str(), Count, Flags->c_str(), "RVM_BOOT_PGTBL", Pos_Src, Index);

        Para->Add("    RVM_LOG_S(\"Init:Added page addr 0x%llX size 0x%llX to page table %s @ position 0x%llX.\\r\\n\");",
                  Page_Start, Page_Size, Pgtbl->RVM_Macro->c_str(), Count);
    }
}
/* End Function:RVM_Gen::Pgtbl_Map *******************************************/

/* Begin Function:RVM_Gen::Pgtbl_Init *****************************************
Description : Initialize page tables.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Pgtbl_Init(std::unique_ptr<class Doc>& Doc)
{
    class Pgtbl* Pgtbl;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Pgtbl_Init");

    /* Page table initialization */
    Para->Cfunc_Desc("RVM_Boot_Pgtbl_Init",
                     "Initialize the page tables of all processes.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Pgtbl_Init(void)");
    Para->Add("{");
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Initializing process page tables.\\r\\n\");");
    /* Do page table construction first */
    for(std::unique_ptr<class Proc>& Proc:this->Main->Proj->Proc)
    {
        Para->Add("    /* Constructing page tables for process: %s */",Proc->Name->c_str());
        Pgtbl_Con(Para,Proc->Pgtbl.get());
        Para->Add("");
    }
    
    /* Then do the mapping for all page tables */
    for(std::unique_ptr<class Cap>& Info:this->Main->Proj->RVM->Pgtbl)
    {
        Pgtbl=static_cast<class Pgtbl*>(Info->Kobj);
        Para->Add("    /* Mapping pages into page tables - %s */", Pgtbl->RVM_Macro->c_str());
        Pgtbl_Map(Para, Pgtbl, this->Main->Plat->Word_Bits-this->Main->Plat->Init_Num_Ord);
        Para->Add("");
    }

    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Pgtbl_Init");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Pgtbl_Init ******************************************/

/* Begin Function:RVM_Gen::Thd_Init *******************************************
Description : Initialize threads.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Thd_Init(std::unique_ptr<class Doc>& Doc)
{
    ptr_t Capacity;
    class Virt* Virt;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Thd_Init");
    
    /* Thread initialization */
    Para->Cfunc_Desc("RVM_Boot_Thd_Init",
                     "Initialize the all threads.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Thd_Init(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Init_Stack_Addr;");
    Para->Add("");
    Para->Add("    Init_Stack_Addr=0;");
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Initializing threads.\\r\\n\");");
    for(std::unique_ptr<class Proc>& Proc:this->Main->Proj->Proc)
    {
        Para->Add("    /* Initializing thread for process: %s */", Proc->Name->c_str());
        
        for(std::unique_ptr<class Thd>& Thd:Proc->Thd)
        {
            /* The marker is non-zero only for virtual machine threads */
            if(Proc->Type==PROC_TYPE_VIRT)
            {
                Para->Add("    RVM_ASSERT(RVM_Thd_Sched_Bind(%s, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, %s|RVM_VIRT_THDID_MARKER, %lld)==0);",
                          Thd->RVM_Macro->c_str(), Thd->RVM_Macro->c_str(), Thd->Prio);
            }
            else
            {
                Para->Add("    RVM_ASSERT(RVM_Thd_Sched_Bind(%s, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, %s, %lld)==0);",
                          Thd->RVM_Macro->c_str(), Thd->RVM_Macro->c_str(), Thd->Prio);
            }

            /* If this process is a virtual machine and we are dealing with its user thread, we need to have its HYP set */
            if(Proc->Type==PROC_TYPE_VIRT)
            {
                if(*(Thd->Name)=="User")
                {
                    Virt=static_cast<class Virt*>(Proc.get());
                    Para->Add("    RVM_ASSERT(RVM_Thd_Hyp_Set(%s, 0x%llX)==0);",Thd->RVM_Macro->c_str(), Virt->Virt_Map->Reg_Base);
                }
            }

            Para->Add("    Init_Stack_Addr=RVM_Stack_Init(0x%llX, 0x%llX, 0x%llX, 0x%llX);",
                      Thd->Map->Stack_Base, Thd->Map->Stack_Size, Thd->Map->Entry_Addr, Proc->Map->Entry_Code_Front);
            Para->Add("    RVM_ASSERT(RVM_Thd_Exec_Set(%s, 0x%llX, Init_Stack_Addr, 0x%llX)==0);",
                      Thd->RVM_Macro->c_str(), Thd->Map->Entry_Addr, Thd->Map->Param_Value);
            Para->Add("    RVM_ASSERT(RVM_Thd_Time_Xfer(%s, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);",
                      Thd->RVM_Macro->c_str());
            Para->Add("    RVM_LOG_S(\"Init:Initialized thread %s process %s entry 0x%llX stack 0x%llX - 0x%llX param 0x%llX.\\r\\n\");",
                      Thd->RVM_Macro->c_str(), Proc->RVM_Macro->c_str(), 
                      Thd->Map->Entry_Addr, Thd->Map->Stack_Base, Thd->Map->Stack_Size, Thd->Map->Param_Value);
            Para->Add("");
        }
    }

    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Thd_Init");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Thd_Init ********************************************/

/* Begin Function:RVM_Gen::Inv_Init *******************************************
Description : Initialize invocations.
Input       : std::unique_ptr<class Doc>& Doc - The document to add this to.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Inv_Init(std::unique_ptr<class Doc>& Doc)
{
    ptr_t Capacity;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Capacity=this->Main->Plat->Capacity;

    Para=std::make_unique<class Para>("Func:RVM_Boot_Inv_Init");
    
    /* Invocation initialization */
    Para->Cfunc_Desc("RVM_Boot_Inv_Init",
                     "Initialize the all invocations.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Inv_Init(void)");
    Para->Add("{");
    Para->Add("    rvm_ptr_t Init_Stack_Addr;");
    Para->Add("");
    Para->Add("    Init_Stack_Addr=0;");
    Para->Add("    RVM_ASSERT(Init_Stack_Addr==0);");
    Para->Add("    RVM_LOG_S(\"-------------------------------------------------------------------------------\\r\\n\");");
    Para->Add("    RVM_LOG_S(\"Init:Initializing invocations.\\r\\n\");");
    for(std::unique_ptr<class Proc>& Proc:this->Main->Proj->Proc)
    {
        Para->Add("    /* Initializing invocation for process: %s */", Proc->Name->c_str());
        
        for(std::unique_ptr<class Inv>& Inv:Proc->Inv)
        {
            Para->Add("    Init_Stack_Addr=RVM_Stack_Init(0x%llX, 0x%llX, 0x%llX, 0x%llX);",
                      Inv->Map->Stack_Base, Inv->Map->Stack_Size, Inv->Map->Entry_Addr, Proc->Map->Entry_Code_Front);
            /* We always return directly on fault for MCUs, because RVM does not do fault handling there */
            Para->Add("    RVM_ASSERT(RVM_Inv_Set(%s, 0x%llX, Init_Stack_Addr, 1)==0);",
                      Inv->RVM_Macro->c_str(), Inv->Map->Entry_Addr);
            Para->Add("    RVM_LOG_S(\"Init:Initialized invocation %s process %s entry 0x%llX stack 0x%llX - 0x%llX.\\r\\n\");",
                      Inv->RVM_Macro->c_str(), Proc->RVM_Macro->c_str(), 
                      Inv->Map->Entry_Addr, Inv->Map->Stack_Base, Inv->Map->Stack_Size);
            Para->Add("");
        }
    }
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Inv_Init");
    Doc->Add(std::move(Para));
}
/* End Function:RVM_Gen::Inv_Init ********************************************/

/* Begin Function:RVM_Gen::Boot_Src *******************************************
Description : Generate the rvm_boot.c.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::Boot_Src(void)
{
    FILE* File;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Doc=std::make_unique<class Doc>();

    Doc->Csrc_Desc("rvm_boot.c", "The boot-time initialization file.");

    /* Print all header includes */
    Para=std::make_unique<class Para>("Doc:Includes");
    Para->Add("/* Includes ******************************************************************/");
    Include(Para);
    Para->Add("#include \"rvm_boot.h\"");
    Para->Add("/* End Includes **************************************************************/");
    Doc->Add(std::move(Para));

    /* Print global variables for virtual machine management - don't print when no VMs exist */
    if(this->Main->Proj->Virt.size()!=0)
    {
        Para=std::make_unique<class Para>("Doc:Public Global Varibles");
        Para->Add("/* Public Global Varibles ****************************************************/");
        Para->Add("struct RVM_Virt RVM_Virt_DB[%d];", this->Main->Proj->Virt.size());
        Para->Add("const struct RVM_Virt_Map RVM_Virt_Map_DB[%d]=", this->Main->Proj->Virt.size());
        Para->Add("{");
        Virtmap_Crt(Para);
        Para->Add("};");
        Para->Add("/* End Public Global Varibles ************************************************/");
        Doc->Add(std::move(Para));
    }

    /* Print all prototypes */
    Para=std::make_unique<class Para>("Doc:Private C Function Prototypes");
    Para->Add("/* Private C Function Prototypes *********************************************/");
    Para->Add("/* Kernel object creation */");
    if(this->Main->Proj->Virt.size()!=0)
        Para->Add("static void RVM_Boot_Virtep_Crt(void);");
    Para->Add("static void RVM_Boot_Captbl_Crt(void);");
    Para->Add("static void RVM_Boot_Pgtbl_Crt(void);");
    Para->Add("static void RVM_Boot_Proc_Crt(void);");
    Para->Add("static void RVM_Boot_Inv_Crt(void);");
    Para->Add("static void RVM_Boot_Recv_Crt(void);");
    Para->Add("");
    Para->Add("/* Kernel object initialization */");
    if(this->Main->Proj->Virt.size()!=0)
        Para->Add("static void RVM_Boot_Virtcap_Init(void);");
    Para->Add("static void RVM_Boot_Captbl_Init(void);");
    Para->Add("static void RVM_Boot_Pgtbl_Init(void);");
    Para->Add("static void RVM_Boot_Thd_Init(void);");
    Para->Add("static void RVM_Boot_Inv_Init(void);");
    Para->Add("/* End Private C Function Prototypes *****************************************/");
    Doc->Add(std::move(Para));
    
    Para=std::make_unique<class Para>("Doc:Public C Function Prototypes");
    Para->Add("/* Public C Function Prototypes **********************************************/");
    Para->Add("void RVM_Boot_Kobj_Crt(void);");
    Para->Add("void RVM_Boot_Kobj_Init(void);");
    Para->Add("/* End Public C Function Prototypes ******************************************/");
    Doc->Add(std::move(Para));

    /* Kernel object creation */
    if(this->Main->Proj->Virt.size()!=0)
        Virtep_Crt(Doc);
    Captbl_Crt(Doc);
    Pgtbl_Crt(Doc);
    Proc_Crt(Doc);
    Thd_Crt(Doc);
    Inv_Crt(Doc);
    Recv_Crt(Doc);

    /* Main creation function */
    Para=std::make_unique<class Para>("Func:RVM_Boot_Kobj_Crt");
    Para->Cfunc_Desc("RVM_Boot_Kobj_Crt",
                     "Create all kernel objects at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Kobj_Crt(void)");
    Para->Add("{");
    if(this->Main->Proj->Virt.size()!=0)
        Para->Add("    RVM_Boot_Virtep_Crt();");
    Para->Add("    RVM_Boot_Captbl_Crt();");
    Para->Add("    RVM_Boot_Pgtbl_Crt();");
    Para->Add("    RVM_Boot_Proc_Crt();");
    Para->Add("    RVM_Boot_Thd_Crt();");
    Para->Add("    RVM_Boot_Inv_Crt();");
    Para->Add("    RVM_Boot_Recv_Crt();");
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Kobj_Crt");
    Doc->Add(std::move(Para));

    /* Kernel object initialization */
    if(this->Main->Proj->Virt.size()!=0)
        Virtcap_Init(Doc);
    Captbl_Init(Doc);
    Pgtbl_Init(Doc);
    Thd_Init(Doc);
    Inv_Init(Doc);

    /* Main initialization function */
    Para=std::make_unique<class Para>("Func:RVM_Boot_Kobj_Init");
    Para->Cfunc_Desc("RVM_Boot_Kobj_Init",
                     "Initialize all kernel objects at boot-time.", Input, Output, "None.");
    Para->Add("void RVM_Boot_Kobj_Init(void)");
    Para->Add("{");
    if(this->Main->Proj->Virt.size()!=0)
        Para->Add("    RVM_Boot_Virtcap_Init();");
    Para->Add("    RVM_Boot_Captbl_Init();");
    Para->Add("    RVM_Boot_Pgtbl_Init();");
    Para->Add("    RVM_Boot_Thd_Init();");
    Para->Add("    RVM_Boot_Inv_Init();");
    if(this->Main->Proj->Virt.size()!=0)
        Para->Add("    RVM_Virt_Crt(RVM_Virt_DB, (struct RVM_Virt_Map*)RVM_Virt_Map_DB, %d);", this->Main->Proj->Virt.size());
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Kobj_Init");
    Doc->Add(std::move(Para));

    Doc->Csrc_Foot();

    /* Generate rvm_boot.c */
    File=this->Main->Dstfs->Open_File("M7M2_MuAmmonite/Project/Source/rvm_boot.c");
    Doc->Write(File);
    fclose(File);
}
/* End Function:RVM_Gen::Boot_Src ********************************************/

/* Begin Function:RVM_Gen::User_Src *******************************************
Description : Generate the rvm_user.c. This file is mainly responsible for user-
              supplied hooks. If the user needs to add functionality, consider
              modifying this file.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Gen::User_Src(void)
{
    FILE* File;
    std::unique_ptr<class Doc> Doc;
    std::unique_ptr<class Para> Para;
    std::vector<std::unique_ptr<std::string>> Input;
    std::vector<std::unique_ptr<std::string>> Output;

    Doc=std::make_unique<class Doc>();

    /* Create user stubs - pre initialization and post initialization */
    Doc->Csrc_Desc("rvm_user.c", "The user hook file.");

    /* Print all header includes */
    Para=std::make_unique<class Para>("Doc:Includes");
    Para->Add("/* Includes ******************************************************************/");
    Include(Para);
    Para->Add("#include \"rvm_boot.h\"");
    Para->Add("/* End Includes **************************************************************/");
    Doc->Add(std::move(Para));

    /* Print all global prototypes */
    Para=std::make_unique<class Para>("Doc:Public C Function Prototypes");
    Para->Add("/* Public C Function Prototypes **********************************************/");
    Para->Add("void RVM_Boot_Pre_Init(void);");
    Para->Add("void RVM_Boot_Post_Init(void);");
    Para->Add("/* End Public C Function Prototypes ******************************************/");
    Doc->Add(std::move(Para));

    /* Preinitialization of hardware */
    Para=std::make_unique<class Para>("Func:RVM_Boot_Pre_Init");
    Para->Cfunc_Desc("RVM_Boot_Pre_Init",
                     "Initialize critical hardware before any kernel object creation takes place.",
                     Input, Output, "None.");
    Para->Add("void RVM_Boot_Pre_Init(void)");
    Para->Add("{");
    Para->Add("    /* Add code here */");
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Pre_Init");
    Doc->Add(std::move(Para));

    /* Postinitialization of hardware */
    Para=std::make_unique<class Para>("Func:RVM_Boot_Post_Init");
    Para->Cfunc_Desc("RVM_Boot_Post_Init",
                     "Initialize hardware after all kernel object creation took place.",
                     Input, Output, "None.");
    Para->Add("void RVM_Boot_Post_Init(void)");
    Para->Add("{");
    Para->Add("    /* Add code here */");
    Para->Add("}");
    Para->Cfunc_Foot("RVM_Boot_Post_Init");
    Doc->Add(std::move(Para));

    Doc->Csrc_Foot();
    
    /* Generate rvm_user.c */
    File=this->Main->Dstfs->Open_File("M7M2_MuAmmonite/Project/Source/rvm_user.c");
    Doc->Write(File);
    fclose(File);
}
/* End Function:RVM_Gen::User_Src ********************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
