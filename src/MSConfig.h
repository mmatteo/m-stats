/*
 * General class to store configuation parameters.
 * The parameter is stored in a map<std::string, double> (MSConfigSubSetMap)
 * which provides a flexible key-based access where the key is the name of the
 * parameter. A different map is created for each data set and it is also
 * stored into a map map<std::string, MSConfigSubSetMap*> (MSConfigMap) where
 * the key is the subset name.
 */

#ifndef MST_MSConfig_H
#define MST_MSConfig_H

#include "MSDataObject.h"

// c++ libs
#include<string>
#include<map>

namespace mst {

class MSConfig : public MSDataObject 
{
   public:
      //! Constructor
      MSConfig(const std::string& name = "");
      //! Destructor
      ~MSConfig();

      //! Basic parameterName-value pair
      typedef std::pair<std::string, double> MSConfigSubSetPair;
      //! Map of parameterName-value pairs
      typedef std::map <std::string, double> MSConfigSubSetMap;
      //! Pair of subSetName-MSConfigSubSetMap
      typedef std::pair<std::string, MSConfigSubSetMap*> MSConfigPair;
      //! Map to store subSetName-MSConfigSubSetMap pairs
      typedef std::map <std::string, MSConfigSubSetMap*> MSConfigMap;

      //! Set Parameter value in the specify subSet
      bool   SetPar(const std::string& subSetName, const std::string& parName, double val);
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
      MSConfigMap* fConfMap;
};

} // namespace mst

#endif //MST_MSConfig_H
