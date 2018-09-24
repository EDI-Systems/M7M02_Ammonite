/******************************************************************************
Filename    : rme_platform_STM32F767IG.ld
Author      : pry
Date        : 24/06/2017
Licence     : LGPL v3+; see COPYING for details.
Description : The scatter file for Cortex-M7 layout. This file is intended 
              to be used with STM32F767IGT6, and the GNU toolchain.
              ROM: 0x08000000 0x00100000
              RAM: 0x20000000 0x00080000
******************************************************************************/

/* Memory Definitions *********************************************************
Description : This section will define the memory layout of the system. This linker
              script is only responsible for the Init process.
Component   : ORIGIN - Starting address of the memory region.
              LENGTH - Length of the region.
******************************************************************************/
MEMORY
{
    /* Interrupt flag sharing segment */
    FRAM    (xrw) : ORIGIN = 0x20010000, LENGTH = 4K
    /* Init initalized segment */
    IRAM    (xrw) : ORIGIN = 0x20011000, LENGTH = 56K
    /* Stack segment */
    SRAM    (xrw) : ORIGIN = 0x2001F000, LENGTH = 4K
    
    /* Init flash segment */
    FLASH  (rx)   : ORIGIN = 0x08010000, LENGTH = 64K
}
/* End Memory Definitions ****************************************************/

/* Stack Definitions *********************************************************/
/* The '__stack' definition is required by crt0, do not remove it. */
__stack = ORIGIN(SRAM) + LENGTH(SRAM);
/* End Stack Definitions *****************************************************/

/* Entry Point Definitions ***************************************************/
/* The entry point is informative, for debuggers and simulators,
 * since the Cortex-M vector points to it anyway. */
ENTRY(_RVM_Entry);
SECTIONS
{
/* End Entry Point Definitions ***********************************************/

/* Begin Section:.text ********************************************************
Description : The program code is stored in the .text section, which goes to FLASH.
Location    : Flash
Component   : .text - The code segment.
              .rodata.* - The read-only data segment.
******************************************************************************/
.text : ALIGN(4)
{
    /* the startup code */
    *rvm_platform_cmx_asm_gcc.o(.text .text.*)
    /* All remaining code */
    *(.text .text.*)
    /* Read-only data (constants) */
    *(.rodata .rodata.* .constdata .constdata.*)
} > FLASH
/* End Section:.text *********************************************************/

/* Begin Section:.data ********************************************************
Description : The main initialized data section. The program executes knowing that
              the data is in the RAM but the loader puts the initial values in the
              FLASH (inidata). It is one task of the startup to copy the initial
              values from FLASH to RAM. The RME kernel does not really rely on the
              data section to be initialized because it never uses preinitialized
              global variables.
Location    : RAM
Component   : .data - The sections to put into the RAM.
******************************************************************************/
/* Used by the startup code to initialise the .data section */
_sidata = LOADADDR(.data);
.data : ALIGN(8192)
{
    FILL(0xFF)
    /* This is used by the startup code to initialise the .data section */
    _sdata = . ;        	/* STM specific definition */
    __data_start__ = . ;
    *(.data_begin .data_begin.*)
    *(.data .data.*)
	*(.data_end .data_end.*)
	. = ALIGN(4);

    /* This is used by the startup code to initialise the .data section */
    _edata = . ;        	/* STM specific definition */
    __data_end__ = . ;
} > IRAM AT > FLASH
/* End Section:.data *********************************************************/

/* Begin Section:.bss *********************************************************
Description : The initialised-to-0 data sections. NOLOAD is used to avoid
              the "section `.bss' type changed to PROGBITS" warning. This is the
              main region which is placed in RAM. Actually the RME does not
              initialize its bss because there is no such need.
Location    : RAM
Component   : .bss - The sections to put into the RAM, and initialized to 0.
******************************************************************************/
.bss (NOLOAD) : ALIGN(4)
{
    __bss_start__ = .;     	/* standard newlib definition */
    _sbss = .;              /* STM specific definition */
    *(.bss_begin .bss_begin.*)
    *(.bss .bss.*)
    *(COMMON)
    *(.bss_end .bss_end.*)
	. = ALIGN(4);

    __bss_end__ = .;        /* standard newlib definition */
    _ebss = . ;             /* STM specific definition */
} > IRAM
/* End Section:.bss **********************************************************/

/* Begin Section:.noinit ******************************************************
Description : The uninitialised data sections. NOLOAD is used to avoid
              the "section `.noinit' type changed to PROGBITS" warning.
Location    : RAM
Component   : .noinit - The sections to put into the RAM, and not initialized.
******************************************************************************/
.noinit (NOLOAD) : ALIGN(4)
{
    _noinit = .;
    *(.noinit .noinit.*) 
    . = ALIGN(4) ;
    _end_noinit = .;   
} > IRAM
/* Mandatory to be word aligned, _sbrk assumes this */
PROVIDE ( end = _end_noinit ); /* was _ebss */
PROVIDE ( _end = _end_noinit );
PROVIDE ( __end = _end_noinit );
PROVIDE ( __end__ = _end_noinit );
/* End Section:.noinit *******************************************************/

/* End Of File ***************************************************************/
}
/* Copyright (C) Evo-Devo Instrum. All rights reserved ***********************/

