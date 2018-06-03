/******************************************************************************
Filename    : init.c
Author      : pry
Date        : 11/06/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The init process of MPU-based RVM systems. This process just sets
              up all other parts of the system then go idle. 
              The startup sequence of the system is:
              1> Load all processes on startup.
              2> Initialize the processes one by one until it finishes.
              The efficiency is just tooooo slow, and the user-level is not standalone,
              which is no good at all. We need to implement NTR.
******************************************************************************/

/* Includes ******************************************************************/
#include "RME.h"

#define __HDR_DEFS__
#include "Platform/RVM_platform.h"
#include "Init/syssvc.h"
#include "Init/init.h"
#include "Init/hyper.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/RVM_platform.h"
#include "Init/syssvc.h"
#include "Init/init.h"
#include "Init/hyper.h"
#undef __HDR_STRUCTS__

/* Private include */
#include "Init/init.h"

#define __HDR_PUBLIC_MEMBERS__
#include "Platform/RVM_platform.h"
#include "Init/syssvc.h"
#include "Init/hyper.h"
#undef __HDR_PUBLIC_MEMBERS__
/* End Includes **************************************************************/

/* Begin Function:RVM_Clear ***************************************************
Description : Memset a memory area to zero.
Input       : void* Addr - The address to clear.
              ptr_t Size - The size to clear.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Clear(void* Addr, ptr_t Size)
{
    ptr_t* Word_Inc;
    u8* Byte_Inc;
    ptr_t Words;
    ptr_t Bytes;
    
    /* On processors not that fast, copy by word is really important */
    Word_Inc=(ptr_t*)Addr;
    for(Words=Size/sizeof(ptr_t);Words>0;Words--)
    {
        *Word_Inc=0;
        Word_Inc++;
    }
    
    /* Get the final bytes */
    Byte_Inc=(u8*)Word_Inc;
    for(Bytes=Size%sizeof(ptr_t);Bytes>0;Bytes--)
    {
        *Byte_Inc=0;
        Byte_Inc++;
    }
}
/* End Function:RVM_Clear ****************************************************/

/* Begin Function:RVM_VM_Cnt **************************************************
Description : Count the page tables' and VM images' total number.
Input       : None.
Output      : ptr_t* VM_Num - The number of virtual machine images.
              ptr_t* Pgtbl_Num - The number of page tables.
Return      : None.
******************************************************************************/
void RVM_VM_Cnt(ptr_t* VM_Num, ptr_t* Pgtbl_Num)
{
    ptr_t VM_Cnt;
    ptr_t Pgtbl_Cnt;
    const struct RVM_Image* Image;
    
    VM_Cnt=0;
    Pgtbl_Cnt=0;
    Image=(struct RVM_Image*)(*(ptr_t*)RVM_IMAGE_HEADER_START);
    RVM_LOG_SUS("Init:First VM address 0x",(ptr_t)Image,".\r\n");
    
    while(1)
    {
        VM_Cnt++;
        Pgtbl_Cnt+=Image->Pgtbl_Num;
        if(Image->Next_Image==0)
            break;
    }
    
    *VM_Num=VM_Cnt;
    *Pgtbl_Num=Pgtbl_Cnt;
}
/* End Function:RVM_VM_Cnt ***************************************************/

