/*
 * Class to store all info concerning a data point.
 * All observables measured are stored into a std::map.
 */

#ifndef MST_MSDataPoint_H
#define MST_MSDataPoint_H

#include "MSDataObject.h"

// c++ libs
#include <string>
#include <map>
#include <vector>

namespace mst {

class MSDataPoint : public MSDataObject
{
   public:
      //! Constructor
      MSDataPoint(const std::string& name = "");
      //! Destructor
      ~MSDataPoint();

      //! Pair containing the observable name and value
      typedef std::pair<std::string, double> MSDataPointPair;
      //! Map storing the observables
      typedef std::map <std::string, double> MSDataPointMap;

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
      MSDataPointMap* fMap;
};

//! Type defining a vector of dataPoint. Used to create a dataset
typedef std::vector<MSDataPoint*> MSDataPointVector;

} // namespace mst

#endif // MST_MSDataPoint_H
