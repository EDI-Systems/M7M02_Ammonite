//----------------------------------------------------
// Copyright (c) 2026, Evo-Devo Instrum. All rights reserved.
//
// This file is NOT part of the DjyOS distribution.  It belongs to the
// DjyOS-on-RVM port and is released under the Unlicense; refer to the
// LICENSE file for details.  The DjyOS kernel itself is copyright the
// DjyOS Open Source Development team and is not modified by this file.
//----------------------------------------------------
//Module: Board configuration (RVM guest)
//Author:  Evo-Devo Instrum.
//Version: V1.0.0
//Description: the NUCLEO-H743ZI clock tree, tick parameters and endianness the
//             kernel needs, sized for the RVM guest.
//Notes: Self-contained stand-in for the DjyOS V1.1.0 header of the same
//       name; the declarations and values follow the upstream header so
//       that the unmodified kernel sources see the same surface.
//Revision history:
//1. Date: 2026-10-29
//   Author:  Evo-Devo Instrum.
//   Version: V1.0.0
//   Change: Initial version for the RVM guest.
//----------------------------------------------------

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DJY_BOARD    "NUCLEO-H743ZI"

#define Mhz 1000000
#define CN_CFG_MCLK (400*Mhz)  //core clock, mandatory for the kernel
#define CN_CFG_FCLK CN_CFG_MCLK  //CM7 free-running peripheral clock
#define BOARD_MCK   (CN_CFG_MCLK/2) //MCK

#define CN_CFG_PCLK1  (CN_CFG_MCLK/4)
#define CN_CFG_PCLK2  (CN_CFG_MCLK/2)

#define CN_CFG_SYS_CLK     CN_CFG_MCLK
#define CN_CFG_AXI_CLK    (CN_CFG_MCLK/2)
#define CN_CFG_HCLK3      (CN_CFG_AXI_CLK)

#define CN_CFG_APB1_CLK   (CN_CFG_AXI_CLK/2)
#define CN_CFG_APB2_CLK   (CN_CFG_AXI_CLK/2)
#define CN_CFG_APB3_CLK   (CN_CFG_AXI_CLK/2)
#define CN_CFG_APB4_CLK   (CN_CFG_AXI_CLK/2)



#define CN_CFG_LSE (32768)
#define CN_CFG_HSE (8*Mhz)


/*____tick parameters below____*/
#define CN_CFG_PARAS_LIMIT 40   //event parameter records
#define CN_CFG_TICK_US 1000  //tick interval in us
#define CN_CFG_TICK_HZ 1000  //kernel tick frequency in Hz
#define CN_USE_TICKLESS_MODE    (0U)
#if (!CN_USE_TICKLESS_MODE)
#define CN_CFG_FINE_US 0x00000A3  //1/400M, tick source period in us as a 32-bit fixed point
                                  //number (16.16), which limits a tick span to 65535us
#define CN_CFG_FINE_HZ CN_CFG_MCLK  //tick source frequency, the reciprocal of CN_CFG_FINE_US
#else
#define CN_CFG_USE_USERTIMER        (0U)//1 = use the LPTIMER as the tick source
#define CN_CFG_TIME_BASE_HZ         CN_CFG_MCLK//(32000U)//(8000U)
#if (!CN_CFG_USE_USERTIMER)
#define CN_CFG_USE_BYPASSTIMER      (0U)//0 = no bypass timer, 1 = use the bypass timer
#define CN_CFG_TIME_PRECISION       (500U)/*accuracy unit: us*/
#else
#define CN_CFG_USERTIMER_PRESC      (1U)//user timer prescaler, used when SysTick is not the source
#define CN_CFG_FINE_US (0x1F4000U)  //1/32000, tick source period in us as a 32-bit fixed point
                                    //number (16.16), which limits a tick span to 65535us
#define CN_CFG_FINE_HZ (0x831U)  //tick source frequency, the reciprocal of CN_CFG_FINE_US
#define CN_CFG_TIME_PRECISION       (200U)/*accuracy unit: us*/
#endif
#endif

//CPU architecture options: only the optional (or possibly optional) items
//live here, e.g. the endianness.  The word length is fixed and belongs
//to the BSP's arch.h.  Memory endianness below:
#define CN_CFG_LITTLE_ENDIAN        0
#define CN_CFG_BIG_ENDIAN           1
#define CN_CFG_BYTE_ORDER          CN_CFG_LITTLE_ENDIAN



#ifdef __cplusplus
}
#endif
#endif // __CPU_OPTIONAL_H__



