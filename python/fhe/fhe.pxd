# pylibfhe
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from libc.stddef cimport size_t
from libc.stdint cimport int64_t, uint64_t
from gmpy2 cimport *

import_gmpy2()

cdef extern from "fhe.h":
    ctypedef struct ring_t:
        size_t lgd
        size_t d
        size_t n
        mpz_t M
        mpz_t M_half
        mpz_t *ms
        uint64_t *roots
        uint64_t *iroots
        uint64_t *invms
        uint64_t *m
        uint64_t *minv
        uint64_t *dinv

    int ring_init(ring_t *, size_t lgd, size_t lgq, size_t lgm)
    void ring_free(ring_t *r)

cdef extern from "fhe.h":
    ctypedef struct poly_t:
        int64_t *b
        ring_t *r
        char is_ntt

    int poly_zero(ring_t *r, poly_t *p)
    void poly_encode(ring_t *r, uint64_t *u, poly_t *p)
    void poly_decode(uint64_t *out, poly_t *p, uint64_t t)
    void poly_ntt(poly_t *p)
    void poly_intt(poly_t *p)
    void poly_clone(poly_t *dst, poly_t *src)
    void poly_cmul(poly_t *out, poly_t *in_, int64_t c)
    void poly_neg(poly_t *p)
    void poly_add(poly_t *c, poly_t *a, poly_t *b)
    void poly_sub(poly_t *c, poly_t *a, poly_t *b)
    void poly_mul(poly_t *c, poly_t *a, poly_t *b)
    int poly_cmp(poly_t *a, poly_t *b);
    void poly_free(poly_t *p)

cdef extern from "fhe.h":
    ctypedef struct bgv_t:
        size_t t
        ring_t r

    ctypedef struct bgv_keypair_t:
        poly_t a
        poly_t b

    ctypedef struct bgv_key_t:
        poly_t s
        bgv_keypair_t pub
        bgv_keypair_t eval

    ctypedef struct bgv_ct_t:
        size_t n
        poly_t *c

    int bgv_init(bgv_t *b, size_t lgd, size_t lgq, size_t lgm, size_t t)
    void bgv_free(bgv_t *b)

    void bgv_keygen(bgv_t *b, bgv_key_t *k)
    void bgv_key_zero(ring_t *r, bgv_key_t *k);
    void bgv_key_serialize(unsigned char *buf, bgv_key_t *k)
    void bgv_key_deserialize(ring_t *r, bgv_key_t *k, unsigned char *buf)
    void bgv_encrypt(bgv_t *b, bgv_ct_t *c, bgv_keypair_t *k, poly_t *m)
    void bgv_decrypt(poly_t *m, bgv_ct_t *c, poly_t *s)
    int bgv_key_cmp(bgv_key_t* a, bgv_key_t* b)
    void bgv_key_free(bgv_key_t *k)

    int bgv_ct_init(ring_t *r, bgv_ct_t *c, size_t n)
    void bgv_ct_add(bgv_ct_t *c, bgv_ct_t *a, bgv_ct_t *b)
    void bgv_ct_mul(bgv_ct_t *c, bgv_keypair_t *e, bgv_ct_t *a, bgv_ct_t *b)
    void bgv_ct_relin(bgv_ct_t *c, bgv_keypair_t *k)
    void bgv_ct_serialize(unsigned char *buf, bgv_ct_t *c);
    void bgv_ct_deserialize(ring_t *r, bgv_ct_t *c, unsigned char *buf)
    void bgv_ct_serialize(unsigned char *buf, bgv_ct_t *c)
    void bgv_ct_free(bgv_ct_t *c)
