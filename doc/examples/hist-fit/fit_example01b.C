// Illustrative macro to create a data hist by sampling from MC and run a fit

#include <THn.h>
#include <m-stats/MSPDFBuilderTHn.h>
#include <m-stats/MSModelTHnBMLF.h>
#include <m-stats/MSMinimizer.h>
#include <m-stats/MSModelPulls.h>

#include "ParInfo.h"

using namespace mst;

MSMinimizer* basicSpectralFit () {
  
   TString pdfPath = "myPDFs.root";
   auto v = BuildParInfo(EConfig::kHist1);

   // Initialize the pdf builder and use it to generate a data set for the fit
   MSPDFBuilderTHn* pdfBuilder = new MSPDFBuilderTHn;

   // pdfBuilder settings
   Int_t nDim    = 2;           // number of dimensions considered
   Int_t vDim[]  = {0, 1};      // THn axis to be considered
   
   // Rebin     Energy  r3      User
   Int_t bin[]   = {1,   1};


   // Hist used for generating the PDF must first be registered
   for (auto&i:v) pdfBuilder->LoadHist(pdfPath.Data(), i.fPdfName, i.fName, nDim, vDim);
   // Set Seed for pseudo-dataset generation
   pdfBuilder->SetSeed(0);
   pdfBuilder->Rebin(bin);

   // Set range user        x0  x1      dimension 
   //pdfBuilder->SetRangeUser(100,1450,   0);
   pdfBuilder->SetRangeUser(19, 56,      0);
   pdfBuilder->SetRangeUser( 0, 27,      1);


   pdfBuilder->NormalizeHists(false);

   // Define exposure and build your pdf 
   double exposure = 1000 /*days*/ * 75.5/100.0 /* tons FV */;

   // Build a data set with a fixed number of cts
   double totalCounts = 0;
   for (auto&i:v) pdfBuilder->AddHistToPDF(i.fName, i.fTrueValue);
   for (auto&i:v) totalCounts += i.fTrueValue*exposure;
   // Set 'true' to include Poisson fluctuations
   THn* dataHist = pdfBuilder->GetMCRealizaton(totalCounts, false);


   // Initilize analysis model and parse to it the pointers
   // the name of the model is needed to retrieve from the fitter
   // the model parameters which are local (see below)
   MSModelTHnBMLF* mod = new MSModelTHnBMLF("myMod");
   mod->SetPDFBuilder(pdfBuilder);
   mod->SetDataSet(dataHist);
   mod->SetExposure(exposure);

   // initialize parameters
   for (auto&i:v) {
      auto par = new mst::MSParameter(i.fName);
      par->SetGlobal(i.fIsGlobal);
      par->SetFitStartValue(i.fFitStartValue);
      //par->SetFitStartStep(i.fFitStartStep);
      par->SetFitStartStep((i.fFitRangeMax -i.fFitRangeMin)/1000.);
      par->SetRange(i.fFitRangeMin, i.fFitRangeMax);
      mod->AddParameter(par);
   }

   // Initialize the minimizer
   MSMinimizer* fitter = new MSMinimizer();
   fitter->AddModel(mod);
   // Initialize minuit and set verbosity and errVal
   // NOTE that errVal is 1 for Chi2, 0.5 for NLL
   fitter->InitializeMinuit(1, 0.5);
   fitter->SetMinuitVerbosity(2);
   // Call Migrad with specific number of calls and error tolerance
   fitter->Minimize("SIMPLEX", true, 1e4,0.1);
   fitter->Minimize("MINIMIZE",false,1e8,0.1);

   if (fitter->GetMinuitStatus()) {
      std::cerr << "MSMinimizer: minuit return status=" << fitter->GetMinuitStatus()
            << ", indicating problems in the convergence of the fit\n"; 
   } else {
      // Print parameter summary
      fitter->PrintParSummary();
   }

   TCanvas* c1 = new TCanvas;
   c1->Divide(dataHist->GetNdimensions(),1);
   for (int d = 0; d < dataHist->GetNdimensions(); d++) {
      c1->cd(d+1);
      auto data_pr = dataHist->Projection(d);
      data_pr->SetLineColor(kBlack);
      data_pr->DrawClone();
      gPad->SetLogy();
      delete data_pr;
   }

   // Draw Sub components
   for (auto&i:v) {
      pdfBuilder->ResetPDF();
      mst::MSParameter*  par = mod->GetParameter(i.fName);
      pdfBuilder->AddHistToPDF(i.fName,  exposure*par->GetFitBestValue());
      auto pdf_tmp = pdfBuilder->GetPDF(i.fName);

      for (int d = 0; d < dataHist->GetNdimensions(); d++) {
         c1->cd(d+1);
         auto pdf_tmp_pr = pdf_tmp->Projection(d);
         pdf_tmp_pr->SetLineColor(i.fColor);
         pdf_tmp_pr->SetLineWidth(2);
         pdf_tmp_pr->DrawClone("hist same");
         delete pdf_tmp_pr;
      }
      delete pdf_tmp;
   }

   // Draw total fit
   pdfBuilder->ResetPDF();
   for (auto&i:v) {
      mst::MSParameter*  par = mod->GetParameter(i.fName);
      pdfBuilder->AddHistToPDF(i.fName,  exposure*par->GetFitBestValue());
   }

  auto tot = pdfBuilder->GetPDF("tot");
   for (int d = 0; d < dataHist->GetNdimensions(); d++) {
      c1->cd(d+1);
      auto tot_pr = tot->Projection(d);
      tot_pr->SetLineColor(kGreen);
      tot_pr->SetLineWidth(2);
      tot_pr->DrawClone("hist same");
      delete tot_pr;
   }
   delete tot;

   for (int d = 0; d < dataHist->GetNdimensions(); d++) {
      c1->cd(d+1);
      auto data_pr = dataHist->Projection(d);
      TString dataName = data_pr->GetName();
      dataName += Form("%i", d);
      data_pr->SetName(dataName);
      data_pr->SetMarkerColor(kBlack);
      data_pr->SetLineColor(kBlack);
      data_pr->SetMarkerStyle(20);
      data_pr->SetMarkerSize(0.7);
      data_pr->SetLineColor(24);
      data_pr->DrawClone("PE same");
      gPad->SetLogy();
      delete data_pr;
   }

   // Saving fit output to file

   int    minuitStats = fitter->GetMinuitStatus();
   auto   parList     = fitter->GetParameterMap();
   double minNLL      = fitter->GetMinNLL();

   TFile * fitOutput = new TFile("test.root", "update");
   TTree * t;                                   // create tree pointer
   if ( !(TTree*)fitOutput->Get("t") ) {        // if tree is not present in the file
     t = new TTree("t", "fit results");         // create a new one
     t->Branch("minuitStats", &minuitStats);    // and set branches
     t->Branch("minNLL"     , &minNLL);  
     int counter = 0;
     for (auto it = parList->begin(); it != parList->end(); it++) {
       t->Branch(it->second->GetName().c_str(), 
                 &v.at(counter).fBestFitValue );
       t->Branch(Form("%sErr", it->second->GetName().c_str()), 
                 &v.at(counter).fFitError );
       counter++;
     }
   } 
   else {                                               // else if tree is altready there
     t = (TTree*)fitOutput->Get("t");;                  // just set the right branch address
     t->SetBranchAddress("minuitStats", &minuitStats);
     t->SetBranchAddress("minNLL"     , &minNLL);  
     int counter = 0;
     for (auto it = parList->begin(); it != parList->end(); it++) {
       t->SetBranchAddress(it->second->GetName().c_str(), 
                           &(v.at(counter).fBestFitValue) );
       t->SetBranchAddress(Form("%sErr", it->second->GetName().c_str()), 
                 &v.at(counter).fFitError );
       counter++;
     }
   } 

   for (auto &p : v) {                                  // retrieve parameters and errors value
      MSParameter * par = mod->GetParameter(p.fName);
      p.fBestFitValue = par->GetFitBestValue();
      p.fFitError     = par->GetFitBestValueErr();
   } 
    
   t->Fill();                                           // fill the tree
   t->Write();
   fitOutput->Close(); 

   return fitter;
}
