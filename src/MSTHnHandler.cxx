// Copyright (C) 2016 Matteo Agostini <matteo.agostini@ph.tum.de>

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
//

// c/c++ libs
#include <iostream>

// ROOT libgs
#include <TFile.h>
#include <THnBase.h>

// m-stats libs
#include "MSTHnHandler.h"

namespace mst {

THnBase* MSTHnHandler::BuildHist(const std::string& fileName, 
                                 const std::string& histName,
                                 const std::string& newHistName) {

   // Get pointer of the file 
   TFile inputFile(fileName.c_str(), "READ");
   if (inputFile.IsOpen() == kFALSE) {
      std::cerr << "error: input file not found\n";
      return nullptr;
   }

   // load new hist checking for the type
   THnBase* inHist = nullptr;
   inputFile.GetObject(histName.c_str(),inHist);

   if (!inHist) {
      std::cerr << "error: PDF not found in the file\n";
      return nullptr;
   }

   THn* outHist =  THn::CreateHn(newHistName.c_str(), newHistName.c_str(), inHist);
   inputFile.Close();

   // project hist
   {
      const int ndim = m.size();
      int IDs[ndim];
      int counter = 0;
      for (auto i : m) IDs[counter++] = i.first;

      THn* tmp = tmp->Projection(ndim, IDs);
      delete outHist;
      outHist = tmp;
   }
   // rebin
   {
      // a new hist is substitute to the original one because the method THn 
      // doesn't not provide a method that modyfy the object itself
      const int ndim = m.size();
      int ngroup[ndim];
      for (auto i : m) ngroup[i.first] = i.second.ngroup;

      THn* tmp = outHist->Rebin(ngroup);
      delete outHist;
      outHist = tmp;
   }

   // set range user
   for (auto i : m) outHist->GetAxis(i.first)->SetRangeUser(i.second.min,i.second.max);


   // normalize 
   if (fNormalize) {
      auto it = outHist->CreateIter(fRespectUserRange);
      Long64_t i = 0;
      double integral = 0;
      while ((i = it->Next()) >= 0) integral += outHist->GetBinContent(i);
      outHist->Scale(1./integral);
   }

  outHist->SetTitle(newHistName.c_str());
  outHist->SetName(newHistName.c_str());
  return outHist;
}

} // namespace mst