/* Begin Function:RVM_Load_Image **********************************************
Description : Load one image into the RVM virtual machine database.
Input       : struct RVM_Image** Image - The pointer to the pointer to the image.
              ptr_t* Pgtbl_Bump - the pointer to the pgtbl captbl bump pointer.
              ptr_t* Kmem_Bump - The kernel memory bump pointer.
              ptr_t VMID - The ID of the virtual machine.
Output      : struct RVM_Image** Image - The pointer to the pointer to the image.
              ptr_t* Pgtbl_Bump - the pointer to the pgtbl captbl bump pointer.
              ptr_t* Kmem_Bump - The kernel memory bump pointer.
Return      : None.
******************************************************************************/
void RVM_Load_Image(struct RVM_Image** Image, ptr_t* Pgtbl_Bump, ptr_t* Kmem_Bump, ptr_t VMID)
{
    ptr_t Count;
    struct RVM_Virt* Virt;
    struct RVM_Hdr_Pgtbl* Pgtbl;

    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");

    /* Is the magic number correct? */
    if((*Image)->Magic==RVM_VM_MAGIC)
        RVM_LOG_SUS("Init:Found VM image header @ address 0x",(ptr_t)(*Image),".\r\n");
    else
    {
        RVM_LOG_SUS("Init:Found corrupted image header @ address 0x",(ptr_t)(*Image),".\r\n");
        RVM_ASSERT(0);
    }

    /* Allocate one structure */
    if(RVM_Free.Next==&RVM_Free)
    {
        RVM_LOG_S("Init:Virtual machine control block exhausted.\r\n");
        RVM_ASSERT(0);
    }
    Virt=(struct RVM_Virt*)(RVM_Free.Next);
    RVM_List_Del(Virt->Head.Prev,Virt->Head.Next);
    
    /* Fill in the process structure */
    Virt->Image=*Image;

    /* Setup the pgtbl - only log the top level in the database */
    Virt->Cap.Pgtbl=RVM_CAPID(RVM_VIRT_TBL_PGTBL,*Pgtbl_Bump);
    Pgtbl=(struct RVM_Hdr_Pgtbl*)((*Image)->Pgtbl);
    
    /* Do some checks here to make sure that the page table fits our needs - critical memory is all mapped in */
    /* Check user thread entry address&user stack */
    RVM_ASSERT(_RVM_Pgtbl_Check(Virt->Image->Pgtbl,Virt->Image->Pgtbl_Num,Virt->Image->User_Entry,4)==0);
    RVM_ASSERT(_RVM_Pgtbl_Check(Virt->Image->Pgtbl,Virt->Image->Pgtbl_Num,Virt->Image->User_Stack,Virt->Image->User_Size)==0);
    /* Check interrupt thread entry address & interrupt stack */
    RVM_ASSERT(_RVM_Pgtbl_Check(Virt->Image->Pgtbl,Virt->Image->Pgtbl_Num,Virt->Image->Int_Entry,4)==0);
    RVM_ASSERT(_RVM_Pgtbl_Check(Virt->Image->Pgtbl,Virt->Image->Pgtbl_Num,Virt->Image->Int_Stack,Virt->Image->Int_Size)==0);
    /* Check parameter, register & interrupt flag space */
    RVM_ASSERT(_RVM_Pgtbl_Check(Virt->Image->Pgtbl,Virt->Image->Pgtbl_Num,Virt->Image->Param,sizeof(struct RVM_Param))==0);
    RVM_ASSERT(_RVM_Pgtbl_Check(Virt->Image->Pgtbl,Virt->Image->Pgtbl_Num,Virt->Image->Regs,sizeof(struct RVM_Regs))==0);
    RVM_ASSERT(_RVM_Pgtbl_Check(Virt->Image->Pgtbl,Virt->Image->Pgtbl_Num,Virt->Image->Int_Flags,sizeof(ptr_t)*RVM_VECT_BITMAP(Virt->Image->Int_Num))==0);
    /* Check console space */
    RVM_ASSERT(_RVM_Pgtbl_Check(Virt->Image->Pgtbl,Virt->Image->Pgtbl_Num,Virt->Image->Console_Buf,Virt->Image->Console_Size)==0);
    RVM_LOG_S("Init:Virtual machine system shared memory mapping check complete.\r\n");
    /* Clean up the interrupt flag space */
    RVM_Clear(Virt->Image->Int_Flags,sizeof(ptr_t)*RVM_VECT_BITMAP(Virt->Image->Int_Num));
    
    /* Actually do the page table setup */
    for(Count=0;Count<Virt->Image->Pgtbl_Num;Count++)
        _RVM_Pgtbl_Setup(Pgtbl, Count, RVM_VIRT_TBL_PGTBL, Pgtbl_Bump, RVM_BOOT_INIT_KMEM, Kmem_Bump);
    RVM_LOG_SIS("Init:Created ",Virt->Image->Pgtbl_Num," page tables.\r\n");   
    
    /* Create the virtual machine capability table */
    Virt->Cap.Captbl=RVM_VIRT_CAPTBL(VMID);
    RVM_ASSERT(RVM_Captbl_Crt(RVM_VIRT_TBL_CAPPROC, RVM_BOOT_INIT_KMEM, VMID*2, *Kmem_Bump, Virt->Image->Kcap_Num+2)==0);
    RVM_LOG_SISUS("Init:Created virtual machine capability table CID ",VMID*2," @ address 0x",*Kmem_Bump,".\r\n");
    RVM_LOG_SIS("Init:Virtual machine capability table size ",Virt->Image->Kcap_Num+2,".\r\n");
    *Kmem_Bump+=RVM_CAPTBL_SIZE(Virt->Image->Kcap_Num+2);
    
    /* Create the virtual machine process */
    Virt->Cap.Proc=RVM_VIRT_PROC(VMID);
    RVM_ASSERT(RVM_Proc_Crt(RVM_VIRT_TBL_CAPPROC, RVM_BOOT_INIT_KMEM, VMID*2+1, RVM_VIRT_CAPTBL(VMID), Virt->Cap.Pgtbl, *Kmem_Bump)==0);
    RVM_LOG_SISUS("Init:Created virtual machine process CID ",VMID*2+1," @ address 0x",*Kmem_Bump,".\r\n");
    *Kmem_Bump+=RVM_PROC_SIZE;
    
    /* Create the virtual machine threads and endpoint in the virtual machine process */
    Virt->Cap.User_Thd=RVM_VIRT_USERTHD(VMID);
    Virt->Cap.User_TID=RVM_Thd_Crt(RVM_VIRT_TBL_THDSIG, RVM_BOOT_INIT_KMEM, VMID*3, RVM_VIRT_PROC(VMID), RVM_USER_PRIO, *Kmem_Bump);
    RVM_ASSERT(Virt->Cap.User_TID>=0);
    RVM_LOG_SISUS("Init:Created virual machine user thread CID ",VMID*3 ," @ address 0x",*Kmem_Bump," internal TID ");RVM_LOG_I(Virt->Cap.User_TID);
    *Kmem_Bump+=RVM_THD_SIZE;
    
    Virt->Cap.Int_Thd=RVM_VIRT_INTTHD(VMID);
    Virt->Cap.Int_TID=RVM_Thd_Crt(RVM_VIRT_TBL_THDSIG, RVM_BOOT_INIT_KMEM, VMID*3+1, RVM_VIRT_PROC(VMID), RVM_VINT_PRIO, *Kmem_Bump);
    RVM_ASSERT(Virt->Cap.Int_TID>=0);
    RVM_LOG_SISUS(".\r\nInit:Created virual machine interrupt thread CID ",VMID*3+1 ," @ address 0x",*Kmem_Bump," internal TID ");RVM_LOG_I(Virt->Cap.Int_TID);
    *Kmem_Bump+=RVM_THD_SIZE;
    
    Virt->Cap.Int_Sig=RVM_VIRT_INTSIG(VMID);
    RVM_ASSERT(RVM_Sig_Crt(RVM_VIRT_TBL_THDSIG, RVM_BOOT_INIT_KMEM, VMID*3+2, *Kmem_Bump)==0);
    RVM_LOG_SISUS(".\r\nInit:Created virual machine interrupt endpoint CID ",VMID*3+2," @ address 0x",*Kmem_Bump,".\r\n");
    *Kmem_Bump+=RVM_SIG_SIZE;
    
    /* Bind both threads to the processor */
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_VIRT_USERTHD(VMID), RVM_VMM_GUARD_THD, RVM_WAIT_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_VIRT_INTTHD(VMID), RVM_VMM_GUARD_THD, RVM_WAIT_PRIO)==0);
    
    /* Set the execution/hypervior properties for both threads, the stack information is from the image header - always set stack to 256 bytes */
    RVM_ASSERT(RVM_Thd_Hyp_Set(RVM_VIRT_USERTHD(VMID), (ptr_t)(Virt->Image->Regs))==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_VIRT_USERTHD(VMID), (ptr_t)(Virt->Image->User_Entry), 
                                RVM_Stack_Init((ptr_t)(Virt->Image->User_Stack),Virt->Image->User_Size),0)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_VIRT_INTTHD(VMID), (ptr_t)(Virt->Image->Int_Entry), 
                                RVM_Stack_Init((ptr_t)(Virt->Image->Int_Stack),Virt->Image->Int_Size),0)==0);
    RVM_LOG_S("Init:Thread initialization complete.\r\n");
    
    /* Delegete infinite time to both threads */
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_VIRT_USERTHD(VMID), RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_VIRT_INTTHD(VMID), RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    
    /* Delegate the endpoints to the virtual machine's capability table, only sending allowed.
     * The VMM daemon endpoint is always at 0, the interrupt handler endpoint is always at 1 */
    RVM_ASSERT(RVM_Captbl_Add(RVM_VIRT_CAPTBL(VMID), 0, RVM_BOOT_CAPTBL, RVM_VMM_VMMD_SIG, RME_SIG_FLAG_SND)==0);
    RVM_ASSERT(RVM_Captbl_Add(RVM_VIRT_CAPTBL(VMID), 1, RVM_VIRT_TBL_THDSIG, VMID*3+2, RME_SIG_FLAG_SND|RME_SIG_FLAG_RCV)==0);
    /* Delegate the kernel function capabilities to the VM according to their respective declarations */
    for(Count=0;Count<Virt->Image->Kcap_Num;Count++)
    {
        RVM_ASSERT(RVM_Captbl_Kern(RVM_VIRT_CAPTBL(VMID), Count+2, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, 
                                   Virt->Image->Kcap[Count], Virt->Image->Kcap[Count])==0);
    }
    
    /* Initialize all other data structures */
    RVM_List_Crt(&(Virt->Evt_Head));
    /* By default, interrupts are disabled, and booting not done */
    Virt->Sched.State=RVM_VM_RUNNING;
    Virt->Sched.Period=RVM_DEF_PERIOD;
    Virt->Sched.Slices_Left=Virt->Image->Slices;
    _RVM_Set_Rdy(Virt);
    _RVM_Wheel_Ins(Virt,Virt->Sched.Period);

    /* Update the image */
    if(((*Image)->Next_Image)!=0)
        (*Image)=(struct RVM_Image*)(*((*Image)->Next_Image));
    else
        (*Image)=0;
}
/* End Function:RVM_Load_Image ***********************************************/

