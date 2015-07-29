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
 * \class mst::MSDataSet
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


#ifndef MST_MSDataSet_H
#define MST_MSDataSet_H

#include "MSDataObject.h"
#include "MSDataPoint.h"

// c++ libs
#include <string>
#include <iostream>
#include <stdlib.h>

namespace mst {

class MSDataPoint;

class MSDataSet : public MSDataObject
{
   public:
      //! Constructor
      MSDataSet(const std::string& name = "");
      //! Destructor
      virtual ~MSDataSet();

      //! Add new point (the function takes ownership of the object)
      void AddDataPoint(MSDataPoint* dataPoint) {
         if (fVector) fVector->push_back(dataPoint);
      }
      //! Get pointer to a point
      MSDataPoint* GetDataPoint(unsigned int index) const {
         if (index < fVector->size()) return fVector->at(index);
         else {
            std::cerr << "MSDataSet::GetDataPoint: index our of range\n";
            exit(1);
         }
      }

      //! Get observable value for a specific data point
      double GetDataPoint(int index, const std::string& field) const {
         return fVector ? fVector->at(index)->Get(field) : 0;
      }

      //! Get number of data points
      unsigned int  GetNDataPoints() const {
         return fVector ? fVector->size() : 0;
      }

      //! Clear data set
      void Clear() { if (fVector) fVector->clear(); }

      //! Print data set content
      void Print() const;

   private:
      //! Pointer to the vector of data points
      MSDataPointVector* fVector;
};

} // namespace mst

#endif // MST_MSDataSet_H
