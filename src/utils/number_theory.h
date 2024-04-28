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
/// This file contains the declaration of common number theory routines
/// used throughout libfhe
///
//===----------------------------------------------------------------------===//

#ifndef UTILS_NUMBER_THEORY_H
#define UTILS_NUMBER_THEORY_H

#include "fhe_config.h"

int is_prime(uint_t);
uint_t find_proot(uint_t, uint_t);
uint_t modexp(uint_t, uint_t, uint_t);
void gen_primes(uint_t, uint_t, uint_t *, unsigned);

static inline uint_t modmul(uint_t a, uint_t b, uint_t m) {
  uint_dt mul = (uint_dt)(a % m) * (b % m);
  return mul % (uint_dt)(m);
}

static inline uint_t modadd(uint_t a, uint_t b, uint_t m) {
  return (a + b) % m;
}

static inline uint_t modsub(uint_t a, uint_t b, uint_t m) {
  return (a + m - (b % m)) % m;
}

static inline uint_t modinv(uint_t a, uint_t m) { return modexp(a, m - 2, m); }

static inline uint_t mul64(uint_t a, uint_t b, uint_t *hi) {
  uint_dt res = (uint_dt)a * b;
  *hi = res >> 64;
  return res;
}

static inline uint_t inv(uint_t a) {
  uint_t r = 1;
  for (uint_t m = 2; m; m <<= 1) {
    r |= (a * r) & m;
  }
  return r;
}

#endif /* UTILS_NUMBER_THEORY_H */