/* Begin Function:RVM_VM_Init *************************************************
Description : Initialize the virtual machines and their data structures.
Input       : ptr_t* Kmem_Bump - The kernel memory bump pointer position, before allocations.
Output      : ptr_t* Kmem_Bump - The kernel memory bump pointer position, after allocations.
Return      : None.
******************************************************************************/
void RVM_VM_Init(ptr_t* Kmem_Bump)
{
    cnt_t Count;
    ptr_t VM_Num;
    ptr_t Pgtbl_Num;
    ptr_t Pgtbl_Bump;
    struct RVM_Image* Image;
    
    /* Traverse the images and see how many of them are there, and how many page table caps do they need */
    RVM_VM_Cnt(&VM_Num, &Pgtbl_Num);
    RVM_LOG_SIS("Init:Total virtual machine count ",VM_Num,".\r\n");
    RVM_LOG_SIS("Init:Total page table count ",Pgtbl_Num,".\r\n");
    
    /* Create the high-level captbls for virtual machines */
    /* This is the capability table for VM captbl/process caps */
    RVM_LOG_SISUS("Init:Virtual machine process/captbl table size ",VM_Num*2," @ address 0x",*Kmem_Bump,".\r\n");
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_VIRT_TBL_CAPPROC, *Kmem_Bump, VM_Num*2)==0);
    *Kmem_Bump+=RVM_CAPTBL_SIZE(VM_Num*2);
    
    /* This is the capability table for VM userthd/intthd/intsig caps */
    RVM_LOG_SISUS("Init:Virtual machine userthd/intthd/intsig table size ",VM_Num*3," @ address 0x",*Kmem_Bump,".\r\n");
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_VIRT_TBL_THDSIG, *Kmem_Bump, VM_Num*3)==0);
    *Kmem_Bump+=RVM_CAPTBL_SIZE(VM_Num*3);
    
    /* This is the capability table for VM pgtbl caps */
    RVM_LOG_SISUS("Init:Virtual machine pgtbl table size ",Pgtbl_Num," @ address 0x",*Kmem_Bump,".\r\n");
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_VIRT_TBL_PGTBL, *Kmem_Bump, Pgtbl_Num)==0);
    *Kmem_Bump+=RVM_CAPTBL_SIZE(Pgtbl_Num);
    
    /* Now load all the images */
    Image=(struct RVM_Image*)(*((ptr_t*)RVM_IMAGE_HEADER_START));
    Pgtbl_Bump=0;
    
    Count=0;
    while(Image!=0)
        RVM_Load_Image(&Image, &Pgtbl_Bump, Kmem_Bump, Count++);
}
/* End Function:RVM_VM_Init **************************************************/

