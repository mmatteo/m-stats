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

/*!
 * \class mst::MSModelDataSet
 *
 * \brief 
 * Base class for all analysis modules.
 *
 * \details 
 * Pure virutal class providing a general interace for  all analysis models. 
 * It provides virtual functions to be used by the minimizer and handles
 * the data sets.
 *
 * \author Matteo Agostini
 */


#ifndef MST_MSModelDataSet_H
#define MST_MSModelDataSet_H

#include "MSParameter.h"
#include "MSDataSet.h"

#include <vector>

namespace mst {

class MSModelDataSet : public MSModel
{
   public:
      //! Constructor
      MSModelDataSet(const std::string& name = ""): MSModel(name), fDataSet(0) {}

      //! Destructor
      virtual ~MSModelDataSet() { delete fDataSet; }

      //! Virtual function from MSModel.
      //! To be overloaded in the concrete analysis module.
      virtual void InitializeParameters() = 0;

      //! Virtual function from MSModel to be overloaded in the concrete class
      //! To be overloaded in the concrete analysis module.
      virtual double NLogLikelihood(double* par) = 0;

      //! Set data set (the function takes ownership of the object)
      void SetDataSet(MSDataSet* dataSet) {
         if (fDataSet) delete fDataSet; 
         fDataSet = dataSet;
      }

      //! Get pointer to the data set
      const MSDataSet* GetDataSet() const { return fDataSet; }

      //! Get number of points in the data set
      unsigned int GetNDataPoints() const {
         return (fDataSet ? fDataSet->GetNDataPoints() : 0);
      }

      //! Get pointer to a data point
      const MSDataPoint* GetDataPoint(int index) const {
           return (fDataSet ? fDataSet->GetDataPoint(index) : 0);
      }

      //! Get value of an observable in a data point
      double GetDataPoint(int index, const std::string& field) const {
         return (fDataSet ? fDataSet->GetDataPoint(index)->Get(field) : 0);
      }

   protected:
      //! Pointer to the data set
      const MSDataSet* fDataSet;
};

} // namespace mst

#endif // MST_MSModelDataSet_H
