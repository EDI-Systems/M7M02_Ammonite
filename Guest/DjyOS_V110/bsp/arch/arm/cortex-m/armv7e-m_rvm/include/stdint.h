/*
 * DjyOS V1.1.0 RVM port - the machine's stdint wrapper.
 *
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
 * This file is NOT part of the DjyOS distribution.  It belongs to the
 * DjyOS-on-RVM port and is released under the Unlicense; refer to the
 * LICENSE file for details.  The DjyOS kernel itself is copyright the
 * DjyOS Open Source Development team and is not modified by this file.
 *
 * The DjyOS headers include <stdint.h> expecting it to chain-load the
 * architecture fast types, exactly as the arch_stdint.h comment says:
 * "User code must not include this file directly; go through the C
 * library's stdint.h instead".
 * This wrapper is self-contained: the full arm32_stdint.h content is
 * inlined (a cross-directory #include of it proved fragile - a fatal
 * resolution error anywhere in the chain silently truncated the
 * includer and left later macros undefined), the newlib secondary
 * typedef guards are pre-satisfied, and the volatile register-view
 * types plus the short machine types are added on top.
 */
#ifndef __DJY_RVM_STDINT_H__
#define __DJY_RVM_STDINT_H__

/* Fixed-width types (arm32_stdint.h). */
typedef unsigned long long      uint64_t;
typedef signed long long        sint64_t;
typedef unsigned int            uint32_t;
typedef int                     sint32_t;
typedef unsigned short          uint16_t;
typedef short int               sint16_t;
typedef unsigned char           uint8_t;
typedef signed char             sint8_t;

/* The standard intNN_t spellings (newlib sys headers use them). */
typedef signed char             int8_t;
typedef short int               int16_t;
typedef int                     int32_t;
typedef signed long long        int64_t;
typedef signed long long        intptr_t;
typedef unsigned long           uintptr_t;

/* Fast and pointer-width types. */
typedef uint32_t                ucpu_t;
typedef sint32_t                scpu_t;
typedef uint32_t                ptu32_t;
typedef ucpu_t                  ufast_t;
typedef scpu_t                  sfast_t;
typedef ufast_t                 bool_t;

/* Machine config. */
#define CN_CPU_BITS             32U
#define CN_PTR_BITS             32U
#define CN_CPU_BITS_SUFFIX_ZERO 5U
#define CN_BYTE_BITS            8U

/* Short types. */
typedef uint8_t                 u8;
typedef sint8_t                 s8;
typedef uint16_t                u16;
typedef sint16_t                s16;
typedef uint32_t                u32;
typedef sint32_t                s32;
typedef uint64_t                u64;
typedef sint64_t                s64;

/* Boolean constants. */
#define true                    1
#define false                   0
#define TRUE                    1
#define FALSE                   0

/* Limit macros. */
#define CN_LIMIT_UINT64         (0xFFFFFFFFFFFFFFFFULL)
#define CN_MIN_UINT64           (0ULL)
#define CN_LIMIT_UINT32         (0xFFFFFFFFUL)
#define CN_MIN_UINT32           (0UL)
#define CN_LIMIT_UINT24         (0xFFFFFFUL)
#define CN_LIMIT_UINT16         (0xFFFFUL)
#define CN_MIN_UINT16           (0U)
#define CN_LIMIT_UINT8          (0xFFU)
#define CN_MIN_UINT8            (0U)
#define CN_LIMIT_SINT32         (0x7FFFFFFFL)
#define CN_LIMIT_SINT16         (0x7FFFL)
#define CN_LIMIT_SINT8          (0x7FL)
#define CN_LIMIT_PTU32          CN_LIMIT_UINT32
#define CN_MIN_PTU32            CN_MIN_UINT32
#define CN_LIMIT_ALIGN          CN_LIMIT_UINT64
#define CN_MIN_ALIGN            0
#define CN_LIMIT_UCPU           CN_LIMIT_UINT32
#define CN_MIN_UCPU             CN_MIN_UINT32
#define CN_LIMIT_UFAST          CN_LIMIT_UINT32
#define CN_LIMIT_SINT32_ALT     CN_LIMIT_SINT32

/* The volatile register-view types used by the CM header blocks. */
typedef volatile uint8_t        vu8;
typedef volatile uint16_t       vu16;
typedef volatile uint32_t       vu32;
typedef volatile uint8_t        vuc8;
typedef volatile uint32_t       vuc32;

/* The alignment type (arm32_stdint.h): u64 satisfies every alignment
 * requirement on this machine. */
typedef uint64_t                align_type;

/* Pre-satisfy the newlib secondary typedef guards so that a later
 * <sys/types.h> (pulled in by <fcntl.h> etc.) does not re-typedef the
 * fixed width types with a different base type. */
#define ___int8_t_defined 1
#define ___int16_t_defined 1
#define ___int32_t_defined 1
#define ___int64_t_defined 1
#define _INT8_T_DECLARED
#define _UINT8_T_DECLARED
#define _INT16_T_DECLARED
#define _UINT16_T_DECLARED
#define _INT32_T_DECLARED
#define _UINT32_T_DECLARED
#define _INT64_T_DECLARED
#define _UINT64_T_DECLARED
#define _SYS__STDINT_H

/* The DjyOS heap function pointer table.  The original DIDE toolchain
 * distributed these declarations through its private types.h; without
 * them, C89 implicit declaration turns every call into a direct "bl"
 * to the BSS address of the pointer VARIABLE (not the function it
 * points to) - the classic UNDEFINSTR boot crash of this port.
 * They live here because every kernel TU includes this wrapper. */
struct HeapCB;
extern void *(*M_Malloc)(unsigned int size, unsigned int timeout);
extern void *(*M_Realloc)(void *, unsigned int NewSize);
extern unsigned int (*M_Free)(void *pl_mem, unsigned int timeout);
extern void *(*M_MallocHeap)(unsigned int size, struct HeapCB *Heap, unsigned int timeout);
extern unsigned int (*M_FormatSize)(unsigned int size);
extern unsigned int (*M_GetFreeMem)(void);
extern unsigned int (*M_GetMaxFreeBlock)(void);
extern unsigned int (*M_CheckSize)(void *mp);

#endif /* __DJY_RVM_STDINT_H__ */
