/* SPDX-License-Identifier: Unlicense */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMV7MRVM
 *
 * @brief BSP tm27 header.
 *
 * The RVM guest has no testable hardware timer.  Tests that need tm27 are
 * excluded in tst_v7m_rvm.yml; this header exists so that the test suite
 * headers compile.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <rtems/tm27-default.h>

#endif /* __tm27_h */
