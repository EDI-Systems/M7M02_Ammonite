/******************************************************************************
Filename    : rme_mcu.hpp
Author      : pry
Date        : 08/04/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The header of the mcu tool.
******************************************************************************/

/* Defines *******************************************************************/
#ifdef __HDR_DEFS__
#ifndef __RME_MCU_HPP_TYPES__
#define __RME_MCU_HPP_TYPES__
/*****************************************************************************/
typedef char                                s8_t;
typedef short                               s16_t;
typedef int                                 s32_t;
typedef long long                           s64_t;
typedef unsigned char                       u8_t;
typedef unsigned short                      u16_t;
typedef unsigned int                        u32_t;
typedef unsigned long long                  u64_t;
/* Make things compatible in 32-bit or 64-bit environments */
typedef s64_t                               ret_t;
typedef u64_t                               ptr_t;
/*****************************************************************************/
/* __RME_MCU_HPP_TYPES__ */
#endif
/* __HDR_DEFS__ */
#endif

namespace rme_mcu
{
#ifdef __HDR_DEFS__
#ifndef __RME_MCU_HPP_DEFS__
#define __RME_MCU_HPP_DEFS__
/*****************************************************************************/
/* Power of 2 macros */
#define POW2(POW)                           (((ptr_t)1)<<(POW))
#define ROUND_DOWN(X,POW)                   (((X)>>(POW))<<(POW))
#define ROUND_UP(X,POW)                     ROUND_DOWN((X)+POW2(POW)-1,POW)

/* The code generator author name */
#define CODE_AUTHOR                         "The RME project generator."
/* The license for the generator */
#define CODE_LICENSE                        "LGPL v3+; see COPYING for details."
/* Generator macro alignment */
#define MACRO_ALIGN                         (56)
/* Buffer size for all temporary buffers */
#define BUF_SIZE                            (1024)

/* Vector flag area size (in bytes), fixed across all architectures */
#define KERNEL_VCTF_SIZE                    (512)
/* Event flag area size (in bytes), fixed across all architectures */
#define KERNEL_EVTF_SIZE                    (512)
/* Entry point slot size (in words), fixed across all architectures */
#define ENTRY_SLOT_SIZE                     (8)

/* Priority limit for threads */
#define PROC_THD_PRIO_MIN                   (5)
/*****************************************************************************/
/* __RME_MCU_HPP_DEFS__ */
#endif
/* __HDR_DEFS__ */
#endif
/* End Defines ***************************************************************/

/* Classes *******************************************************************/
#ifdef __HDR_CLASSES__
#ifndef __RME_MCU_HPP_CLASSES__
#define __RME_MCU_HPP_CLASSES__
/*****************************************************************************/
/* The application instance class */
class Main
{
private:
    void Add_Extmem(void);
    void Alloc_Code(void);
    void Alloc_Data(void);
    void Add_Shmem(void);
    void Check_Addrspace(void);
    void Check_Code(void);
    void Check_Device(void);

public:
    std::unique_ptr<std::string> Input;
    std::unique_ptr<std::string> Output;
    std::unique_ptr<std::string> Format;

    std::unique_ptr<class Srcfs> Srcfs;
    std::unique_ptr<class Dstfs> Dstfs;
    std::unique_ptr<class Proj> Proj;
    std::unique_ptr<class Chip> Chip;
    std::unique_ptr<class Plat> Plat;

    std::unique_ptr<class RME_Gen> RME_Gen;
    std::unique_ptr<class RVM_Gen> RVM_Gen;
    std::unique_ptr<class Proc_Gen> Proc_Gen;
    std::unique_ptr<class Proj_Gen> Proj_Gen;
    
    Main(int argc, char* argv[]);

    void Parse(void);
    void Check_Chip(void);

    void Alloc_Mem(void);
    void Alloc_Cap(void);
    void Link_Cap(void);
    void Alloc_Obj(void);

    void Gen_RME(void);
    void Gen_RVM(void);
    void Gen_Proc(void);
    void Gen_Proj(void);

    void Gen_Report(void);
};

class Kobj_Stats
{
public:
    ptr_t Main_Capability_Table_Usage;
    ptr_t Main_Capability_Table_Total;
    ptr_t Main_Capability_Table_Limit;
    ptr_t Total_Kernel_Objects;
    ptr_t Auxiliary_Capability_Tables;
    ptr_t Processes;
    ptr_t Virtual_Machines;
    ptr_t Capability_Tables;
    ptr_t Page_Tables;
    ptr_t Threads;
    ptr_t Invocations;
    ptr_t Endpoints;
};

class Kmem_Stats
{
public:
    ptr_t RME_Code_Memory;
    ptr_t RME_Data_Memory;
    ptr_t RVM_Code_Memory;
    ptr_t RVM_Data_Memory;
    ptr_t Kernel_Memory_Usage;
    ptr_t Kernel_Memory_Total;
    ptr_t Processes;
    ptr_t Capability_Tables;
    ptr_t Page_Tables;
    ptr_t Threads;
    ptr_t Invocations;
    ptr_t Endpoints;
};

class User_Stats
{
public:
    ptr_t Private_Code_Memory;
    ptr_t Private_Data_Memory;
    ptr_t Private_Device_Memory;
    ptr_t Shared_Code_Memory;
    ptr_t Shared_Data_Memory;
    ptr_t Shared_Device_Memory;
};

class Plat_Stats
{
public:
    ptr_t Used_Code_Memory;
    ptr_t Used_Data_Memory;
    ptr_t Used_Device_Memory;
    ptr_t Platform_Code_Memory;
    ptr_t Platform_Data_Memory;
    ptr_t Platform_Device_Memory;
};

class Proc_Stats
{
public:
    ptr_t Capability_Table_Usage;
    ptr_t Capability_Table_Total;
    ptr_t Capability_Table_Limit;
    ptr_t Code_Memory;
    ptr_t Data_Memory;
    ptr_t Device_Memory;
};

class Stats
{
public:
    class Kobj_Stats Kobj;
    class Kmem_Stats Kmem;
    class User_Stats User;
    class Plat_Stats Plat;
    class Proc_Stats Proc;
    
    void Kobj_Stats(class Main* Main);
    void Kmem_Stats(class Main* Main);
    void User_Stats(class Main* Main);
    void Plat_Stats(class Main* Main);
    void Proc_Stats(class Proc* Proc);
};
/*****************************************************************************/
/* __RME_MCU_HPP_CLASSES__ */
#endif
/* __HDR_CLASSES__ */
#endif
}
/* End Classes ***************************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
