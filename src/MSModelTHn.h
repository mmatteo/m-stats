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
 * \class mst::MSModelTHn
 *
 * \brief 
 * Abstract class to implement analysis based on histograms
 * 
 * 
 * \details 
 * Pure virtual class for analysis based on histograms. It provides a common 
 * interface to the n-dimensional data histogram and the  pdf-builder
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSModelTHn_H
#define MST_MSModelTHn_H

#include "MSModel.h"

// c++ libs
#include <climits>

// ROOT libs
#include <THn.h>

namespace mst {

class MSModelTHn : public mst::MSModel
{
   public:
      //! Constructor
      MSModelTHn(const std::string& name = ""): mst::MSModel(name), fDataHist(0) {}

      //! Destructor
      virtual ~MSModelTHn() { delete fDataHist; }

      //! Virtual function from MSModel.
      //! To be overloaded in the concrete analysis module.
      virtual void InitializeParameters() = 0;

      //! Virtual function from MSModel to be overloaded in the concrete class
      //! To be overloaded in the concrete analysis module.
      virtual double NLogLikelihood(double* par) = 0;

      //! Associate to the module the pointer the data histogram.
      //! The function does not move the ownership of the object
      void SetDataHist(const THn* data) {fDataHist = data;}

      //! Return the pointer the data histogram.
      const THn* GetDataHist() const {return fDataHist;}


   protected:
      //! Pointer to the data histogram
      const THn* fDataHist; 
};

} // namespace mst

#endif //MST_MSModelTHn_H
