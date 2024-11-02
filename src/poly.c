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
/// This file implements arithmetic over the cyclotomic polynomial ring
/// \f$R = Z_M[X] / <x^d + 1>\f$ for a generic modulus \f$M\f$.
///
//===----------------------------------------------------------------------===//

#include <errno.h>

#include "fhe_config.h"
#include "fhe_poly.h"

#include "rand/sample.h"
#include "utils/number_theory.h"

#define POLY_BINOP(C, A, B, BINOP)                                             \
  do {                                                                         \
    ring_t *r = (C)->r;                                                        \
    OMP_FOR                                                                    \
    for (size_t i = 0; i < r->n; ++i) {                                        \
      for (size_t j = 0; j < r->d; ++j) {                                      \
        size_t index = (i << r->lgd) + j;                                      \
        (C)->b[index] = BINOP((A)->b[index], (B)->b[index], r->m[i]);          \
      }                                                                        \
    }                                                                          \
    (C)->is_ntt = A->is_ntt | B->is_ntt;                                       \
  } while (0)

int poly_zero(const ring_t *const r, poly_t *p) {
  if (!(p->b = calloc(1, (sizeof(int_t) * r->n) << r->lgd)))
    return -errno;
  p->r = (ring_t *)r;
  p->is_ntt = 0;
  return 0;
}

void poly_clone(poly_t *dst, const poly_t *const src) {
  poly_zero(src->r, dst);
  for (unsigned i = 0; i < src->r->d * src->r->n; ++i) {
    dst->b[i] = src->b[i];
  }
  dst->is_ntt = src->is_ntt;
}

void poly_rand(const ring_t *const r, poly_t *p, DISTRIBUTION d) {
  poly_zero(r, p);

  OMP_FOR
  for (size_t j = 0; j < r->d; ++j) {
    int_t s = sample(d);
    for (size_t i = 0; i < r->n; ++i)
      p->b[(i << r->lgd) + j] = s % r->m[i];
  }
}

void poly_cmul(poly_t *c, const poly_t *const a, int_t b) {
  ring_t *r = c->r;

  OMP_FOR
  for (size_t i = 0; i < r->n; ++i) {
    for (size_t j = 0; j < r->d; ++j) {
      int index = (i << r->lgd) + j;
      c->b[index] = modmul(a->b[index], b, r->m[i]);
    }
  }

  c->is_ntt = a->is_ntt;
}

void poly_neg(poly_t *p) {
  ring_t *r = p->r;

  OMP_FOR
  for (size_t i = 0; i < r->n; ++i) {
    for (size_t j = 0; j < r->d; ++j) {
      int index = (i << r->lgd) + j;
      p->b[index] = modsub(r->m[i], p->b[index], r->m[i]);
    }
  }
}

inline void poly_add(poly_t *c, const poly_t *const a, const poly_t *const b) {
  POLY_BINOP(c, a, b, modadd);
}

inline void poly_sub(poly_t *c, const poly_t *const a, const poly_t *const b) {
  POLY_BINOP(c, a, b, modsub);
}

inline void poly_mul(poly_t *c, const poly_t *const a, const poly_t *const b) {
  POLY_BINOP(c, a, b, modmul);
}

void poly_encode(const ring_t *const r, const uint_t *const x, poly_t *p) {
  poly_zero(r, p);

  OMP_FOR
  for (size_t i = 0; i < r->n; ++i) {
    for (size_t j = 0; j < r->d; ++j)
      p->b[(i << r->lgd) + j] = x[j] % r->m[i];
  }

  poly_ntt(p);
}

void poly_decode(uint_t *out, const poly_t *const p, uint_t mod) {
  ring_t *r = p->r;

  OMP_FOR
  for (size_t i = 0; i < r->d; ++i) {
    mpz_t v, x;
    mpz_init(x);
    mpz_init(v);

    for (size_t j = 0; j < r->n; ++j) {
      mpz_mul_ui(v, r->ms[j], r->invms[j]);
      mpz_mul_ui(v, v, p->b[(j << r->lgd) + i]);
      mpz_add(x, x, v);
    }
    mpz_mod(x, x, r->M);
    if (mpz_cmp(x, r->M_half) > 0)
      mpz_sub(x, x, r->M);
    out[i] = mpz_fdiv_ui(x, mod);

    mpz_clear(v);
    mpz_clear(x);
  }
}

void poly_serialize(unsigned char *out, const poly_t *const p) {
  ring_t *r = p->r;
  const int len = r->d * r->n * sizeof(int_t);
  for (int i = 0; i < len; ++i)
    out[i] = ((unsigned char *)p->b)[i];
}

void poly_deserialize(poly_t *p, const unsigned char *const buf) {
  ring_t *r = p->r;
  const int len = r->d * r->n * sizeof(int_t);
  for (int i = 0; i < len; ++i)
    ((unsigned char *)p->b)[i] = buf[i];
}

int poly_cmp(const poly_t *const a, const poly_t *const b) {
  int res = 0;
  ring_t *r = a->r;
  for (size_t i = 0; i < r->d * r->n; ++i)
    res |= (a->b[i] - b->b[i]);
  return !res;
}

void poly_free(poly_t *r) {
  free(r->b);
  r->b = NULL;
  r->r = NULL;
  r->is_ntt = 0;
}
