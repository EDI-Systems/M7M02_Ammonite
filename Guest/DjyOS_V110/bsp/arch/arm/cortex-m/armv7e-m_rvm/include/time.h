/*
 * DjyOS V1.1.0 RVM port - the time shim.
 *
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The DjyOS kernel itself is copyright the
 * DjyOS Open Source Development team and is not modified by this file.
 *
 * The DjyOS time module uses a struct tm extended with tm_us and its
 * own Time_* prototypes; the original DIDE toolchain carried both in
 * its time.h.  This shim is the only time.h the guest sees: it
 * deliberately does NOT chain newlib's header, because the kernel's
 * time.c defines the POSIX entry points with this extended structure -
 * pulling in newlib's declarations of the same names would make the two
 * prototypes conflict.  time_t and the timeval/timezone structures come
 * from <sys/time.h>, which this header is always reached through.
 */
#ifndef __DJY_RVM_TIME_H__
#define __DJY_RVM_TIME_H__

#undef tm

struct tm_djy
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    long __tm_gmtoff;
    const char *__tm_zone;
    long tm_us;
};

/* Every struct tm the DjyOS sources spell is the extended one. */
#define tm tm_djy

struct tm_djy *Time_LocalTime_r(const s64 *time, struct tm_djy *result);
struct tm_djy *Time_LocalTimeUs_r(const s64 *time, struct tm_djy *result);
extern long timezone;
extern int daylight;

/* Error codes of the time module (originally in the DIDE errno). */
#define EN_CLOCK_FMT_ERROR              1
#define EN_CLOCK_YEAR_ERROR             2
#define EN_CLOCK_MON_ERROR              3
#define EN_CLOCK_DAY_ERROR              4
#define EN_CLOCK_HOUR_ERROR             5
#define EN_CLOCK_MIN_ERROR              6
#define EN_CLOCK_SEC_ERROR              7
#define EN_CLOCK_US_ERROR               8

#endif /* __DJY_RVM_TIME_H__ */
