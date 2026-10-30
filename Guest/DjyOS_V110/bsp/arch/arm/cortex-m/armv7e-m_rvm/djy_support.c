//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Runtime support shim (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: Supplies the kernel's remaining libc surface: a minimal
//             printf over the debug console hypercall (the stdio
//             component is not part of this guest), the errno storage
//             that newlib expects, a word-wise memset for the kernel's
//             stack repaint, the static heap cession table, and the
//             inert low power hooks.
//Notes: Only the conversions the kernel actually prints are
//       implemented: %s %c %d %u %x %p %%, with the l/ll length
//       modifiers and zero/width padding.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#include "rvm.h"
#include "stdint.h"
#include "stddef.h"
#include "stdarg.h"

extern void djy_dbg_puts(const char *str);

/*---- memset --------------------------------------------------------------
The kernel repaints a whole thread stack with 'd' every time a pooled
event thread is recycled (the pattern its stack overflow check scans
for).  newlib-nano's memset is a byte-at-a-time loop - three
instructions per byte - which makes that repaint cost about 3.3 cycles
per byte and dominate the event turnover.  This word-wise version is
link-time preferred over the library one and makes the same repaint
roughly ten times cheaper, without touching the kernel or losing the
'fence' semantics the overflow check depends on. */
void *memset(void *dst, int c, size_t n);

void *memset(void *dst, int c, size_t n)
{
    unsigned char *d = (unsigned char *)dst;
    u32 word = (u32)(unsigned char)c;
    u32 count = (u32)n;

    word = word | (word << 8) | (word << 16) | (word << 24);

    while((count != 0U) && ((((u32)d) & 3U) != 0U))
    {
        *d++ = (unsigned char)c;
        count--;
    }
    while(count >= 16U)
    {
        ((u32 *)d)[0] = word;
        ((u32 *)d)[1] = word;
        ((u32 *)d)[2] = word;
        ((u32 *)d)[3] = word;
        d += 16;
        count -= 16U;
    }
    while(count >= 4U)
    {
        *(u32 *)d = word;
        d += 4;
        count -= 4U;
    }
    while(count != 0U)
    {
        *d++ = (unsigned char)c;
        count--;
    }
    return dst;
}

/* The guest-wide errno storage - the errno shim declares it. */
int errno;

/* The link-script symbols of the original DjyOS images: the C++
 * constructor table bounds and the heap list head.  The RVM guest
 * has no static constructors and a single static heap, so both are
 * inert placeholders that satisfy the references. */
extern const char __ctors_start__;
extern const char __ctors_end__;
const char __ctors_start__;
const char __ctors_end__;

/* The static heap of the guest, described in the cession table format
 * the original link scripts emitted (see __Heap_MemHeapScan): a 3-word
 * header, one 3-word cession span, a NUL-terminated compact heap name
 * and a zero word terminating the heap list.  The name must be packed
 * bytes - u32-spaced characters derail the scan's strlen walk.
 *
 * 32KB because the Activation item allocates a fresh thread per round
 * and the completed ones are only freed by the idle event - the driver
 * deliberately yields every few rounds so the peak stays bounded. */
static char djy_heap_pool[32768] __attribute__((aligned(8)));

/* pHeapList IS the table head - the scan reads the cession count from
 * *(u32*)&pHeapList, so the symbol must sit exactly on the descriptor,
 * exactly as the original link scripts placed it.  The union keeps the
 * C type of the pointer (heap-static.c declares it as void *) while
 * making &pHeapList the table itself. */
const union
{
    void *as_ptr;
    struct
    {
        u32 cession_num;
        u32 align;
        u32 property;
        u32 bottom;
        u32 top;
        u32 page_size;
        char name[4];
        u32 list_end;
    } table;
} pHeapList =
{
    .table =
    {
        1U, 0U, 0U,
        (u32)djy_heap_pool,
        (u32)(djy_heap_pool + sizeof(djy_heap_pool)),
        0U,
        {'s', 'y', 's', 0},
        0U
    }
};

/* The idle action of the application.  CleanWakeupEvent is the only
 * safe place for it: the kernel calls it with the virtual interrupt
 * mask open, while the low power entry below runs inside the atom
 * started right after this call.  Parking the user thread from there
 * would never be woken - the vector thread does not deliver the timer
 * while RVM_Int_Mask is set.  The application installs its hook from
 * main(): the plain one parks in the hypervisor, the benchmark installs
 * its idle driver pulse, and NULL keeps the busy idle. */
static void (*djy_idle_hook)(void);

void djy_set_idle_hook(void (*hook)(void))
{
    djy_idle_hook = hook;
}

void CleanWakeupEvent(void)
{
    if(djy_idle_hook != NULL)
        djy_idle_hook();
}

