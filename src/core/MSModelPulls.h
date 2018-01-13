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
 * Virtual model implementing pull/penalty terms and derived classes
 * 
 * \details 
 * 
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSModelPullGaus_H
#define MST_MSModelPullGaus_H

// m-stats libs
#include "MSModel.h"

namespace mst {

class MSModelPull : public mst::MSModel
{
   public:
      //! Constructor
      MSModelPull(const std::string& name = ""): MSModel(name) {}
      //! Destructor
      virtual ~MSModelPull() {}

      //! function returning the negative log likelihood function to be 
      //! minimized (NLL)
      double NLogLikelihood(double* par) override = 0;

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

      //! function returning the negative log likelihood function to be 
      //! minimized (NLL)
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
      double fCentroid {0.0};
      double fSigma {0.0};
};

class MSModelPullExp : public mst::MSModelPull
{
   public:
      //! Constructor
      MSModelPullExp(const std::string& name = ""): MSModelPull(name) {}
      //! Destructor
      virtual ~MSModelPullExp() {}

      //! function returning the negative log likelihood function to be 
      //! minimized (NLL)
      double NLogLikelihood(double* par) override;

      //! Set limit
      void SetLimit (double limit) {
         if (limit > 0) fLimit = limit;
         else std::cerr << "BXModelExp >> error: limit must be positive.\n";
      }
      //! Set quantile
      void SetQuantile (double quantile) {
         if (quantile > 0.0 && quantile < 1.0) fQuantile = quantile;
         else std::cerr << "BXModelExp >> error: quantile must be positive.\n";
      }
      //! Set offset
      void SetOffset (double offset) {
         if (offset >= 0) fOffset = offset;
         else std::cerr << "BXModelExp >> error: offset must be positive.\n";
      }

      //! Set all gaussian parameters
      void SetExpPar (double limit, double quantile = .9, double offset = 0.0) { 
         SetLimit(limit); SetQuantile(quantile); SetOffset(offset);
      }

   public:
      double fLimit    {0.0};
      double fQuantile {0.0};
      double fOffset   {0.0};
};
} // namespace mst

#endif //MST_MSModelPullGaus_H
