/******************************************************************************
Filename    : rvm_boot.c
Author      : The RME project generator.
Date        : 10/11/2019
License     : LGPL v3+; see COPYING for details.
Description : The boot-time initialization file.
******************************************************************************/

/* Includes ******************************************************************/
#include "rvm.h"

#define __HDR_DEFS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#include "Init/rvm_hyper.h"
#undef __HDR_DEFS__

#define __HDR_STRUCTS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#include "Init/rvm_hyper.h"
#undef __HDR_STRUCTS__

#define __HDR_PUBLIC_MEMBERS__
#include "Platform/A7M/rvm_platform_a7m.h"
#include "Init/rvm_syssvc.h"
#include "Init/rvm_init.h"
#include "Init/rvm_hyper.h"
#undef __HDR_PUBLIC_MEMBERS__

#include "rvm_boot.h"
/* End Includes **************************************************************/

/* Public Global Varibles ****************************************************/
struct RVM_Virt RVM_Virt_DB[1];
const struct RVM_Virt_Map RVM_Virt_Map_DB[1]=
{
{(rvm_s8_t*)"Virt1", 5, 10, 100, 0, 100,
 (struct RVM_Regs*)0x20005400, (struct RVM_Param_Area*)0x20005600,
 (struct RVM_Vect_Flag*)0x20005700, 0x8030040, RVM_CAPID(RVM_BOOT_CTVIRTEP0,0),
 RVM_PROC_VIRT1_THD_VECT, RVM_PROC_VIRT1_THD_VECT, 0x8030000, 0x20005C00, 0x400,
 RVM_PROC_VIRT1_THD_USER, RVM_PROC_VIRT1_THD_USER, 0x8030020, 0x20005800, 0x400},
};
/* End Public Global Varibles ************************************************/

/* Private C Function Prototypes *********************************************/
/* Kernel object creation */
static void RVM_Boot_Virtep_Crt(void);
static void RVM_Boot_Captbl_Crt(void);
static void RVM_Boot_Pgtbl_Crt(void);
static void RVM_Boot_Proc_Crt(void);
static void RVM_Boot_Inv_Crt(void);
static void RVM_Boot_Recv_Crt(void);

/* Kernel object initialization */
static void RVM_Boot_Virtcap_Init(void);
static void RVM_Boot_Captbl_Init(void);
static void RVM_Boot_Pgtbl_Init(void);
static void RVM_Boot_Thd_Init(void);
static void RVM_Boot_Inv_Init(void);
/* End Private C Function Prototypes *****************************************/

/* Public C Function Prototypes **********************************************/
void RVM_Boot_Kobj_Crt(void);
void RVM_Boot_Kobj_Init(void);
/* End Public C Function Prototypes ******************************************/

/* Begin Function:RVM_Boot_Virtep_Crt *****************************************
Description : Create all virtual machine vector endpoints at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Virtep_Crt(void)
{
    rvm_ptr_t Cur_Addr;

    Cur_Addr=0xB00;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Creating virtual machine endpoints.\r\n");
    /* Create all the virtual machine endpoint capability tables first */
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTVIRTEP0, Cur_Addr, 1)==0);
    RVM_LOG_SISUS("Init:Created virtual machine endpoint capability table CID ", RVM_BOOT_CTVIRTEP0, " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(1));

    /* Then the endpoints themselves */
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CTVIRTEP0, 0)==0);
    RVM_LOG_SIS("Init:Created virtual machine endpoint CID ", RVM_CAPID(RVM_BOOT_CTVIRTEP0,0), ".\r\n");

    RVM_ASSERT(Cur_Addr==0xB20);
}
/* End Function:RVM_Boot_Virtep_Crt ******************************************/

