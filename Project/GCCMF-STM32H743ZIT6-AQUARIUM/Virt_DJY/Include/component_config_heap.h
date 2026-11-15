//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Heap configuration (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: the allocator flavour the kernel links in.
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

//Heap component: the allocator flavour the kernel links in.
#if(CFG_MODULE_ENABLE_HEAP == false)

#define CFG_MODULE_ENABLE_HEAP      false //set to true by project_config.h when enabled
#define CFG_DYNAMIC_MEM             true  //full dynamic allocation support

#endif
