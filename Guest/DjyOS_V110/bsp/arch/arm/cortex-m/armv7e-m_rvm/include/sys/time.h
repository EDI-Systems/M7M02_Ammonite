/*
 * DjyOS V1.1.0 RVM port - the sys/time shim.
 *
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The DjyOS kernel itself is copyright the
 * DjyOS Open Source Development Team and is not modified by this file.
 *
 * The kernel time module defines gettimeofday/settimeofday with a
 * struct timezone second parameter, which clashes with the newlib
 * prototype (void *).  This shim provides the structures the kernel
 * sources use plus prototypes that match the kernel definitions, and
 * forwards to the newlib header for everything else with the two
 * conflicting declarations neutralized.
 */
#ifndef __DJY_RVM_SYS_TIME_H__
#define __DJY_RVM_SYS_TIME_H__

/* Neutralize the newlib declarations of the two conflicting names,
 * then pull in the rest of the header. */
#define gettimeofday                     djy_gettimeofday
#define settimeofday                     djy_settimeofday

#include_next <sys/time.h>

#undef gettimeofday
#undef settimeofday

/* The kernel signatures. */
struct timeval;
struct timezone;
int gettimeofday(struct timeval *tv, struct timezone *tz);
int settimeofday(const struct timeval *tv, const struct timezone *tz);

#endif /* __DJY_RVM_SYS_TIME_H__ */
