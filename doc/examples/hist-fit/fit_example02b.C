// Illustrative macro to create a data hist by sampling from MC and run a fit

#include <THn.h>
#include "ParInfo.h"
#include <m-stats/MSPDFBuilderTHn.h>
#include <m-stats/MSModelTHnBMLF.h>
#include <m-stats/MSMinimizer.h>
#include <m-stats/MSModelPulls.h>


using namespace mst;


MSMinimizer* basicC112SpectralFit () {

   TString pdfPath = "myPDFs.root";
   // load parameters
   auto v1 = BuildParInfo(EConfig::kHist1);
   auto v2 = BuildParInfo(EConfig::kHist2);

   // Initialize the pdf builder and use it to generate a data set for the fit
   MSPDFBuilderTHn* pdfBuilder1 = new MSPDFBuilderTHn;
   MSPDFBuilderTHn* pdfBuilder2 = new MSPDFBuilderTHn;
   
   // pdfBuilder settings
   Int_t nDim    = 3;           // number of dimensions considered
   Int_t vDim[]  = {0, 1, 2};      // THn axis to be considered
   // Rebin     Energy  r3      User
   Int_t bin[]   = {1,   1, 1};


   // Hist used for generating the PDF must first be registered
   for (auto&i:v1) pdfBuilder1->LoadHist(pdfPath.Data(), i.fPdfName, i.fName, nDim, vDim);
   for (auto&i:v2) pdfBuilder2->LoadHist(pdfPath.Data(), i.fPdfName, i.fName, nDim, vDim);

   // original binning is 1 kev
   pdfBuilder1->SetSeed(0);
   pdfBuilder2->SetSeed(0);
   pdfBuilder1->NormalizeHists(false);
   pdfBuilder2->NormalizeHists(false);
   pdfBuilder1->Rebin(bin);
   pdfBuilder2->Rebin(bin);
   pdfBuilder1->SetRangeUser(19, 56 ,0);
   //pdfBuilder1->SetRangeUser(4,6,1);
   pdfBuilder2->SetRangeUser(19, 56 ,0);
   //pdfBuilder2->SetRangeUser(4,6,1);

   // Define exposure and build your pdf 
   double exposure = 1000 /*days*/ * 75.5/100.0 /* tons FV */ ;
   const double tfcSurvProb = .5;
   const double exposure1 = exposure * tfcSurvProb;
   const double exposure2 = exposure * (1.0 - tfcSurvProb);

   // build 1 data set
   double totalCounts1 = 0;
   for (auto&i:v1) pdfBuilder1->AddHistToPDF(i.fName, i.fTrueValue);
   for (auto&i:v1) totalCounts1 += i.fTrueValue * exposure1;
   THn* dataHist1 = pdfBuilder1->GetMCRealizaton(totalCounts1);

   // build 2 data set
   double totalCounts2 = 0;
   for (auto&i:v2) pdfBuilder2->AddHistToPDF(i.fName, i.fTrueValue);
   for (auto&i:v2) totalCounts2 += i.fTrueValue * exposure2;
   THn* dataHist2 = pdfBuilder2->GetMCRealizaton(totalCounts2);

   // Initilize analysis model and parse to it the pointers
   // the name of the model is needed to retrieve from the fitter
   // the model parameters which are local (see below)
   MSModelTHnBMLF* mod1 = new MSModelTHnBMLF("myMod1");
   mod1->SetPDFBuilder(pdfBuilder1);
   mod1->SetDataSet(dataHist1);
   mod1->SetExposure(exposure1);
   // initialize parameters
   for (auto&i:v1) {
      auto par = new mst::MSParameter(i.fName);
      par->SetGlobal(i.fIsGlobal);
      par->SetFitStartValue(i.fTrueValue);
      //par->SetFitStartStep(i.fFitStartStep);
      par->SetFitStartStep((i.fFitRangeMax -i.fFitRangeMin)/1000.);
      par->SetRange(i.fFitRangeMin, i.fFitRangeMax);
      mod1->AddParameter(par);
   }

   MSModelTHnBMLF* mod2 = new MSModelTHnBMLF("myMod2");
   mod2->SetPDFBuilder(pdfBuilder2);
   mod2->SetDataSet(dataHist2);
   mod2->SetExposure(exposure2);
   // initialize parameters
   for (auto&i:v2) {
      auto par = new mst::MSParameter(i.fName);
      par->SetGlobal(i.fIsGlobal);
      par->SetFitStartValue(i.fTrueValue);
      par->SetFitStartStep((i.fFitRangeMax -i.fFitRangeMin)/1000.);
      par->SetRange(i.fFitRangeMin, i.fFitRangeMax);
      mod2->AddParameter(par);
   }

   MSModelPullGaus* pull1 = new MSModelPullGaus("gaussianPull");
   pull1->SetPullPar("Bi210");
   pull1->SetGaussPar(20,2);
   pull1->InitializeParameters();

   // Initialize the minimizer
   MSMinimizer* fitter = new MSMinimizer();
   fitter->AddModel(mod1);
   fitter->AddModel(mod2);
   fitter->AddModel(pull1);
   // Initialize minuit and set verbosity and errVal
   fitter->InitializeMinuit(0,0.5);
   // Call Migrad with specific number of calls and error tolerance
   fitter->Minimize("SIMPLEX", true, 1e4,0.1);
   fitter->Minimize("MINIMIZE",false,1e8,1e-8);

   if (fitter->GetMinuitStatus()) {
      std::cerr << "MSMinimizer: minuit return status=" << fitter->GetMinuitStatus()
            << ", indicating problems in the convergence of the fit\n"; 
   } else {
      // Print parameter summary
      fitter->PrintParSummary();
   }

   // Plotting part
   //
   TCanvas* c1 = new TCanvas;
   c1->Divide(dataHist1->GetNdimensions(),2);

   // Function to plot data, model and components
   auto Plot = [c1] (
         std::vector<ParInfo>& v,
         MSPDFBuilderTHn* pdfBuilder, 
         double exposure,
         THn* dataHist, 
         MSModel* mod, 
         int line) {

      for (int d = 0; d < dataHist->GetNdimensions(); d++) {
         c1->cd(d+1+line*dataHist->GetNdimensions());
         auto data_pr = dataHist->Projection(d);
         data_pr->SetLineColor(kBlack);
         data_pr->SetMinimum(0.5);
         data_pr->DrawClone();
         gPad->SetLogy();
         delete data_pr;
      }

      // Draw component
      for (auto&i:v) {
         pdfBuilder->ResetPDF();
         mst::MSParameter*  par = mod->GetParameter(i.fName);
         pdfBuilder->AddHistToPDF(i.fName,  exposure*par->GetFitBestValue());
         auto pdf_tmp = pdfBuilder->GetPDF(i.fName);

         for (int d = 0; d < dataHist->GetNdimensions(); d++) {
            c1->cd(d+1+line*dataHist->GetNdimensions());
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
         c1->cd(d+1+line*dataHist->GetNdimensions());
         auto tot_pr = tot->Projection(d);
         tot_pr->SetLineColor(kGreen);
         tot_pr->SetLineWidth(2);
         tot_pr->DrawClone("hist same");
         delete tot_pr;
      }
      delete tot;

      for (int d = 0; d < dataHist->GetNdimensions(); d++) {
         c1->cd(d+1+line*dataHist->GetNdimensions());
         auto data_pr = dataHist->Projection(d);
         data_pr->SetMarkerColor(kBlack);
         data_pr->SetLineColor(kBlack);
         data_pr->SetMarkerStyle(20);
         data_pr->SetMarkerSize(0.7);
         data_pr->SetLineColor(24);
         data_pr->DrawClone("PE same");
         gPad->SetLogy();
         delete data_pr;
      }
   };

   Plot(v1, pdfBuilder1, exposure1, dataHist1, mod1, 0);
   Plot(v2, pdfBuilder2, exposure2, dataHist2, mod2, 1);

   // Saving fit output to file

   int    minuitStats = fitter->GetMinuitStatus();
   double minNLL      = fitter->GetMinNLL();
   auto   parList     = fitter->GetParameterMap();
   double parBestFit[parList->size()];
   double parBestFitErr[parList->size()];

   TFile * fitOutput = new TFile("test2.root", "update");
   TTree * t;                                   // create tree pointer
   if ( !(TTree*)fitOutput->Get("t") ) {        // if tree is not present in the file
     t = new TTree("t", "fit results");         // create a new one
     t->Branch("minuitStats", &minuitStats);    // and set branches
     t->Branch("minNLL"     , &minNLL);  
     int counter = 0;
     for (auto it = parList->begin(); it != parList->end(); it++) {
       t->Branch(it->second->GetName().c_str(), 
                 &parBestFit[counter] );
       t->Branch(Form("%sErr", it->second->GetName().c_str()), 
                 &parBestFitErr[counter] );
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
                           &parBestFit[counter]) ;
       t->SetBranchAddress(Form("%sErr", it->second->GetName().c_str()), 
                           &parBestFitErr[counter]) ;
       counter++;
     }
   } 
  
   int counter = 0;
   for (auto pIt = parList->begin(); pIt != parList->end(); pIt++) { // retrieve parameters and errors value
      parBestFit[counter]    = pIt->second->GetFitBestValue();
      parBestFitErr[counter] = pIt->second->GetFitBestValueErr();
      counter++;
   } 
    
   t->Fill();                                           // fill the tree
   t->Write();
   fitOutput->Close(); 

   TFile * dataSetFile = new TFile("dataset-file.root", "recreate");
   dataHist1->Write("dataHist1");
   dataHist2->Write("dataHist2");
   dataSetFile->Close();

   return fitter;
}
