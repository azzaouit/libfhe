#include <assert.h>
#include <fhe.h>

#include "../params.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  bgv_t b;
  bgv_key_t k;
  bgv_ct_t ct;
  poly_t p, z, r;
  uint_t buf[D] = {0}, res[D] = {0};

  const size_t nbytes = sizeof(uint_t) << LGD;
  const size_t blen = nbytes < size ? nbytes : size;

  bgv_init(&b, LGD, LGQ, LGM, T);
  bgv_keygen(&b, &k);

  for (int i = 0; i < blen; ++i)
    ((uint8_t *)buf)[i] = data[i];

  poly_zero(&b.r, &z);
  poly_encode(&b.r, buf, &p);
  bgv_encrypt(&b, &ct, &k.pub, &p);
  bgv_decrypt(&r, &ct, &k.s);
  poly_intt(&r);
  poly_decode(res, &r, T);

  for (int i = 0; i < D; ++i)
    assert(buf[i] % T == res[i] % T);

  poly_free(&p);
  poly_free(&z);
  poly_free(&r);
  bgv_ct_free(&ct);
  bgv_key_free(&k);
  bgv_free(&b);

  return 0;
}
