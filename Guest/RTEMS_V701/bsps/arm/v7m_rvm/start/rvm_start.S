/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief Minimal start file for the RTE guest.
 *
 * The real entry point is __RVM_Entry, supplied by the RVM platform assembly,
 * and the linker script does not use STARTUP(start.o).  This object exists so
 * that the RTEMS waf link framework has a start file to track; it is not
 * linked into the image.
 */

	.syntax	unified
	.thumb
	.section	.text.rte_start, "ax", %progbits
	.align	2

	.globl	_start
	.type	_start, %function
	.thumb_func
_start:
	b	_start
	.size	_start, . - _start
