/******************************************************************************
Filename    : rme_platform_stm32h743zi.ld
Author      : The RVM project generator.
Date        : 15/11/2026 04:05:58
Licence     : Unlicense; see COPYING for details.
Description : The GCC linker script. This file is intended
              to be used with STM32H743ZI, and the GNU toolchain.
******************************************************************************/

/* Memory ********************************************************************/
MEMORY
{
    /* Statically allocated kernel data segment */
    KDATA (rw) : ORIGIN = 0x20000000 , LENGTH = 0xB400
    /* Dynamically allocated kernel data segment */
    KMEM (rw) : ORIGIN = 0x2000B400 , LENGTH = 0x3B80
    /* Kernel stack segment */
    KSTK (rw) : ORIGIN = 0x2000EFA0 , LENGTH = 0x1000

    /* Kernel code segment */
    KCODE (rx) : ORIGIN = 0x8000000 , LENGTH = 0x20000
    /* Monitor code segment */
    MCODE (rx) : ORIGIN = 0x8020000 , LENGTH = 0x20000
    /* Proc1 code segment */
    PROC1_CODE (rx) : ORIGIN = 0x80E0000 , LENGTH = 0x2000
    /* Proc2 code segment */
    PROC2_CODE (rx) : ORIGIN = 0x80E2000 , LENGTH = 0x2000
    /* Virt_FRT code segment */
    VIRT_FRT_CODE (rx) : ORIGIN = 0x80A0000 , LENGTH = 0x8000
    /* Virt_UO3 code segment */
    VIRT_UO3_CODE (rx) : ORIGIN = 0x80A8000 , LENGTH = 0x8000
    /* Virt_RTT code segment */
    VIRT_RTT_CODE (rx) : ORIGIN = 0x80B0000 , LENGTH = 0x8000
    /* Virt_UO2 code segment */
    VIRT_UO2_CODE (rx) : ORIGIN = 0x80B8000 , LENGTH = 0x8000
    /* Virt_CHB code segment */
    VIRT_CHB_CODE (rx) : ORIGIN = 0x80C0000 , LENGTH = 0x8000
    /* Virt_RMP code segment */
    VIRT_RMP_CODE (rx) : ORIGIN = 0x80C8000 , LENGTH = 0x8000
    /* Virt_KLT code segment */
    VIRT_KLT_CODE (rx) : ORIGIN = 0x80D0000 , LENGTH = 0x8000
    /* Virt_DJY code segment */
    VIRT_DJY_CODE (rx) : ORIGIN = 0x80D8000 , LENGTH = 0x8000
    /* Virt_NTX code segment */
    VIRT_NTX_CODE (rx) : ORIGIN = 0x8090000 , LENGTH = 0x10000
    /* Virt_ZPR code segment */
    VIRT_ZPR_CODE (rx) : ORIGIN = 0x8030000 , LENGTH = 0x30000
    /* Virt_RTE code segment */
    VIRT_RTE_CODE (rx) : ORIGIN = 0x8060000 , LENGTH = 0x30000
}
/* End Memory ****************************************************************/

/* Stack *********************************************************************/
__RME_Stack = ORIGIN(KSTK) + LENGTH(KSTK);
/* End Stack Definitions *****************************************************/

/* Entry *********************************************************************/
ENTRY(__RME_Entry);
/* End Entry *****************************************************************/

