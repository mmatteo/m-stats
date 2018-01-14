// Copyright (C) 2018 Matteo Agostini <matteo.agostini@ph.tum.de>

// This is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// c++ libs
#include <iostream>
#include <limits>
#include <cmath>
#include <vector>

// ROOT libs
#include <TMath.h>

// m-stats libs
#include "MSMath.h"

namespace mst {

double MSMath::LogGaus(double x, double mean, double sigma) {
   // sigma must be positive
   if (sigma <= 0.0) {
      std::cerr << "MSMath::LogGaus >> error: sigma must be positive\n";
      return 0;
   }
   const double dx = (x - mean) / sigma;
   const static double constant = 0.5*log(2*M_PI);
   return -0.5*dx*dx - constant - log(sigma);
}

double MSMath::LogPoisson(double x, double lambda) {

   // If the parameters are negative the Poission probability is not defined
   if (lambda < 0.0 || x < 0.0) {
      std::cerr << "MSMath::LogPoisson >> error: "
                << "function not defined for negative parameters\n";
      return -std::numeric_limits<double>::infinity();
   } 

   // The expectation must be positive. Empty bins in the PSD should be avoided
   else if (lambda == 0.0) {
      if (x == 0) return 0;
      else        return -std::numeric_limits<double>::infinity();
   }

   // Copute Poission probability for positive lambda values
   else {
      if      (x == 0)       return -lambda;
      else if (lambda < 899) return x*log(lambda)-lambda-TMath::LnGamma(x+1.);
      else                   return LogGaus(x, lambda, sqrt(lambda));
   }
}

double MSMath::LogExp (double x, double limit, double quantile, double offset) {
   // the expoential function should be normalized in the range [offset, inf] and
   // have the quantile corresponding to the desidered probablity at the limit
   // value.
   //
   // Starting from the standard exp function normalized betweeen 0 and inf:
   //
   //    f(x) = a*exp(-a*x)
   //
   // the parameter a is hence fixed by:
   //
   //    int _0 ^limit f(x) dx = quantile
   //    => a = -ln(1-quantile)/limit
   //
   // and final the frame must be changed such that 0->offset
   //
   //   => a = -ln(1-quantile)/(limit-offset)
   //   f(x) = a * exp(-a* (x-offset))


   // Check that the quantile is in the range ]0,1[
   if (quantile <= 0.0 && quantile >= 1.0) { 
      std::cerr << "MSMath::Logexp >> error: "
                << "quantile must be >0 && <1\n";
      return std::numeric_limits<double>::quiet_NaN();

   // Check that the limit is above the offset
   } else if (limit <= offset) {
      std::cerr << "MSModelPullExp >> error: "
                << "the limit must be larger than the offset\n";
      return std::numeric_limits<double>::quiet_NaN();

   // Check that the parameter is in the physical range
   } else if (x < offset) {
      std::cerr << "MSMath::Logexp >> error: "
                << "parameter must be larger than the offset\n";
      return std::numeric_limits<double>::quiet_NaN();

   // compute LogExp
   } else {
      const double a = -log(1.0-quantile)/(limit-offset);
      return  log(a)-a*(x-offset);
   }
}
}
