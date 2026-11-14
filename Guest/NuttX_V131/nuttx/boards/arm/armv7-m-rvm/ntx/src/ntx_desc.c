/****************************************************************************
 * boards/arm/armv7-m-rvm/ntx/src/ntx_desc.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The RVM descriptor is placed at the image base by ntx.ld.  Its user entry
 * is the RVM platform entry, which initializes .data/.bss and calls main().
 ****************************************************************************/

#include <nuttx/config.h>

#include "rvm.h"

extern void __RVM_Entry(void);
extern void __RVM_Stub(void);

rvm_ret_t Thd_Vct(rvm_ret_t param)
{
  (void)param;

  RVM_Virt_Init();
  RVM_Vct_Loop();

  return 0;
}

const rvm_ptr_t RVM_Desc[5] =
{
  RVM_MAGIC_VIRTUAL,
  3U,
  (rvm_ptr_t)Thd_Vct,
  (rvm_ptr_t)__RVM_Entry,
  (rvm_ptr_t)__RVM_Stub
};

const void *volatile ntx_desc_keep = (const void *)RVM_Desc;

/* The RVM library uses this for debug output.  The generated per-Virt
 * project takes it from the framework's prc_<vm>.c instead of this file.
 */

void RVM_Putchar(char ch)
{
  RVM_Hyp_Putchar(ch);
}
