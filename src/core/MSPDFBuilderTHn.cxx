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

// c/c++ libs
#include <iostream>

// ROOT libgs
#include <TFile.h>
#include <TH1.h>
#include <THnBase.h>
#include <TROOT.h>

// m-stats libs
#include "MSPDFBuilderTHn.h"

namespace mst {

MSPDFBuilderTHn::MSPDFBuilderTHn(const std::string& name): MSObject(name)
{
  fHistMap = new HistMap;
}

MSPDFBuilderTHn::~MSPDFBuilderTHn()
{
   if (fHistMap != nullptr) {
      for (auto im : *fHistMap) delete im.second;
      fHistMap->clear();
      delete fHistMap;
   }

   delete fTmpPDF;
   delete fRnd;
}

void MSPDFBuilderTHn::LoadHist(const std::string& fileName, 
      const std::string& histName, const std::string& newHistName,
      const Int_t  ndim_pr, const Int_t* dim_pr) {

   // Check if an hist with the same name was already loaded
  if (fHistMap->find(newHistName) != fHistMap->end()) {
    std::cerr << "error: PDF already loaded\n";
    return;
  }

  // Get pointer of the file 
  TFile inputFile(fileName.c_str(), "READ");
  if(inputFile.IsOpen() == kFALSE) {
    std::cerr << "error: input file not found\n";
    exit(1);
  }

  // load new hist checking for the type
  TObject* hist = inputFile.Get(histName.c_str());
  if (!hist) {
    std::cerr << "error: PDF " << newHistName
              << " not found in the file\n";
    exit(1);
  } else {
     THn* tmp = nullptr;
     // Create local THn
     if (dynamic_cast<THnBase*>(hist)) {
        tmp = THn::CreateHn(newHistName.c_str(), newHistName.c_str(), 
              dynamic_cast<THnBase*>(hist));
     } else if (dynamic_cast<TH1*>(hist)) {
        tmp = THn::CreateHn(newHistName.c_str(), newHistName.c_str(), 
              dynamic_cast<TH1*>(hist));
     } else {
        std::cerr << "error: PDF " << newHistName
                  << " is not of type THnBase or TH1\n";
       exit(1);
     }

     if (dim_pr == nullptr) {
        tmp->SetName(newHistName.c_str());
        tmp->SetTitle(newHistName.c_str());
        fHistMap->insert( HistPair( newHistName, tmp));
     } else {
        THn* tmp_pr = tmp->Projection(ndim_pr, dim_pr);
        delete tmp;
        tmp_pr->SetName(newHistName.c_str());
        tmp_pr->SetTitle(newHistName.c_str());
        fHistMap->insert( HistPair( newHistName, tmp_pr));
     }
     delete hist;
  }
  

  inputFile.Close();
  return;
}


void MSPDFBuilderTHn::NormalizeHists(bool respectAxisUserRange) {
   // loop over all hist loaded
   for (auto im : *fHistMap) {
      // loop over dimensions
      auto it = im.second->CreateIter(respectAxisUserRange);
      Long64_t i = 0;
      double integral = 0;
      while ((i = it->Next()) >= 0) integral += im.second->GetBinContent(i);
      im.second->Scale(1./integral);
   }
}

void MSPDFBuilderTHn::SetRangeUser(double min, double max, int axis) {
   // loop over all hists
   for (auto im : *fHistMap) {
      if (im.second->GetAxis(axis) != nullptr)
         im.second->GetAxis(axis)->SetRangeUser(min,max);
   }
}

void MSPDFBuilderTHn::Rebin(Int_t* ngroup) {
   // a hist map is filled and then substitute to the original one
   // because the method THn doesn not provide a method that modyfy the object
   // itself
   auto newHistMap = new HistMap;
   for (auto im : *fHistMap) {
      THn* newPdf = im.second->Rebin(ngroup);
      newPdf->SetName(im.second->GetName());
      newPdf->SetTitle(im.second->GetTitle());
      newHistMap->insert( HistPair( newPdf->GetName(), newPdf));
   }

   // Clean up old map 
   for (auto im : *fHistMap) delete im.second;
   fHistMap->clear();
   delete fHistMap;

   // swap new map
  fHistMap = newHistMap;
}


void MSPDFBuilderTHn::AddHistToPDF(const std::string& histName, double scaling) {
   // find hist by name
   HistMap::iterator im = fHistMap->find(histName);
   if (im == fHistMap->end()) {
      std::cerr << "error: PDF not loaded\n";
      return;
   }
   // check if the temporary PDF exist already
   if (!fTmpPDF) {
      fTmpPDF = (THn*) im->second->Clone();
      fTmpPDF->SetName("privatePDF");
      fTmpPDF->SetTitle("privatePDF");
      ResetPDF();
   }

   // Add hist to pdf with scaling factor
   fTmpPDF->Add(im->second, scaling);
}

THn* MSPDFBuilderTHn::GetPDF (const std::string& objName) { 
   if (!fTmpPDF) return 0;
   THn* clone = (THn*) fTmpPDF->Clone();
   clone->SetName(objName.c_str());
   clone->SetTitle(objName.c_str());
   ResetPDF();
   return clone;
}


THn* MSPDFBuilderTHn::GetMCRealizaton(int ctsNum, bool addPoissonFluctuation) {
   // set internal random number generator if set
   TRandom* rndTmpCopy = nullptr;
   if (fRnd != nullptr)  {
      rndTmpCopy = gRandom;
      gRandom = fRnd;
   }

   // optinally add Poission fluctuatoins on the number of cts
   if (addPoissonFluctuation) {
      ctsNum = gRandom->Poisson(ctsNum);
   }

   // Build a THn<int> with the same axis of the PDF's
   const int dim = fTmpPDF->GetNdimensions();
   Int_t bin[dim], first[dim], last[dim];
   Double_t min[dim], max[dim];
   for (int i = 0 ; i < dim; i++) {
      bin[i]   = fTmpPDF->GetAxis(i)->GetNbins();
      first[i] = fTmpPDF->GetAxis(i)->GetFirst();
      last[i]  = fTmpPDF->GetAxis(i)->GetLast();
      min[i]   = fTmpPDF->GetAxis(i)->GetXmin();
      max[i]   = fTmpPDF->GetAxis(i)->GetXmax();
   }

   THn* realization = new THnI (Form("mc_seed_%u",gRandom->GetSeed()),
                                Form("mc_seed_%u",gRandom->GetSeed()),
                                dim, bin, min, max);

   for (int i = 0 ; i < dim; i++) 
      realization->GetAxis(i)->SetRange(first[i],last[i]);

   // Fill realizations using n-dimensional method
   // Note that GetRandom works on the full range of the histograms and cannot
   // be limited to the user range. That's not a problem since the sampling MUST
   // be done on all histogram range in order to preserve the the actual rate.
   // Note that over- and under-flow bins are not considered
   Double_t rndPoint[dim];
   for ( int j = 0; j < ctsNum; j++ ) {
      fTmpPDF->GetRandom(rndPoint, kFALSE);
      realization->Fill(rndPoint);
   }

   if (rndTmpCopy != nullptr) gRandom = rndTmpCopy;
   return realization;
}

} // namespace mst