/* Begin Function:RVM_Boot_Captbl_Crt *****************************************
Description : Create all capability tables at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Captbl_Crt(void)
{
    rvm_ptr_t Cur_Addr;

    Cur_Addr=0xB20;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Creating process capability tables.\r\n");
    /* Create all the capability table capability tables first */
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTCAPTBL0, Cur_Addr, 3)==0);
    RVM_LOG_SISUS("Init:Created capability table capability table CID ", RVM_BOOT_CTCAPTBL0, " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(3));

    /* Then the capability tables themselves */
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CTCAPTBL0, RVM_BOOT_INIT_KMEM, 0, Cur_Addr, 14)==0);
    RVM_LOG_SISUS("Init:Created capability table RVM_CAPTBL_PROC1 CID ", RVM_CAPID(RVM_BOOT_CTCAPTBL0,0), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(14));
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CTCAPTBL0, RVM_BOOT_INIT_KMEM, 1, Cur_Addr, 6)==0);
    RVM_LOG_SISUS("Init:Created capability table RVM_CAPTBL_PROC2 CID ", RVM_CAPID(RVM_BOOT_CTCAPTBL0,1), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(6));
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CTCAPTBL0, RVM_BOOT_INIT_KMEM, 2, Cur_Addr, 6)==0);
    RVM_LOG_SISUS("Init:Created capability table RVM_CAPTBL_VIRT1 CID ", RVM_CAPID(RVM_BOOT_CTCAPTBL0,2), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(6));

    RVM_ASSERT(Cur_Addr==0xEC0);
}
/* End Function:RVM_Boot_Captbl_Crt ******************************************/

/* Begin Function:RVM_Boot_Pgtbl_Crt ******************************************
Description : Create all page tables at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Pgtbl_Crt(void)
{
    rvm_ptr_t Cur_Addr;

    Cur_Addr=0xEC0;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Creating process page tables.\r\n");
    /* Create all the page tables capability tables first */
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTPGTBL0, Cur_Addr, 16)==0);
    RVM_LOG_SISUS("Init:Created page table capability table CID ", RVM_BOOT_CTPGTBL0, " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(16));

    /* Then the page tables themselves */
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 0, Cur_Addr, 0x0, 1, 29, 3)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC1_N0 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,0), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x80;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 1, Cur_Addr, 0x0, 0, 26, 3)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC1_N1 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,1), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x40;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 2, Cur_Addr, 0x8020000, 0, 15, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC1_N2 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,2), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 3, Cur_Addr, 0x10000000, 0, 15, 1)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC1_N3 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,3), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 4, Cur_Addr, 0x10007000, 0, 12, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC1_N4 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,4), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 5, Cur_Addr, 0x10008000, 0, 13, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC1_N5 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,5), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 6, Cur_Addr, 0x20000000, 0, 14, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC1_N6 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,6), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 7, Cur_Addr, 0xF0000000, 0, 16, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC1_N7 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,7), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 8, Cur_Addr, 0x0, 1, 27, 3)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC2_N0 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,8), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x80;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 9, Cur_Addr, 0x8028000, 0, 15, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC2_N1 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,9), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 10, Cur_Addr, 0x10008000, 0, 12, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC2_N2 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,10), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 11, Cur_Addr, 0x20000000, 0, 13, 3)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_PROC2_N3 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,11), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x40;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 12, Cur_Addr, 0x0, 1, 27, 3)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_VIRT1_N0 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,12), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x80;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 13, Cur_Addr, 0x8030000, 0, 15, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_VIRT1_N1 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,13), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 14, Cur_Addr, 0x10008000, 0, 12, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_VIRT1_N2 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,14), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;
    RVM_ASSERT(RVM_Pgtbl_Crt(RVM_BOOT_CTPGTBL0, RVM_BOOT_INIT_KMEM, 15, Cur_Addr, 0x20004000, 0, 13, 0)==0);
    RVM_LOG_SISUS("Init:Created page table RVM_PGTBL_VIRT1_N3 CID ", RVM_CAPID(RVM_BOOT_CTPGTBL0,15), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=0x20;

    RVM_ASSERT(Cur_Addr==0x1420);
}
/* End Function:RVM_Boot_Pgtbl_Crt *******************************************/