/* Process *******************************************************************/
SECTIONS
{
/* Monitor image */
.MONITOR_CODE : ALIGN(4)
{
    KEEP(*monitor_image.o(.text .text.*))
    KEEP(*monitor_image.o(.rodata .rodata.* .constdata .constdata.*))
} > MCODE

/* Process image */
.PROC1_CODE : ALIGN(4)
{
    KEEP(*proc1_image.o(.text .text.*))
    KEEP(*proc1_image.o(.rodata .rodata.* .constdata .constdata.*))
} > PROC1_CODE

/* Process image */
.PROC2_CODE : ALIGN(4)
{
    KEEP(*proc2_image.o(.text .text.*))
    KEEP(*proc2_image.o(.rodata .rodata.* .constdata .constdata.*))
} > PROC2_CODE

/* Process image */
.VIRT_FRT_CODE : ALIGN(4)
{
    KEEP(*virt_frt_image.o(.text .text.*))
    KEEP(*virt_frt_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_FRT_CODE

/* Process image */
.VIRT_UO3_CODE : ALIGN(4)
{
    KEEP(*virt_uo3_image.o(.text .text.*))
    KEEP(*virt_uo3_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_UO3_CODE

/* Process image */
.VIRT_RTT_CODE : ALIGN(4)
{
    KEEP(*virt_rtt_image.o(.text .text.*))
    KEEP(*virt_rtt_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_RTT_CODE

/* Process image */
.VIRT_UO2_CODE : ALIGN(4)
{
    KEEP(*virt_uo2_image.o(.text .text.*))
    KEEP(*virt_uo2_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_UO2_CODE

/* Process image */
.VIRT_CHB_CODE : ALIGN(4)
{
    KEEP(*virt_chb_image.o(.text .text.*))
    KEEP(*virt_chb_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_CHB_CODE

/* Process image */
.VIRT_RMP_CODE : ALIGN(4)
{
    KEEP(*virt_rmp_image.o(.text .text.*))
    KEEP(*virt_rmp_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_RMP_CODE

/* Process image */
.VIRT_KLT_CODE : ALIGN(4)
{
    KEEP(*virt_klt_image.o(.text .text.*))
    KEEP(*virt_klt_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_KLT_CODE

/* Process image */
.VIRT_DJY_CODE : ALIGN(4)
{
    KEEP(*virt_djy_image.o(.text .text.*))
    KEEP(*virt_djy_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_DJY_CODE

/* Process image */
.VIRT_NTX_CODE : ALIGN(4)
{
    KEEP(*virt_ntx_image.o(.text .text.*))
    KEEP(*virt_ntx_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_NTX_CODE

/* Process image */
.VIRT_ZPR_CODE : ALIGN(4)
{
    KEEP(*virt_zpr_image.o(.text .text.*))
    KEEP(*virt_zpr_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_ZPR_CODE

/* Process image */
.VIRT_RTE_CODE : ALIGN(4)
{
    KEEP(*virt_rte_image.o(.text .text.*))
    KEEP(*virt_rte_image.o(.rodata .rodata.* .constdata .constdata.*))
} > VIRT_RTE_CODE
/* End Process ***************************************************************/

/* Section *******************************************************************/
/* Kernel code segment */
.KERNEL_CODE : ALIGN(4)
{
    __RME_Code_Start = .;
    KEEP(*_gcc.o(.text.rme_vector))
    *(.text .text.*)
    *(.srodata .srodata.* .sconstdata .sconstdata.*)
    *(.rodata .rodata.* .constdata .constdata.*)
    . = ALIGN(4);
    __RME_Code_End = .;
} > KCODE

/* Statically allocated kernel data segment */
__RME_Data_Load = LOADADDR(.KERNEL_DATA);
.KERNEL_DATA : ALIGN(4)
{
    FILL(0xFF)
    __RME_Data_Start = .;
    __RME_Global = . + 0x800;
    *(.sdata_begin .sdata_begin.*)
    *(.sdata .sdata.*)
    *(.sdata2.*)
    *(.sdata_end .sdata_end.*)
    *(.data_begin .data_begin.*)
    *(.data .data.*)
    *(.data_end .data_end.*)
    . = ALIGN(4);
    __RME_Data_End = .;
} > KDATA AT > KCODE

/* Statically allocated kernel bss segment */
.KERNEL_ZERO (NOLOAD) : ALIGN(4)
{
    __RME_Zero_Start = .;
    *(.sbss_begin .sbss_begin.*)
    *(.sbss .sbss.*)
    *(.sbss2.*)
    *(SCOMMON)
    *(.sbss_end .sbss_end.*)
    *(.bss_begin .bss_begin.*)
    *(.bss .bss.*)
    *(COMMON)
    *(.bss_end .bss_end.*)
    . = ALIGN(4);
    __RME_Zero_End = .;
} > KDATA

/* Statically allocated kernel noinit segment */
.KERNEL_NOINIT (NOLOAD) : ALIGN(4)
{
    __RME_Noinit_Start = .;
    *(.snoinit .snoinit.*)
    *(.noinit .noinit.*)
    . = ALIGN(4);
    __RME_Noinit_End = .;
} > KDATA

/* Compatibility symbols for various architectures */
PROVIDE(__stack = __RME_Stack);
PROVIDE(__initial_sp = __RME_Stack);
PROVIDE(__initial_sp$ = __RME_Stack);
PROVIDE(__global_pointer = __RME_Global);
PROVIDE(__global_pointer$ = __RME_Global);
PROVIDE(__start = __RME_Entry);
PROVIDE(_stext = __RME_Code_Start);
PROVIDE(_etext = __RME_Code_End);
PROVIDE(_sidata = __RME_Data_Load);
PROVIDE(_sdata = __RME_Data_Start);
PROVIDE(_edata = __RME_Data_End);
PROVIDE(_sbss = __RME_Zero_Start);
PROVIDE(_ebss = __RME_Zero_End);
PROVIDE(_snoinit = __RME_Noinit_Start);
PROVIDE(_enoinit = __RME_Noinit_End);
PROVIDE(__text_start__ = __RME_Code_Start);
PROVIDE(__text_end__ = __RME_Code_End);
PROVIDE(__data_load__ = __RME_Data_Load);
PROVIDE(__data_start__ = __RME_Data_Start);
PROVIDE(__data_end__ = __RME_Data_End);
PROVIDE(__bss_start__ = __RME_Zero_Start);
PROVIDE(__bss_end__ = __RME_Zero_End);
PROVIDE(__noinit_start__ = __RME_Noinit_Start);
PROVIDE(__noinit_end__ = __RME_Noinit_End);
PROVIDE(end = __RME_Noinit_End);
PROVIDE(_end = __RME_Noinit_End);
PROVIDE(__end = __RME_Noinit_End);
PROVIDE(__end__ = __RME_Noinit_End);
/* End Section ***************************************************************/
}
/* End Of File ***************************************************************/

/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

