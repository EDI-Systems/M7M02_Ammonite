# CHB port for the RVM hypervisor.
# Copyright (C) 2026 Evo-Devo Instrum. All rights reserved.
#
# This file is NOT part of the ChibiOS distribution.  It belongs to the
# ChibiOS-on-RVM guest port and is released under the Unlicense; refer to
# the LICENSE file for details.  The ChibiOS kernel itself is copyright
# Giovanni Di Sirio and is not modified by this file.

# List of the ChibiOS/RT RV32P RVM guest port files.
PORTSRC = $(CHIBIOS)/os/common/ports/RV32P-RVM/chcore.c

PORTASM = $(CHIBIOS)/os/common/ports/RV32P-RVM/compilers/GCC/chcoreasm.s

PORTINC = $(CHIBIOS)/os/common/portability/GCC \
          $(CHIBIOS)/os/common/ports/RV32P-RVM

ALLXASMSRC += $(PORTASM)
ALLCSRC    += $(PORTSRC)
ALLINC     += $(PORTINC)
