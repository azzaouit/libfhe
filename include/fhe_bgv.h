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
/// This file contains the declaration of the bgv_t type, which
/// implements the Brakerski-Gentry-Vaikuntanathan encryption scheme.
///
//===----------------------------------------------------------------------===//

#ifndef FHE_BGV_H
#define FHE_BGV_H

#include "fhe_poly.h"
#include "fhe_ring.h"

///
/// \brief Main BGV type used to instantiate the scheme.
///
typedef struct bgv_t {
  size_t t; ///< Plaintext modulus \f$t\f$
  ring_t r; ///< Polynomial ring \f$R_q = Z_q[x]/<x^d + 1>\f$
} bgv_t;

///
/// \brief Generic key pair wraps a pair of polynomials
///
typedef struct bgv_keypair_t {
  poly_t a; ///< Polynomial a
  poly_t b; ///< Polynomial b
} bgv_keypair_t;

///
/// \brief BGV key pair used to encrypt, decrypt, and relinearize a ciphertext.
///
typedef struct bgv_key_t {
  poly_t s;           ///< Secret key \f$s \in R_q\f$
  bgv_keypair_t pub;  ///< Public key pair \f$(a, b) \in R_q\f$
  bgv_keypair_t eval; ///< Evaluation key pair \f$(a, b) \in R_q\f$
} bgv_key_t;

///
/// \brief BGV Ciphertext consists of \f$n\f$ polynomials over
/// the ciphertext ring \f$R_q = Z_q[x]/<x^d + 1>\f$
//
/// Note: The Ciphertext size \f$n\f$ increases by one after every
/// multiplication.
///
typedef struct bgv_ct_t {
  size_t n;  ///< Number of polynomials
  poly_t *c; ///< \f$n\f$ Ciphertext polynomials \f$c_i \in R_q\f$
} bgv_ct_t;

///
/// \brief Initialize BGV scheme parameters
///
/// \param b BGV context
/// \param lgd where d is the polynomial ring degree (power of 2)
/// \param lgq the bitlength of the ciphertext modulus q
/// \param lgm the bitlength of the residues in the CRT representation of q
/// \param t the plaintext modulus
///
/// \returns 0 on success, nonzero otherwise.
///
int bgv_init(bgv_t *b, size_t lgd, size_t lgq, size_t lgm, size_t t);

///
/// \brief Generate a BGV key pair
///
/// 1. Sample random secret key \f$s \in R_2\f$ (ternary distribution)
/// 2. Sample random \f$a \in R_q\f$ (uniform distribution)
/// 3. Sample random \f$e \in R_q\f$ (error distribution)
///
/// \param b BGV context
/// \param [out] k generated key \f$ = (s, (e - as, a))\f$
///
void bgv_keygen(const bgv_t *const b, bgv_key_t *k);

///
/// \brief Serialize a BGV key pair
///
/// \param [out] buf Serialized key pair byte stream
/// \param k Key pair to be serialized
///
void bgv_key_serialize(unsigned char *buf, const bgv_key_t *const k);

///
/// \brief Deserialize a BGV key pair
///
/// \param r Polynomial ring
/// \param [out] k Deserialized key pair
/// \param buf Serialized key pair
///
void bgv_key_deserialize(const ring_t *const r, bgv_key_t *k,
                         const unsigned char *const buf);

///
/// \brief Encrypt a polynomial using the BGV scheme
/// 1. Sample random secret key \f$s \in R_2\f$ (ternary distribution)
/// 2. Sample random \f$a \in R_q\f$ (uniform distribution)
/// 3. Sample random \f$e \in R_q\f$ (error distribution)
///
/// \param b BGV context
/// \param [out] c Resulting ciphertext
/// \param k BGV public key used for encryption
/// \param m Plaintext message used for encryption
///
void bgv_encrypt(const bgv_t *const b, bgv_ct_t *c,
                 const bgv_keypair_t *const k, const poly_t *const m);

///
/// \brief Decrypt a BGV ciphertext
///
/// \param [out] m Resulting plaintext
/// \param c BGV ciphertext to be decrypted
/// \param s BGV secret key with which to decrypt
///
void bgv_decrypt(poly_t *m, const bgv_ct_t *const c, const poly_t *const s);

///
/// \brief Destroy a BGV struct.
/// Free any memory allocated by the context.
///
/// \param b BGV context
///
void bgv_free(bgv_t *b);

///
/// \brief Destroy a BGV key pair
/// Free any memory allocated by the key generation process.
///
/// \param k BGV key
///
void bgv_key_free(bgv_key_t *k);

///
/// \brief Initialize an empty key pair
///
/// \param r Polynomial ring
/// \param [out] k Empty key pair
///
void bgv_key_zero(const ring_t *const r, bgv_key_t *k);

///
/// \brief Test two keys for equality.
/// Returns 1 if keys are equal, 0 otherwise
///
/// \param a BGV Key
/// \param b BGV Key
///
int bgv_key_cmp(const bgv_key_t *const a, const bgv_key_t* const b);

///
/// \brief Initialize an empty BGV ciphertext
///
/// \param r Polynomial ring
/// \param [out] c Initialized ciphertext
/// \param n The ciphertext length (number of underlying polynomials)
///
int bgv_ct_init(const ring_t *const r, bgv_ct_t *c, size_t n);

///
/// \brief Add two BGV ciphertexts
///
/// \param [out] c The encryption of a + b
/// \param a BGV ciphertext addend
/// \param b BGV ciphertext addend
///
/// Note: Any of a, b, or c may overlap
///
void bgv_ct_add(bgv_ct_t *c, const bgv_ct_t *const a, const bgv_ct_t *const b);

///
/// \brief Multiply two BGV ciphertexts
///
/// \param [out] c The encryption of a * b
/// \param e BGV evaluation key used for auto-relinearization
/// \param a BGV ciphertext addend
/// \param b BGV ciphertext addend
///
/// Note: c will be overwritten by this function
/// and SHOULD NOT overlap with neither the multiplier nor the multiplicand.
///
void bgv_ct_mul(bgv_ct_t *c, const bgv_keypair_t *e, const bgv_ct_t *const a,
                const bgv_ct_t *const b);

///
/// \brief Relinearize a BGV ciphertext in place
///
/// \param [out] c The relinearized ciphertext
/// \param k The relinierization key
///
/// Note: c will be overwritten by this function.
///
void bgv_ct_relin(bgv_ct_t *c, const bgv_keypair_t *const k);

///
/// \brief Serialize a BGV ciphertext into a byte stream
///
/// \param [out] buf Serialized ciphertext byte stream
/// \param c Ciphertext to be serialized
///
void bgv_ct_serialize(unsigned char *buf, const bgv_ct_t *const c);

///
/// \brief Deserialize a BGV ciphertext from a byte stream
///
/// \param r Polynomial ring
/// \param [out] c Deserialized ciphertext
/// \param buf Serialized ciphertext byte stream
///
void bgv_ct_deserialize(const ring_t *const r, bgv_ct_t *c,
                        const unsigned char *buf);

///
/// \brief Destroy a BGV ciphertext
/// Free any memory allocated by the encryption process.
///
/// \param c BGV ciphertext
///
void bgv_ct_free(bgv_ct_t *c);

#endif /* FHE_BGV_H */
