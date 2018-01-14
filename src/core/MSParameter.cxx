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

// c/c++ libs
#include <iostream>
#include <sstream>

// m-stats libs
#include"MSParameter.h"

namespace mst {

MSParameter::MSParameter(const std::string& name): MSObject(name)
{
   ResetFitResult();
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
   fFitBestValue     = 0.0;
   fFitBestValueErr  = 0.0;
   fFitLowerLimit    = 0.0;
   fFitUpperLimit    = 0.0;
}

} // namespace mst
