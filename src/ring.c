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
/// This file implements the cyclotomic polynomial ring
/// \f$R = Z_M[X] / <x^d + 1>\f$ for a generic modulus \f$M\f$.
///
//===----------------------------------------------------------------------===//

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "utils/const_time.h"
#include "utils/number_theory.h"

#include "fhe_ring.h"

#define M32 (1ULL << 32)

int ring_init(ring_t *r, size_t lgd, size_t lgq, size_t lgm) {
  r->lgd = lgd;
  r->d = (1UL << lgd);
  r->n = (lgq / lgm) + 1;

  r->m = calloc(1, sizeof(int_t) * r->n);
  if (!r->m)
    goto FREE_M;

  r->ms = calloc(1, sizeof(mpz_t) * r->n);
  if (!r->ms)
    goto FREE_MS;

  r->invms = calloc(1, sizeof(int_t) * r->n);
  if (!r->invms)
    goto FREE_INVMS;

  r->minv = calloc(1, sizeof(int_t) * r->n);
  if (!r->minv)
    goto FREE_MINV;

  r->dinv = calloc(1, sizeof(int_t) * r->n);
  if (!r->dinv)
    goto FREE_DINV;

  r->roots = calloc(1, sizeof(int_t) * (r->n << lgd));
  if (!r->roots)
    goto FREE_ROOTS;

  r->iroots = calloc(1, sizeof(int_t) * (r->n << lgd));
  if (!r->iroots)
    goto FREE_IROOTS;

  gen_primes(lgm, lgd + 1, r->m, r->n);

  OMP_FOR
  for (size_t i = 0; i < r->n; ++i) {
    uint_t root = find_proot(r->m[i], lgd + 1);
    uint_t iroot = modinv(root, r->m[i]);
    assert(modexp(root, r->d << 1, r->m[i]) == 1);
    assert(modexp(iroot, r->d << 1, r->m[i]) == 1);

    r->minv[i] = inv(r->m[i]);
    r->dinv[i] = modinv(r->d, r->m[i]);
    r->dinv[i] = modmul(r->dinv[i], M32, r->m[i]);
    r->dinv[i] = modmul(r->dinv[i], M32, r->m[i]);

    for (size_t j = 0, power = 1, ipower = 1; j < r->d; ++j) {
      int index = i * r->d + (const_time_reverse32(j) >> (32 - lgd));
      r->roots[index] = modmul(power, M32, r->m[i]);
      r->roots[index] = modmul(r->roots[index], M32, r->m[i]);
      r->iroots[index] = modmul(ipower, M32, r->m[i]);
      r->iroots[index] = modmul(r->iroots[index], M32, r->m[i]);
      power = modmul(power, root, r->m[i]);
      ipower = modmul(ipower, iroot, r->m[i]);
    }
  }

  mpz_init_set_ui(r->M, r->m[0]);
  for (size_t i = 1; i < r->n; ++i)
    mpz_mul_ui(r->M, r->M, r->m[i]);

  mpz_init_set(r->M_half, r->M);
  mpz_cdiv_q_ui(r->M_half, r->M_half, 2);

  for (size_t i = 0; i < r->n; ++i) {
    mpz_init(r->ms[i]);
    mpz_divexact_ui(r->ms[i], r->M, r->m[i]);
    r->invms[i] = mpz_fdiv_ui(r->ms[i], r->m[i]);
    r->invms[i] = modinv(r->invms[i], r->m[i]);
  }

  return 0;

FREE_IROOTS:
  free(r->roots);
FREE_ROOTS:
  free(r->dinv);
FREE_DINV:
  free(r->minv);
FREE_MINV:
  free(r->invms);
FREE_INVMS:
  free(r->ms);
FREE_MS:
  free(r->m);
FREE_M:
  return -errno;
}

void ring_free(ring_t *r) {
  mpz_clear(r->M);
  mpz_clear(r->M_half);
  for (size_t i = 0; i < r->n; ++i)
    mpz_clear(r->ms[i]);
  free(r->iroots);
  free(r->roots);
  free(r->dinv);
  free(r->minv);
  free(r->invms);
  free(r->ms);
  free(r->m);
}
