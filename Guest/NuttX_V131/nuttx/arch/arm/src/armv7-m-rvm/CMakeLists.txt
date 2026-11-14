# ##############################################################################
# arch/arm/src/armv7-m-rvm/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
# ##############################################################################

set(SRCS
    armv7m_rvm_start.c
    armv7m_rvm_initialstate.c
    armv7m_rvm_context.c
    armv7m_rvm_irq.c
    armv7m_rvm_timer.c
    armv7m_rvm_idle.c
    armv7m_rvm_lowputc.c
    armv7m_rvm_fpu.c
    armv7m_rvm_irq_stubs.c
    armv7m_rvm_saveusercontext.S
    armv7m_rvm_switch.S)

target_sources(arch PRIVATE ${SRCS})

# The RVM user-level runtime and platform glue are part of the guest image.
# NTX_RVM_ROOT is supplied by the generated board or by the build wrapper.
if(NOT DEFINED NTX_RVM_ROOT)
  if(DEFINED CONFIG_NTX_RVM_ROOT)
    set(NTX_RVM_ROOT "${CONFIG_NTX_RVM_ROOT}")
  else()
    set(NTX_RVM_ROOT "${NUTTX_DIR}/../../M7M02_Ammonite")
  endif()
endif()

set(NTX_RVM_SRCS
    ${NTX_RVM_ROOT}/Source/Platform/A7M/rvm_platform_a7m.c
    ${NTX_RVM_ROOT}/Source/Platform/A7M/rvm_platform_a7m_gcc.s
    ${NTX_RVM_ROOT}/Source/Syslib/rvm_syslib.c
    ${NTX_RVM_ROOT}/Source/Virtlib/rvm_virtlib.c)

target_sources(arch PRIVATE ${NTX_RVM_SRCS})
target_include_directories(arch PRIVATE ${NTX_RVM_ROOT}/Include
                                        ${NUTTX_BOARD_DIR}/include)