/* Begin Function:RVM_Daemon_Init *********************************************
Description : Initialize the daemons. These daemons include three ones:
              1. Guardd, the daemon for the startup and process recovery.
              2. Timerd, the daemon for time accounting.
              3. RTd, the daemon for real-time scheduling.
              4. Unixd, the daemon for Unix services.
Input       : ptr_t Bump_Ptr - The bump pointer for allocation.
Output      : None.
Return      : ptr_t - The bump pointer's new location.
******************************************************************************/
void RVM_Daemon_Init(ptr_t* Kmem_Bump)
{
    cnt_t Count;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_Tick=0;
    RVM_VM_Num=0;
    
    RVM_List_Crt(&RVM_Wait);
    RVM_List_Crt(&RVM_Free);
    RVM_List_Crt(&RVM_Int_Free);
    RVM_List_Crt(&RVM_Evt_Free);
    
    /* VM init */
    for(Count=0;Count<RVM_MAX_VM_NUM;Count++)
        RVM_List_Ins(&(RVM_Virt_DB[Count].Head),RVM_Free.Prev,&RVM_Free);
    /* Interrupts init */
    for(Count=0;Count<RVM_INT_VECT_NUM;Count++)
        RVM_List_Crt(&RVM_Int_Vect[Count]);
    for(Count=0;Count<RVM_INT_MAP_NUM;Count++)
    {
        RVM_Evt_DB[Count].State=RVM_INT_FREE;
        RVM_List_Ins(&(RVM_Int_DB[Count].Head),RVM_Int_Free.Prev,&RVM_Int_Free);
    }
    /* Event init */
    for(Count=0;Count<RVM_EVT_MAP_NUM;Count++)
    {
        RVM_Evt_DB[Count].State=RVM_EVT_FREE;
        RVM_List_Ins(&(RVM_Evt_DB[Count].Head),RVM_Evt_Free.Prev,&RVM_Evt_Free);
    }
    /* Run list init */
    for(Count=0;Count<RVM_MAX_PREEMPT_VPRIO;Count++)
        RVM_List_Crt(&RVM_Run[Count]);
    /* Timer wheel init */
    for(Count=0;Count<RVM_WHEEL_SIZE;Count++)
        RVM_List_Crt(&(RVM_Wheel[Count]));
    
    RVM_LOG_S("Init:Virtual machine database initialization complete.\r\n");
    
    /* Guard daemon initialization - highest priority as always */
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_VMM_GUARD_THD,
                           RVM_BOOT_INIT_PROC, RVM_GUARD_PRIO, *Kmem_Bump)>=0);
    RVM_LOG_SISUS("Init:Created Guard daemon CID ",RVM_VMM_GUARD_THD," @ address 0x",*Kmem_Bump,".\r\n");
    *Kmem_Bump+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_VMM_GUARD_THD, RVM_BOOT_INIT_THD, RVM_MAX_PREEMPT_PRIO-1)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_VMM_GUARD_THD, (ptr_t)RVM_Guard_Daemon, 
                                RVM_Stack_Init((ptr_t)RVM_GUARD_Stack,RVM_GUARD_STACK_SIZE),0)==0);
    RVM_LOG_S("Init:Guard daemon initialization complete.\r\n");

    /* Timer daemon initialization - main priority */
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_VMM_TIMD_THD,
                           RVM_BOOT_INIT_PROC, RVM_TIMD_PRIO, *Kmem_Bump)>=0);
    RVM_LOG_SISUS("Init:Created Timer daemon CID ",RVM_VMM_TIMD_THD," @ address 0x",*Kmem_Bump,".\r\n");
    *Kmem_Bump+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_VMM_TIMD_THD, RVM_VMM_GUARD_THD, RVM_TIMD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_VMM_TIMD_THD, (ptr_t)RVM_Timer_Daemon, 
                                RVM_Stack_Init((ptr_t)RVM_TIMD_Stack,RVM_TIMD_STACK_SIZE),0)==0);
    RVM_LOG_S("Init:Timer daemon initialization complete.\r\n");

    /* VMM daemon initialization - main priority */
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_VMM_VMMD_THD,
                           RVM_BOOT_INIT_PROC, RVM_VMMD_PRIO, *Kmem_Bump)>=0);
    RVM_LOG_SISUS("Init:Created VMM daemon CID ",RVM_VMM_VMMD_THD," @ address 0x",*Kmem_Bump,".\r\n");
    *Kmem_Bump+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_VMM_VMMD_SIG, *Kmem_Bump)==0);
    RVM_LOG_SISUS("Init:Created VMMD endpoint CID ",RVM_VMM_VMMD_SIG," @ address 0x",*Kmem_Bump,".\r\n");
    *Kmem_Bump+=RVM_SIG_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_VMM_VMMD_THD, RVM_VMM_GUARD_THD, RVM_VMMD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_VMM_VMMD_THD, (ptr_t)RVM_VMM_Daemon, 
                                RVM_Stack_Init((ptr_t)RVM_VMMD_Stack,RVM_VMMD_STACK_SIZE),0)==0);
    RVM_LOG_S("Init:VMM daemon initialization complete.\r\n");

    /* Interrupt daemon initialization - main priority */
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_VMM_INTD_THD,
                           RVM_BOOT_INIT_PROC, RVM_INTD_PRIO, *Kmem_Bump)>=0);
    RVM_LOG_SISUS("Init:Created Interrupt daemon CID ",RVM_VMM_INTD_THD," @ address 0x",*Kmem_Bump,".\r\n");
    *Kmem_Bump+=RVM_THD_SIZE;
    
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_VMM_INTD_THD, RVM_VMM_GUARD_THD, RVM_INTD_PRIO)==0);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_VMM_INTD_THD, (ptr_t)RVM_Interrupt_Daemon, 
                                RVM_Stack_Init((ptr_t)RVM_INTD_Stack,RVM_INTD_STACK_SIZE),0)==0);
    RVM_LOG_S("Init:Interrupt daemon initialization complete.\r\n");
    
    /* Delegate timeslice to guardd, and make it work - we should stop here actually,
     * and we have done operating the database, now all the work is the Guard daemon's.
     * additionally, from then on, no further kernel objects will be created anymore */
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_VMM_GUARD_THD, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_VMM_TIMD_THD, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_VMM_VMMD_THD, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_VMM_INTD_THD, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_LOG_S("Init:Daemon time budget initialization complete.\r\n");
}

