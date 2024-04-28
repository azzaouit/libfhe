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
/// This file implements a thread local psuedorandom generator
///
//===----------------------------------------------------------------------===//

#ifndef RAND_RANDOM_H
#define RAND_RANDOM_H

#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "chacha.h"

#if defined(_WIN32)
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <bcrypt.h>
#include <ntstatus.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/random.h>
#elif defined(__OpenBSD__)
#include <unistd.h>
#else
#error "No suitable entropy source found"
#endif

#define RNG_BUF_LEN (1UL << 10)
#define RNG_RESEED (1UL << 18)

typedef struct rng_ctx_t {
  uint8_t init;
  size_t offset, reseed;
  uint8_t b[RNG_BUF_LEN];
  uint32_t chacha_state[16];
} rng_ctx_t;

static __thread rng_ctx_t __rng;

static void entropy(void *buf, size_t len) {
#if defined(__linux__)
  int ret;
  while (len) {
    ret = getrandom(buf, len, 0);
    if (ret < 0 && errno != EINTR)
      exit(errno);
    buf = (unsigned char *)buf + ret;
    len -= ret;
  }
#elif defined(__unix__) || defined(__APPLE__)
  if (getentropy(buf, len))
    exit(errno);
#elif defined(_WIN32)
  NTSTATUS res =
      BCryptGenRandom(NULL, buf, len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  if (res != STATUS_SUCCESS)
    exit(res);
#else
#error "No suitable entropy source found"
#endif
}

static void rng_init() {
  uint8_t key[CHACHA20_KEYBYTES];
  entropy(key, CHACHA20_KEYBYTES);
  chacha_init(__rng.chacha_state, key);
  __rng.offset = RNG_BUF_LEN;
  __rng.reseed = 0;
  __rng.init = 1;
}

static void rng_refill() {
  if (__rng.reseed >= RNG_RESEED)
    rng_init();
  chacha_keystream_bytes(__rng.chacha_state, __rng.b, RNG_BUF_LEN);
  __rng.offset = 0;
  __rng.reseed += RNG_BUF_LEN;
}

static inline void rng(void *buffer, size_t len) {
  unsigned char *buf = buffer;
  if (!__rng.init)
    rng_init();
  if (len > (RNG_BUF_LEN >> 1))
    chacha_keystream_bytes(__rng.chacha_state, buffer, len);
  else
    while (len) {
      if (__rng.offset >= RNG_BUF_LEN)
        rng_refill();
      size_t remaining = RNG_BUF_LEN - __rng.offset;
      size_t nbytes = len < remaining ? len : remaining;
      for (int i = 0; i < (int)nbytes; ++i)
        buf[i] = __rng.b[__rng.offset + i];
      __rng.offset += nbytes;
      buf += nbytes;
      len -= nbytes;
    }
}

static inline uint32_t uniform32() {
  uint32_t r;
  rng(&r, sizeof(uint32_t));
  return r;
}

static inline uint64_t uniform64() {
  uint64_t r;
  rng(&r, sizeof(uint64_t));
  return r;
}

static inline double uniformr() {
  uint64_t r = uniform64();
  return ldexpf(r & ((1UL << DBL_MANT_DIG) - 1), -DBL_MANT_DIG);
}

static inline uint8_t uniformb() { return uniform32() & 1; }

#endif /* RAND_RANDOM_H */
