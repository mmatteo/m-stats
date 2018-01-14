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
#include <TH1.h>

// m-stats libs
#include "MSTHnHandler.h"

namespace mst {

THn* MSTHnHandler::BuildHist(const std::string& fileName, 
                             const std::string& histName,
                             const std::string& newHistName) {

   // Get pointer of the file 
   TFile inputFile(fileName.c_str(), "READ");
   if(inputFile.IsOpen() == kFALSE) {
      std::cerr << "error: input file not found\n";
      exit(1);
   }

   // load new hist checking for the type
   THn* outHist = nullptr;
   TObject* hist = inputFile.Get(histName.c_str());
   if (!hist) {
      std::cerr << "error: PDF " << newHistName << " not found in the file\n";
      exit(1);
   } else {
      // Create local THn
      if (dynamic_cast<THnBase*>(hist)) {
         outHist = THn::CreateHn(newHistName.c_str(), newHistName.c_str(), 
                   dynamic_cast<THnBase*>(hist));
      } else if (dynamic_cast<TH1*>(hist)) {
         outHist = THn::CreateHn(newHistName.c_str(), newHistName.c_str(), 
                   dynamic_cast<TH1*>(hist));
      } else {
         std::cerr << "error: PDF " << newHistName << " is not of type THnBase or TH1\n";
         exit(1);
      }

      delete hist;
   }
   inputFile.Close();

   // project hist
   if (fProjectID.size()) {
      int IDs[fProjectID.size()] = {0};
      int counter = 0;
      for (auto i : fProjectID) IDs[counter++] = i;

      THn* tmp = outHist->Projection(fProjectID.size(), IDs);
      delete outHist;
      outHist = tmp;
   }
   // rebin
   if (fAxis.size()) {
      // a new hist is substitute to the original one because the method THn 
      // doesn't not provide a method that modyfy the object itself
      int ngroup[fAxis.size()] = {0};
      for (int i = 0; i < fAxis.size(); i++) ngroup[i] = fAxis[i].fNgroup;

      THn* tmp = outHist->Rebin(ngroup);
      delete outHist;
      outHist = tmp;
   }

   // set range user
   for (int i = 0; i < fAxis.size(); i++) {
      if (fAxis[i].fSetRange) 
         outHist->GetAxis(i)->SetRangeUser(fAxis[i].fMin,fAxis[i].fMax);
   }

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
