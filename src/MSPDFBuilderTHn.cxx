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


#include "TROOT.h"
#include "TFile.h"

#include "MSPDFBuilderTHn.h"

// c++ lins
#include <iostream>


namespace mst {

MSPDFBuilderTHn::MSPDFBuilderTHn(const std::string& name):
    fHistMap(0), fTmpPDF(0)
{
  fHistMap = new HistMap;
}

MSPDFBuilderTHn::~MSPDFBuilderTHn()
{
  for (auto im : *fHistMap) delete im.second;
  fHistMap->clear();
  delete fHistMap;

  if (fTmpPDF) delete fTmpPDF;
}

void MSPDFBuilderTHn::LoadHist(const std::string& fileName, 
      const std::string& histName, const std::string& newHistName) {

   // Check if an hist with the same name was already loaded
  if (fHistMap->find(newHistName) != fHistMap->end()) {
    std::cerr << "error: PDF already loaded\n";
    return;
  }

  // Get pointer of the file 
  TFile inputFile(fileName.c_str(), "READ");
  if(inputFile.IsOpen() == kFALSE) {
    std::cerr << "error: input file not found\n";
    return;
  }

  // load new hist checking for the type
  THn* hist = dynamic_cast<THn*>(inputFile.Get(histName.c_str()));
  if (!hist) {
    std::cerr << "error: PDF not found in the file\n";
  } else {
     hist->SetName(newHistName.c_str());
     fHistMap->insert( HistPair( newHistName, hist));
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

   THn* realization = new THnI (Form("mc_%i",gRandom->GetSeed()),
                                Form("mc_%i",gRandom->GetSeed()),
                                dim, bin, min, max);

   for (int i = 0 ; i < dim; i++) 
      realization->GetAxis(i)->SetRange(first[i],last[i]);

   // Fill realizations using n-dimensional method
   // Note that GetRandom works on the full range of the histograms and cannot
   // be limited to the user range. That's not a problem since the sampling MUST
   // be done on all histogram range (including over- and under-shot) in order
   // to preserve the the actual rate
   Double_t rndPoint[dim];
   for ( int j = 0; j < ctsNum; j++ ) {
      fTmpPDF->GetRandom(rndPoint);
      realization->Fill(rndPoint);
   }

   return realization;
}

} // namespace mst
