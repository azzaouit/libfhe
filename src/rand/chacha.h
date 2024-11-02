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
/// This file is based on chacha-merged.c version 20080118
/// D. J. Bernstein Public domain.
///
//===----------------------------------------------------------------------===//

#ifndef RAND_CHACHA
#define RAND_CHACHA

#include <stdint.h>

#define CHACHA20_KEYBYTES 32
#define CHACHA20_BLOCKBYTES 64

#define U8V(v) ((uint8_t)(v)&0xFFU)
#define U32V(v) ((uint32_t)(v)&0xFFFFFFFFU)

#define ROTL32(v, n) (U32V((v) << (n)) | ((v) >> (32 - (n))))

#define U8TO32_LITTLE(p)                                                       \
  (((uint32_t)((p)[0])) | ((uint32_t)((p)[1]) << 8) |                          \
   ((uint32_t)((p)[2]) << 16) | ((uint32_t)((p)[3]) << 24))

#define U32TO8_LITTLE(p, v)                                                    \
  do {                                                                         \
    (p)[0] = U8V((v));                                                         \
    (p)[1] = U8V((v) >> 8);                                                    \
    (p)[2] = U8V((v) >> 16);                                                   \
    (p)[3] = U8V((v) >> 24);                                                   \
  } while (0)

#define ROTATE(v, c) (ROTL32(v, c))
#define XOR(v, w) ((v) ^ (w))
#define PLUS(v, w) (U32V((v) + (w)))
#define PLUSONE(v) (PLUS((v), 1))

#define QUARTERROUND(a, b, c, d)                                               \
  a = PLUS(a, b);                                                              \
  d = ROTATE(XOR(d, a), 16);                                                   \
  c = PLUS(c, d);                                                              \
  b = ROTATE(XOR(b, c), 12);                                                   \
  a = PLUS(a, b);                                                              \
  d = ROTATE(XOR(d, a), 8);                                                    \
  c = PLUS(c, d);                                                              \
  b = ROTATE(XOR(b, c), 7);

static const char __chacha_sigma[16] = "expand 32-byte k";

static void chacha_init(uint32_t st[16], uint8_t k[CHACHA20_KEYBYTES]) {
  st[0] = U8TO32_LITTLE(__chacha_sigma + 0);
  st[1] = U8TO32_LITTLE(__chacha_sigma + 4);
  st[2] = U8TO32_LITTLE(__chacha_sigma + 8);
  st[3] = U8TO32_LITTLE(__chacha_sigma + 12);
  st[4] = U8TO32_LITTLE(k + 0);
  st[5] = U8TO32_LITTLE(k + 4);
  st[6] = U8TO32_LITTLE(k + 8);
  st[7] = U8TO32_LITTLE(k + 12);
  st[8] = U8TO32_LITTLE(k + 16);
  st[9] = U8TO32_LITTLE(k + 20);
  st[10] = U8TO32_LITTLE(k + 24);
  st[11] = U8TO32_LITTLE(k + 28);
  st[12] = 0;
  st[13] = 0;
  st[14] = 0;
  st[15] = 0;
}

static void chacha_keystream_bytes(uint32_t st[16], uint8_t *c,
                                   uint32_t bytes) {
  uint32_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
  uint32_t j0, j1, j2, j3, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, j14, j15;
  uint8_t *ctarget = NULL;
  uint8_t tmp[64];
  int i;

  if (!bytes)
    return;

  j0 = st[0];
  j1 = st[1];
  j2 = st[2];
  j3 = st[3];
  j4 = st[4];
  j5 = st[5];
  j6 = st[6];
  j7 = st[7];
  j8 = st[8];
  j9 = st[9];
  j10 = st[10];
  j11 = st[11];
  j12 = st[12];
  j13 = st[13];
  j14 = st[14];
  j15 = st[15];

  for (;;) {
    if (bytes < 64) {
      ctarget = c;
      c = tmp;
    }
    x0 = j0;
    x1 = j1;
    x2 = j2;
    x3 = j3;
    x4 = j4;
    x5 = j5;
    x6 = j6;
    x7 = j7;
    x8 = j8;
    x9 = j9;
    x10 = j10;
    x11 = j11;
    x12 = j12;
    x13 = j13;
    x14 = j14;
    x15 = j15;
    for (i = 20; i > 0; i -= 2) {
      QUARTERROUND(x0, x4, x8, x12)
      QUARTERROUND(x1, x5, x9, x13)
      QUARTERROUND(x2, x6, x10, x14)
      QUARTERROUND(x3, x7, x11, x15)
      QUARTERROUND(x0, x5, x10, x15)
      QUARTERROUND(x1, x6, x11, x12)
      QUARTERROUND(x2, x7, x8, x13)
      QUARTERROUND(x3, x4, x9, x14)
    }
    x0 = PLUS(x0, j0);
    x1 = PLUS(x1, j1);
    x2 = PLUS(x2, j2);
    x3 = PLUS(x3, j3);
    x4 = PLUS(x4, j4);
    x5 = PLUS(x5, j5);
    x6 = PLUS(x6, j6);
    x7 = PLUS(x7, j7);
    x8 = PLUS(x8, j8);
    x9 = PLUS(x9, j9);
    x10 = PLUS(x10, j10);
    x11 = PLUS(x11, j11);
    x12 = PLUS(x12, j12);
    x13 = PLUS(x13, j13);
    x14 = PLUS(x14, j14);
    x15 = PLUS(x15, j15);

    j12 = PLUSONE(j12);
    if (!j12) {
      j13 = PLUSONE(j13);
      /* stopping at 2^70 bytes per nonce is user's responsibility */
    }

    U32TO8_LITTLE(c + 0, x0);
    U32TO8_LITTLE(c + 4, x1);
    U32TO8_LITTLE(c + 8, x2);
    U32TO8_LITTLE(c + 12, x3);
    U32TO8_LITTLE(c + 16, x4);
    U32TO8_LITTLE(c + 20, x5);
    U32TO8_LITTLE(c + 24, x6);
    U32TO8_LITTLE(c + 28, x7);
    U32TO8_LITTLE(c + 32, x8);
    U32TO8_LITTLE(c + 36, x9);
    U32TO8_LITTLE(c + 40, x10);
    U32TO8_LITTLE(c + 44, x11);
    U32TO8_LITTLE(c + 48, x12);
    U32TO8_LITTLE(c + 52, x13);
    U32TO8_LITTLE(c + 56, x14);
    U32TO8_LITTLE(c + 60, x15);

    if (bytes <= 64) {
      if (bytes < 64) {
        for (i = 0; i < (int)bytes; ++i)
          ctarget[i] = c[i];
      }
      st[12] = j12;
      st[13] = j13;
      return;
    }
    bytes -= 64;
    c += 64;
  }
}

#endif /* RAND_CHACHA */
