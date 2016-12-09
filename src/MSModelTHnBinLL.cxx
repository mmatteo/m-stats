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


#include "MSModelTHnBinLL.h"
#include "MSMath.h"

namespace mst {


MSModelTHnBinLL::MSModelTHnBinLL(const std::string& name) : 
   MSModelTHn(name), fPDFBuilder(0)
{
}

MSModelTHnBinLL::~MSModelTHnBinLL()
{
   delete fPDFBuilder;
}


double MSModelTHnBinLL::NLogLikelihood(double* par)
{
   fPDFBuilder->ResetPDF();

   // retrieve parameters from Minuit and compute the total exposure
   for (int i =0; i < fParNameList->size(); i++) {
      const double par_cts = GetMinuitParameter(par, fParNameList->at(i));
      fPDFBuilder->AddHistToPDF(fParNameList->at(i),  par_cts);
   }

   const THn* pdf = fPDFBuilder->GetPDF("tmpPDF");
   if (pdf == 0) {
      std::cerr << "NLogLikelihood >> error: PDFBuilder returned unknown object type\n";
      exit(1);
   }
   if (fDataHist == 0) {
      std::cerr << "NLogLikelihood >> error: DataHist of unknown object type\n";
      exit(1);
   }

   double nLogLikelihood = 0.0;
   // loop over dimensions
   auto it = fDataHist->CreateIter(kTRUE);
   Long64_t i = 0;
   while ((i = it->Next()) >= 0)
         nLogLikelihood += MSMath::LogPoisson(fDataHist->GetBinContent(i), 
                                              fExposure*pdf->GetBinContent(i));

   delete pdf;
   delete it;
   return -2.0*nLogLikelihood;
}

} // namespace mst
