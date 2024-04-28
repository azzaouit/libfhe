#include <assert.h>
#include <fhe.h>
#include <stdlib.h>

#include "../params.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  ring_t r;
  poly_t x, y;
  uint_t vec[D];
  unsigned char *buf;
  const size_t nbytes = sizeof(uint_t) << LGD;
  const size_t blen = nbytes < size ? nbytes : size;

  ring_init(&r, LGD, LGQ, LGM);

  buf = malloc((r.d * r.n) << 3);

  for (int i = 0; i < blen; ++i)
    ((uint8_t *)vec)[i] = data[i];

  poly_encode(&r, vec, &x);
  poly_zero(&r, &y);

  poly_serialize(buf, &x);
  poly_deserialize(&y, buf);
  assert(poly_cmp(&x, &y));

  poly_free(&x);
  poly_free(&y);
  ring_free(&r);
  free(buf);

  return 0;
}
