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

// c/c++ libs
#include <cstdlib>
#include <iomanip>
#include <iostream>

// m-stats libs
#include "MSConfig.h"

namespace mst {

MSConfig::MSConfig(const std::string& name): MSObject(name)
{
   fConfMap = new MSConfigMap();
}

MSConfig::~MSConfig()
{
   if (fConfMap != nullptr) {
      for (auto& it : *fConfMap) delete it.second;
      fConfMap->clear();
      delete fConfMap;
   }
}

bool MSConfig::SetPar (const std::string& subSetName,
                       const std::string& parName,
                       double val)
{
   MSConfigMap::iterator it = fConfMap->find(subSetName);
   if (it == fConfMap->end()) {
      MSConfigSubSetMap* tmpMap = new MSConfigSubSetMap();
      // insert returns a pair with
      // first  = pointer to obj
      // second = bool  wasKeyPresent
      it = (fConfMap->insert(MSConfigPair(subSetName, tmpMap)).first);
   }

   MSConfigSubSetMap::iterator itSubSet = it->second->find(parName);
   if (itSubSet == it->second->end()) {
      it->second->insert(MSConfigSubSetPair(parName, val));
      return 0;
   } else {
      it->second->at(parName) = val;
      return 1;
   }

}

double MSConfig::GetPar (const std::string& subSetName,
                         const std::string& parName) const
{
   MSConfigMap::iterator it = fConfMap->find(subSetName);
   if(it != fConfMap->end()) {
      MSConfigSubSetMap::iterator itSubSet = it->second->find(parName);
      if (itSubSet != it->second->end()) return itSubSet->second;
   }

   std::cerr << "error: parameter <"
             << subSetName << "." << parName
             << "> not found in the configuration map\n";
   std::exit(1);
}

std::string MSConfig::GetSubSetName(unsigned int index) const
{
   if (index >= GetNSubSets())  return 0;

   MSConfigMap::iterator it = fConfMap->begin();
   for (unsigned int i = 0; i < index; i++, ++it)
      if (it == fConfMap->end()) {
         std::cerr << "error: subset corresponding to index "
                   << index
                   << " not found in the configuration map\n";
         std::exit(1);
      }

   return it->first;
}

void MSConfig::PrintSummary()
{
   for (const auto& it : *fConfMap) 
      for (const auto& itSubSet : *(it.second)) 
         std::cout << it.first
                   << "."  << itSubSet.first << " = "
                   << itSubSet.second << std::endl;
}

} // namespace mst
