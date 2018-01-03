// Copyright (C) 2016 Matteo Agostini <matteo.agostini@ph.tum.de>

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

// m-stats libs
#include "MSMath.h"
#include "MSModelPulls.h"
#include "MSParameter.h"

namespace mst {

double MSModelPullGaus::NLogLikelihood(double* par)
{
   const double x = GetMinuitParameter(par, fPullPar.c_str());
   return  (-mst::MSMath::LogGaus(x, fCentroid, fSigma));
}


double MSModelPullExp::NLogLikelihood(double* par)
{
   const double x = GetMinuitParameter(par, fPullPar.c_str());
   return  (-mst::MSMath::LogExp(x,fLimit, fQuantile, fOffset));
}

} // namespace mst
