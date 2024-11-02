#include <assert.h>
#include <fhe.h>

#include "../params.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  poly_t p;
  ring_t r;
  uint_t buf[D] = {0}, res[D] = {0};

  const size_t nbytes = sizeof(uint_t) << LGD;
  const size_t blen = nbytes < size ? nbytes : size;

  ring_init(&r, LGD, LGQ, LGM);

  for (int i = 0; i < blen; ++i)
    ((uint8_t *)buf)[i] = data[i];

  poly_encode(&r, buf, &p);
  poly_intt(&p);
  poly_decode(res, &p, T);

  for (int i = 0; i < D; ++i)
    assert(buf[i] % T == res[i] % T);

  poly_free(&p);
  ring_free(&r);

  return 0;
}
