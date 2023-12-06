/******************************************************************************
Filename    : process.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the process.
******************************************************************************/

/* Define ********************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEF__
#ifndef __PROCESS_DEF__
#define __PROCESS_DEF__
/*****************************************************************************/
/* Process type */
#define PROCESS_NATIVE          (0)
#define PROCESS_VIRTUAL          (1)

/* Magic number for native processes */
#define MAGIC_NATIVE            (0x49535953U)
/* Magic number for virtual machine processes */
#define MAGIC_VIRTUAL           (0x56495254U)

/* Every processes have the some capability slots at the front preserved.
 * For processes,
 * [0] is the event send kernel capability,
 * For virtual machines,
 * [0] is the Hypd hypercall endpoint,
 * [1] is the Vctd vector endpoint. */
#define NATIVE_CPT_BASE            (1)
#define VIRTUAL_CPT_BASE        (2)

/* Description header alignment */
#define PRC_DESC_ALIGN(X)          ROUND_UP(X,16)
/*****************************************************************************/
/* __PROCESS_DEF__ */
#endif
/* __HDR_DEF__ */
#endif
/* End Define ****************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASS__
#ifndef __PROCESS_CLASS__
#define __PROCESS_CLASS__
/*****************************************************************************/
/* For parsing and storing process information */
class Process:public Kobj
{
public:
    /* Process type */
    ptr_t Type;
    /* Extra capability table size */
    ptr_t Extra_Captbl;

    /* Coprocessor list */
    std::vector<std::string> Coprocessor;
    std::set<std::string> Coprocessor_Set;
    /* Build system to use */
    std::string Buildsystem;
    /* Toolchain to use for that build system */
    std::string Toolchain;
    /* Compiler optimization level */
    std::string Optimization;
    /* Project output folder/overwrite */
    std::string Project_Output;
    ptr_t Project_Overwrite;
    /* Linker output folder */
    std::string Linker_Output;
    /* Main header folder */
    std::string Main_Header_Output;
    /* Main source folder */
    std::string Main_Source_Output;
    /* Entry source folder - native process only */
    std::string Entry_Source_Output;
    ptr_t Entry_Source_Overwrite;

    /* Memory trunk information */
    std::vector<std::unique_ptr<class Mem_Info>> Memory;
    std::vector<class Mem_Info*> Memory_Code;
    std::vector<class Mem_Info*> Memory_Data;
    std::vector<class Mem_Info*> Memory_Device;

    /* Shared memory trunk information */
    std::vector<std::unique_ptr<class Shmem>> Shmem;
    std::map<std::string,class Shmem*> Shmem_Map;

    /* Combined memory information */
    std::vector<std::unique_ptr<class Mem_Info>> Memory_All;

    /* Cpt/Pgt */
    std::unique_ptr<class Captbl> Captbl;
    std::unique_ptr<class Pgtbl> Pgtbl;
    std::unique_ptr<std::vector<ptr_t>> Rawtbl;

    /* Threads */
    std::vector<std::unique_ptr<class Thread>> Thread;
    std::map<std::string,class Thread*> Thread_Map;

    /* Invocations */
    std::vector<std::unique_ptr<class Invocation>> Invocation;
    std::map<std::string,class Invocation*> Invocation_Map;

    /* Ports */
    std::vector<std::unique_ptr<class Port>> Port;
    std::map<std::string,class Port*> Port_Map;

    /* Receive endpoints */
    std::vector<std::unique_ptr<class Receive>> Receive;
    std::map<std::string,class Receive*> Receive_Map;

    /* Send endpoints */
    std::vector<std::unique_ptr<class Send>> Send;
    std::map<std::string,class Send*> Send_Map;

    /* Vector endpoints */
    std::vector<std::unique_ptr<class Vect_Info>> Vector;
    std::map<std::string,class Vect_Info*> Vector_Map;
    std::map<ptr_t,class Vect_Info*> Vector_Number_Map;

    /* Kernel functions */
    std::vector<std::unique_ptr<class Kfunc>> Kfunc;
    std::map<std::string,class Kfunc*> Kfunc_Map;

    /* Code memory */
    ptr_t Code_Base;
    ptr_t Code_Size;
    /* Data memory */
    ptr_t Data_Base;
    ptr_t Data_Size;
    /* Descriptor header entry frontier */
    ptr_t Desc_Front;
    /* Actual code frontier */
    ptr_t Code_Front;

    /* Linker filename */
    std::string Linker_Filename;
    /* Project filename */
    std::string Project_Filename;

    /* void */ Process(xml_node_t* Root, ptr_t Type);

    void Check(void);

    void Local_Alloc(ptr_t Max);
    void Global_Alloc_Captbl(std::vector<class Captbl*>& Global);
    void Global_Alloc_Pgtbl(std::vector<class Pgtbl*>& Global,
                            std::unique_ptr<class Pgtbl>& Pgtbl);
    void Global_Alloc_Process(std::vector<class Process*>& Global);
    void Global_Alloc_Thread(std::vector<class Thread*>& Global);
    void Global_Alloc_Invocation(std::vector<class Invocation*>& Global);
    void Global_Alloc_Receive(std::vector<class Receive*>& Global);
    void Global_Alloc_Vector(std::vector<class Vect_Info*>& Global);

    void Mem_Alloc(ptr_t Wordlength, ptr_t Hyp_Reg_Size, ptr_t Kom_Order);
};
/*****************************************************************************/
/* __PROCESS_CLASS__ */
#endif
/* __HDR_CLASS__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
