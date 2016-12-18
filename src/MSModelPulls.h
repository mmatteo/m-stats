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


/*!
 * \class mst::MSModelPullGaus
 *
 * \brief
 * basic model to add pull terms to a global parameter
 * 
 * \details 
 * 
 *
 * \author Matteo Agostini
 */


#ifndef MST_MSModelPullGaus_H
#define MST_MSModelPullGaus_H

#include "MSModel.h"

namespace mst {

class MSModelPull : public mst::MSModel
{
   public:
      //! Constructor
      MSModelPull(const std::string& name = ""): MSModel(name) {}
      //! Destructor
      virtual ~MSModelPull() {}

      //! Function used for initializing the model parameters
      void InitializeParameters() = 0;
      //! function returning the negative log likelihood function to be 
      //! minimized (-2LL)
      double NLogLikelihood(double* par) = 0 ;

      //! Set parameter to pull
      void SetPullPar (const std::string& par) { fPullPar = par;}
      //! Get parameter to pull
      std::string GetPullPar () const { return fPullPar;}

   public:
      std::string fPullPar {""};
};

class MSModelPullGaus : public mst::MSModelPull
{
   public:
      //! Constructor
      MSModelPullGaus(const std::string& name = ""): MSModelPull(name) {}
      //! Destructor
      virtual ~MSModelPullGaus() {}

      //! Function used for initializing the model parameters
      void InitializeParameters() override;
      //! function returning the negative log likelihood function to be 
      //! minimized (-2LL)
      double NLogLikelihood(double* par) override;

      //! Set centroid
      void SetCentroid (double centroid) {fCentroid = centroid;}
      //! Set sigma
      void SetSigma (double sigma) {
         if (sigma > 0) fSigma = sigma;
         else std::cerr << "BXModelGauss >> error: sigma must be positive.\n";
      }
      //! Set all gaussian parameters
      void SetGaussPar (double c, double s) { SetCentroid(c); SetSigma(s); }

   public:
      double fCentroid {0};
      double fSigma {0};
};

} // namespace mst

#endif //MST_MSModelPullGaus_H
