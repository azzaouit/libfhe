// libfhe
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of common constant time routines
/// used throughout libfhe
///
//===----------------------------------------------------------------------===//

#ifndef UTILS_CONST_TIME_H
#define UTILS_CONST_TIME_H

#include <stdint.h>

#define U32_TO_BYTES(X, B)                                                     \
  do {                                                                         \
    B[0] = (uint8_t)(X >> 0);                                                  \
    B[1] = (uint8_t)(X >> 8);                                                  \
    B[2] = (uint8_t)(X >> 16);                                                 \
    B[3] = (uint8_t)(X >> 24);                                                 \
  } while (0)

#define U32_FROM_BYTES(X, B)                                                   \
  do {                                                                         \
    X = (uint32_t)B[0] << 0;                                                   \
    X |= (uint32_t)B[1] << 8;                                                  \
    X |= (uint32_t)B[2] << 16;                                                 \
    X |= (uint32_t)B[3] << 24;                                                 \
  } while (0)

static inline uint32_t const_time_reverse32(uint32_t x) {
  x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
  x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
  x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
  x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
  return ((x >> 16) | (x << 16));
}

static inline uint64_t barrier64(uint64_t a) {
#if defined(__GNUC__)
  uint64_t r;
  __asm__("" : "=r"(r) : "0"(a));
#else
  volatile uint64_t r = a;
#endif
  return r;
}

static inline uint64_t const_time_select64(uint64_t mask, uint64_t a,
                                           uint64_t b) {
  return (b ^ ((barrier64(~(mask)) + 1) & (a ^ b)));
}

#endif /* UTILS_CONST_TIME_H */
