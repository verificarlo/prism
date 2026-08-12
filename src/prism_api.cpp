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

#include "prism_api.h"
#include "utils.h"
#include "xoshiro.h"

extern "C" {

void interflop_prism_set_default_virtual_precision_binary32(int32_t t) {
  prism::sr::set_default_virtual_precision<float>(t);
}

void interflop_prism_set_default_virtual_precision_binary64(int32_t t) {
  prism::sr::set_default_virtual_precision<double>(t);
}

int32_t interflop_prism_get_default_virtual_precision_binary32(void) {
  return prism::sr::default_virtual_precision_f32.load(
      std::memory_order_relaxed);
}

int32_t interflop_prism_get_default_virtual_precision_binary64(void) {
  return prism::sr::default_virtual_precision_f64.load(
      std::memory_order_relaxed);
}

int32_t interflop_prism_get_virtual_precision_binary32(void) {
  return prism::sr::get_virtual_precision<float>();
}

int32_t interflop_prism_get_virtual_precision_binary64(void) {
  return prism::sr::get_virtual_precision<double>();
}

void interflop_prism_set_thread_virtual_precision_binary32(int32_t t) {
  prism::sr::set_virtual_precision<float>(t);
}

void interflop_prism_set_thread_virtual_precision_binary64(int32_t t) {
  prism::sr::set_virtual_precision<double>(t);
}

uint64_t interflop_prism_get_seed(void) { return get_user_seed(); }

void interflop_prism_set_seed(uint64_t seed) { set_user_seed(seed); }

void interflop_prism_set_rounding_mode(int32_t mode) {
  prism::sr::set_default_rounding_mode(mode);
}

int32_t interflop_prism_get_rounding_mode(void) {
  return prism::sr::get_rounding_mode();
}

void interflop_prism_set_thread_rounding_mode(int32_t mode) {
  prism::sr::set_rounding_mode(mode);
}

} // extern "C"