/* End Function:RVM_Daemon_Init **********************************************/

/* Begin Function:RVM_Guard_Daemon ********************************************
Description : The guard daemon against system total failures.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Guard_Daemon(void)
{
    tid_t Thd;
    ptr_t VMID;
    struct RVM_Evt* Event;
    RVM_LOG_S("GUARD:Guard daemon start complete.\r\n");

    /* Main cycle - recover faults if possible */
    while(1)
    {
        RVM_Sig_Rcv(RVM_BOOT_INIT_FAULT);
        
        Thd=RVM_Thd_Sched_Rcv(RVM_VMM_GUARD_THD);
        
        /* We stop whenever there is a fault now */
        RVM_LOG_S("GUARD:Fault!.\r\n");
        while(1);
        
        /* Is an error sched rcv? If not, then we must have done something wrong - This is really bad */
        if(((Thd&RVM_THD_FAULT_FLAG)==0)||(Thd<0))
        {
            /* Hang the machine because this error is unrecoverable */
            RVM_LOG_SIS("GUARD:Intangible fault on THDID ",Thd,". Shutting down system.\r\n");
            RVM_ASSERT(0);
        }
        
        /* See what thread faulted. If it is any of the VMM threads, then the system is done */
        Thd&=~((ptr_t)RVM_THD_FAULT_FLAG);
        if((Thd!=RVM_Cur_Virt->Cap.Int_TID)&&(Thd!=RVM_Cur_Virt->Cap.User_TID))
        {
            /* Hang the machine because this error is unrecoverable */
            RVM_LOG_SIS("GUARD:VMM failure, current THDID ",Thd,", unable to recover. Shutting down system.\r\n");
            RVM_ASSERT(0);
        }
        
        VMID=RVM_VMID(RVM_Cur_Virt);
        RVM_LOG_SISUS("GUARD:Guard daemon activated for VMID ", VMID, " @ address 0x", (ptr_t)RVM_Cur_Virt, ".\r\n");
        
        /* Set the execution properties for both threads, the stack information is from the image header - always set stack to 256 bytes */
        RVM_ASSERT(RVM_Thd_Exec_Set(RVM_VIRT_USERTHD(VMID), (ptr_t)(RVM_Cur_Virt->Image->User_Entry), 
                                    RVM_Stack_Init((ptr_t)(RVM_Cur_Virt->Image->User_Stack),RVM_Cur_Virt->Image->User_Size),0)==0);
        RVM_ASSERT(RVM_Thd_Exec_Set(RVM_VIRT_INTTHD(VMID), (ptr_t)(RVM_Cur_Virt->Image->Int_Entry), 
                                    RVM_Stack_Init((ptr_t)(RVM_Cur_Virt->Image->Int_Stack),RVM_Cur_Virt->Image->Int_Size),0)==0);
        
        /* Delegate infinite time to both threads */
        RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_VIRT_USERTHD(VMID), RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
        RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_VIRT_INTTHD(VMID), RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
        
        /* Place the priority as uninitialized */
        RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_Cur_Virt->Cap.User_Thd, RVM_USER_PRIO)==0);
        RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_Cur_Virt->Cap.Int_Thd, RVM_WAIT_PRIO)==0);
        
        /* Set the state back to running */
        RVM_Cur_Virt->Sched.State=RVM_VM_RUNNING;
        RVM_Cur_Virt->Sched.Period=RVM_DEF_PERIOD;
        RVM_Cur_Virt->Sched.Slices_Left=RVM_Cur_Virt->Image->Slices;
        
        /* If there are any events in its event header, delete them and return them to database */
        while(RVM_Cur_Virt->Evt_Head.Next!=&(RVM_Cur_Virt->Evt_Head))
        {
            Event=(struct RVM_Evt*)(RVM_Cur_Virt->Evt_Head.Next);
            RVM_List_Del(Event->Head.Prev,Event->Head.Next);
            Event->State=RVM_EVT_FREE;
            RVM_List_Ins(&(Event->Head),&RVM_Evt_Free,RVM_Evt_Free.Next);
        }
        
        /* Reinsert this into the wheel */
        RVM_List_Del(RVM_Cur_Virt->Delay.Prev, RVM_Cur_Virt->Delay.Next);
        _RVM_Wheel_Ins(RVM_Cur_Virt,RVM_Cur_Virt->Sched.Period);
        
        RVM_LOG_SIS("GUARD:Recovery of VM ", VMID," complete.\r\n");
    }
}
/* End Function:RVM_Guard_Daemon *********************************************/

