/****************************************************************************
 * boards/arm/armv7-m-rvm/ntx/include/prc_virt.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Guest-side RVM parameters.  A generated per-Virt board replaces this file
 * with the values from its .rvp project; the in-tree defaults describe the
 * reference H743 Virt.
 ****************************************************************************/

#ifndef __BOARDS_ARM_ARMV7_M_RVM_NTX_PRC_VIRT_H
#define __BOARDS_ARM_ARMV7_M_RVM_NTX_PRC_VIRT_H

#include <nuttx/config.h>

#define RVM_VIRT_LIB_ENABLE             1U
#define RVM_VIRT_VCT_NUM                4U

#define RVM_PGT_RAW_ENABLE              0U

#define RVM_VIRT_STATE_BASE             CONFIG_NTX_RVM_STATE_BASE
#define RVM_VIRT_STATE_SIZE             CONFIG_NTX_RVM_STATE_SIZE
#define RVM_VIRT_REG_BASE               CONFIG_NTX_RVM_REG_BASE
#define RVM_VIRT_REG_SIZE               CONFIG_NTX_RVM_REG_SIZE

#define RVM_ASSERT_ENABLE               1U
#define RVM_DBGLOG_ENABLE               1U

#if defined(CONFIG_NTX_RVM_COP_FPV4_SP)
#  define RVM_COP_NUM                   1U
#  define RVM_A7M_COP_FPV4_SP           1U
#  define RVM_A7M_COP_FPV5_SP           0U
#  define RVM_A7M_COP_FPV5_DP           0U
#elif defined(CONFIG_NTX_RVM_COP_FPV5_SP)
#  define RVM_COP_NUM                   1U
#  define RVM_A7M_COP_FPV4_SP           0U
#  define RVM_A7M_COP_FPV5_SP           1U
#  define RVM_A7M_COP_FPV5_DP           0U
#elif defined(CONFIG_NTX_RVM_COP_FPV5_DP)
#  define RVM_COP_NUM                   1U
#  define RVM_A7M_COP_FPV4_SP           0U
#  define RVM_A7M_COP_FPV5_SP           0U
#  define RVM_A7M_COP_FPV5_DP           1U
#else
#  define RVM_COP_NUM                   0U
#endif

#endif /* __BOARDS_ARM_ARMV7_M_RVM_NTX_PRC_VIRT_H */
