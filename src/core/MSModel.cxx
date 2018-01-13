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
#include <algorithm>

// m-stats libs
#include "MSModel.h"

namespace mst {

MSParameterMap* MSModel::fParameters = 0;

MSModel::MSModel(const std::string& name) : MSObject(name)
{
   if (!fParameters) fParameters = new MSParameterMap();
   fParNameList = new std::vector<std::string>;
}

MSModel::~MSModel()
{
   if (fParameters != nullptr) {
      for (auto& it : *fParameters) delete it.second;
      fParameters->clear();
      delete fParameters;
   }

   delete fParNameList;
}

void MSModel::AddParameter(MSParameter* par)
{
   if(!par){
      std::cerr << "MSModel::AddParameter: null MSParameter pointer"
                << std::endl;
      exit(1);
   }

   // check if the parameter was already registered by this instance of the
   // model
   if (std::find(fParNameList->begin(),fParNameList->end(),par->GetName()) 
       != fParNameList->end()) {
         std::cerr
            << "MSModel::AddParameter: parameter  \""
            << par->GetName()
            << "\" already registered in the model" << std::endl;
         delete par;
         return;
   } else {
      fParNameList->push_back(par->GetName());

      // Check if the parmater is already in the map. This is the case for
      // global parameters that are registered by the first instance of the
      // model. Before searching build and apply the global name
      par->SetName( GetGlobalName(par->GetName(), par->IsGlobal()));

      if(fParameters->find(par->GetName()) == fParameters->end()) {
         fParameters->insert(MSParameterPair(par->GetName(),par));
      } else {
         delete par;
      }
   }
   return;
}

MSParameterMap::iterator MSModel::GetParameterIterator(const std::string& localName) const
{
   // First search for a global parmater
   MSParameterMap::iterator it = fParameters->find(GetGlobalName(localName, true));
   if (it != fParameters->end()) return it;
   // then for a local parameter
   it = fParameters->find(GetGlobalName(localName, false));
   if (it != fParameters->end()) return it;
   else {
      std::cerr << "MSModel::GetParameterIndex: parameter \""
                << localName << "\" not found" << std::endl;
      exit(1);
   }
}

MSParameter* MSModel::GetParameter(const std::string& localName)  const
{
   MSParameterMap::const_iterator it = GetParameterIterator(localName);
   if (it == fParameters->end()) {
      std::cerr << "MSModel::GetParameter: parameter \""
                << localName << "\" not found" << std::endl;
      exit(1);
   }
   return it->second;
}

unsigned int MSModel::GetParameterIndex(const std::string& localName) const
{
   MSParameterMap::const_iterator it = GetParameterIterator(localName);
   MSParameterMap::const_iterator it0 = fParameters->begin();
   return std::distance(it0, it);
}

} // namespace mst

