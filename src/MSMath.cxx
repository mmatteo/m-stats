// Copyright (C) 2014 Matteo Agostini <matteo.agostini@ph.tum.de>

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


#include "MSMath.h"

// c++ libs
#include <limits>
#include <math.h>
#include <iostream>
#include <vector>

// ROOT libs
#include <TMath.h>

namespace mst {

double MSMath::LogGaus(double x, double mean, double sigma) {
   if (sigma <= 0.0) 
      std::cerr << "MSMath::LogGaus >> error: sigma must be positive\n";
   double dx = (x - mean) / sigma;
   return -0.5*dx*dx - 0.5*log(2*M_PI) - log(sigma);
}

double MSMath::LogPoisson(double x, double lambda) {

   // If the parameters are negative the Poission probability is not defined
   if (lambda < 0.0 || x < 0.0) {
      std::cerr << "MSMath::LogPoisson >> error: "
                << "function not defined for negative parameters\n";
      return std::numeric_limits<double>::quiet_NaN();
   } 

   // The expectation must be positive. Empty bins in the PSD should be avoided
   else if (lambda == 0.0) {
      if (x == 0) return 0;
      else        return std::numeric_limits<double>::quiet_NaN();
   }

   // Copute Poission probability for positive lambda values
   else {
      if      (x == 0)       return -lambda;
      else if (lambda < 899) return x*log(lambda)-lambda-TMath::LnGamma(x+1.);
      else                   return LogGaus(x, lambda, sqrt(lambda));
   }
}

}
