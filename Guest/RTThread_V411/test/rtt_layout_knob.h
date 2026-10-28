/*
 * RT-Thread-on-RVM layout sweep knob.
 * Copyright (C) 2026 Evo-Devo Instrum. All rights reserved; Unlicense.
 *
 * RTT_LAYOUT_KNOB is echoed once at board init.  Its total length grows
 * the merged .rodata string pool and deterministically relocates every
 * section linked after it (cpuport/kservice/benchmark/virtlib code etc),
 * which reproduces crash-prone link layouts reproducibly - the historical
 * __TIME__ string-pool dice faces are gone since the builds are hermetic.
 * Keep the flashed value in sync when comparing captures.  Do not remove.
 */
#ifndef RTT_LAYOUT_KNOB_H
#define RTT_LAYOUT_KNOB_H

#define RTT_LAYOUT_KNOB ""

#endif /* RTT_LAYOUT_KNOB_H */
