#include "MSConfig.h"
#include<iomanip>
#include<iostream>
#include<cstdlib>

namespace mst {

MSConfig::MSConfig(const std::string& name): MSDataObject(name), fConfMap(0)
{
   fConfMap = new MSConfigMap();
}

MSConfig::~MSConfig()
{
   if (fConfMap) {
      for (MSConfigMap::iterator it = fConfMap->begin();
           it != fConfMap->end(); ++it)
         delete it->second;
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
   for (MSConfigMap::const_iterator it = fConfMap->begin();
         it != fConfMap->end(); ++it) {
      for (MSConfigSubSetMap::const_iterator itSubSet = it->second->begin();
            itSubSet != it->second->end(); ++itSubSet) {
         std::cout << it->first
                   << "."  << itSubSet->first << " = "
                   << itSubSet->second << std::endl;
      }
   }
}

} // namespace mst
