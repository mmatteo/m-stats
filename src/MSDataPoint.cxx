#include "MSDataPoint.h"

// c++ libs
#include<iostream>
#include<iomanip>

namespace mst {

MSDataPoint::MSDataPoint(const std::string& name) : MSDataObject(name), fMap(0)
{
  fMap = new MSDataPointMap();
}

MSDataPoint::~MSDataPoint()
{
   if (fMap) {
      delete fMap;
      fMap = 0;
   }
}

void MSDataPoint::Print() const
{
   for (MSDataPointMap::const_iterator it = fMap->begin();
         it != fMap->end(); ++it) {
       std::cout << "  " << it->first << ": " << std::setw(8) << it->second;
   }
   std::cout << std::endl;

}

} // namespace mst


