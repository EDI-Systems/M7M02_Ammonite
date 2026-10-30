//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Heap module (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: the heap API surface the kernel calls, plus the heap function
//             pointer table that the runtime support module fills in.
//Notes: Self-contained stand-in for the DjyOS V1.1.0 header of the same
//       name; the declarations and values follow the upstream header so
//       that the unmodified kernel sources see the same surface.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#ifndef __HEAP_H__
#define __HEAP_H__
/* The DjyOS heap function pointer table declarations.  The original
 * DIDE toolchain distributed these through its private types.h; without
 * them, C89 implicit declaration turns every call into a direct "bl"
 * to the BSS address of the pointer VARIABLE - the classic UNDEFINSTR
 * boot crash of this port. */
struct HeapCB;
typedef unsigned int djy_bool_t;
extern void *(*M_Malloc)(unsigned int size, unsigned int timeout);
extern void *(*M_Realloc)(void *, unsigned int NewSize);
extern djy_bool_t (*M_Free)(void *pl_mem, unsigned int timeout);
extern void *(*M_MallocHeap)(unsigned int size, struct HeapCB *Heap, unsigned int timeout);
extern unsigned int (*M_FormatSize)(unsigned int size);
extern unsigned int (*M_GetFreeMem)(void);
extern unsigned int (*M_GetMaxFreeBlock)(void);
extern unsigned int (*M_CheckSize)(void *mp);

#include "errno.h"
#ifdef __cplusplus
extern "C" {
#endif

#define     CN_SYS_HEAP_NAME    "sys"   //name of the system heap

#define     CN_HEAP_GENERAL     0
#define     CN_HEAP_PRIVATE     1
enum _MEM_ERROR_CODE_
{
    EN_MEM_NO_ERROR = CN_MEM_NO_ERROR,  //no error
    EN_MEM_ERROR,               //memory error
    EN_MEM_PAGE_ERROR,          //page size error
    EN_MEM_TRIED,               //out of memory
    EN_MEM_LEAK,                //memory leak
    EN_MEM_FREE_SCH             //free while scheduling is disabled
};

typedef struct HeapCB * pHeap_t;

bool_t Heap_DynamicModuleInit(void);

#ifdef __cplusplus
}
#endif

#endif //__HEAP_H__

