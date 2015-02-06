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

