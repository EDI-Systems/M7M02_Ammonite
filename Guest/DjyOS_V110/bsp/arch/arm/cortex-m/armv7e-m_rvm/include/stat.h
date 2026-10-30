/*
 * DjyOS V1.1.0 RVM port - the stat shim.
 *
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The DjyOS kernel itself is copyright the
 * DjyOS Open Source Development team and is not modified by this file.
 *
 * objhandle.h includes <stat.h> for the object handle file modes; the
 * RVM guest has no file system, so this is the minimal surface it
 * compiles against.
 */
#ifndef __DJY_RVM_STAT_H__
#define __DJY_RVM_STAT_H__

#define S_IFMT          0170000U
#define S_IFDIR         0040000U
#define S_IFREG         0100000U

#endif /* __DJY_RVM_STAT_H__ */
