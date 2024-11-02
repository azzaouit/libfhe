#include <assert.h>
#include <string.h>

#include <fhe.h>

#include "params.h"

int main() {
  bgv_t b;
  bgv_key_t k;
  uint_t x[D] = {1};
  bgv_ct_t cu, cv, cw, cuv, cvu, cvw, cvuw, cuw, cuwv;
  poly_t zero, one, du, dv, u, v, w, uv, uw;

  bgv_init(&b, LGD, LGQ, LGM, T);
  bgv_keygen(&b, &k);

  poly_zero(&b.r, &uv);
  poly_zero(&b.r, &uw);
  poly_zero(&b.r, &zero);
  poly_encode(&b.r, x, &one);
  poly_rand(&b.r, &u, UNIFORM);
  poly_rand(&b.r, &v, UNIFORM);
  poly_rand(&b.r, &w, UNIFORM);

  bgv_encrypt(&b, &cu, &k.pub, &u);
  bgv_encrypt(&b, &cv, &k.pub, &v);
  bgv_encrypt(&b, &cw, &k.pub, &w);

  {
    bgv_ct_add(&cuv, &cu, &cv);
    bgv_ct_add(&cvu, &cv, &cu);
    bgv_decrypt(&du, &cuv, &k.s);
    bgv_decrypt(&dv, &cvu, &k.s);
    assert(memcmp(du.b, dv.b, b.r.n) == 0);

    bgv_ct_free(&cuv);
    bgv_ct_free(&cvu);
    poly_free(&du);
    poly_free(&dv);
  }

  {
    bgv_ct_mul(&cuv, &k.eval, &cu, &cv);
    bgv_ct_mul(&cvu, &k.eval, &cv, &cu);
    bgv_decrypt(&du, &cuv, &k.s);
    bgv_decrypt(&dv, &cvu, &k.s);
    assert(memcmp(du.b, dv.b, b.r.n) == 0);

    bgv_ct_free(&cuv);
    bgv_ct_free(&cvu);
    poly_free(&du);
    poly_free(&dv);
  }

  {
    bgv_ct_mul(&cvu, &k.eval, &cv, &cu);
    bgv_ct_mul(&cvw, &k.eval, &cv, &cw);
    bgv_ct_add(&cvuw, &cvu, &cvw);
    bgv_ct_add(&cuw, &cu, &cw);
    bgv_ct_mul(&cuwv, &k.eval, &cuw, &cv);
    bgv_decrypt(&du, &cvuw, &k.s);
    bgv_decrypt(&dv, &cuwv, &k.s);
    assert(memcmp(du.b, dv.b, b.r.n) == 0);

    bgv_ct_free(&cvu);
    bgv_ct_free(&cvw);
    bgv_ct_free(&cvuw);
    bgv_ct_free(&cuw);
    bgv_ct_free(&cuwv);
    poly_free(&du);
    poly_free(&dv);
  }

  bgv_ct_free(&cv);
  bgv_ct_free(&cu);
  bgv_ct_free(&cw);

  poly_free(&uv);
  poly_free(&uw);
  poly_free(&zero);
  poly_free(&one);
  poly_free(&u);
  poly_free(&v);
  poly_free(&w);

  bgv_key_free(&k);
  bgv_free(&b);

  return 0;
}
