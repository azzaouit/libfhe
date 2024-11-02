#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <fhe.h>

#include "params.h"

int main() {
  bgv_t b;
  poly_t x, y;
  bgv_key_t k, l;
  bgv_ct_t u, v;
  unsigned char *buf;

  bgv_init(&b, LGD, LGQ, LGM, T);
  bgv_keygen(&b, &k);

  {
    buf = malloc(b.r.d * b.r.n * 8);
    poly_rand(&b.r, &x, UNIFORM);
    poly_zero(&b.r, &y);

    poly_serialize(buf, &x);
    poly_deserialize(&y, buf);

    assert(poly_cmp(&x, &y));
    free(buf);
  }

  {
    buf = malloc((2 * b.r.d * b.r.n * 8) + 4);
    bgv_encrypt(&b, &u, &k.pub, &x);

    bgv_ct_serialize(buf, &u);
    bgv_ct_deserialize(&b.r, &v, buf);

    assert(u.n == v.n);
    for (size_t i = 0; i < u.n; ++i)
      assert(poly_cmp(u.c + i, v.c + i));

    free(buf);
  }

  {
    buf = malloc(5 * b.r.d * b.r.n * 8);
    bgv_key_serialize(buf, &k);
    bgv_key_deserialize(&b.r, &l, buf);

    assert(poly_cmp(&k.s, &l.s));
    assert(poly_cmp(&k.pub.a, &l.pub.a));
    assert(poly_cmp(&k.pub.b, &l.pub.b));
    assert(poly_cmp(&k.eval.a, &l.eval.a));
    assert(poly_cmp(&k.eval.b, &l.eval.b));

    free(buf);
  }

  bgv_ct_free(&v);
  bgv_ct_free(&u);
  poly_free(&x);
  poly_free(&y);
  bgv_key_free(&k);
  bgv_key_free(&l);
  bgv_free(&b);

  return 0;
}
