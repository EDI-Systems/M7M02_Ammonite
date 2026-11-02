/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                               Copyright (C) 2026 Evo-Devo Instrum.
*                                       All rights reserved.
*
*                This file is NOT part of the uC/OS-II distribution.  It belongs
*                to the uC/OS-II-on-RVM test suite and is released under the
*                Unlicense; refer to the LICENSE file for details.  The original
*                uC/OS-II kernel is copyright Weston Embedded and is not
*                modified by this file.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                  ARMv6-M Port - Template/app_cfg.h
*
* File      : app_cfg.h
* Version   : V1.00.00
*********************************************************************************************************
*/

#ifndef  APP_CFG_RVM_H
#define  APP_CFG_RVM_H

/* This file is intentionally empty: uC/OS-II does not require any
 * application-level configuration that the port cannot provide itself.
 * The official ARMv7-M ports require CPU_CFG_KA_IPL_BOUNDARY and
 * CPU_CFG_NVIC_PRIO_BITS here for the BASEPRI masking; under RVM the
 * virtual interrupt mask replaces the BASEPRI and no such constants
 * are needed. */

#endif
