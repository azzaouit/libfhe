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
/// This file contains compile time configuration options for
/// parametrizing builds.
///
//===----------------------------------------------------------------------===//

#ifndef FHE_CONFIG_H
#define FHE_CONFIG_H

#include <stdint.h>

#ifdef _OPENMP
#include <omp.h>
#ifdef _MSC_VER
#define OMP_FOR __pragma(omp parallel for)
#else
#define OMP_FOR _Pragma("omp parallel for")
#endif
#else
#define OMP_FOR
#endif


///
/// \brief Default integer width is 64 bits
///
typedef int64_t int_t;

///
/// \brief Default unsigned integer width is 64 bits
///
typedef uint64_t uint_t;

///
/// \brief Default double integer width is 128 bits
///
typedef __int128_t int_dt;

///
/// \brief Default double unsigned integer width is 128 bits
///
typedef __uint128_t uint_dt;

///
/// \brief Enum determines sampling distribution
///
typedef enum DISTRIBUTION {
  UNIFORM = 0,
  TERNARY,
  ERR,
} DISTRIBUTION;

#endif /* FHE_CONFIG_H */
