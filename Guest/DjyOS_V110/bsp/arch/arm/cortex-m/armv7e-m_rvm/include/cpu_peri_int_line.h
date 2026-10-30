/*
 * DjyOS V1.1.0 RVM port - the interrupt line number definitions.
 *
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The DjyOS kernel itself is copyright the
 * DjyOS Open Source Development team and is not modified by this file.
 *
 * The original file lives in the per-chip cpudrv; the RVM guest takes
 * its vectors from the hypervisor, so the only line number that
 * matters is the benchmark interrupt source, mapped through the chip
 * test header.  The CN_INT_LINE_LAST bound sizes the engine tables.
 */
#ifndef __CPU_PERI_INT_LINE_H__
#define __CPU_PERI_INT_LINE_H__

#define CN_INT_LINE_LAST                (95U)

#endif /* __CPU_PERI_INT_LINE_H__ */