/* Begin Function:RVM_Boot_Proc_Crt *******************************************
Description : Create all processes at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Proc_Crt(void)
{
    rvm_ptr_t Cur_Addr;

    Cur_Addr=0x1420;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Creating processes.\r\n");
    /* Create all the process capability tables first */
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTPROC0, Cur_Addr, 3)==0);
    RVM_LOG_SISUS("Init:Created process capability table CID ", RVM_BOOT_CTPROC0, " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(3));

    /* Then the processes themselves */
    RVM_ASSERT(RVM_Proc_Crt(RVM_BOOT_CTPROC0, 0, RVM_CAPTBL_PROC1, RVM_PGTBL_PROC1_N0)==0);
    RVM_LOG_SIS("Init:Created process RVM_PROC_PROC1 CID ", RVM_CAPID(RVM_BOOT_CTPROC0,0), ".\r\n");
    RVM_ASSERT(RVM_Proc_Crt(RVM_BOOT_CTPROC0, 1, RVM_CAPTBL_PROC2, RVM_PGTBL_PROC2_N0)==0);
    RVM_LOG_SIS("Init:Created process RVM_PROC_PROC2 CID ", RVM_CAPID(RVM_BOOT_CTPROC0,1), ".\r\n");
    RVM_ASSERT(RVM_Proc_Crt(RVM_BOOT_CTPROC0, 2, RVM_CAPTBL_VIRT1, RVM_PGTBL_VIRT1_N0)==0);
    RVM_LOG_SIS("Init:Created process RVM_PROC_VIRT1 CID ", RVM_CAPID(RVM_BOOT_CTPROC0,2), ".\r\n");

    RVM_ASSERT(Cur_Addr==0x1480);
}
/* End Function:RVM_Boot_Proc_Crt ********************************************/

/* Begin Function:RVM_Boot_Thd_Crt ********************************************
Description : Create all threads at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Thd_Crt(void)
{
    rvm_ptr_t Cur_Addr;

    Cur_Addr=0x1480;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Creating threads.\r\n");
    /* Create all the thread capability tables first */
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTTHD0, Cur_Addr, 4)==0);
    RVM_LOG_SISUS("Init:Created thread capability table CID ", RVM_BOOT_CTTHD0, " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(4));

    /* Then the threads themselves */
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CTTHD0, RVM_BOOT_INIT_KMEM, 0, RVM_PROC_PROC1, 17, Cur_Addr)==0);
    RVM_LOG_SISUS("Init:Created thread RVM_PROC_PROC1_THD_THD1 CID ", RVM_CAPID(RVM_BOOT_CTTHD0,0), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_THD_SIZE);
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CTTHD0, RVM_BOOT_INIT_KMEM, 1, RVM_PROC_PROC2, 16, Cur_Addr)==0);
    RVM_LOG_SISUS("Init:Created thread RVM_PROC_PROC2_THD_THD1 CID ", RVM_CAPID(RVM_BOOT_CTTHD0,1), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_THD_SIZE);
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CTTHD0, RVM_BOOT_INIT_KMEM, 2, RVM_PROC_VIRT1, 3, Cur_Addr)==0);
    RVM_LOG_SISUS("Init:Created thread RVM_PROC_VIRT1_THD_VECT CID ", RVM_CAPID(RVM_BOOT_CTTHD0,2), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_THD_SIZE);
    RVM_ASSERT(RVM_Thd_Crt(RVM_BOOT_CTTHD0, RVM_BOOT_INIT_KMEM, 3, RVM_PROC_VIRT1, 2, Cur_Addr)==0);
    RVM_LOG_SISUS("Init:Created thread RVM_PROC_VIRT1_THD_USER CID ", RVM_CAPID(RVM_BOOT_CTTHD0,3), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_THD_SIZE);

    RVM_ASSERT(Cur_Addr==0x1800);
}
/* End Function:RVM_Boot_Thd_Crt *********************************************/

