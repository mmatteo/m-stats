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
 * \class mst::MSConfig
 *
 * \brief 
 * General class to store configuation parameters.
 *
 * \details 
 * The parameter is stored in a map<std::string, double> (MSConfigSubSetMap)
 * which provides a flexible key-based access where the key is the name of the
 * parameter. A different map is created for each data set and it is also
 * stored into a map map<std::string, MSConfigSubSetMap*> (MSConfigMap) where
 * the key is the subset name.
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSConfig_H
#define MST_MSConfig_H

// c/c++ libs
#include<map>
#include<string>

// m-stats libs
#include "MSObject.h"

namespace mst {

class MSConfig : public MSObject 
{
   public:
      //! Constructor
      MSConfig(const std::string& name = "");
      //! Destructor
      virtual ~MSConfig();

      //! Basic parameterName-value pair
      using MSConfigSubSetPair = std::pair<std::string, double>;
      //! Map of parameterName-value pairs
      using MSConfigSubSetMap = std::map<std::string, double>;
      //! Pair of subSetName-MSConfigSubSetMap
      using MSConfigPair = std::pair<std::string, MSConfigSubSetMap*>;
      //! Map to store subSetName-MSConfigSubSetMap pairs
      using MSConfigMap  = std::map <std::string, MSConfigSubSetMap*>;

      //! Set Parameter value in the specify subSet
      bool SetPar(const std::string& subSetName, const std::string& parName, double val);
      //! Get Parameter value in the specify subSet
      double GetPar(const std::string& subSetName, const std::string& parName) const;

      //! Get the number of subsets
      unsigned int  GetNSubSets() const {return fConfMap->size();}
      //! Get the name of a subset through its index
      std::string  GetSubSetName(unsigned int index) const;

      //! Erase subset
      void  EraseSubSet(const std::string& subSetName) {
         fConfMap->erase(subSetName);
      }

      //! Erase a parameter in a subset
      void  ErasePar(const std::string& subSetName, const std::string& parName) {
         MSConfigMap::iterator it = fConfMap->find(subSetName);
         if (it != fConfMap->end()) it->second->erase(parName);
      }

      //! Print summary
      void PrintSummary();

   private:
      //! Pointer the the map
      MSConfigMap* fConfMap {nullptr};
};

} // namespace mst

#endif // MST_MSConfig_H