/* Begin Function:_RVM_Wheel_Ins **********************************************
Description : Insert one VM into the timer wheel.
Input       : struct RVM_Virt* Virt - The virtual machine to insert.
              ptr_t Period - The period of the timer interrupt.
Output      : None.
Return      : None.
******************************************************************************/
void _RVM_Wheel_Ins(struct RVM_Virt* Virt, ptr_t Period)
{
    struct RVM_List* Slot;
    struct RVM_List* Trav_Ptr;
    struct RVM_Virt* Trav_Virt;
    
    Slot=&(RVM_Wheel[(RVM_Tick+Period)%RVM_WHEEL_SIZE]);
    
    Trav_Ptr=Slot->Next;
    while(Trav_Ptr!=Slot)
    {
        Trav_Virt=RVM_DLY2VM(Trav_Ptr);
        if((Trav_Virt->Sched.Timeout-RVM_Tick)>Period)
            break;
        
        Trav_Ptr=Trav_Ptr->Next;
    }

    /* Insert this into the list */
    Virt->Sched.Timeout=RVM_Tick+Period;
    RVM_List_Ins(&(Virt->Delay),Trav_Ptr->Prev,Trav_Ptr);
}
/* End Function:_RVM_Wheel_Ins ***********************************************/

/* Begin Function:RVM_Int_Send ************************************************
Description : Send an interrupt to a virtual machine.
Input       : struct RVM_Virt* Virt - The pointer to the virtual machine.
              ptr_t Int_Num - The virtual interrupt number.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Int_Send(struct RVM_Virt* Virt, ptr_t Int_Num)
{
    RVM_ASSERT(Int_Num<Virt->Image->Int_Num);
    
    if(RVM_VM_STATE(Virt->Sched.State)==RVM_VM_WAITEVT)
    {
        RVM_List_Del(Virt->Head.Prev,Virt->Head.Next);
        _RVM_Set_Rdy(Virt);
        RVM_VM_STATE_SET(Virt->Sched.State,RVM_VM_RUNNING);
    }
    
    /* Set the corresponding interrupt bit */
    _RVM_Set_Int_Flag(Virt, Int_Num);
    
    /* Send interrupt to it, if its interrupts are enabled */
    if((Virt->Sched.State&RVM_VM_INTENA)!=0)
        RVM_Sig_Snd(Virt->Cap.Int_Sig);
}
/* End Function:RVM_Int_Send *************************************************/

/* Begin Function:RVM_Timer_Daemon ********************************************
Description : The timer daemon for timer interrupt handling and time accounting.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Timer_Daemon(void)
{
    struct RVM_Virt* Virt;
    struct RVM_List* Slot;
    struct RVM_List* Trav;
    
    RVM_LOG_S("TIMD:Timer daemon start complete.\r\n");

    /* We will receive a timer interrupt every tick here */
    while(1)
    {
        RVM_Sig_Rcv(RVM_BOOT_INIT_TIMER);
        RVM_Tick++;
        /*
        if((RVM_Tick%1000)==0)
            RVM_LOG_S("TIMD:Timer daemon passed 1000 ticks.\r\n");
        */
        Slot=&(RVM_Wheel[RVM_Tick%RVM_WHEEL_SIZE]);
        
        Trav=Slot->Next;
        /* See if we need to process the timer wheel to deliver timer interrupts to virtual machines */
        while(Trav!=Slot)
        {
            Virt=RVM_DLY2VM(Trav);
            Trav=Trav->Next;
            /* If the value is more than this, then it means that the time have
             * already passed and we have to process this */
            if((RVM_Tick-Virt->Sched.Timeout)>=(RVM_ALLBITS>>1))
                break;
            
            /* This VM should be processed, place it at the new position */
            RVM_List_Del(Virt->Delay.Prev,Virt->Delay.Next);
            _RVM_Wheel_Ins(Virt,Virt->Sched.Period);
            
            /* Send number 0 interrupt to it */
            RVM_Int_Send(Virt, 0);
        }
        
        /* If there is at least one virtual machine working, check scheduler time expiration */
        if(RVM_Cur_Virt!=0)
        {
            if(RVM_Cur_Virt->Sched.Slices_Left==1)   
            {
                RVM_Cur_Virt->Sched.Slices_Left=RVM_Cur_Virt->Image->Slices;
                /* Place it at the end of the run queue */
                RVM_List_Del(RVM_Cur_Virt->Head.Prev,RVM_Cur_Virt->Head.Next);
                RVM_List_Ins(&(RVM_Cur_Virt->Head),
                             RVM_Run[RVM_Cur_Virt->Image->Prio].Prev,
                             &(RVM_Run[RVM_Cur_Virt->Image->Prio]));
            }
            else
                RVM_Cur_Virt->Sched.Slices_Left--;
        }
        
        /* There must at least be a virtual machine that is active */
        Virt=_RVM_Get_High_Rdy();
        _RVM_Virt_Switch(RVM_Cur_Virt, Virt);
        RVM_Cur_Virt=Virt;
    }
}
/* End Function:RVM_Timer_Daemon *********************************************/

