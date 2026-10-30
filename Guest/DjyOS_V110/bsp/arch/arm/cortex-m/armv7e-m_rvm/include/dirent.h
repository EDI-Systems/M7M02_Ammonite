/*
 * DjyOS V1.1.0 RVM port - the dirent shim.
 *
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The DjyOS kernel itself is copyright the
 * DjyOS Open Source Development team and is not modified by this file.
 *
 * object.c includes <dirent.h> unconditionally for the object
 * directory enumeration; the RVM guest has no file system, so this
 * provides the minimal type surface it compiles against.
 */
#ifndef __DJY_RVM_DIRENT_H__
#define __DJY_RVM_DIRENT_H__

#define DT_UNKNOWN      0
#define DT_FIFO         1
#define DT_CHR          2
#define DT_DIR          4
#define DT_REG          8

#define DIRENT_IS_DIR                  (4U)
#define DIRENT_IS_REG                  (8U)

struct __dir
{
    int __fd;
    int __dd_loc;
    int __dd_size;
    char *__dd_buf;
    void *__ptr;
};
typedef struct __dir tagDIR;
typedef struct __dir DIR;

struct dirent
{
    unsigned long   d_ino;
    unsigned long   d_off;
    unsigned short  d_reclen;
    unsigned char   d_type;
    char            d_name[256];
};

#endif /* __DJY_RVM_DIRENT_H__ */
