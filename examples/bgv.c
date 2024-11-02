#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <fhe.h>

#define LGD 16
#define D (1 << LGD)
#define LGQ 800
#define LGM 60
#define T 65537

int main() {
  bgv_t b;
  bgv_key_t k;
  poly_t m, m2, p;
  bgv_ct_t c, c2, c3;
  uint_t x[D] = {0}, y[D] = {0}, z[D];

  /* Init BGV Context */
  bgv_init(&b, LGD, LGQ, LGM, T);

  /* Generate encryption keys */
  bgv_keygen(&b, &k);

  /* Sample random polynomials */
  for (int i = 0; i < D; ++i) {
    x[i] = rand() % T;
    y[i] = rand() % T;
  }

  {
    /* Encode polynomials */
    poly_encode(&b.r, x, &m);
    poly_encode(&b.r, y, &m2);

    /* Encrypt polynomials */
    bgv_encrypt(&b, &c, &k.pub, &m);
    bgv_encrypt(&b, &c2, &k.pub, &m2);

    /* Encrypted addition */
    bgv_ct_add(&c3, &c2, &c);
    bgv_decrypt(&p, &c3, &k.s);

    /* Recover polynomial */
    poly_decode(z, &p, T);
    printf("Testing encrypted addition...\n");
    for (int i = 0; i < D; ++i)
      assert(z[i] == ((x[i] + y[i]) % T));

    bgv_ct_free(&c);
    bgv_ct_free(&c2);
    bgv_ct_free(&c3);
    poly_free(&p);
    poly_free(&m);
    poly_free(&m2);
  }

  {
    /* Encode constant polynomials */
    for (int i = 1; i < D; ++i)
      x[i] = y[i] = 0;
    poly_encode(&b.r, x, &m);
    poly_encode(&b.r, y, &m2);

    /* Encrypt polynomials */
    bgv_encrypt(&b, &c, &k.pub, &m);
    bgv_encrypt(&b, &c2, &k.pub, &m2);

    /* Encrypted multiplication */
    bgv_ct_mul(&c3, &k.eval, &c2, &c);
    bgv_decrypt(&p, &c3, &k.s);

    /* Recover polynomial */
    poly_decode(z, &p, T);

    printf("Testing encrypted multiplication...\n");
    assert(z[0] == (x[0] * y[0]) % T);

    bgv_ct_free(&c);
    bgv_ct_free(&c2);
    bgv_ct_free(&c3);
    poly_free(&p);
    poly_free(&m);
    poly_free(&m2);
  }

  printf("[+] All checks passed\n");

  bgv_key_free(&k);
  bgv_free(&b);

  return 0;
}