void __Heap_CleanUp(unsigned short event_id)
{
    (void)event_id;
}

//----Low power hooks----------------------------------------------------------
//The idle event parks through LP_EntryLowPower, whose callbacks come
//from the BSP.  A guest cannot power the core down - the hypervisor
//owns the hardware - so the hooks are inert: the registration also
//installs the kernel's LP_EmptyReCall as the gate, which keeps the
//level machine consistent without entering any sleep state.
extern void Register_LowPower_Function(void (*EntrySleep)(u8, u32),
                                       bool_t (*SaveSleepLevel)(u32),
                                       bool_t (*SaveRamL3)(void),
                                       void (*AsmSaveReg)(struct ThreadVm *,
                                                          bool_t (*)(void),
                                                          void (*)(u8, u32)));

static void djy_lp_entry_sleep(u8 sleep_level, u32 pend_ticks)
{
    (void)sleep_level;
    (void)pend_ticks;
}

static bool_t djy_lp_save_level(u32 sleep_level)
{
    (void)sleep_level;
    return false;
}

static bool_t djy_lp_save_ram(void)
{
    return false;
}

static void djy_lp_asm_save_reg(struct ThreadVm *vm,
                                bool_t (*SaveRamL3)(void),
                                void (*EntrySleep)(u8, u32))
{
    (void)vm;
    (void)SaveRamL3;
    (void)EntrySleep;
}

void djy_lp_module_init(void)
{
    Register_LowPower_Function(djy_lp_entry_sleep, djy_lp_save_level,
                               djy_lp_save_ram, djy_lp_asm_save_reg);
}

static void djy_putc(char c)
{
    RVM_Hyp_Putchar(c);
}

static void djy_puts(const char *s)
{
    djy_dbg_puts(s);
}

static void djy_putu(unsigned long long v, unsigned base, int upper,
                     int width, char pad)
{
    char buf[24];
    int len = 0;
    const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";

    do
    {
        buf[len] = digits[v % base];
        v /= base;
        len++;
    }while(v != 0U);

    while(len < width)
    {
        buf[len] = pad;
        len++;
    }

    while(len != 0)
    {
        len--;
        djy_putc(buf[len]);
    }
}

int printf(const char *fmt, ...)
{
    va_list ap;
    const char *p = fmt;
    int lcnt, width;
    char pad;
    long long sv;
    unsigned long long uv;

    va_start(ap, fmt);

    while(*p != '\0')
    {
        if(*p != '%')
        {
            djy_putc(*p);
            p++;
            continue;
        }

        p++;
        pad = ' ';
        width = 0;
        lcnt = 0;

        if(*p == '0')
        {
            pad = '0';
            p++;
        }
        while((*p >= '0') && (*p <= '9'))
        {
            width = width * 10 + (*p - '0');
            p++;
        }
        while((*p == 'l') || (*p == 'h'))
        {
            if(*p == 'l')
                lcnt++;
            p++;
        }

        switch(*p)
        {
            case '%':
                djy_putc('%');
                break;
            case 'c':
                djy_putc((char)va_arg(ap, int));
                break;
            case 's':
                djy_puts(va_arg(ap, const char *));
                break;
            case 'd':
            case 'i':
                sv = (lcnt >= 2) ? va_arg(ap, long long) :
                    ((lcnt == 1) ? (long long)va_arg(ap, long) :
                                   (long long)va_arg(ap, int));
                if(sv < 0)
                {
                    djy_putc('-');
                    uv = (unsigned long long)(-sv);
                }
                else
                    uv = (unsigned long long)sv;
                djy_putu(uv, 10U, 0, width, pad);
                break;
            case 'u':
            case 'x':
            case 'X':
                uv = (lcnt >= 2) ? va_arg(ap, unsigned long long) :
                    ((lcnt == 1) ? va_arg(ap, unsigned long) :
                                   (unsigned long long)va_arg(ap, unsigned int));
                djy_putu(uv, (*p == 'u') ? 10U : 16U, (*p == 'X'), width, pad);
                break;
            case 'p':
                djy_puts("0x");
                djy_putu((unsigned long long)(unsigned long)va_arg(ap, void *),
                         16U, 0, 8, '0');
                break;
            default:
                djy_putc('%');
                djy_putc(*p);
                break;
        }

        if(*p != '\0')
            p++;
    }

    va_end(ap);
    return 0;
}

/* The kernel's console funnel - djyos.c and the lock module report
 * their warnings through printk with the same format string family. */
void printk(char *fmt, ...)
{
    (void)fmt;
}

int putchar(int c)
{
    RVM_Hyp_Putchar((char)c);
    return c;
}
