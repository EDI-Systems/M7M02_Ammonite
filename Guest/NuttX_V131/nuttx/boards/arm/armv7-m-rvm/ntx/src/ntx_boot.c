/****************************************************************************
 * boards/arm/armv7-m-rvm/ntx/src/ntx_boot.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Reference application entry for the in-tree NTX board.  The RVM generator
 * replaces this file with the per-Virt application; the in-tree version only
 * demonstrates that the guest is alive.
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <nuttx/clock.h>

#include <unistd.h>

/* Force the descriptor object out of libboard.a even though the monitor,
 * not a C reference, consumes it.
 */

extern const uint32_t RVM_Desc[5];
extern const void *volatile ntx_desc_keep;

static void ntx_puthex(uint32_t v)
{
  static const char h[] = "0123456789abcdef";
  int i;

  for (i = 28; i >= 0; i -= 4)
    {
      up_putc(h[(v >> i) & 15U]);
    }
}

int ntx_main(int argc, char *argv[])
{
  static const char banner[] = "\r\nNTX: NuttX on RVM\r\n";
  int i;

  (void)argc;
  (void)argv;

  /* Keep the descriptor object linked; the monitor consumes it. */

  if (ntx_desc_keep == 0)
    {
      return 0;
    }

  for (i = 0; banner[i] != '\0'; i++)
    {
      up_putc(banner[i]);
    }

  for (; ; )
    {
      sleep(1);
      up_putc('T');
      ntx_puthex(clock_systime_ticks());
      up_putc('\r');
      up_putc('\n');
    }

  return 0;
}
