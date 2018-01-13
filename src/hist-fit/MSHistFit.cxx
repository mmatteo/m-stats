#ifndef MST_MSHistFit_H
#define MST_MSHistFit_H

// c/c++ libs
#include <csignal>
#include <cstdlib> 
#include <map>
#include <sstream>

// ROOT libs
#include <TBranch.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <THn.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TTree.h>

// rapidjson's DOM-style API
#include "../rapidjson/document.h"

// m-stats libs
#include <MSPDFBuilderTHn.h>
#include <MSModelTHnBMLF.h>
#include <MSModelPulls.h>
#include <MSMinimizer.h>


using namespace std;

namespace mst {

/* 
 * Load and check integrity of the json config file
 */
void LoadConfig (const rapidjson::Document& json) {

   assert(json.HasMember("fittingModel"));
   assert(json["fittingModel"].IsObject());

   assert(json["fittingModel"].HasMember("dataSets"));
   assert(json["fittingModel"]["dataSets"].IsObject());

   for (const auto& dataSet : json["fittingModel"]["dataSets"].GetObject()) {

      assert(dataSet.value.HasMember("exposure"));
      assert(dataSet.value["exposure"].IsNumber());

      assert(dataSet.value.HasMember("components"));
      assert(dataSet.value["components"].IsObject());
      for (const auto& component : dataSet.value["components"].GetObject()) {
         assert(component.value.HasMember("global"));
         assert(component.value["global"].IsBool());

         assert(component.value.HasMember("refVal"));
         assert(component.value["refVal"].IsNumber());

         assert(component.value.HasMember("range"));
         assert(component.value["range"].IsArray());
         assert(component.value["range"].Size() == 2);
         for (const auto& i : component.value["range"].GetArray()) 
            assert(i.IsNumber());

         assert(component.value.HasMember("fitStep"));
         assert(component.value["fitStep"].IsNumber());

         assert(component.value.HasMember("pdf"));
         assert(component.value["pdf"].IsArray());
         assert(component.value["pdf"].Size()== 2);
         for (const auto& i : component.value["pdf"].GetArray()) 
            assert(i.IsString());

         assert(component.value.HasMember("injVal"));
         assert(component.value["injVal"].IsNumber());

         assert(component.value.HasMember("color"));
         assert(component.value["color"].IsInt());
      }

      // optional block
      if (dataSet.value.HasMember("projectOnAxis")) {
         assert(dataSet.value["projectOnAxis"].IsArray());
         for (const auto& i : dataSet.value["projectOnAxis"].GetArray()) 
            assert(i.IsInt());
      }
      
      // optional block
      if (dataSet.value.HasMember("axis")) {
         assert(dataSet.value["axis"].IsObject());
         for (const auto& axis : dataSet.value["axis"].GetObject()) {
            if (axis.value.HasMember("range")) {
               assert(axis.value["range"].IsArray());
               assert(axis.value["range"].Size() == 2);
               for (const auto& i : axis.value["range"].GetArray())
                  assert(i.IsNumber());
            }
         }
         if (dataSet.value["axis"].HasMember("rebin")) {
            assert(dataSet.value["axis"]["rebin"].IsInt());
         }
      }

      // optional block
      if (dataSet.value.HasMember("normalizePDFInUserRange")) {
         assert(dataSet.value["normalizePDFInUserRange"].IsBool());
      }
   }

   // optional block
   if (json.HasMember("pulls")) {
      assert(json["pulls"].IsObject());
      for (const auto& pull : json["pulls"].GetObject()) {
         assert(pull.value.HasMember("type"));
         assert(pull.value["type"].IsString());
         assert(pull.value.HasMember("centroid"));
         assert(pull.value["centroid"].IsNumber());
         assert(pull.value.HasMember("sigma"));
         assert(pull.value["sigma"].IsNumber());
      }
   }
   
   // Mandatory block
   assert(json.HasMember("MinimizerSteps"));
   assert(json["MinimizerSteps"].IsObject());
   for (const auto& step : json["MinimizerSteps"].GetObject()) {
      assert(step.value.HasMember("method"));
      assert(step.value["method"].IsString());
      assert(step.value.HasMember("resetMinuit"));
      assert(step.value["resetMinuit"].IsBool());
      assert(step.value.HasMember("maxCall"));
      assert(step.value["maxCall"].IsNumber());
      assert(step.value.HasMember("tollerance"));
      assert(step.value["tollerance"].IsNumber());
      assert(step.value.HasMember("verbosity"));
      assert(step.value["verbosity"].IsInt());
   }
   
   // optional block
   if (json.HasMember("MC")) {
      assert(json["MC"].IsObject());
      assert(json["MC"].HasMember("realizations"));
      assert(json["MC"]["realizations"].IsInt());
      assert(json["MC"].HasMember("seed"));
      assert(json["MC"]["seed"].IsInt());
      assert(json["MC"].HasMember("enablePoissonFluctuations"));
      assert(json["MC"]["enablePoissonFluctuations"].IsBool());
      assert(json["MC"].HasMember("outputFile"));
      assert(json["MC"]["outputFile"].IsString());
   }

   return;
}

/* 
 * Initialize all analysis structures, i.e.: the minimizer, the PDFBuilder and
 * the statistical models composing the likelihood. 
 */
MSMinimizer* InitializeAnalysis (const rapidjson::Document& json) {

   // initialize fitter
   MSMinimizer* fitter = new MSMinimizer();

   // loop over data sets and for each create the model and PDFBuilder
   for (const auto& dataSet : json["fittingModel"]["dataSets"].GetObject()) {

      // Create a separate pdfBuilder for each model. The pointer of each pdfBuilder 
      // will be associated to the model.
      MSPDFBuilderTHn* pdfBuilder = new MSPDFBuilderTHn(dataSet.name.GetString());
  
      // Set seed of the random number generator. Note that each PSDBuilder is
      // initialized with the same seed. This will produce the same data set if
      // the parameters of the data sets are all exactly the same.
      if (json.HasMember("MC")) pdfBuilder->SetSeed(json["MC"]["seed"].GetInt());

      // Prepare variables to project the multidimensional PSD's on a sub
      // set of its axis:
      //
      // - ndim_pr: number of dimensions on which to project
      // - dim_pr: array storing the ordered set of axis indexes on which to
      //           project. I.e.  dim_pr = {3,1,4} will project the original 
      //           PDF on its axis 1,3,4 in the following order: x=3, y=1, z=4
      int ndim_pr = 0; 
      int* dim_pr = nullptr; 

      // load ndim_pr and dim_pr from the json file (only if the block
      // projectOnAxis is defined)
      if (dataSet.value.HasMember("projectOnAxis")) {
            ndim_pr = dataSet.value["projectOnAxis"].Size();
            dim_pr = new int[ndim_pr];
            for (int i = 0; i < ndim_pr; i++) 
               dim_pr[i] = (dataSet.value["projectOnAxis"].GetArray())[i].GetInt();
      }

      // Load histograms for each component and possibly project it on a sub-set
      // of the axis
      for (const auto& component : dataSet.value["components"].GetObject()) {
         // Build file path possibly addng prefix from env variable 
         TString pathToFile (getenv("M_STATS_BINNED_FIT_PDF_DIR"));
         if (pathToFile != "") pathToFile += "/";
         pathToFile +=component.value["pdf"][0].GetString();

         pdfBuilder->LoadHist(pathToFile.Data(),
               component.value["pdf"][1].GetString(),
               component.name.GetString(), ndim_pr, dim_pr);
      }

      // set the binning of the pdf's.
      const int maxAxisNum = 100;
      std::array<Int_t,maxAxisNum> rebin;
      for (auto& i : rebin) i = 1;
      if (dataSet.value.HasMember("axis")) {
         for (const auto& axis : dataSet.value["axis"].GetObject()) {
            if (axis.value.HasMember("rebin")) {
               // the stringstream is used just to convert a string into an integer
               stringstream conversion; 
               int axisID = 0;
               conversion << axis.name.GetString();
               conversion >> axisID;
               if (axisID < maxAxisNum ) {
                     rebin[axisID] = axis.value["rebin"].GetInt();
               } else {
                  // The hard-coded maximum number of axis can be increased but it is
                  // not expected to have so many dimension...
                  std::cerr << "error: the max number of concived axis is " 
                       << maxAxisNum << std::endl;
               }
            }
         }
         pdfBuilder->Rebin(&rebin.at(0));

         // set the UserRange of the pdf's
         for (const auto& axis : dataSet.value["axis"].GetObject()) {
            if (axis.value.HasMember("range")) {
               stringstream conversion; 
               int axisID = 0;
               conversion << axis.name.GetString();
               conversion >> axisID;
               pdfBuilder->SetRangeUser(axis.value["range"][0].GetDouble(),
                                        axis.value["range"][1].GetDouble(),
                                        axisID);
            }
         }
      }
      // Renormilize the histograms, in a specic range or over the full axis
      // (ecluding over- and under-flow bins)
      if (dataSet.value.HasMember("normalizePDFInUserRange"))
         pdfBuilder->NormalizeHists(dataSet.value["normalizePDFInUserRange"].GetBool());

      // Initilize analysis model and associate to them the pdfBuilder.
      // The name of the model is needed to retrieve it from the fitter
      // and to retrieve its local parameters
      MSModelTHnBMLF* mod = new MSModelTHnBMLF(dataSet.name.GetString());

      // Initialize the parameters of each model and set their properties
      for (const auto& component : dataSet.value["components"].GetObject()) {

         auto par = new mst::MSParameter(component.name.GetString());
         par->SetGlobal       (component.value["global"].GetBool());
         par->SetFitStartValue(component.value["refVal"].GetDouble());

         // Check if fitStep is registered and is different from zero
         if (component.value["fitStep"].GetDouble())
            par->SetFitStartStep(component.value["fitStep"].GetDouble());
         // otherwise use the range to guess to the the fit starting step
         else 
            par->SetFitStartStep( ((component.value["range"].GetArray())[1].GetDouble()
                                -  (component.value["range"].GetArray())[0].GetDouble())/100.);

         par->SetRange(component.value["range"].GetArray()[0].GetDouble(),
                       component.value["range"].GetArray()[1].GetDouble());

         par->SetFixed(component.value["fixed"].GetBool());
         mod->AddParameter(par);
      }

      // Move the pointer of the pdfBuilder to the model
      mod->SetPDFBuilder(pdfBuilder);
      // Set the exposure
      mod->SetExposure(dataSet.value["exposure"].GetDouble());

      // Move pointer of the model to the fitter
      fitter->AddModel(mod);
   }

   // Add models implementing pulls if requested in the config file
   // FIXME: check if pull are present
   if (json["fittingModel"].HasMember("pulls")) {
      for (const auto& pull : json["fittingModel"]["pulls"].GetObject()) {
         // initialize and add gaussian pulls
         if (strcmp(pull.value["type"].GetString(), "gauss") == 0) {
            MSModelPullGaus* mod = new MSModelPullGaus(pull.name.GetString());
            mod->SetPullPar(pull.name.GetString());
            mod->SetGaussPar(pull.value["centroid"].GetDouble(),
                             pull.value["sigma"].GetDouble());
            fitter->AddModel(mod);
         // initialize and add exponential pulls
         } else if (strcmp(pull.value["type"].GetString(), "exp") == 0) {
            MSModelPullExp* mod = new MSModelPullExp(pull.name.GetString());
            mod->SetPullPar(pull.name.GetString());
            mod->SetExpPar(pull.value["limit"].GetDouble(),
                           pull.value["quantile"].GetDouble());
            fitter->AddModel(mod);
         } else {
            std::cerr << "error: pull type not implemented.\n";
            exit(1);
         }
      }
   }

   // Sync the parameters. This call is needed to finilize the initializatoin of
   // the minimizer
   fitter->SyncFitParameters();
   return fitter;
}

/*
 * Create data sets and automatically associate it to the models
 */
bool SetDataSetFromFile (MSMinimizer* fitter, const std::string& fileName) {

   TFile inputFile(fileName.c_str(), "READ");
   if(inputFile.IsOpen() == kFALSE) {
      std::cerr << "error: file of input data not found\n";
      return false;
   } else {
      std::cout << "info: loading input data from " << fileName << std::endl;
   }

   // loop over the models and create and try to load the data set 
   for (const auto& model: *fitter->GetModels()) {

      // filter only models that  have a data sets (no pulls)
      const auto mod = dynamic_cast<MSModelTHnBMLF*>(model);
      if(mod == nullptr) continue;

      THnBase* hist = dynamic_cast<THnBase*>(inputFile.Get(mod->GetName().c_str()));
      if (!hist) {
         std::cerr << "error: data histograms not found in the file\n";
         return false;
      } else {
         mod->SetDataSet(hist);
      }
   }
   return true;
}

/*
 * Create data sets and automatically associate it to the models
 */
bool SetDataSetFromMC (const rapidjson::Document& json, MSMinimizer* fitter) {

   // loop over the models and create a new data set for each of them
   for (const auto& model: *fitter->GetModels()) {

      // filter only models that  have a data sets (no pulls)
      const auto mod = dynamic_cast<MSModelTHnBMLF*>(model);
      if(mod == nullptr) continue;

      // get the specific pdfBuilder and reset it
      const auto pdfBuilder = mod->GetPDFBuilder();
      pdfBuilder->ResetPDF();

      // add hists to pdfBuilder with the desired rate and copute the number 
      // of counts to extract to create the data set
      double totalCounts = 0;
      for (const auto& par: *mod->GetLocalParameters()) {
         const double trueVal = json["fittingModel"]["dataSets"][mod->GetName().c_str()]
                                    ["components"][par.c_str()]["injVal"].GetDouble();

         pdfBuilder->AddHistToPDF(par.c_str(), trueVal);
         totalCounts += trueVal * mod->GetExposure();
      }
      // register new data set. The previous one is delete inside the model
      // Define whether to add poission fluctuation to the number of events
      mod->SetDataSet(pdfBuilder->GetMCRealizaton(totalCounts, 
                      json["MC"]["enablePoissonFluctuations"].GetBool()));
   }
   return true;
}

/*
 * Minimization of the likelihood
 */
bool Minimize (const rapidjson::Document& json, MSMinimizer* fitter) {

   // Take Minuit calls from config file in the proper order
   for (const auto& step : json["MinimizerSteps"].GetObject()) {
      fitter->SetMinuitVerbosity(step.value["verbosity"].GetInt());
      fitter->Minimize(step.value["method"].GetString(),
                       step.value["resetMinuit"].GetBool(),
                       step.value["maxCall"].GetDouble(),
                       step.value["tollerance"].GetDouble());
   }

   if (fitter->GetMinuitStatus()) {
      std::cerr << "MSMinimizer: minuit return status=" << fitter->GetMinuitStatus()
                << ", indicating problems in the convergence of the fit\n"; 
   }
   return true;
}

/*
 * General routine for plotting the results
 */
TCanvas* GetCanvasFit (const rapidjson::Document& json, const MSMinimizer* fitter) {

   // Retrieve the canvas and define number of canvases
   int nMaxDim = 0;
   int nMaxMod = 0;
   delete gROOT->GetListOfCanvases()->FindObject("cMLF");
   // Look for the max number of dimensions 
   for (int modelNum = 0; modelNum < fitter->GetNModels(); modelNum++) {
      const auto mod = dynamic_cast<MSModelTHnBMLF*>(fitter->GetModels()->at(modelNum));
      if (mod == nullptr) continue;

      nMaxMod++;
      if (mod->GetDataSet()->GetNdimensions() > nMaxDim) 
         nMaxDim = mod->GetDataSet()->GetNdimensions();
   }
   TCanvas* cc = new TCanvas("cMLF","Max likelihood fit",400*nMaxDim, 400*nMaxMod); 
   cc->Divide(nMaxDim, nMaxMod);

   // Set general style
   gStyle->SetOptStat(0);
   gStyle->SetTitle(0);

   // index storing the number of models added to the canvas
   int m = 0;
   // loop over the models
   for (const auto& model : *fitter->GetModels())  {
      const auto mod = dynamic_cast<MSModelTHnBMLF*>(model);
      if (mod == nullptr) continue; 

      // retrieve structures to be plot
      const auto pdfBuilder   = mod->GetPDFBuilder();
      const auto dataHist     = mod->GetDataSet();
      const auto localParList = mod->GetLocalParameters();

      // loop over the dimensions
      for (int d = 0; d < mod->GetDataSet()->GetNdimensions(); d++) {
         if (dataHist->GetNdimensions()<d) break;
         // set the pad
         cc->cd(1 + m*nMaxDim + d);

         // Plot first the original data to set the plots ranges
         auto data_pr = dataHist->Projection(d);
         data_pr->SetLineWidth(0);
         data_pr->SetMinimum(0.01);
         data_pr->DrawCopy();
         gPad->SetLogy();

         // Draw best fit components
         for (auto& parName : (*localParList)) {
            pdfBuilder->ResetPDF();
            mst::MSParameter*  par = mod->GetParameter(parName.c_str());
            pdfBuilder->AddHistToPDF(parName, mod->GetExposure()*par->GetFitBestValue());
            auto pdf_tmp = pdfBuilder->GetPDF("");

            auto pdf_tmp_pr = pdf_tmp->Projection(d);
            pdf_tmp_pr->SetName(parName.c_str());
            pdf_tmp_pr->SetTitle(parName.c_str());
            int color = json["fittingModel"]["dataSets"][mod->GetName().c_str()]
                            ["components"][parName.c_str()]["color"].GetInt();
            pdf_tmp_pr->SetLineColor(color);
            pdf_tmp_pr->SetLineWidth(2);
            pdf_tmp_pr->DrawCopy("hist same");

            delete pdf_tmp_pr;
            delete pdf_tmp;
         }

         // Build and Draw best fit
         pdfBuilder->ResetPDF();
         for (auto&i: (*localParList)) {
            mst::MSParameter*  par = mod->GetParameter(i);
            pdfBuilder->AddHistToPDF(i, mod->GetExposure()*par->GetFitBestValue());
         }
         auto tot = pdfBuilder->GetPDF("tot");
         auto tot_pr = tot->Projection(d);
         tot_pr->SetLineColor(kGreen);
         tot_pr->SetLineWidth(2);
         tot_pr->DrawCopy("hist same");
         delete tot_pr;
         delete tot;

         // Plot data
         data_pr->SetMarkerColor(kBlack);
         data_pr->SetLineColor(kBlack);
         data_pr->SetMarkerStyle(20);
         data_pr->SetMarkerSize(0.7);
         data_pr->DrawCopy("PE hist same");
         delete data_pr;
      }
      // increase model index
      m++;
   }
  cc->Update();
  return cc;
}

/*
 * Build profile likelihood scan for a specific parameter
 */
TGraph* Profile (const rapidjson::Document& json, MSMinimizer* fitter, 
      const string& parName, const double NLL, const int nPts) {

   // retrieve parameter of interest (poi) from the fitter
   mst::MSParameter* poi  = fitter->GetParameter(parName.c_str());
   if (!poi) {
         std::cerr << "Profile >> error: parameter " << parName << " not found\n";
         return nullptr;
   }

   // save best fit values to restore the status of the parameters after the
   // scanning
   vector<double> fitBestValue    (fitter->GetParameterMap()->size(), -1);
   vector<double> fitBestValueErr (fitter->GetParameterMap()->size(), -1);
   {
      int parIndex = 0;
      for ( auto it : *fitter->GetParameterMap()) {
         fitBestValue.at(parIndex)    = it.second->GetFitBestValue();
         fitBestValueErr.at(parIndex) = it.second->GetFitBestValueErr();
         parIndex++;
      }
   }

   // Initialize output gaph
   TGraph* gpll = new TGraph();
   // store absolute minimum of the likelihood
   double absMinNLL = std::numeric_limits<double>::max();

   // define auxiliary lambda function for profiling, which has visibility over
   // all variables defined up to now
   auto Scan = [&] (double tVal) {
      if (tVal < poi->GetRangeMin() || tVal > poi->GetRangeMax()) return false;
      poi->FixTo(tVal);

      for (const auto& step : json["MinimizerSteps"].GetObject()) {
         fitter->SetMinuitVerbosity(step.value["verbosity"].GetInt());
         fitter->Minimize(step.value["method"].GetString(),
               step.value["resetMinuit"].GetBool(),
               step.value["maxCall"].GetDouble(),
               step.value["tollerance"].GetDouble());
      }


      // extract temporary best fit value
      const double tmpMinNLL = fitter->GetMinNLL();
      gpll->SetPoint(gpll->GetN(), tVal, tmpMinNLL);
      // update absolute minimum if needed
      if (tmpMinNLL < absMinNLL) absMinNLL = tmpMinNLL;

      // check the status of minuit
      if (fitter->GetMinuitStatus()) {
         std::cerr << "Profile >> error: minuit returned failed status ["
            << fitter->GetMinuitStatus() << "]"
            << " while fitting with " << parName 
            << " fixed to " << tVal << std::endl;
      } 
      // chek if the exit conditions are met
      if (tmpMinNLL - absMinNLL <= NLL)  return true;
      else return false;
   };

   // perform actual scan
   {
      // index used to define the points to scans
      // best fit value and error
      const double poiFitBestValue    = poi->GetFitBestValue();
      const double poiFitBestValueErr = poi->GetFitBestValueErr();

      const double step = 2*poiFitBestValueErr / double(nPts);
      // scan to the right of the min
      int counter = 0;
      fitter->SyncFitParameters(true);
      while (Scan(poiFitBestValue + counter*step) && counter<10*nPts) counter++;
      // scan to the left of the min starting from -1 to not add again the best fit
      // value in the TGraph
      counter = -1;
      fitter->SyncFitParameters(true);
      while (Scan(poiFitBestValue + counter*step) && counter<10*nPts) counter--;

      // normilize profile to the absolute minimum found during while profiling
      for (int i = 0; i < gpll->GetN(); i++ ) {
         gpll->SetPoint(i, gpll->GetX()[i], (gpll->GetY()[i]-absMinNLL));
      }
   }

   // reset parameter original status
   // restore best fit values of the parameters
   {
      int parIndex = 0;
      for ( auto it : *fitter->GetParameterMap()) {
         it.second->SetFitBestValue(fitBestValue.at(parIndex));
         it.second->SetFitBestValueErr(fitBestValueErr.at(parIndex));
         parIndex++;
      }
   }
   poi->Release();

   // Set titles (this must be done after filling the TGraph. Probably it's a
   // bug of ROOT
   gpll->SetName(Form("nll_%s", parName.c_str())); 
   gpll->GetXaxis()->SetTitle(Form("%s rate [cts/100T/d]", parName.c_str())); 
   gpll->GetYaxis()->SetTitle("-LogLikelihood)"); 
   gpll->Sort();
   return gpll;
}

/*
 * Build Profile for each parameter of the fit
 */

TCanvas* GetCanvasProfiles (const rapidjson::Document& json, MSMinimizer* fitter, 
      const double NLL, const int nPts) {

   // retrieve the canvas or initialize it
   delete gROOT->GetListOfCanvases()->FindObject("cPLL");
   const int poiNum = fitter->GetParameterMap()->size();
   TCanvas* cc = new TCanvas("cPLL","profile log likelihoods",
                              400*4, 400*ceil(poiNum/4.0)); 
   cc->Divide(4, ceil(poiNum/4.0));
   

   int parIndex = 0;
   for ( auto it : *fitter->GetParameterMap()) {
      TGraph* tmp = Profile(json, fitter, it.second->GetName().c_str(), NLL, nPts);
      cc->cd(parIndex+1);
      tmp->Draw("al*");
      cc->Update();
      parIndex++;
   }
   return cc;
}


} // namespace mst

#endif // MST_MSHistFit_H
