/*
 * DjyOS V1.1.0 RVM port - the errno shim.
 *
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The DjyOS kernel itself is copyright the
 * DjyOS Open Source Development team and is not modified by this file.
 *
 * djyos.h includes <errno.h> and expects it to provide the kernel's
 * error bases; the object handle code assigns errno/EBADF directly,
 * so the full POSIX constant set and a writable errno storage are
 * provided here (the RVM guest never returns through newlib's TLS
 * errno, a plain global is enough).
 */
#ifndef __DJY_RVM_ERRNO_H__
#define __DJY_RVM_ERRNO_H__

#define CN_KNL_NO_ERROR                 (0)
#define CN_LOCK_NO_ERROR                (0)
#define CN_MEM_NO_ERROR                 (0)

#define EPERM                           1
#define ENOENT                          2
#define EBADF                           9
#define EAGAIN                          11
#define ENOMEM                          12
#define EACCES                          13
#define EFAULT                          14
#define EBUSY                           16
#define EEXIST                          17
#define EINVAL                          22
#define ENOSPC                          28
#define EPIPE                           32
#define ERANGE                          34

extern int errno;

#endif /* __DJY_RVM_ERRNO_H__ */
