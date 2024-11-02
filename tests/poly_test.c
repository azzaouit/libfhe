#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <fhe.h>

#include "params.h"

int main() {
  ring_t r;
  unsigned char *buff;
  poly_t zero, one, a, b, c, d, ab, ac, bc;
  uint_t x[D] = {1}, y[D] = {0};

  ring_init(&r, LGD, LGQ, LGM);
  poly_encode(&r, x, &one);
  poly_zero(&r, &zero);
  poly_zero(&r, &c);
  poly_zero(&r, &d);
  poly_zero(&r, &ab);
  poly_zero(&r, &ac);
  poly_zero(&r, &bc);
  poly_rand(&r, &a, UNIFORM);
  poly_rand(&r, &b, UNIFORM);

  poly_decode(x, &a, T);
  poly_decode(y, &b, T);

  poly_mul(&c, &a, &b);
  poly_mul(&d, &b, &a);
  poly_intt(&c);
  poly_intt(&d);
  poly_decode(x, &c, T);
  poly_decode(y, &d, T);
  assert(!memcmp(x, y, sizeof x));

  poly_mul(&ab, &a, &b);
  poly_mul(&ac, &a, &c);
  poly_add(&ab, &ab, &ac);
  poly_add(&bc, &b, &c);
  poly_mul(&bc, &a, &bc);
  poly_decode(x, &ab, T);
  poly_decode(y, &bc, T);
  assert(!memcmp(x, y, sizeof x));

  poly_mul(&a, &a, &zero);
  poly_intt(&a);
  poly_decode(x, &a, T);
  memset(y, 0, sizeof y);
  assert(!memcmp(x, y, sizeof x));

  poly_free(&a);
  poly_clone(&a, &b);
  poly_mul(&b, &b, &one);
  assert(poly_cmp(&a, &b));

  buff = malloc(r.d * r.n * 8);
  poly_serialize(buff, &a);
  poly_deserialize(&b, buff);
  assert(poly_cmp(&a, &b));

  free(buff);
  poly_free(&b);
  poly_free(&a);
  poly_free(&bc);
  poly_free(&ac);
  poly_free(&ab);
  poly_free(&d);
  poly_free(&c);
  poly_free(&zero);
  poly_free(&one);
  ring_free(&r);

  return 0;
}
