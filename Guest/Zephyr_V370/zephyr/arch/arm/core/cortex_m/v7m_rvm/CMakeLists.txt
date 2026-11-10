# SPDX-License-Identifier: Unlicense

zephyr_library()
zephyr_library_sources(
  prep_c.c
  thread.c
  swap.c
  cpu_idle.c
)

# Put the RVM header overlay in front of the generic Cortex-M headers.
target_include_directories(zephyr_interface BEFORE INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