/* Begin Function:RVM_VMM_Daemon **********************************************
Description : The virtual machine monitor daemon for hypercall handling.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_VMM_Daemon(void)
{
    ptr_t* Param;
    RVM_LOG_S("VMMD:VMMD initialization complete.\r\n");
    
    /* Main cycle */
    while(1)
    {
        RVM_Sig_Rcv(RVM_VMM_VMMD_SIG);
        
        //RVM_LOG_SIS("VMMD:Hypercall ",RVM_Cur_Virt->Image->Param->Number," called.\r\n");
        /* See what system call it is */
        Param=RVM_Cur_Virt->Image->Param->Param;
        switch(RVM_Cur_Virt->Image->Param->Number)
        {
            case RVM_HYP_ENAINT:
            {
                Param[0]=RVM_Hyp_Enable_Int();
                break;
            }
            case RVM_HYP_DISINT:
            {
                Param[0]=RVM_Hyp_Disable_Int();
                break;
            }
            case RVM_HYP_REGINT:
            {
                Param[0]=RVM_Hyp_Reg_Int(Param[0] /* ptr_t Vect_Num */,
                                         Param[1] /* ptr_t Int_Num */);
                break;
            }
            case RVM_HYP_DELINT:
            {
                Param[0]=RVM_Hyp_Del_Int(Param[0] /* cnt_t Int_ID */);
                break;
            }
            case RVM_HYP_REGEVT:
            {
                Param[0]=RVM_Hyp_Reg_Evt(Param[0] /* ptr_t Int_Num */,
                                         Param[1] /* ptr_t VMID */);
                break;
            }
            case RVM_HYP_DELEVT:
            {
                Param[0]=RVM_Hyp_Del_Evt(Param[0] /* cnt_t Evt_ID */);
                break;
            }
            case RVM_HYP_WAITEVT:
            {
                Param[0]=RVM_Hyp_Wait_Evt();
                break;
            }
            case RVM_HYP_SENDEVT:
            {
                Param[0]=RVM_Hyp_Send_Evt(Param[0] /* ptr_t Evt_ID */);
                break;
            }
            case RVM_HYP_QUERYEVT:
            {
                Param[0]=RVM_Hyp_Query_Evt(Param[0] /* ptr_t VMID */);
                break;
            }
            case RVM_HYP_QUERYVM:
            {
                Param[0]=RVM_Hyp_Query_VM(((s8*)Param) /* s8* Name */);
                break;
            }
            case RVM_HYP_TIMPROG:
            {
                Param[0]=RVM_Hyp_Tim_Prog(Param[0] /* ptr_t Period */);
                break;
            }
            case RVM_HYP_PRINT:
            {
                Param[0]=RVM_Hyp_Print();
                break;
            }
            
            default:break;
        }
        //RVM_LOG_SIS("VMMD:Hypercall ",RVM_Cur_Virt->Image->Param->Number," processed.\r\n");
    }
}
/* End Function:RVM_VMM_Daemon ***********************************************/

/* Begin Function:RVM_Int_Get *************************************************
Description : Get the interrupt source from the interrupt set. When this is called,
              there must be at least one interrupt pending in this set.
Input       : struct RVM_Flag_Set* Set - The interrupt set data.
Output      : struct RVM_Flag_Set* Set - The interrupt set data.
Return      : cnt_t - The interrupt number.
******************************************************************************/
cnt_t RVM_Int_Get(struct RVM_Flag_Set* Set)
{
    cnt_t Group;
    cnt_t Member;
    /* See which source group could possibly have any interrupts */
    Group=_RVM_MSB_Get(Set->Group);
    RVM_ASSERT(Group>=0);
    
    Member=_RVM_MSB_Get(Set->Flags[Group]);
    RVM_ASSERT(Member>=0);
    
    /* Clean up the slot now */
    Set->Flags[Group]&=~(((ptr_t)1)<<Member);
    if(Set->Flags[Group]==0)
        Set->Group&=~(((ptr_t)1)<<Group);
    
    return (Group<<RVM_WORD_ORDER)|Member;
}
/* End Function:RVM_Int_Get **************************************************/