/* Begin Function:RVM_Boot_Inv_Crt ********************************************
Description : Create all invocations at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Inv_Crt(void)
{
    rvm_ptr_t Cur_Addr;

    Cur_Addr=0x1800;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Creating invocations.\r\n");
    /* Create all the invocation capability tables first */
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTINV0, Cur_Addr, 1)==0);
    RVM_LOG_SISUS("Init:Created invocation capability table CID ", RVM_BOOT_CTINV0, " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(1));

    /* Then the invocations themselves */
    RVM_ASSERT(RVM_Inv_Crt(RVM_BOOT_CTINV0, RVM_BOOT_INIT_KMEM, 0, RVM_PROC_PROC2, Cur_Addr)==0);
    RVM_LOG_SISUS("Init:Created invocation RVM_PROC_PROC2_INV_INV1 CID ", RVM_CAPID(RVM_BOOT_CTINV0,0), " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_INV_SIZE);

    RVM_ASSERT(Cur_Addr==0x1850);
}
/* End Function:RVM_Boot_Inv_Crt *********************************************/

/* Begin Function:RVM_Boot_Recv_Crt *******************************************
Description : Create all receive endpoints at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Recv_Crt(void)
{
    rvm_ptr_t Cur_Addr;

    Cur_Addr=0x1850;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Creating receive endpoints.\r\n");
    /* Create all the receive endpoint capability tables first */
    RVM_ASSERT(RVM_Captbl_Crt(RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KMEM, RVM_BOOT_CTRECV0, Cur_Addr, 2)==0);
    RVM_LOG_SISUS("Init:Created receive endpoint capability table CID ", RVM_BOOT_CTRECV0, " @ address 0x", Cur_Addr, ".\r\n");
    Cur_Addr+=RVM_KOTBL_ROUND(RVM_CAPTBL_SIZE(2));

    /* Then the receive endpoints themselves */
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CTRECV0, 0)==0);
    RVM_LOG_SIS("Init:Created receive endpoint RVM_PROC_PROC1_RECV_RECV1 CID ", RVM_CAPID(RVM_BOOT_CTRECV0,0), ".\r\n");
    RVM_ASSERT(RVM_Sig_Crt(RVM_BOOT_CTRECV0, 1)==0);
    RVM_LOG_SIS("Init:Created receive endpoint RVM_PROC_PROC2_RECV_RECV1 CID ", RVM_CAPID(RVM_BOOT_CTRECV0,1), ".\r\n");

    RVM_ASSERT(Cur_Addr==0x1890);
}
/* End Function:RVM_Boot_Recv_Crt ********************************************/

/* Begin Function:RVM_Boot_Kobj_Crt *******************************************
Description : Create all kernel objects at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Kobj_Crt(void)
{
    RVM_Boot_Virtep_Crt();
    RVM_Boot_Captbl_Crt();
    RVM_Boot_Pgtbl_Crt();
    RVM_Boot_Proc_Crt();
    RVM_Boot_Thd_Crt();
    RVM_Boot_Inv_Crt();
    RVM_Boot_Recv_Crt();
}
/* End Function:RVM_Boot_Kobj_Crt ********************************************/

/* Begin Function:RVM_Boot_Virtcap_Init ***************************************
Description : Initialize all virtual machine capability table special contents.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Virtcap_Init(void)
{
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Initializing virtual machine capability tables.\r\n");
    /* Initializing virtual machine: Virt1 */
    /* System call endpoint */
    RVM_ASSERT(RVM_Captbl_Add(RVM_CAPTBL_VIRT1, 0, RVM_BOOT_CAPTBL, RVM_Vmmd_Sig_Cap, RVM_SIG_FLAG_SND)==0);
    RVM_LOG_S("Init:Delegated system call send endpoint to virtual machine RVM_PROC_VIRT1.\r\n");
    /* Vector endpoint - this is a special one */
    RVM_ASSERT(RVM_Captbl_Add(RVM_CAPTBL_VIRT1, 1, RVM_BOOT_CTVIRTEP0, 0, RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV)==0);
    RVM_LOG_S("Init:Delegated system call send endpoint to virtual machine RVM_PROC_VIRT1.\r\n");

}
/* End Function:RVM_Boot_Virtcap_Init ****************************************/

