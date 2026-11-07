/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief Console output for the RVM ARMv7-M guest.
 *
 * The guest has no UART of its own.  Output is forwarded to the RVM monitor
 * debug console with RVM_Hyp_Putchar(); input is not supported.
 */

#include <rtems/bspIo.h>

#include "rvm.h"

static void rvm_output_char( char c )
{
  RVM_Hyp_Putchar(c);
}

BSP_output_char_function_type BSP_output_char = rvm_output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
