//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Linked list module (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: the doubly linked list primitives the kernel's queues are built on.
//Notes: Self-contained stand-in for the DjyOS V1.1.0 header of the same
//       name; the declarations and values follow the upstream header so
//       that the unmodified kernel sources see the same surface.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#ifndef __LIST_H__
#define __LIST_H__
#ifdef __cplusplus
extern "C" {
#endif

struct dListNode
{
    struct dListNode *next;
    struct dListNode *prev;
};
typedef struct dListNode list_t;

struct sListNode
{
    struct sListNode *next;
};
typedef struct sListNode slist_t;

#define dListEntry(node, type, member) \
    ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))

#define dListForEach(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define sListForEach(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)


void dListInit(list_t *l);
void dListInsertAfter(list_t *l, list_t *n);
void dListInsertBefore(list_t *l, list_t *n);
list_t *dListGetBefore(list_t *loc);
list_t *dListGetAfter(list_t *loc);
void dListRemove(list_t *n);
s32 dListIsEmpty(const list_t *l);

void sListInit(slist_t *l);
void sListInsertAfter(slist_t *l, slist_t *n);
s32 sListIsEmpty(const slist_t *l);

#ifdef __cplusplus
}
#endif

#endif //__LIST_H__


