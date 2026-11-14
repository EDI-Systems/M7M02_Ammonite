/*
    ChibiOS - Copyright (C) 2006-2026 Giovanni Di Sirio.
    CHB modifications Copyright (C) 2026 Evo-Devo Instrum.

    This file is part of ChibiOS.

    ChibiOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 3 of the License.

    ChibiOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    ARMv7-M-RVM/cmparams.h
 * @brief   Cortex-M parameters for the RVM guest port.
 *
 * @addtogroup ARMV7M_RVM_CORE
 * @{
 */

#ifndef CMPARAMS_H
#define CMPARAMS_H

/**
 * @brief   Cortex core model.
 */
#if defined(CHB_CPU_M4) && CHB_CPU_M4
#define CORTEX_MODEL            4
#else
#define CORTEX_MODEL            7
#endif

/**
 * @brief   Floating Point unit presence.
 * @note    This describes the Cortex-M core.  The guest port uses basic
 *          frames when the VM has no coprocessor, and extended frames when
 *          the VM is granted an FPU by the project configuration.
 */
#define CORTEX_HAS_FPU          1

/**
 * @brief   Number of bits in priority masks.
 */
#define CORTEX_PRIORITY_BITS    4U

/**
 * @brief   Number of interrupt vectors.
 * @note    Informational only; the RVM monitor owns the physical vectors.
 */
#define CORTEX_NUM_VECTORS      168

/**
 * @brief   Number of MPU regions.
 * @note    The RVM monitor owns the MPU; the guest cannot change it.
 */
#define CORTEX_MPU_REGIONS      16

#endif /* CMPARAMS_H */

/** @} */