/* Begin Function:RVM_Int_Proc ************************************************
Description : Process the physical interrupt, and send real interrupts to virtual machines.
Input       : cnt_t Vect_Num - The physical interrupt number to process.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Int_Proc(cnt_t Vect_Num)
{
    struct RVM_Int* Int;
    RVM_ASSERT(Vect_Num<RVM_INT_VECT_NUM);
    
    Int=(struct RVM_Int*)(RVM_Int_Vect[Vect_Num].Next);
    
    while(Int!=(struct RVM_Int*)(&RVM_Int_Vect[Vect_Num]))
    {
        RVM_Int_Send(&RVM_Virt_DB[Int->VM_ID],Int->Int_Num);
        Int=(struct RVM_Int*)(Int->Head.Next);
    }
}
/* End Function:RVM_Int_Proc *************************************************/

/* Begin Function:RVM_Int_Daemon **********************************************
Description : The interrupt daemon for interrupt handling.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Interrupt_Daemon(void)
{
    cnt_t Num;
    struct RVM_Flags* Flags;
    RVM_LOG_S("INTD:Interrupt daemon start complete.\r\n");

    Flags=(struct RVM_Flags*)RVM_INT_FLAG_ADDR;
    /* Main cycle - do nothing for now */
    while(1)
    {
        RVM_Sig_Rcv(RVM_BOOT_INIT_INT);
        /* Try to lock the first set of table */
        Flags->Set0.Lock=1;
        while(Flags->Set0.Group!=0)
        {
            /* Process the interrupts in the first group one by one */
            Num=RVM_Int_Get(&(Flags->Set0));
            RVM_Int_Proc(Num);
        }
        Flags->Set0.Lock=0;
        Flags->Set1.Lock=1;
        while(Flags->Set1.Group!=0)
        {
            /* Process the interrupts in the first group one by one */
            Num=RVM_Int_Get(&(Flags->Set1));
            RVM_Int_Proc(Num);
        }
        Flags->Set1.Lock=0;
        /* Now many VMs are woken up, and many interrupts are delivered. Choose the
         * one with the highest priority to run */
    }
}
/* End Function:RVM_Interrupt_Daemon *****************************************/

/* Begin Function:main ********************************************************
Description : The entry of the VMM's init thread. 
Input       : None.
Output      : None.
Return      : int - This function shall never return.
******************************************************************************/
int main(void)
{
    /* The kernel heap allocation bump pointer */
    ptr_t Kmem_Bump;

    RVM_LOG_S("\r\n\r\n-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("                               Welcome to the\r\n");
    RVM_LOG_S("\r\n");
    RVM_LOG_S("                        ######    ###  ###  ########\r\n");
    RVM_LOG_S("                        #######   ###  ###  ########\r\n");
    RVM_LOG_S("                        ##    ##  ###  ###  ##\r\n");
    RVM_LOG_S("                        ##    ##  ###  ###  ##\r\n");
    RVM_LOG_S("                        ##    ##  ## ## ##  ##\r\n");
    RVM_LOG_S("                        #######   ## ## ##  #######\r\n");
    RVM_LOG_S("                        ######    ## ## ##  #######\r\n");
    RVM_LOG_S("                        ##   ##   ## ## ##  ##\r\n");
    RVM_LOG_S("                        ##   ##   ##    ##  ##\r\n");
    RVM_LOG_S("                        ##    ##  ##    ##  ##\r\n");
    RVM_LOG_S("                        ##    ##  ##    ##  ########\r\n");
    RVM_LOG_S("                        ##    ### ##    ##  ########\r\n");
    RVM_LOG_S("\r\n");
    RVM_LOG_S("                    Microcontroller User-level Platform!\r\n");
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");

    /* Check if all the macros are defined correctly */
    /* Check the priority settings - are they all correct? */
    RVM_ASSERT(RVM_GUARD_PRIO>RVM_VMD_PRIO+2);
    RVM_ASSERT(RVM_VMD_PRIO>1);
    
    /* Raise our own priority to the top of the system */
    RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_BOOT_INIT_THD, RVM_MAX_PREEMPT_PRIO-1)==0);
    RVM_LOG_S("Init:Preparation - priority raised.\r\n");
    
    Kmem_Bump=RVM_KMEM_BOOT_FRONTIER;
    
    /* Create the startup thread in the init process, because the init thread
     * cannot block. Bind that to the processor, and let it have infinite budget.
     * after this the task will be handled by this task, and we will never return
     * to init unless there is nothing to run */
    RVM_Daemon_Init(&Kmem_Bump);
    RVM_LOG_S("Init:Daemon initialization done.\r\n");
    
    /* Initialize the virtual machine databases, and create whatever is needed */
    RVM_VM_Init(&Kmem_Bump);
    RVM_LOG_S("Init:Virtual machine image initialization done.\r\n");
    
    /* Now pick a virtual machine to run */
    RVM_Cur_Virt=_RVM_Get_High_Rdy();
    _RVM_Virt_Switch(0,RVM_Cur_Virt);
    
    /* Put our priority to the lowest in system - this should get everything else running */
    RVM_LOG_S("Init:Job finished - lowering priority.\r\n");
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n\r\n");
    RVM_ASSERT(RVM_Thd_Sched_Prio(RVM_BOOT_INIT_THD, RVM_INIT_PRIO)==0);

    while(1)
    {
        /* Receive the scheduler notifications for the interrupt threads. If any of
         * them failed, then we just panic, because none of them are supposed to fail. */
        /* RVM_LOG_S("I"); */
        RVM_Idle();
    }
}
/* End Function:main *********************************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/
