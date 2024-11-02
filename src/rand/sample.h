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
/// This file implements discrete gaussian sampling over the integers.
/// See https://arxiv.org/pdf/1303.6257.pdf for algorithm details.
///
//===----------------------------------------------------------------------===//

#ifndef RAND_SAMPLE_H
#define RAND_SAMPLE_H

#include <math.h>
#include <stdint.h>

#include "fhe_config.h"
#include "random.h"

#define EXP_MINUS_HALF ((double)0.606530659712633)
#define MU 0
#define SIGMA 3.19

static inline uint8_t bernoulli() { return uniformr() < EXP_MINUS_HALF; }

static inline uint32_t G() {
  int n = 0;
  while (bernoulli())
    ++n;
  return n;
}

static inline int32_t S() {
  for (;;) {
    int k = G();
    if (k < 2)
      return k;
    int z = k * (k - 1);
    while (z-- && bernoulli())
      ;
    if (z < 0)
      return k;
  }
}

static inline int32_t sample_err() {
  for (;;) {
    int k = S();

    int s = uniformb() ? -1 : 1;

    double xn0 = k * SIGMA + s * MU;
    int i0 = ceil(xn0);
    xn0 = (i0 - xn0) / SIGMA;
    int j = uniform32() % (int)ceil(SIGMA);

    double x = xn0 + ((double)j) / SIGMA;
    if (x >= 1 || (x == 0 && s < 0 && k == 0))
      continue;

    xn0 = exp(-x * ((k << 1) + x) / 2);
    if (x == 0 || uniformr() <= xn0)
      return s * (i0 + j);
  }
}

static inline int32_t sample(DISTRIBUTION d) {
  switch (d) {
  case UNIFORM:
    return uniform32();
  case TERNARY:
    return (uniform32() % 3) - 1;
  case ERR:
    return sample_err();
  };
  return -1;
}

#endif /* RAND_SAMPLE_H */