/* Begin Function:RVM_Boot_Captbl_Init ****************************************
Description : Initialize the capability tables of all processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Captbl_Init(void)
{
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Initializing process capability tables.\r\n");
    /* Initializing captbl for process: Proc1 */
    /* Event kernel capability */
    RVM_ASSERT(RVM_Captbl_Kern(RVM_CAPTBL_PROC1, 0, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, RVM_KERN_EVT_LOCAL_TRIG, RVM_KERN_EVT_LOCAL_TRIG)==0);
    RVM_LOG_S("Init:Delegated kernel capability for event sending to process RVM_PROC_PROC1 capability table position 0.\r\n");
    /* Ports */
    RVM_ASSERT(RVM_Captbl_Add(RVM_CAPTBL_PROC1, 1, RVM_BOOT_CTINV0, 0, RVM_INV_FLAG_ACT)==0);
    RVM_LOG_S("Init:Delegated invocation port RVM_PROC_PROC2_INV_INV1 to process RVM_PROC_PROC1 capability table position 1.\r\n");
    /* Receive endpoints */
    RVM_ASSERT(RVM_Captbl_Add(RVM_CAPTBL_PROC1, 2, RVM_BOOT_CTRECV0, 0, RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV)==0);
    RVM_LOG_S("Init:Delegated receive endpoint RVM_PROC_PROC1_RECV_RECV1 to process RVM_PROC_PROC1 capability table position 2.\r\n");
    /* Send endpoints */
    /* Vector endpoints */
    /* Kernel functions */
    RVM_ASSERT(RVM_Captbl_Kern(RVM_CAPTBL_PROC1, 3, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, 0x60, 0x90)==0);
    RVM_LOG_S("Init:Delegated kernel function 0x60 - 0x90 to process RVM_PROC_PROC1 capability table position 3.\r\n");
    /* Initializing captbl for process: Proc2 */
    /* Event kernel capability */
    RVM_ASSERT(RVM_Captbl_Kern(RVM_CAPTBL_PROC2, 0, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, RVM_KERN_EVT_LOCAL_TRIG, RVM_KERN_EVT_LOCAL_TRIG)==0);
    RVM_LOG_S("Init:Delegated kernel capability for event sending to process RVM_PROC_PROC2 capability table position 0.\r\n");
    /* Ports */
    /* Receive endpoints */
    RVM_ASSERT(RVM_Captbl_Add(RVM_CAPTBL_PROC2, 1, RVM_BOOT_CTRECV0, 1, RVM_SIG_FLAG_SND|RVM_SIG_FLAG_RCV)==0);
    RVM_LOG_S("Init:Delegated receive endpoint RVM_PROC_PROC2_RECV_RECV1 to process RVM_PROC_PROC2 capability table position 1.\r\n");
    /* Send endpoints */
    RVM_ASSERT(RVM_Captbl_Add(RVM_CAPTBL_PROC2, 2, RVM_BOOT_CTRECV0, 0, RVM_SIG_FLAG_SND)==0);
    RVM_LOG_S("Init:Delegated send endpoint RVM_PROC_PROC1_RECV_RECV1 to process RVM_PROC_PROC2 capability table position 2.\r\n");
    /* Vector endpoints */
    RVM_ASSERT(RVM_Captbl_Add(RVM_CAPTBL_PROC2, 3, RVM_BOOT_CTVECT0, 0, RVM_SIG_FLAG_RCV)==0);
    RVM_LOG_S("Init:Delegated vector endpoint RVM_BOOT_VECT_TIM2 to process RVM_PROC_PROC2 capability table position 3.\r\n");
    /* Kernel functions */
    RVM_ASSERT(RVM_Captbl_Kern(RVM_CAPTBL_PROC2, 4, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, 0x125, 0x126)==0);
    RVM_LOG_S("Init:Delegated kernel function 0x125 - 0x126 to process RVM_PROC_PROC2 capability table position 4.\r\n");
    /* Initializing captbl for process: Virt1 */
    /* Ports */
    /* Receive endpoints */
    /* Send endpoints */
    RVM_ASSERT(RVM_Captbl_Add(RVM_CAPTBL_VIRT1, 2, RVM_BOOT_CTRECV0, 0, RVM_SIG_FLAG_SND)==0);
    RVM_LOG_S("Init:Delegated send endpoint RVM_PROC_PROC1_RECV_RECV1 to process RVM_PROC_VIRT1 capability table position 2.\r\n");
    /* Vector endpoints */
    /* Kernel functions */
    RVM_ASSERT(RVM_Captbl_Kern(RVM_CAPTBL_VIRT1, 3, RVM_BOOT_CAPTBL, RVM_BOOT_INIT_KERN, 0x100, 0x200)==0);
    RVM_LOG_S("Init:Delegated kernel function 0x100 - 0x200 to process RVM_PROC_VIRT1 capability table position 3.\r\n");
}
/* End Function:RVM_Boot_Captbl_Init *****************************************/

