//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Multiplex module (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: the multiplex (polling) object surface the kernel exposes.
//Notes: Self-contained stand-in for the DjyOS V1.1.0 header of the same
//       name; the declarations and values follow the upstream header so
//       that the unmodified kernel sources see the same surface.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#ifndef __MULTIPLEX_H__
#define __MULTIPLEX_H__
#include "lock.h"
#ifdef __cplusplus
extern "C" {
#endif

#define CN_MULTIPLEX_MAX_SETS      10
#define CN_MULTIPLEX_MAX_OBJECTS   1000

#define CN_MULTIPLEX_SUCCESS   0   //call succeeded
#define CN_MULTIPLEX_INVALID   1   //the object does not support multiplex
#define CN_MULTIPLEX_ERROR     2   //the object supports multiplex but refused this call


#define CN_MULTIPLEX_MODEMSK                0xff000000  //mode bit mask
#define CN_MULTIPLEX_STATUSMSK              0x00ffffff  //status bit mask

//SensingBit layout (24 bits) of struct MultiplexObjectCB;
//bits 16..23 are user defined, e.g. the UART CN_UART_FIFO_OVER_ERR
#define CN_MULTIPLEX_SENSINGBIT_READ        (1<<0)  //object is readable
#define CN_MULTIPLEX_SENSINGBIT_WRITE       (1<<1)  //object is writable
#define CN_MULTIPLEX_SENSINGBIT_ERROR       (1<<2)  //object reported an error
#define CN_MULTIPLEX_SENSINGBIT_USERBASE    (1<<16)  //bits 16..23 are user defined

//trigger mode, carried in SensingBit.24..31
//#define CN_MULTIPLEX_SENSINGBIT_ONESHOT     (1<<28) //1=listen once, 0=listen repeatedly
#define CN_MULTIPLEX_SENSINGBIT_OR          (1<<29) //1=trigger on any sensing bit, 0=on all
#define CN_MULTIPLEX_SENSINGBIT_ET          (1<<30) //1=edge triggered, 0=level triggered

//whether the multiplex object is active, carried in PendingBit.31
#define CN_MULTIPLEX_OBJECT_ACTIVED         (1<<31) //1=object is actived

struct MultiplexSetsCB;
struct MultiplexObjectCB;

struct MultiplexSetsCB *Multiplex_Create(u32 ActiveLevel);
bool_t Multiplex_AddObject(struct MultiplexSetsCB *Sets,s32 Fd, u32 SensingBit,ptu32_t MpObjTag);
bool_t Multiplex_DelObject(struct MultiplexSetsCB  *Sets,s32 Fd);
s32 Multiplex_Wait(struct MultiplexSetsCB *Sets, u32 *Status, ptu32_t *MpObjTag,u32 Timeout);

#ifdef __cplusplus
}
#endif

#endif //__MULTIPLEX_H__


