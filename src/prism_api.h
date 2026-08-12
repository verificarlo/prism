/*****************************************************************************\
 *                                                                           *\
 *  This file is part of the Verificarlo project,                            *\
 *  under the Apache License v2.0 with LLVM Exceptions.                      *\
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.                 *\
 *  See https://llvm.org/LICENSE.txt for license information.                *\
 *                                                                           *\
 *  Copyright (c) 2026                                                       *\
 *     Verificarlo Contributors                                              *\
 *                                                                           *\
 ****************************************************************************/

#ifndef __PRISM_API_H__
#define __PRISM_API_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Process-wide setters: the new value reaches every thread, including threads
 * that have already executed instrumented arithmetic, on their next operation.
 * A process-wide set discards any outstanding per-thread override. */
void interflop_prism_set_default_virtual_precision_binary32(int32_t t);
void interflop_prism_set_default_virtual_precision_binary64(int32_t t);

int32_t interflop_prism_get_default_virtual_precision_binary32(void);
int32_t interflop_prism_get_default_virtual_precision_binary64(void);

/* Precision the calling thread will actually round at. Use this, not the
 * default getter, to check that a setting took effect. */
int32_t interflop_prism_get_virtual_precision_binary32(void);
int32_t interflop_prism_get_virtual_precision_binary64(void);

/* Per-thread override, in effect until the next process-wide set. */
void interflop_prism_set_thread_virtual_precision_binary32(int32_t t);
void interflop_prism_set_thread_virtual_precision_binary64(int32_t t);

uint64_t interflop_prism_get_seed(void);
void interflop_prism_set_seed(uint64_t seed);

/* Rounding modes */
#define INTERFLOP_PRISM_SR 0
#define INTERFLOP_PRISM_RN 1

/* Process-wide, like the precision setter above. */
void interflop_prism_set_rounding_mode(int32_t mode);
/* Mode the calling thread will actually round with. */
int32_t interflop_prism_get_rounding_mode(void);
/* Per-thread override, in effect until the next process-wide set. */
void interflop_prism_set_thread_rounding_mode(int32_t mode);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __PRISM_API_H__