/* Begin Function:RVM_Boot_Pgtbl_Init *****************************************
Description : Initialize the page tables of all processes.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Pgtbl_Init(void)
{
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Initializing process page tables.\r\n");
    /* Constructing page tables for process: Proc1 */
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC1_N0, 0x0, RVM_PGTBL_PROC1_N1, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC1_N1 into page table parent RVM_PGTBL_PROC1_N0 @ position 0x0.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC1_N1, 0x2, RVM_PGTBL_PROC1_N2, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC1_N2 into page table parent RVM_PGTBL_PROC1_N1 @ position 0x2.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC1_N1, 0x4, RVM_PGTBL_PROC1_N3, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC1_N3 into page table parent RVM_PGTBL_PROC1_N1 @ position 0x4.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC1_N3, 0x0, RVM_PGTBL_PROC1_N4, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC1_N4 into page table parent RVM_PGTBL_PROC1_N3 @ position 0x0.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC1_N3, 0x1, RVM_PGTBL_PROC1_N5, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC1_N5 into page table parent RVM_PGTBL_PROC1_N3 @ position 0x1.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC1_N0, 0x1, RVM_PGTBL_PROC1_N6, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC1_N6 into page table parent RVM_PGTBL_PROC1_N0 @ position 0x1.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC1_N0, 0x7, RVM_PGTBL_PROC1_N7, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC1_N7 into page table parent RVM_PGTBL_PROC1_N0 @ position 0x7.\r\n");

    /* Constructing page tables for process: Proc2 */
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC2_N0, 0x1, RVM_PGTBL_PROC2_N1, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC2_N1 into page table parent RVM_PGTBL_PROC2_N0 @ position 0x1.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC2_N0, 0x2, RVM_PGTBL_PROC2_N2, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC2_N2 into page table parent RVM_PGTBL_PROC2_N0 @ position 0x2.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_PROC2_N0, 0x4, RVM_PGTBL_PROC2_N3, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_PROC2_N3 into page table parent RVM_PGTBL_PROC2_N0 @ position 0x4.\r\n");

    /* Constructing page tables for process: Virt1 */
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_VIRT1_N0, 0x1, RVM_PGTBL_VIRT1_N1, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_VIRT1_N1 into page table parent RVM_PGTBL_VIRT1_N0 @ position 0x1.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_VIRT1_N0, 0x2, RVM_PGTBL_VIRT1_N2, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_VIRT1_N2 into page table parent RVM_PGTBL_VIRT1_N0 @ position 0x2.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Con(RVM_PGTBL_VIRT1_N0, 0x4, RVM_PGTBL_VIRT1_N3, RVM_PGTBL_ALL_PERM)==0);
    RVM_LOG_S("Init:Constructed page table child RVM_PGTBL_VIRT1_N3 into page table parent RVM_PGTBL_VIRT1_N0 @ position 0x4.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_PROC1_N0 */

    /* Mapping pages into page tables - RVM_PGTBL_PROC1_N1 */

    /* Mapping pages into page tables - RVM_PGTBL_PROC1_N2 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC1_N2, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_EXECUTE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x1004)==0);
    RVM_LOG_S("Init:Added page addr 0x8020000 size 0x8000 to page table RVM_PGTBL_PROC1_N2 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_PROC1_N3 */

    /* Mapping pages into page tables - RVM_PGTBL_PROC1_N4 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC1_N4, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x10007)==0);
    RVM_LOG_S("Init:Added page addr 0x10007000 size 0x1000 to page table RVM_PGTBL_PROC1_N4 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_PROC1_N5 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC1_N5, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x8004)==0);
    RVM_LOG_S("Init:Added page addr 0x10008000 size 0x2000 to page table RVM_PGTBL_PROC1_N5 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_PROC1_N6 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC1_N6, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_EXECUTE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x8000)==0);
    RVM_LOG_S("Init:Added page addr 0x20000000 size 0x4000 to page table RVM_PGTBL_PROC1_N6 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_PROC1_N7 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC1_N7, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0xF000)==0);
    RVM_LOG_S("Init:Added page addr 0xF0000000 size 0x10000 to page table RVM_PGTBL_PROC1_N7 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_PROC2_N0 */

    /* Mapping pages into page tables - RVM_PGTBL_PROC2_N1 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC2_N1, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_EXECUTE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x1005)==0);
    RVM_LOG_S("Init:Added page addr 0x8028000 size 0x8000 to page table RVM_PGTBL_PROC2_N1 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_PROC2_N2 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC2_N2, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x10008)==0);
    RVM_LOG_S("Init:Added page addr 0x10008000 size 0x1000 to page table RVM_PGTBL_PROC2_N2 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_PROC2_N3 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC2_N3, 0x3, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_EXECUTE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x10003)==0);
    RVM_LOG_S("Init:Added page addr 0x20006000 size 0x2000 to page table RVM_PGTBL_PROC2_N3 @ position 0x3.\r\n");
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_PROC2_N3, 0x4, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_EXECUTE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x10004)==0);
    RVM_LOG_S("Init:Added page addr 0x20008000 size 0x2000 to page table RVM_PGTBL_PROC2_N3 @ position 0x4.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_VIRT1_N0 */

    /* Mapping pages into page tables - RVM_PGTBL_VIRT1_N1 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_VIRT1_N1, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_EXECUTE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x1006)==0);
    RVM_LOG_S("Init:Added page addr 0x8030000 size 0x8000 to page table RVM_PGTBL_VIRT1_N1 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_VIRT1_N2 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_VIRT1_N2, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x10008)==0);
    RVM_LOG_S("Init:Added page addr 0x10008000 size 0x1000 to page table RVM_PGTBL_VIRT1_N2 @ position 0x0.\r\n");

    /* Mapping pages into page tables - RVM_PGTBL_VIRT1_N3 */
    RVM_ASSERT(RVM_Pgtbl_Add(RVM_PGTBL_VIRT1_N3, 0x0, \
                             RVM_PGTBL_READ|RVM_PGTBL_WRITE|RVM_PGTBL_EXECUTE|RVM_PGTBL_CACHEABLE|RVM_PGTBL_BUFFERABLE|RVM_PGTBL_STATIC, \
                             RVM_BOOT_PGTBL, 0x0, 0x10002)==0);
    RVM_LOG_S("Init:Added page addr 0x20004000 size 0x2000 to page table RVM_PGTBL_VIRT1_N3 @ position 0x0.\r\n");

}
/* End Function:RVM_Boot_Pgtbl_Init ******************************************/

