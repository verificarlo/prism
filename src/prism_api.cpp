/*****************************************************************************\
 *                                                                           *\
 *  This file is part of the Verificarlo project,                            *\
 *  under the Apache License v2.0 with LLVM Exceptions.                      *\
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.                 *\
 *  See https://llvm.org/LICENSE.txt for license information.                *\
 *                                                                           *\
 *  Copyright (c) 2019-2026                                                  *\
 *     Verificarlo Contributors                                              *\
 *                                                                           *\
 ****************************************************************************/

#include "utils.h"

extern "C" {

void interflop_prism_set_default_virtual_precision_binary32(int32_t t) {
  prism::sr::default_virtual_precision_f32 = t;
}

void interflop_prism_set_default_virtual_precision_binary64(int32_t t) {
  prism::sr::default_virtual_precision_f64 = t;
}

int32_t interflop_prism_get_default_virtual_precision_binary32(void) {
  return prism::sr::default_virtual_precision_f32;
}

int32_t interflop_prism_get_default_virtual_precision_binary64(void) {
  return prism::sr::default_virtual_precision_f64;
}

} // extern "C"
