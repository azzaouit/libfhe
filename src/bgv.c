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
/// This file implements the Brakerski-Gentry-Vaikuntanathan encryption scheme.
///
//===----------------------------------------------------------------------===//

#include "fhe_bgv.h"
#include "utils/const_time.h"

#include <errno.h>
#include <stdlib.h>

void bgv_ksgen(const bgv_t *const b, bgv_key_t *k, const poly_t *const s) {
  bgv_keypair_t *eval = &k->eval;
  poly_t e;

  poly_rand(&b->r, &e, ERR);
  poly_cmul(&e, &e, b->t);
  poly_ntt(&e);

  poly_rand(&b->r, &eval->a, UNIFORM);
  poly_ntt(&eval->a);

  poly_clone(&eval->b, &eval->a);
  poly_mul(&eval->b, &eval->b, &k->s);
  poly_sub(&eval->b, &e, &eval->b);
  poly_add(&eval->b, &eval->b, s);

  poly_free(&e);
}

int bgv_init(bgv_t *b, size_t lgd, size_t lgq, size_t lgm, size_t t) {
  b->t = t;
  return ring_init(&b->r, lgd, lgq, lgm);
}

void bgv_keygen(const bgv_t *const b, bgv_key_t *k) {
  bgv_keypair_t *pub = &k->pub;
  poly_t e;

  poly_rand(&b->r, &k->s, TERNARY);
  poly_ntt(&k->s);

  poly_rand(&b->r, &pub->a, UNIFORM);
  poly_ntt(&pub->a);

  poly_rand(&b->r, &e, ERR);
  poly_cmul(&e, &e, b->t);
  poly_ntt(&e);

  poly_clone(&pub->b, &pub->a);
  poly_mul(&pub->b, &pub->b, &k->s);
  poly_sub(&pub->b, &e, &pub->b);

  poly_mul(&e, &k->s, &k->s);
  bgv_ksgen(b, k, &e);

  poly_free(&e);
}

void bgv_key_zero(const ring_t *const r, bgv_key_t *k) {
  poly_zero(r, &k->s);
  poly_zero(r, &k->pub.a);
  poly_zero(r, &k->pub.b);
  poly_zero(r, &k->eval.a);
  poly_zero(r, &k->eval.b);
}

int bgv_key_cmp(const bgv_key_t *const a, const bgv_key_t *const b) {
  int flag = poly_cmp(&a->s, &b->s);
  flag |= poly_cmp(&a->pub.a, &b->pub.a);
  flag |= poly_cmp(&a->pub.b, &b->pub.b);
  flag |= poly_cmp(&a->eval.a, &b->eval.a);
  flag |= poly_cmp(&a->eval.b, &b->eval.b);
  return flag;
}

void bgv_encrypt(const bgv_t *const b, bgv_ct_t *c,
                 const bgv_keypair_t *const k, const poly_t *const m) {
  poly_t u, e1, e2;

  bgv_ct_init(&b->r, c, 2);

  poly_rand(&b->r, &u, TERNARY);
  poly_ntt(&u);

  poly_rand(&b->r, &e1, ERR);
  poly_cmul(&e1, &e1, b->t);
  poly_ntt(&e1);

  poly_rand(&b->r, &e2, ERR);
  poly_cmul(&e2, &e2, b->t);
  poly_ntt(&e2);

  poly_mul(c->c + 1, &u, &k->a);
  poly_add(c->c + 1, c->c + 1, &e1);

  poly_mul(c->c, &u, &k->b);
  poly_add(c->c, c->c, &e2);
  poly_add(c->c, c->c, m);

  poly_free(&u);
  poly_free(&e1);
  poly_free(&e2);
}

void bgv_decrypt(poly_t *m, const bgv_ct_t *const c, const poly_t *const s) {
  if (c->n > 0) {
    poly_clone(m, c->c + c->n - 1);
    for (size_t i = c->n - 1; i > 0; --i) {
      poly_mul(m, m, s);
      poly_add(m, m, c->c + i - 1);
    }
    poly_intt(m);
  }
}

void bgv_free(bgv_t *b) {
  b->t = 0;
  ring_free(&b->r);
}

void bgv_key_free(bgv_key_t *k) {
  poly_free(&k->s);
  poly_free(&k->pub.a);
  poly_free(&k->pub.b);
  poly_free(&k->eval.a);
  poly_free(&k->eval.b);
}