/* Begin Function:RVM_Boot_Thd_Init *******************************************
Description : Initialize the all threads.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Thd_Init(void)
{
    rvm_ptr_t Init_Stack_Addr;

    Init_Stack_Addr=0;
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Initializing threads.\r\n");
    /* Initializing thread for process: Proc1 */
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_PROC_PROC1_THD_THD1, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_PROC_PROC1_THD_THD1, 17)==0);
    Init_Stack_Addr=RVM_Stack_Init(0x20003E00, 0x200, 0x8020000, 0x8020020);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_PROC_PROC1_THD_THD1, 0x8020000, Init_Stack_Addr, 0x1234)==0);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_PROC_PROC1_THD_THD1, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_LOG_S("Init:Initialized thread RVM_PROC_PROC1_THD_THD1 process RVM_PROC_PROC1 entry 0x8020000 stack 0x20003E00 - 0x200 param 0x1234.\r\n");

    /* Initializing thread for process: Proc2 */
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_PROC_PROC2_THD_THD1, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_PROC_PROC2_THD_THD1, 16)==0);
    Init_Stack_Addr=RVM_Stack_Init(0x20009E00, 0x200, 0x8028000, 0x8028040);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_PROC_PROC2_THD_THD1, 0x8028000, Init_Stack_Addr, 0x1234)==0);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_PROC_PROC2_THD_THD1, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_LOG_S("Init:Initialized thread RVM_PROC_PROC2_THD_THD1 process RVM_PROC_PROC2 entry 0x8028000 stack 0x20009E00 - 0x200 param 0x1234.\r\n");

    /* Initializing thread for process: Virt1 */
    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_PROC_VIRT1_THD_VECT, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_PROC_VIRT1_THD_VECT|RVM_VIRT_THDID_MARKER, 3)==0);
    Init_Stack_Addr=RVM_Stack_Init(0x20005C00, 0x400, 0x8030000, 0x8030040);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_PROC_VIRT1_THD_VECT, 0x8030000, Init_Stack_Addr, 0x0)==0);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_PROC_VIRT1_THD_VECT, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_LOG_S("Init:Initialized thread RVM_PROC_VIRT1_THD_VECT process RVM_PROC_VIRT1 entry 0x8030000 stack 0x20005C00 - 0x400 param 0x0.\r\n");

    RVM_ASSERT(RVM_Thd_Sched_Bind(RVM_PROC_VIRT1_THD_USER, RVM_Sftd_Thd_Cap, RVM_Sftd_Sig_Cap, RVM_PROC_VIRT1_THD_USER|RVM_VIRT_THDID_MARKER, 2)==0);
    RVM_ASSERT(RVM_Thd_Hyp_Set(RVM_PROC_VIRT1_THD_USER, 0x20005400)==0);
    Init_Stack_Addr=RVM_Stack_Init(0x20005800, 0x400, 0x8030020, 0x8030040);
    RVM_ASSERT(RVM_Thd_Exec_Set(RVM_PROC_VIRT1_THD_USER, 0x8030020, Init_Stack_Addr, 0x0)==0);
    RVM_ASSERT(RVM_Thd_Time_Xfer(RVM_PROC_VIRT1_THD_USER, RVM_BOOT_INIT_THD, RVM_THD_INF_TIME)==RVM_THD_INF_TIME);
    RVM_LOG_S("Init:Initialized thread RVM_PROC_VIRT1_THD_USER process RVM_PROC_VIRT1 entry 0x8030020 stack 0x20005800 - 0x400 param 0x0.\r\n");

}
/* End Function:RVM_Boot_Thd_Init ********************************************/

