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
/// This file contains the declaration of the poly_t type, which
/// implements arithmetic over the cyclotomic polynomial ring
/// \f$R = Z_M[X] / <x^d + 1>\f$ for a generic modulus \f$M\f$.
///
//===----------------------------------------------------------------------===//

#ifndef FHE_POLY_H
#define FHE_POLY_H

#include "fhe_ring.h"

///
/// \brief Main Polynomial type used to represent polynomials
/// over \f$R = Z_M[X] / <x^d + 1>\f$ for a generic modulus \f$M\f$.
///
/// Polynomials are represented using the ring isomorphism
/// provided by the Chinese Remainder Theorem.
///
/// Namely, a polynomial with coefficients mod \f$M\f$ is decomposed
/// into \f$n\f$ polynomials with coefficients mod \f$m_i\f$
/// of degree \f$d\f$ where \f$ M = \prod_i m_i \f$.
///
/// Residues \f$m_i\f$ are pairwise coprime word-sized integers (typically
/// between 32-60 bits)
///
///
typedef struct poly_t {
  ring_t *r;   ///< Reference to the base ring
  uint_t *b;   ///< Polynomial coefficients
  char is_ntt; ///< Boolean Flag marks whether the polynomial is
               ///< in coefficient or evaluation form
} poly_t;

///
/// \brief Initialize the zero polynomial
///
/// \param r Underlying ring
/// \param [out] p Zero polynomial
///
int poly_zero(const ring_t *const r, poly_t *p);

///
/// \brief Encode a polynomial into its CRT representation
///
/// \param r Underlying ring
/// \param u Polynomial coefficients
/// \param [out] p Encoded polynomial
///
void poly_encode(const ring_t *const r, const uint_t *const u, poly_t *p);

///
/// \brief Decode a polynomial into its original form
///
/// \param [out] out decoded polynomial
/// \param p encoded polynomial
/// \param t modulus
///
void poly_decode(uint_t *out, const poly_t *const p, uint_t t);

///
/// \brief Convert to NTT form
///
/// \param p Polynomial
///
void poly_ntt(poly_t *p);

///
/// \brief Convert back from NTT form
///
/// \param p Polynomial
///
void poly_intt(poly_t *p);

///
/// \brief Clone a polynomial
///
/// \param [out] dst Destination polynomial
/// \param src Source polynomial
///
void poly_clone(poly_t *dst, const poly_t *const src);

///
/// \brief Constant multiplication
///
/// \param [out] out Resulting polynomial
/// \param in Input polynomial
/// \param c Constant multiplier
///
void poly_cmul(poly_t *out, const poly_t *const in, int_t c);

///
/// \brief Sample a random polynomial
///
/// \param r Base ring
/// \param [out] out Random polynomial
/// \param d Sampling distribution
///
void poly_rand(const ring_t *const r, poly_t *out, DISTRIBUTION d);

///
/// \brief Negate a polynomial
///
/// \param p Polynomial
///
void poly_neg(poly_t *p);

///
/// \brief Polynomial addition
///
/// \param [out] c Resulting sum
/// \param a Addend
/// \param b Addend
///
void poly_add(poly_t *c, const poly_t *const a, const poly_t *const b);
///
/// \brief Polynomial subtraction
///
/// \param [out] c Resulting difference
/// \param a Minuend
/// \param b Subtrahend
///
void poly_sub(poly_t *c, const poly_t *const a, const poly_t *const b);

///
/// \brief Polynomial multiplication
///
/// \param [out] c Resulting product
/// \param a multiplicand
/// \param b multiplier
///
void poly_mul(poly_t *c, const poly_t *const a, const poly_t *const b);

///
/// \brief Serialize a polynomial into a byte stream
///
/// \param [out] buf Serialized polynomial
/// \param p Polynomial to be serialized
///
void poly_serialize(unsigned char *buf, const poly_t *const p);

///
/// \brief Deserialize a polynomial from a byte stream
///
/// \param [out] p Deserialized polynomial
/// \param buf Serialized polynomial
///
void poly_deserialize(poly_t *p, const unsigned char *const buf);

///
/// \brief Test two polynomials for equality
/// Returns 1 if polynomials are equal, 0 otherwise
///
/// \param a Polynomial
/// \param b Polynomial
///
int poly_cmp(const poly_t *const a, const poly_t *const b);

///
/// \brief Destroy a Polynomial
/// Free any memory allocated by the polynomial.
///
/// \param p Polynomial
///
void poly_free(poly_t *p);

#endif /* FHE_POLY_H */
