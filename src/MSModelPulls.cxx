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


#include "MSModelPulls.h"
#include <MSParameter.h>
#include <MSMath.h>

namespace mst {


MSModelPullGaus::MSModelPullGaus(const std::string& name) : 
   mst::MSModel(name), fPullPar(""), fCentroid(0), fSigma(0)
{
}

MSModelPullGaus::~MSModelPullGaus()
{
}

void MSModelPullGaus::InitializeParameters()
{
   // Check if the name of the parameter to pull has been set
   if (fPullPar == "") {
      std::cerr << "MSModelPullGaus >> error: parameter name not defined.\n";
      return;
   }

   // Define the paramters of the model
   mst::MSParameter* par = new mst::MSParameter(fPullPar.c_str());
   // all parameters properties should have been defined by the model in which
   // this parameter is used
   par->SetGlobal();
   par->SetFitStartValue(fCentroid);
   AddParameter(par);
}



double MSModelPullGaus::NLogLikelihood(double* par)
{
   const double x = GetMinuitParameter(par, fPullPar.c_str());
   return  -2 * mst::MSMath::LogGaus(x, fCentroid, fSigma);
}

} // namespace mst