int bgv_ct_init(const ring_t *const r, bgv_ct_t *c, size_t n) {
  if (!(c->c = malloc(sizeof(poly_t) * n)))
    return -errno;
  c->n = n;
  for (size_t i = 0; i < n; ++i)
    poly_zero(r, c->c + i);
  return 0;
}

void bgv_ct_add(bgv_ct_t *out, const bgv_ct_t *const x,
                const bgv_ct_t *const y) {
  if (out && x->n == y->n) {
    bgv_ct_init(x->c->r, out, x->n);
    for (size_t i = 0; i < out->n; ++i)
      poly_add(out->c + i, x->c + i, y->c + i);
  }
}

void bgv_ct_mul(bgv_ct_t *c, const bgv_keypair_t *const ek,
                const bgv_ct_t *const x, const bgv_ct_t *const y) {
  if (x->n == 2 && y->n == 2) {
    poly_t tmp;

    bgv_ct_init(x->c->r, c, x->n + 1);
    poly_zero(c->c->r, &tmp);

    poly_mul(c->c, x->c, y->c);
    poly_mul(c->c + 2, x->c + 1, y->c + 1);

    poly_mul(c->c + 1, x->c, y->c + 1);
    poly_mul(&tmp, x->c + 1, y->c);
    poly_add(c->c + 1, c->c + 1, &tmp);

    poly_free(&tmp);

    bgv_ct_relin(c, ek);
  }
}

void bgv_ct_relin(bgv_ct_t *c, const bgv_keypair_t *const k) {
  if (c->n == 3) {
    poly_t tmp;
    poly_clone(&tmp, c->c + 2);
    poly_mul(&tmp, &tmp, &k->b);
    poly_mul(c->c + 2, c->c + 2, &k->a);

    poly_add(c->c, c->c, &tmp);
    poly_add(c->c + 1, c->c + 1, c->c + 2);

    c->n = 2;
    poly_free(c->c + 2);
    poly_free(&tmp);
  }
}

void bgv_key_serialize(unsigned char *buf, const bgv_key_t *const k) {
  ring_t *r = k->pub.a.r;
  size_t len = (r->d * r->n) << 3;
  poly_serialize(buf, &k->s);
  buf += len;
  poly_serialize(buf, &k->pub.a);
  buf += len;
  poly_serialize(buf, &k->pub.b);
  buf += len;
  poly_serialize(buf, &k->eval.a);
  buf += len;
  poly_serialize(buf, &k->eval.b);
  buf += len;
}

void bgv_key_deserialize(const ring_t *const r, bgv_key_t *k,
                         const unsigned char *buf) {
  size_t len = (r->d * r->n) << 3;
  poly_zero(r, &k->s);
  poly_deserialize(&k->s, buf);
  buf += len;
  poly_zero(r, &k->pub.a);
  poly_deserialize(&k->pub.a, buf);
  buf += len;
  poly_zero(r, &k->pub.b);
  poly_deserialize(&k->pub.b, buf);
  buf += len;
  poly_zero(r, &k->eval.a);
  poly_deserialize(&k->eval.a, buf);
  buf += len;
  poly_zero(r, &k->eval.b);
  poly_deserialize(&k->eval.b, buf);
  buf += len;
}

void bgv_ct_serialize(unsigned char *buf, const bgv_ct_t *const c) {
  ring_t *r = c->c->r;
  size_t len = (r->d * r->n) << 3;
  U32_TO_BYTES(c->n, buf);
  buf += 4;
  for (size_t i = 0; i < c->n; ++i, buf += len)
    poly_serialize(buf, c->c + i);
}

void bgv_ct_deserialize(const ring_t *const r, bgv_ct_t *c,
                        const unsigned char *buf) {
  size_t n, len = (r->d * r->n) << 3;
  U32_FROM_BYTES(n, buf);
  buf += 4;
  bgv_ct_init(r, c, n);
  for (size_t i = 0; i < n; ++i, buf += len)
    poly_deserialize(c->c + i, buf);
}

void bgv_ct_free(bgv_ct_t *c) {
  for (size_t i = 0; i < c->n; ++i)
    poly_free(c->c + i);
  free(c->c);
  c->c = NULL;
  c->n = 0;
}
