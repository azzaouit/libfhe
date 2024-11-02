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
/// This file implements the number theoretic transform
///
//===----------------------------------------------------------------------===//

#include "fhe_poly.h"
#include "fhe_ring.h"

#include "utils/const_time.h"
#include "utils/number_theory.h"

void _ntt(uint_t *roots, uint_t *x, uint_t d, uint_t q, uint_t qinv) {
  uint_t hi, lo, carry;
  for (uint_t m = 1, t = d >> 1; m < d; m <<= 1, t >>= 1) {
    for (uint_t i = 0, k = 0; i < m; ++i, k += (t << 1)) {
      uint_t S = roots[m + i];
      for (uint_t j = k; j < k + t; ++j) {
        lo = mul64(x[j + t], S, &hi);
        mul64(lo * qinv, q, &carry);
        hi = const_time_select64(hi < carry, hi + q - carry, hi - carry);
        x[j + t] = const_time_select64(x[j] < hi, x[j] + q - hi, x[j] - hi);
        x[j] += hi;
        x[j] = const_time_select64(x[j] > q, x[j] - q, x[j]);
      }
    }
  }
}

void _intt(uint_t *iroots, uint_t *x, uint_t d, uint_t q, uint_t qinv,
           uint_t dinv) {
  uint_t hi, lo, carry;
  for (uint_t m = d >> 1, t = 1; m > 0; m >>= 1, t <<= 1) {
    for (uint_t i = 0, k = 0; i < m; ++i, k += (t << 1)) {
      uint_t S = iroots[m + i];
      for (uint_t j = k; j < k + t; ++j) {
        carry = const_time_select64(x[j] < x[j + t], x[j] + q - x[j + t],
                                    x[j] - x[j + t]);
        x[j] += x[j + t];
        x[j] = const_time_select64(x[j] > q, x[j] - q, x[j]);
        lo = mul64(carry, S, &hi);
        mul64(lo * qinv, q, &carry);
        x[j + t] = const_time_select64(hi < carry, hi + q - carry, hi - carry);
      }
    }
  }

  for (uint_t i = 0; i < d; ++i) {
    lo = mul64(x[i], dinv, &hi);
    mul64(lo * qinv, q, &carry);
    x[i] = const_time_select64(hi < carry, hi + q - carry, hi - carry);
  }
}

void poly_ntt(poly_t *p) {
  if (!p->is_ntt) {
    ring_t *r = p->r;

    OMP_FOR
    for (size_t i = 0; i < r->n; ++i) {
      int offset = i << r->lgd;
      _ntt(r->roots + offset, p->b + offset, r->d, r->m[i], r->minv[i]);
    }

    p->is_ntt = 1;
  }
}

void poly_intt(poly_t *p) {
  if (p->is_ntt) {
    ring_t *r = p->r;

    OMP_FOR
    for (size_t i = 0; i < r->n; ++i) {
      int offset = i << r->lgd;
      _intt(r->iroots + offset, p->b + offset, r->d, r->m[i], r->minv[i],
            r->dinv[i]);
    }

    p->is_ntt = 0;
  }
}
