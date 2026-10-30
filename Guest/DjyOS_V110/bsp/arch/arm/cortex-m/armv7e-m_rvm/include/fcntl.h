/*
 * DjyOS V1.1.0 RVM port - the fcntl shim.
 *
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The DjyOS kernel itself is copyright the
 * DjyOS Open Source Development team and is not modified by this file.
 *
 * object.h includes <fcntl.h> and objhandle.c uses the F_* commands;
 * the RVM guest has no file descriptor layer, this provides the
 * constant surface it compiles against.
 */
#ifndef __DJY_RVM_FCNTL_H__
#define __DJY_RVM_FCNTL_H__

#define O_RDONLY        0x0000U
#define O_WRONLY        0x0001U
#define O_RDWR          0x0002U
#define O_CREAT         0x0100U
#define O_TRUNC         0x0200U
#define O_APPEND        0x0400U
#define O_EXCL          0x0080U
#define O_DIRECTORY     0x10000U

#define F_GETFD         1U
#define F_SETFD         2U
#define F_GETFL         3U
#define F_SETFL         4U
#define F_SETCONTEXT    5U
#define F_GETCONTEXT    6U
#define F_GETFILE       7U
#define F_SETEVENT      8U
#define F_CLREVENT      9U
#define F_SETTIMEOUT    10U
#define F_GETTIMEOUT    11U
#define F_DUPFD         12U
#define F_GETOWN        13U
#define F_SETOWN        14U
#define O_BLOCK_COMPLETE 0x20000U
#define O_NONBLOCK      0x0004U

#endif /* __DJY_RVM_FCNTL_H__ */
