#include"MSDataSet.h"

namespace mst {

MSDataSet::MSDataSet(const std::string& name) : MSDataObject(name), fVector(0)
{
   fVector = new MSDataPointVector();
}

MSDataSet::~MSDataSet()
{
   if (fVector) {
      for (size_t i = 0; i < fVector->size(); i++)
         delete fVector->at(i);
      delete fVector;
      fVector = 0;
   }
}

void MSDataSet::Print() const
{
   if (fVector) {
      for (unsigned int i =0; i < fVector->size(); i++) {
         fVector->at(i)->Print();
      }
   }
}

} // namespace mst

