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
/// This file implements common number theory routines used throughout libfhe
///
//===----------------------------------------------------------------------===//

#include "utils/number_theory.h"
#include "rand/sample.h"

#define MR_RUNS (1 << 8)

#define BARRET_LOW ((uint_t)0)
#define BARRET_HI ((uint_t)1)

#ifdef _MSC_VER
#include <intrin.h>
#endif

int is_prime(uint_t p) {
  uint_t x, r, s, t;

  if (p < 2 || (p != 2 && p % 2 == 0))
    return 0;

  s = p - 1;

#ifdef _WIN64
  x = _tzcnt_u64(s);
#else
  x = __builtin_ctzll(s);
#endif

  s >>= x;

  for (int i = 0; i < MR_RUNS; ++i) {
    r = uniform64() % (p - 1) + 1;
    t = modexp(r, s, p);

    uint_t j = 0;
    while (j++ < x && t != 1 && t != p - 1)
      t = modmul(t, t, p);

    if (t != p - 1 && j > 1)
      return 0;
  }
  return 1;
}

/* Generates a sequence of l-bit primes of the form
 * p = k * 2^m + 1
 */
void gen_primes(uint_t l, uint_t m, uint_t *p, unsigned n) {
  static int k = 0;

  m = (1ULL << m);
  l = (1ULL << l);

  for (unsigned i = 0; i < n; ++i)
    do {
      p[i] = l + k++ * m + 1;
    } while (!is_prime(p[i]));
}

/* Find a primitive 2^m-th root of unity */
uint_t find_proot(uint_t p, uint_t lgn) {
  uint_t r;
  do {
    r = (uniform64() % (p - 1)) + 2;
  } while (modexp(r, (p - 1) >> 1, p) == 1);

  return modexp(r, (p - 1) >> lgn, p);
}

uint_t modexp(uint_t x, uint_t y, uint_t p) {
  uint_t r = 1, z = x;
  for (uint_t i = y; i; i >>= 1) {
    if (i & 1)
      r = modmul(r, z, p);
    z = modmul(z, z, p);
  }
  return r;
}
