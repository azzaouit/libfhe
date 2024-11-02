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
/// This file contains the declaration of the ring_t type, which
/// implements the cyclotomic polynomial ring \f$R = Z_M[X] / <x^d + 1>\f$
/// for a generic modulus \f$M\f$.
///
//===----------------------------------------------------------------------===//

#ifndef FHE_RING_H
#define FHE_RING_H

#include "fhe_config.h"
#include <gmp.h>

///
/// \brief Main Ring type used to define a polynomial ring
///
typedef struct ring_t {
  size_t lgd;     ///< log d where d is the polynomial degree
  size_t d;       ///< Polynomial degree
  size_t n;       ///< Number of residues in the CRT representation of M
  mpz_t M;        ///< Multiprecision representation of M
  mpz_t M_half;   ///< Multiprecision representation of M/2
  mpz_t *ms;      ///< M/m_i for each CRT residue m_i
  uint_t *roots;  ///< Primitive roots of unity
  uint_t *iroots; ///< Inverse primitive roots of unity
  uint_t *invms;  ///< [M / m_i]_{m_i}^-1
  uint_t *m;      ///< CRT decomposition of M
  uint_t *minv;   ///< [m_i]^-1
  uint_t *dinv;   ///< [d]_{m_i}^-1
} ring_t;

///
/// \brief Initialize a polynomial ring
///
/// \param [out] r The polynomial ring
/// \param lgd The bit length of the polynomial degree
/// \param lgq The bit length of the base ring modulus
/// \param lgm The bit length of the CRT residues
///
///
int ring_init(ring_t *r, size_t lgd, size_t lgq, size_t lgm);

///
/// \brief Destroy a polynomial ring
/// Free any memory allocated by the polynomial ring
///
/// \param r Polynomial ring
///
void ring_free(ring_t *r);

#endif /* FHE_RING_H */
