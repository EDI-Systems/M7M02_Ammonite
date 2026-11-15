//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Kernel configuration (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: the event/thread limits and the initialisation stack.
//Notes: Self-contained stand-in for the DjyOS V1.1.0 header of the same
//       name.  The component configuration DSL of the DIDE tool is not
//       reproduced here - only the configuration macros the kernel
//       reads, with the upstream defaults.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#include "project_config.h"

//Kernel component: the event/thread limits and the initialisation stack.
#if(CFG_MODULE_ENABLE_KERNEL == false)

#define CFG_MODULE_ENABLE_KERNEL    false //set to true by project_config.h when enabled
#define CFG_INIT_STACK_SIZE         4096  //stack used by the initialisation sequence
#define CFG_EVENT_LIMIT             15    //event control blocks
#define CFG_EVENT_TYPE_LIMIT        15    //event type control blocks
#define CFG_IDLESTACK_LIMIT         1024  //stack of the IDLE event handler
#define CFG_IDLE_MONITOR_CYCLE      30    //IDLE share monitor period in s, 0 = off
#define CFG_MAINSTACK_LIMIT         4096  //stack used by main()
#define CFG_IDLE_WDT_RESET          false //reset when the IDLE watchdog barks
#define CFG_OS_TINY                 false //trim event type names and timing statistics
#define CFG_IDLE_CHECK_STACK        false //stack overflow check inside IDLE

#endif