/* Begin Function:RVM_Boot_Inv_Init *******************************************
Description : Initialize the all invocations.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Inv_Init(void)
{
    rvm_ptr_t Init_Stack_Addr;

    Init_Stack_Addr=0;
    RVM_ASSERT(Init_Stack_Addr==0);
    RVM_LOG_S("-------------------------------------------------------------------------------\r\n");
    RVM_LOG_S("Init:Initializing invocations.\r\n");
    /* Initializing invocation for process: Proc1 */
    /* Initializing invocation for process: Proc2 */
    Init_Stack_Addr=RVM_Stack_Init(0x20009C00, 0x200, 0x8028020, 0x8028040);
    RVM_ASSERT(RVM_Inv_Set(RVM_PROC_PROC2_INV_INV1, 0x8028020, Init_Stack_Addr, 1)==0);
    RVM_LOG_S("Init:Initialized invocation RVM_PROC_PROC2_INV_INV1 process RVM_PROC_PROC2 entry 0x8028020 stack 0x20009C00 - 0x200.\r\n");

    /* Initializing invocation for process: Virt1 */
}
/* End Function:RVM_Boot_Inv_Init ********************************************/

/* Begin Function:RVM_Boot_Kobj_Init ******************************************
Description : Initialize all kernel objects at boot-time.
Input       : None.
Output      : None.
Return      : None.
******************************************************************************/
void RVM_Boot_Kobj_Init(void)
{
    RVM_Boot_Virtcap_Init();
    RVM_Boot_Captbl_Init();
    RVM_Boot_Pgtbl_Init();
    RVM_Boot_Thd_Init();
    RVM_Boot_Inv_Init();
    RVM_Virt_Crt(RVM_Virt_DB, (struct RVM_Virt_Map*)RVM_Virt_Map_DB, 1);
}
/* End Function:RVM_Boot_Kobj_Init *******************************************/

/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

