# ##############################################################################
# boards/arm/armv7-m-rvm/ntx/src/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
# ##############################################################################

set(SRCS ntx_boot.c ntx_desc.c)
target_sources(board PRIVATE ${SRCS})

# The memory window comes from the Virt's .rvp project through Kconfig.  The
# in-tree board generates its linker script so the same board serves any
# assigned code/data/stack layout.

configure_file("${NUTTX_BOARD_DIR}/scripts/ntx.ld.in"
               "${CMAKE_BINARY_DIR}/ntx.ld" @ONLY)
set_property(GLOBAL PROPERTY LD_SCRIPT "${CMAKE_BINARY_DIR}/ntx.ld")
