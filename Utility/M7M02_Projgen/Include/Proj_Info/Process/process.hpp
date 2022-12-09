/******************************************************************************
Filename    : process.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header for the process.
******************************************************************************/

/* Defines *******************************************************************/
namespace RVM_GEN
{
#ifdef __HDR_DEFS__
#ifndef __PROCESS_HPP_DEFS__
#define __PROCESS_HPP_DEFS__
/*****************************************************************************/
/* Process type */
#define PROC_NATIVE         (0)
#define PROC_VIRTUAL        (1)

/* Every processes have the some capability slots at the front preserved.
 * For processes,
 * [0] is the potential event send kernel capability,
 * For virtual machines,
 * [0] is the Vmmd hypercall endpoint,
 * [1] is the Vctd vector endpoint. */
#define PROC_CAPTBL_BASE    (1)
#define VIRT_CAPTBL_BASE    (2)
/*****************************************************************************/
/* __PROCESS_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __PROCESS_HPP_CLASSES__
#define __PROCESS_HPP_CLASSES__
/*****************************************************************************/
/* For parsing and storing process information */
class Process:public Kobj
{
public:
    /* Process type */
    ptr_t Type;
    /* Extra capability table size */
    ptr_t Extra_Captbl;

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
    /* Initialization source folder/overwrite */
    std::string Source_Output;
    ptr_t Source_Overwrite;

    /* Memory trunk information */
    std::vector<std::unique_ptr<class Mem_Info>> Memory;
    std::vector<class Mem_Info*> Memory_Code;
    std::vector<class Mem_Info*> Memory_Data;
    std::vector<class Mem_Info*> Memory_Device;

    /* Shared memory trunk information */
    std::vector<std::unique_ptr<class Mem_Info>> Shmem;
    std::map<std::string,class Mem_Info*> Shmem_Map;

    /* Combined memory information */
    std::vector<std::unique_ptr<class Mem_Info>> Memory_All;

    /* Captbl/Pgtbl */
    std::unique_ptr<class Captbl> Captbl;
    std::unique_ptr<class Pgtbl> Pgtbl;

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
    /* Code memory frontier for entries */
    ptr_t Entry_Code_Front;

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

    void Mem_Alloc(ptr_t Wordlength, ptr_t Reg_Size, ptr_t Param_Size, ptr_t Kmem_Order);
};
/*****************************************************************************/
/* __PROCESS_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
