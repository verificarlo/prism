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

void interflop_prism_set_default_virtual_precision_binary32(int32_t t);
void interflop_prism_set_default_virtual_precision_binary64(int32_t t);

int32_t interflop_prism_get_default_virtual_precision_binary32(void);
int32_t interflop_prism_get_default_virtual_precision_binary64(void);

uint64_t interflop_prism_get_seed(void);
void interflop_prism_set_seed(uint64_t seed);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __PRISM_API_H__
