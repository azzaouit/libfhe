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
/// This file is the top level libfhe header, it includes all other
/// public headers.
///
/// Programs using libfhe should include this header file
/// to access the API like so:
///
/// #include <fhe.h>
///
//===----------------------------------------------------------------------===//
/// \mainpage libfhe 0.0.0 - Manual
///
/// # Overview
///
/// libfhe is a fast and portable fully homomorphic encryption library.
///
/// # API Layout
///
/// | Header file     | Elements                                          |
/// | :-------------- | :------------------------------------------------ |
/// | fhe.h  		  | Top Level Header                                  |
/// | fhe_ring.h	  | Polynomial Ring Definition                        |
/// | fhe_poly.h	  | Polynomial Ring Arithmetic                        |
/// | fhe_bgv.h		  | BGV Scheme Instantiation                          |
/// | fhe_config.h    | Compile time options                              |
///
///
/// Programs using libfhe need only include the top level header to access the
/// API like so:
///
/// #include <fhe.h>

#ifndef FHE_H
#define FHE_H

#include "fhe_bgv.h"
#include "fhe_config.h"
#include "fhe_poly.h"
#include "fhe_ring.h"

#endif /* FHE_H */
