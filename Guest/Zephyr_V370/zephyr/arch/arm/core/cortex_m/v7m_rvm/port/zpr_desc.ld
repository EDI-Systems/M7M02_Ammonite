/*
 * SPDX-License-Identifier: Unlicense
 *
 * Place RVM_Desc at the image base and reserve the rest of the first 0x20
 * bytes.  This file is included inside Zephyr's rom_start section before
 * .text.
 */

. = ALIGN(4);
/*
 * The generator emits a plain const RVM_Desc[] (no section attribute).
 * Match the generated descriptor object by name and pull its read-only
 * data here, before .text, so the monitor always finds the magic at the
 * image base.  .o* covers both GNU ld's .o and Zephyr/Ninja's .obj.
 */
KEEP(*prc_*_desc*.o*(.rodata .rodata.* .data.rel.ro .data.rel.ro.*))
. = ALIGN(0x20);

/* __RVM_Entry is not used by this port, but the shared RVM platform asm
 * still references these symbols in its dead first-entry path.
 */
__RVM_Data_Start = __data_region_start;
__RVM_Data_End = __data_region_end;
__RVM_Data_Load = __data_region_load_start;
__RVM_Zero_Start = __bss_start;
__RVM_Zero_End = __bss_end;
