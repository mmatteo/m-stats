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

/*!
 * \class mst::MSDataPointVector
 *
 * \brief 
 * Collection of MSDataPoint's
 *
 *
 * \details 
 * Class collecting MSDataPoints belonging to a single data set.
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSDataPointVector_H
#define MST_MSDataPointVector_H

// c/c++ libs
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>

// m-stats libs
#include "MSObject.h"
#include "MSDataPoint.h"

namespace mst {

class MSDataPoint;

class MSDataPointVector : public MSObject
{
   public:
      //! Type defining a vector of dataPoint. Used to create a dataset
      using MSDataPointContainer = std::vector<MSDataPoint*>;

   public:
      //! Constructor
      MSDataPointVector(const std::string& name = ""): MSObject(name) {
         fPoints = new MSDataPointContainer();
      }

      //! Destructor
      virtual ~MSDataPointVector() { 
         if (fPoints) for (auto& i : *fPoints) delete i;
         delete fPoints;
      }

      //! Add new point (the function takes ownership of the object)
      void AddDataPoint(MSDataPoint* dataPoint) {
         if (fPoints) fPoints->push_back(dataPoint);
      }
      //! Get pointer to a point
      MSDataPoint* GetDataPoint(unsigned int index) const {
         if (index < fPoints->size()) {
            return fPoints->at(index);
         } else {
            std::cerr << "MSDataPointVector::GetDataPoint: index our of range\n";
            exit(1);
         }
      }

      //! Get observable value for a specific data point
      double GetDataPoint(int index, const std::string& field) const {
         return fPoints ? fPoints->at(index)->Get(field) : 0;
      }

      //! Get number of data points
      unsigned int  GetNDataPoints() const {
         return fPoints ? fPoints->size() : 0;
      }

      //! Clear data set
      void Clear() { if (fPoints) fPoints->clear(); }

      //! Print data set content
      void Print() const {
         if (fPoints) for (const auto& i : *fPoints) i->Print();
      }

   private:
      //! Pointer to the vector of data points
      MSDataPointContainer* fPoints;
};

} // namespace mst

#endif // MST_MSDataPointVector_H
