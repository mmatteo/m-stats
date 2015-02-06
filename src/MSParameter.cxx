#include"MSParameter.h"

// c++ libs
#include <iostream>
#include <sstream>

namespace mst {

MSParameter::MSParameter(const std::string& name, double rangeMin, double rangeMax) :
   MSDataObject(name),
   fParameterType(kTypeUndefined), fVariableType(kVarUndefined),
   fFixed(false), fGlobal(false),
   fRangeMinSet(false), fRangeMaxSet(false), fRangeMin(0.0), fRangeMax(0.0),
   fFitStartValue(0.0), fFitStartValueSet(false), fFitStartStep(0.01), //Check default minuit value
   fFitBestValue(0.0), fFitBestValueErr(0.0), fFitLowerLimit(0.0), fFitUpperLimit(0.0)
{
   SetParameterType(MSParameter::kTypeUndefined);
   SetVariableType(MSParameter::kVarUndefined);
   //SetRange(rangeMin, rangeMax);
   ResetFitResult();
   SetFixed(false);
}

MSParameter::~MSParameter()
{
}

void MSParameter::PrintSummary() const
{
   std::ostringstream os;
   os << GetName();

   os << " [ ";
   if (IsPoi())  os << "kPoi";
   else if (IsNuisance())  os << "kNuisance";
   else if (IsInput())  os << "kInput";
   else  os << "kTypeUndefined";

   if(IsGlobal())  os << " : global";
   else  os << " : local";

   if(IsFixed())  os << " : fixed";

    os << " ]";

    os << " [ best value " << GetFitBestValue()
       << " +- " << GetFitBestValueErr()
       << " ] [ interval "  << GetFitLowerLimit()
       << " , "  << GetFitUpperLimit()
       << " ]";
   std::cout << os.str() << std::endl;
}

void MSParameter::ResetFitResult()
{
   fFitBestValue  = 0;
   fFitBestValueErr  = 0;
   fFitLowerLimit = 0;
   fFitUpperLimit = 0;
}

} // namespace mst
