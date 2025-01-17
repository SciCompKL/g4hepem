#include "ad_type.h"
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// G4Exp
//
// Class description:
//
// The basic idea is to exploit Pade polynomials.
// A lot of ideas were inspired by the cephes math library
// (by Stephen L. Moshier moshier@na-net.ornl.gov) as well as actual code.
// The Cephes library can be found here:  http://www.netlib.org/cephes/
// Code and algorithms for G4Exp have been extracted and adapted for Geant4
// from the original implementation in the VDT mathematical library
// (https://svnweb.cern.ch/trac/vdt), version 0.3.7.

// Original implementation created on: Jun 23, 2012
// Authors: Danilo Piparo, Thomas Hauth, Vincenzo Innocente
//
// --------------------------------------------------------------------
/*
 * VDT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// --------------------------------------------------------------------
#ifndef G4HepEmExp_HH
#define G4HepEmExp_HH 1

/**
 * @file    G4HepEmExp.hh
 * @author  M. Novak
 * @date    2021
 *
 * @brief Just a copy of `G4Exp`, that's in fact a simplified version of VDT exp.
 *
 * That's why the function names are `VDTExp` and `VDTExpf` for double and float.
 *
 */

#ifdef WIN32

#define VDTExp std::exp

#else  // WIN32

#include <limits>
#include <stdint.h>

namespace DVTExpConsts {
  const double EXP_LIMIT = 708;

  const double PX1exp = 1.26177193074810590878E-4;
  const double PX2exp = 3.02994407707441961300E-2;
  const double PX3exp = 9.99999999999999999910E-1;
  const double QX1exp = 3.00198505138664455042E-6;
  const double QX2exp = 2.52448340349684104192E-3;
  const double QX3exp = 2.27265548208155028766E-1;
  const double QX4exp = 2.00000000000000000009E0;

  const double LOG2E = 1.4426950408889634073599;  // 1/log(2)

  const float MAXLOGF = 88.72283905206835f;
  const float MINLOGF = -88.f;

  const float C1F = 0.693359375f;
  const float C2F = -2.12194440e-4f;

  const float PX1expf = 1.9875691500E-4f;
  const float PX2expf = 1.3981999507E-3f;
  const float PX3expf = 8.3334519073E-3f;
  const float PX4expf = 4.1665795894E-2f;
  const float PX5expf = 1.6666665459E-1f;
  const float PX6expf = 5.0000001201E-1f;

  const float LOG2EF = 1.44269504088896341f;

  //----------------------------------------------------------------------------
  // Used to switch between different type of interpretations of the data
  // (64 bits)
  //
  union ieee754 {
    ieee754(){};
    ieee754(double thed) { d = thed; };
    ieee754(uint64_t thell) { ll = thell; };
    ieee754(float thef) { f[0] = thef; };
    ieee754(uint32_t thei) { i[0] = thei; };
    double d;
    float f[2];
    uint32_t i[2];
    uint64_t ll;
    uint16_t s[4];
  };

  //----------------------------------------------------------------------------
  // Converts an unsigned long long to a double
  //
  inline double uint642dp(uint64_t ll) {
    ieee754 tmp;
    tmp.ll = ll;
    return tmp.d;
  }

  //----------------------------------------------------------------------------
  // Converts an int to a float
  //
  inline float uint322sp(int x) {
    ieee754 tmp;
    tmp.i[0] = x;
    return tmp.f[0];
  }

  //----------------------------------------------------------------------------
  // Converts a float to an int
  //
  inline uint32_t sp2uint32(float x) {
    ieee754 tmp;
    tmp.f[0] = x;
    return tmp.i[0];
  }

  //----------------------------------------------------------------------------
  /**
   * A vectorisable floor implementation, not only triggered by fast-math.
   * These functions do not distinguish between -0.0 and 0.0, so are not IEC6509
   * compliant for argument -0.0
   **/
  inline double fpfloor(const double x) {
    // no problem since exp is defined between -708 and 708. Int is enough for
    // it!
    int32_t ret = int32_t(x);
    ret -= (sp2uint32(x) >> 31);
    return ret;
  }

  //----------------------------------------------------------------------------
  /**
   * A vectorisable floor implementation, not only triggered by fast-math.
   * These functions do not distinguish between -0.0 and 0.0, so are not IEC6509
   * compliant for argument -0.0
   **/
  inline float fpfloor(const float x) {
    int32_t ret = int32_t(x);
    ret -= (sp2uint32(x) >> 31);
    return ret;
  }
}  // namespace DVTExpConsts

// Exp double precision --------------------------------------------------------

/// Exponential Function double precision
inline double VDTExp(double initial_x) {
  double x  = initial_x;
  double px = DVTExpConsts::fpfloor(DVTExpConsts::LOG2E * x + 0.5);

  const int32_t n = int32_t(px);

  x -= px * 6.93145751953125E-1;
  x -= px * 1.42860682030941723212E-6;

  const double xx = x * x;

  // px = x * P(x**2).
  px = DVTExpConsts::PX1exp;
  px *= xx;
  px += DVTExpConsts::PX2exp;
  px *= xx;
  px += DVTExpConsts::PX3exp;
  px *= x;

  // Evaluate Q(x**2).
  double qx = DVTExpConsts::QX1exp;
  qx *= xx;
  qx += DVTExpConsts::QX2exp;
  qx *= xx;
  qx += DVTExpConsts::QX3exp;
  qx *= xx;
  qx += DVTExpConsts::QX4exp;

  // e**x = 1 + 2x P(x**2)/( Q(x**2) - P(x**2) )
  x = px / (qx - px);
  x = 1.0 + 2.0 * x;

  // Build 2^n in double.
  x *= DVTExpConsts::uint642dp((((uint64_t) n) + 1023) << 52);

  if(initial_x > DVTExpConsts::EXP_LIMIT)
    x = std::numeric_limits<double>::infinity();
  if(initial_x < -DVTExpConsts::EXP_LIMIT)
    x = 0.;

  return x;
}

// Exp single precision --------------------------------------------------------

/// Exponential Function single precision
inline float VDTExpf(float initial_x) {
  float x = initial_x;
  float z = DVTExpConsts::fpfloor(DVTExpConsts::LOG2EF * x +0.5f); /* std::floor() truncates toward -infinity. */

  x -= z * DVTExpConsts::C1F;
  x -= z * DVTExpConsts::C2F;
  const int32_t n = int32_t(z);

  const float x2 = x * x;

  z = x * DVTExpConsts::PX1expf;
  z += DVTExpConsts::PX2expf;
  z *= x;
  z += DVTExpConsts::PX3expf;
  z *= x;
  z += DVTExpConsts::PX4expf;
  z *= x;
  z += DVTExpConsts::PX5expf;
  z *= x;
  z += DVTExpConsts::PX6expf;
  z *= x2;
  z += x + 1.0f;

  /* multiply by power of 2 */
  z *= DVTExpConsts::uint322sp((n + 0x7f) << 23);

  if(initial_x > DVTExpConsts::MAXLOGF)
    z = std::numeric_limits<float>::infinity();
  if(initial_x < DVTExpConsts::MINLOGF)
    z = 0.f;

  return z;
}


//------------------------------------------------------------------------------

void expv(const uint32_t size, double const* __restrict__ iarray,
          double* __restrict__ oarray);
void expfv(const uint32_t size, float const* __restrict__ iarray,
           float* __restrict__ oarray);

#endif // WIN32

#endif // G4HepEmExp
