#ifndef MST_MSHistFit_H
#define MST_MSHistFit_H

// c/c++ libs
#include <csignal>
#include <cstdlib> 
#include <map>
#include <sstream>
#include <fstream>

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
#include "../rapidjson/istreamwrapper.h"

// m-stats libs
#include <MSPDFBuilderTHn.h>
#include <MSModelTHnBMLF.h>
#include <MSModelPulls.h>
#include <MSMinimizer.h>
#include <MSTHnHandler.h>


using namespace std;

namespace mst {

/* 
 * Load and check integrity of the json config file
 */
rapidjson::Document LoadConfig (const string& configFileName, bool verbose = false) {

   std::ifstream inputStream (configFileName);
   rapidjson::IStreamWrapper inputStreamWrapper (inputStream);
   rapidjson::Document json;
   if (json.ParseStream(inputStreamWrapper).HasParseError()) {
      cerr << "error in json config file: invalid syntax" << endl;
      exit(1);
   }
   inputStream.close();


   // lambda to check if a file exist and has the correct properties
   auto isMemberCorrect = [&verbose] (const rapidjson::Value& json, 
                                      const string& memberName, 
                                      const string& memberType, 
                                      const string& subMemberType = "",  
                                      const int memberSize = 0) {

      // verbose dump
      if (verbose) cout << "info: checking json config file: member " << memberName << "... ";

      // check existance of the member
      const auto member = json.FindMember(memberName.c_str());
      if (member == json.MemberEnd()) {
         cerr << "error in json config file: member " << memberSize 
              << " not found" << endl;
         exit(1);
      }
      
      // check member type
      auto isTypeCorrect = [] (const rapidjson::Value& member, const string& memberType){
         bool c = false;
         if      (!strcmp (memberType.c_str(), "Bool"  )) c = member.IsBool();  
         else if (!strcmp (memberType.c_str(), "Number")) c = member.IsNumber();
         else if (!strcmp (memberType.c_str(), "Int"   )) c = member.IsInt();   
         else if (!strcmp (memberType.c_str(), "Double")) c = member.IsDouble();
         else if (!strcmp (memberType.c_str(), "String")) c = member.IsString();
         else if (!strcmp (memberType.c_str(), "Array" )) c = member.IsArray(); 
         else if (!strcmp (memberType.c_str(), "Object")) c = member.IsObject();
         return c;
      };
      if (isTypeCorrect(member->value, memberType) == false) {
         cerr << "error in json config file: member " << memberName
              << " must be of type " << memberType << endl;
         exit(1);
      }

      // check array submembers
      if (strcmp(subMemberType.c_str(), "")) {
         for (const auto& i : member->value.GetArray()) {
            if (isTypeCorrect(i, subMemberType.c_str()) == false) {
               cerr << "error in json config file: members of member " << memberName
                  << " must be of type " << subMemberType << endl;
               exit(1);
            }
         }
      }

      // check size
      if (memberSize > 0 && member->value.Size() != memberSize) {
         cerr << "error in json config file: member " << memberName
              << " is not of size " << memberSize << endl;
         exit(1);
      }

      if (verbose) cout << "done." << endl;
      return;
   };

   isMemberCorrect(json,"fittingModel", "Object");                             // json/fittingModel                                                 
   isMemberCorrect(json["fittingModel"], "dataSets", "Object");                // json/fittingModel/dataSets
   for (const auto& dataSet : json["fittingModel"]["dataSets"].GetObject()) {  // json/fittingModel/dataSets/*
      if (verbose) cout << "info: checking dataSet "                           //
                        << dataSet.name.GetString() << endl;                   //
      isMemberCorrect(dataSet.value, "exposure", "Number");                    // json/fittingModel/dataSets/*/exposure
      isMemberCorrect(dataSet.value, "components", "Object");                  // json/fittingModel/dataSets/*/components
      for (const auto& component : dataSet.value["components"].GetObject()) {  // json/fittingModel/dataSets/*/components/*
         if (verbose) cout << "info: checking component "                      //
                           << component.name.GetString() << endl;              //
         isMemberCorrect(component.value, "global", "Bool");                   // json/fittingModel/dataSets/*/components/*/global
         isMemberCorrect(component.value, "refVal", "Number");                 // json/fittingModel/dataSets/*/components/*/refVal
         isMemberCorrect(component.value, "range", "Array", "Number", 2);      // json/fittingModel/dataSets/*/components/*/range[]
         isMemberCorrect(component.value, "fitStep", "Number");                // json/fittingModel/dataSets/*/components/*/fitStep
         isMemberCorrect(component.value, "pdf", "Array", "String", 2);        // json/fittingModel/dataSets/*/components/*/pdf[]
         isMemberCorrect(component.value, "injVal", "Number");                 // json/fittingModel/dataSets/*/components/*/injVal
         isMemberCorrect(component.value, "color", "Int");                     // json/fittingModel/dataSets/*/components/*/color
      }                                                                        //
      isMemberCorrect(dataSet.value, "projectOnAxis", "Array", "Int");         // json/fittingModel/dataSets/*/projectOnAxis[]
      isMemberCorrect(dataSet.value, "axis", "Object");                        // json/fittingModel/dataSets/*/axis
      for (const auto& axis : dataSet.value["axis"].GetObject()) {             // json/fittingModel/dataSets/*/axis/*
         if (verbose) cout << "info: checking axis "                           //
                           << axis.name.GetString() << endl;              //
         isMemberCorrect(axis.value, "range", "Array", "Number", 2);           // json/fittingModel/dataSets/*/axis/*/range[]
         isMemberCorrect(axis.value, "rebin", "Int");                          // json/fittingModel/dataSets/*/axis/*/rebin
      }                                                                        // 
      if (dataSet.value.HasMember("normalizePDFInUserRange")) {                // optional block:
         isMemberCorrect(dataSet.value, "normalizePDFInUserRange", "Bool");    // json/fittingModel/dataSets/*/normalizePDFInUserRange
      }                                                                        //
   }                                                                           //
   if (json.HasMember("pulls")) {                                              // optional block:
      isMemberCorrect(json, "pulls", "Object");                                // json/pulls
      for (const auto& pull : json["pulls"].GetObject()) {                     // json/pulls/*
         if (verbose) cout << "info: checking pull "                           // 
                           << pull.name.GetString() << endl;                   //
         isMemberCorrect(pull.value, "type", "String");                        // json/pulls/*/type
         isMemberCorrect(pull.value, "centroid", "Number");                    // json/pulls/*/centroid
         isMemberCorrect(pull.value, "sigma", "Number");                       // json/pulls/*/sigma
      }                                                                        //
   }                                                                           //
   isMemberCorrect(json, "MinimizerSteps", "Object");                          // json/MinimizerSteps
   for (const auto& step : json["MinimizerSteps"].GetObject()) {               // json/MinimizerSteps/*
         if (verbose) cout << "info: checking step "                           //  
                           << step.name.GetString() << endl;                   //
      isMemberCorrect(step.value, "method", "String");                         // json/MinimizerSteps/*
      isMemberCorrect(step.value, "resetMinuit", "Bool");                      // json/MinimizerSteps/*/resetMinuit
      isMemberCorrect(step.value, "maxCall", "Number");                        // json/MinimizerSteps/*/maxCall
      isMemberCorrect(step.value, "tollerance", "Number");                     // json/MinimizerSteps/*/tollerance
      isMemberCorrect(step.value, "verbosity", "Int");                         // json/MinimizerSteps/*/verbosity
   }                                                                           //
   if (json.HasMember("MC")) {                                                 // optional block:
      isMemberCorrect(json, "MC", "Object");                                   // json/MC
      isMemberCorrect(json["MC"], "realizations", "Number");                   // json/MC/realizations
      isMemberCorrect(json["MC"], "seed", "Int");                              // json/MC/seed
      isMemberCorrect(json["MC"], "enablePoissonFluctuations", "Bool");        // json/MC/enablePoissonFluctuations
      isMemberCorrect(json["MC"], "outputFile", "String");                     // json/MC/outputFile
   }

   return json;
}

/* 
 * Initialize all analysis structures, i.e.: the minimizer, the PDFBuilder and
 * the statistical models composing the likelihood. 
 */
MSMinimizer* InitializeAnalysis (const rapidjson::Document& json,  
                                 const std::string& datafileName) {

   // initialize fitter
   MSMinimizer* fitter = new MSMinimizer();

   // loop over data sets and for each create the model and PDFBuilder
   for (const auto& dataSet : json["fittingModel"]["dataSets"].GetObject()) {

      // Initilize analysis model and associate to them the pdfBuilder.
      // The name of the model is needed to retrieve it from the fitter
      // and to retrieve its local parameters
      MSModelTHnBMLF* mod = new MSModelTHnBMLF(dataSet.name.GetString());

      // Set the exposure
      mod->SetExposure(dataSet.value["exposure"].GetDouble());

      // Initialize hist handler //////////////////////////////////////////////
      MSTHnHandler handler;

      // read list of projected axis from the json file 
      // (only if the block projectOnAxis is defined)
      if (dataSet.value.HasMember("projectOnAxis")) {
         std::vector<int> v;
         for (int i = 0; i < dataSet.value["projectOnAxis"].Size(); i++) 
            v.push_back((dataSet.value["projectOnAxis"].GetArray())[i].GetInt());
         handler.ProjectToAxis(v);
      }

      // set the range and binning of the axis
      if (dataSet.value.HasMember("axis")) {
         for (const auto& axis : dataSet.value["axis"].GetObject()) {
            // the stringstream is used just to convert a string into an integer
            stringstream conversion; 
            int axisID = 0;
            conversion << axis.name.GetString();
            conversion >> axisID;
            handler.SetRange(axisID, axis.value["range"][0].GetDouble(), 
                  axis.value["range"][1].GetDouble());
            handler.Rebin(axisID, axis.value["rebin"].GetInt());
         }
      }

      // Renormilize the histograms, in a specic range or over the full axis
      // (ecluding over- and under-flow bins)
      if (dataSet.value.HasMember("normalizePDFInUserRange"))
         handler.RespectAxisUserRange(dataSet.value["normalizePDFInUserRange"].GetBool());

      // end hist handler ////////////////////////////////////////////////////

      // Create a separate pdfBuilder for each model. The pointer of each pdfBuilder 
      // will be associated to the model.
      MSPDFBuilderTHn* pdfBuilder = new MSPDFBuilderTHn(dataSet.name.GetString());

      // Set seed of the random number generator. Note that each PSDBuilder is
      // initialized with the same seed. This will produce the same data set if
      // the parameters of the data sets are all exactly the same.
      if (json.HasMember("MC")) pdfBuilder->SetSeed(json["MC"]["seed"].GetInt());


      // Initialize the parameters of each model, set their properties and load
      // the pdf's
      for (const auto& component : dataSet.value["components"].GetObject()) {

         auto par = new mst::MSParameter(component.name.GetString());
         par->SetGlobal(component.value["global"].GetBool());
         par->SetFixed(component.value["fixed"].GetBool());
         par->SetRange(component.value["range"].GetArray()[0].GetDouble(),
                       component.value["range"].GetArray()[1].GetDouble());

         // Check if fitStep is registered and is different from zero
         if (component.value["fitStep"].GetDouble())
            par->SetFitStartStep(component.value["fitStep"].GetDouble());
         // otherwise use the range to guess to the the fit starting step
         else 
            par->SetFitStartStep( ((component.value["range"].GetArray())[1].GetDouble()
                                -  (component.value["range"].GetArray())[0].GetDouble())/100.);

         par->SetFitStartValue(component.value["refVal"].GetDouble());

         mod->AddParameter(par);

         // Load histograms for each component and possibly project it on a sub-set
         // of the axis
         TString pathToFile (getenv("M_STATS_HIST_FIT"));
         if (pathToFile != "") pathToFile += "/";
         pathToFile +=component.value["pdf"][0].GetString();

         pdfBuilder->RegisterHist( handler.BuildHist( pathToFile.Data(),
                                   component.value["pdf"][1].GetString(),
                                   component.name.GetString(),
                                   true));
      }

      // Move the pointer of the pdfBuilder to the model
      mod->SetPDFBuilder(pdfBuilder);

      // Set data if loaded from file
      if (!datafileName.empty()) {
         std::cout << "info: loading input data from " << datafileName << std::endl;
         mod->SetDataSet( handler.BuildHist(datafileName, mod->GetName(), mod->GetName(), false) );
      }

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
