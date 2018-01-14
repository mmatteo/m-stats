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
 * \class mst::MSDataPoint
 *
 * \brief 
 * Data point info
 *
 * \details 
 * Class storing multiple info concerning a data point. All observables measured 
 * are stored into a std::map. This container is designed for analysis not based
 * on histograms, typically unbinned fits.
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSDataPoint_H
#define MST_MSDataPoint_H

// c/c++ libs
#include <map>
#include <string>
#include <vector>

// m-stats libs
#include "MSObject.h"

namespace mst {

class MSDataPoint : public MSObject
{
   public:
      //! Constructor
      MSDataPoint(const std::string& name = "");
      //! Destructor
      virtual ~MSDataPoint();

      //! Pair containing the observable name and value
      using MSDataPointPair = std::pair<std::string, double>;
      //! Map storing the observables
      using  MSDataPointMap = std::map <std::string, double>;

      //! Clear the map (interface to std::map::clear)
      void Clear() { if(fMap) fMap->clear(); }

      //! Erase the map (interface to std::map::erase)
      double Erase(const std::string& key) { return fMap ? fMap->erase(key) : 0; }

      //! Get the value of an observable
      double Get(const std::string& key) const {
         return fMap && fMap->count(key) ? fMap->at(key) : 0;
      }
      //! Insert new observable (interface to std::map::insert)
      void Insert(const std::string& key, double value) {
         if (fMap) fMap->insert(MSDataPointPair(key,value));
      }
      //! Size of the map (interface to std::map::size)
      double Size() const { return fMap ? fMap->size() : 0; }

      //! Print the map conten
      void Print() const;

   private:
      //! Pointer to the map
      MSDataPointMap* fMap {nullptr};
};

} // namespace mst

#endif // MST_MSDataPoint_H
