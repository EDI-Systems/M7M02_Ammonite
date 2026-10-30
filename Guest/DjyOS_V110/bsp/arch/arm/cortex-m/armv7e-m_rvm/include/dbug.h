/*
 * DjyOS V1.1.0 RVM port - Copyright (C) 2026 Evo-Devo Instrum.
 * All rights reserved
 *
 * Minimal replacement of the component debug header for the RVM
 * guest build: the kernel sources call the debug/info/error print
 * family, and the guest console is the per-character debug hypercall
 * - far too slow for chatty debugging.  Everything is compiled out
 * except the error level, which the port funnels to the same console
 * so that bring-up problems remain visible.
 */
#ifndef __DBUG_H__
#define __DBUG_H__

#define CN_DEBUG     4
#define CN_INFO      3
#define CN_WARN      2
#define CN_ERR       1
#define CN_CRIT      0

extern void djy_dbg_puts(const char *str);

/* The single compiled-out level indicator - errors still print. */
#define debuglevel (0)

#define debug_printf(module,fmt, ...)   do { } while(0)
#define info_printf(module,fmt, ...)    do { } while(0)
#define warn_printf(module,fmt, ...)    do { } while(0)

#define error_printf(module,fmt, ...)   do { } while(0)
#define critical_printf(module,fmt, ...) do { } while(0)

#endif /* __DBUG_H__ */
