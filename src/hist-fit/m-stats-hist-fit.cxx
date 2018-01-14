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


// main.cc
/*!
 * m-stat-hist-fit: multi variate binned analysis with THn histograms
 */

#define VERSION     "v 1.5 "
#define DATE        "(Jan 2018)"
#define AUTHOR      "Matteo Agostini"
#define EMAIL       "<matteo.agostini@ph.tum.de>"
#define DESCRIPTION "A tool for multi variate binned analysis with THn histograms"

// c/c++ libs
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <string>

// ROOT libs
#include <TApplication.h>
#include <TROOT.h>
#include <TObject.h>

// m-stats libs
#include <MSPDFBuilderTHn.h>
#include <MSModelTHnBMLF.h>
#include <MSModelPulls.h>
#include <MSMinimizer.h>
#include "MSHistFit.cxx"

// rapidjson's DOM-style API
#include "../rapidjson/document.h"   

using namespace std;


/*****************************************************************************
 *  Command line option parsing                                              *
 *****************************************************************************/
   //! process program arguments
   int process_arguments(int argc, char** argv);
   //! print program usage 
   void usage();
   //! Program name 
   string gProgramName;

   //! cofig file name
   string gConfigFileName {""};
   //! pdf path
   string gPDFPath {""};

   //! operation modes 
   enum class EOperationMode {kUndefined=0, kInteractiveFit=1, kBatchFit=2};
   enum EOperationMode  gOperationMode  = EOperationMode::kInteractiveFit;
   bool gDatafromFile = false;

   //! input file name
   string gInputFileName {""};
   //! output file name
   string gOutputFileName {"bx-stats.root"};

   //! do profiles
   bool gBuildProfiles = false;
   //! approx number of points in profiles
   int gProfilePts = 10;
   //! approx delta CL to cover in profiles
   double gProfilesCL = 0.95;

   //! store Data sets in multi-fit operation mode:
   bool gStoreMFDataSets = false;
   //! store canvas with maximum likelihood fit in multi-fit operations
   bool gStoreMFCanvasMLF = false;
   //! appended output in exitisting file (default overwrite)
   bool gAppendOnFile = false;

   //! Verbose level:
   int gVerbosityLevel = 0;


/*****************************************************************************
 *  Interrupt structures                                                     *
 *****************************************************************************/
   //! interrupt variable
   static sig_atomic_t volatile gRunning = true;
   //! interrupt handler
   void sig_handler(int signum) { gRunning = false; }
/*****************************************************************************/


int main(int argc, char** argv)
{
   // process options parsed by command line
   process_arguments(argc, argv);

   // retrieve json file
   cout << "Loading configuration from " << gConfigFileName << endl;
   rapidjson::Document json = mst::LoadConfig(gConfigFileName, gVerbosityLevel);


   // Open output file
   TString oFileName (json["MC"]["outputFile"].GetString());
   if (oFileName=="") { 
   //! store canvas
   bool gStoreCanvasFit = false;
      // take the name of the config file
      oFileName = gConfigFileName;
      // remove .json
      oFileName.Resize(oFileName.Sizeof() - 6);
      oFileName+=".root";
   }

   std::string readmode;
   if (gAppendOnFile) readmode = "update";
   else               readmode = "recreate";
   TFile ofile (oFileName.Data(), readmode.c_str());

   /*
    * Perform interactive fit from external data or from a MC generated data set
    */
   if (gOperationMode == EOperationMode::kInteractiveFit) {
      gROOT->SetBatch(kFALSE);

      TApplication theApp("App",&argc, argv);

      auto fitter = mst::InitializeAnalysis(json);
      // FIXME: Here load external data set if the name is parsed by command
      // line
      if (gDatafromFile) mst::SetDataSetFromFile(fitter, gInputFileName);
      else mst::SetDataSetFromMC(json, fitter);

      mst::Minimize (json,fitter);

      // Print parameter summary
      fitter->PrintParSummary();
      std::cout << "MinNLL= " << fitter->GetMinNLL() << std::endl;

      ofile.cd();
      mst::GetCanvasFit(json,fitter)->Write(0, TObject::kOverwrite);
      if (gBuildProfiles) mst::GetCanvasProfiles(json, fitter,
                             TMath::ChisquareQuantile(gProfilesCL,1),
                             gProfilePts)->Write(0, TObject::kOverwrite);
      ofile.Close();
      theApp.Run();

   /*
    * Perform fits in batch from MC generated data sets or external data sets
    */
   } else if (gOperationMode == EOperationMode::kBatchFit) {
      gROOT->SetBatch();

      auto fitter = mst::InitializeAnalysis(json);

      // Initialize output variables
      int minuitStatus = 0;
      double absNLLMin = std::numeric_limits<double>::max();
      vector<double> fitBestValue    (fitter->GetParameterMap()->size(), -1);
      vector<double> fitBestValueErr (fitter->GetParameterMap()->size(), -1);
      TCanvas* cMLF {nullptr};
      TCanvas* cPLL {nullptr};

      // Initialize tree structure:
      // Open root file to collect outputs
      ofile.cd();

      // create pointer to the tree
      TTree* otree = nullptr;
      // if gAppendOnFile try to load tree
      if (gAppendOnFile) ofile.GetObject("t",otree);
      // in case the tree has to be created set branches otherwise set branch
      // addresses. Code in the two blocks is basically duplicated.
      if (gAppendOnFile == false || otree == nullptr) {
         otree = new TTree("t","t");

         // Initialize branches 
         otree->Branch("absNLLMin", &absNLLMin);
         otree->Branch("minuitStatus", &minuitStatus);
         {
            int counter = 0;
            for ( auto it : *fitter->GetParameterMap()) {
               otree->Branch(it.second->GetName().c_str(),  
                     &(fitBestValue.at(counter)));
               otree->Branch(Form("%sErr",it.second->GetName().c_str()), 
                     &(fitBestValueErr.at(counter)));
               counter++;
            }
         }
         if (gStoreMFCanvasMLF) otree->Branch("cMLF", &cMLF);
         if (gBuildProfiles)    otree->Branch("cPLL", &cPLL);

      } else {
         otree->SetBranchAddress("minuitStatus", &minuitStatus);
         otree->SetBranchAddress("absNLLMin",    &absNLLMin);
         {
            int counter = 0;
            for ( auto it : *fitter->GetParameterMap()) {
               otree->SetBranchAddress(it.second->GetName().c_str(),  
                     &(fitBestValue.at(counter)));
               otree->SetBranchAddress(Form("%sErr",it.second->GetName().c_str()), 
                     &(fitBestValueErr.at(counter)));
               counter++;
            }
         }
         if (gStoreMFCanvasMLF) otree->SetBranchAddress("cMLF", &cMLF);
         if (gBuildProfiles)    otree->SetBranchAddress("cPLL", &cPLL);
      }

      // start loop over realizations 
      // Note: if the input is taken from file, the loop will be broken after
      // the first iteration
      const int iMax= gDatafromFile ? 1 : json["MC"]["realizations"].GetDouble();
      for (int i=0; i< iMax; i++) {
         if (!gDatafromFile) 
            cout << "# processing MC realization " << i+1 << " of " << iMax << endl;

         // Check for interrupts 
         signal(SIGINT,  &sig_handler);
         signal(SIGTERM, &sig_handler);
         if (!gRunning) break;

         if (gDatafromFile) mst::SetDataSetFromFile(fitter, gInputFileName); 
         else mst::SetDataSetFromMC(json, fitter);
         mst::Minimize(json, fitter);

         { // transfer output values to the vectors associated to the tree
            int counter = 0;
            for ( auto it : *fitter->GetParameterMap()) {
               fitBestValue.at(counter)    = it.second->GetFitBestValue();
               fitBestValueErr.at(counter) = it.second->GetFitBestValueErr();
               counter++;
            }
         }

         minuitStatus = fitter->GetMinuitStatus();
         absNLLMin = fitter->GetMinNLL();

         if (gVerbosityLevel) {
            fitter->PrintParSummary();
            std::cout << "MinNLL= " << absNLLMin << std::endl;
         }

         if (gStoreMFCanvasMLF) cMLF = mst::GetCanvasFit(json, fitter);
         if (gBuildProfiles)    cPLL = mst::GetCanvasProfiles(json, fitter,
                                            TMath::ChisquareQuantile(gProfilesCL, 1),
                                            gProfilePts);

         otree->Fill();

         // Optinally store data sets into files
         // FIXME: Store also the data set in the tree
         if (gStoreMFDataSets) {
            // get main output file name
            TString tmpFileName = ofile.GetName();
            // remove ".root" extensions
            tmpFileName.Resize(tmpFileName.Sizeof() - 6);
            tmpFileName += "-dataSets.root";
            TFile tmpFile (tmpFileName, "update");

            for (const auto& j: *fitter->GetModels()) {
               const auto mod = dynamic_cast<mst::MSModelTHnBMLF*>(j);
               if(mod == nullptr) continue;

               TString dataSetName (mod->GetName().c_str());
               dataSetName += "_";
               dataSetName += i;
               mod->GetDataSet()->Write(dataSetName, TObject::kOverwrite);
            }
            tmpFile.Close();
         }
      }
      ofile.cd();
      otree->Write("", TObject::kOverwrite);
      ofile.Close();
   } else {
      cout << "error: operation mode not defined" << endl;
   }
   
   return 0;
}

/* long options */
static struct option long_options[] =
{
   // operation modes
   {"single-fit",        no_argument,       0,             's' },
   {"multi-fit",         no_argument,       0,             'm' },

   {"data-from-file",    required_argument, 0,             'f' },
   {"output-file",       required_argument, 0,             'o' },

   {"build-profiles",    no_argument,       0,             'p' },
   {"profile-Npts",      required_argument, 0,             'n' },
   {"profile-CL",        required_argument, 0,             'c' },

   {"store-data-set",    no_argument,       0,             'd' },
   {"store-MLF-plot",    no_argument,       0,             't' },
   {"append-to-file",    no_argument,       0,             'a' },

   // software info
   {"help",              no_argument,       0,             'h' },
   {"verbose",           no_argument,       0,             'v' },
   {"version",           no_argument,       0,             'V' },
   { 0,                  0,                 0,              0  }
};

int process_arguments(int argc, char** argv)
{
   // set programm name
   gProgramName = argv[0];

   // auxiliary variables for option parsing
   int operationModeCheck = 0;
   int c;

   while ((c = getopt_long (argc, argv, "ib f:o: pn:c: dta hvV0",
             long_options, NULL)) != -1 ) {

      switch (c) {
         // Main operation mode:
         case 'i':
            gOperationMode = EOperationMode::kInteractiveFit;
            operationModeCheck++;
            break;
         case 'b':
            gOperationMode = EOperationMode::kBatchFit;
            operationModeCheck++;
            break;

         case 'f':
            gInputFileName = optarg;
            gDatafromFile = true;
            break;
         case 'o':
            gOutputFileName = optarg;
            break;

         case 'p': 
            gBuildProfiles = true;
            break;
         case 'n':
            { std::stringstream conversion; conversion << optarg;
            conversion >> gProfilePts; }
            break;
         case 'c': 
            { std::stringstream conversion; conversion << optarg;
            conversion >> gProfilesCL; }
            break;

         case 'd': 
            gStoreMFDataSets = true;
            break;
         case 't':
            gStoreMFCanvasMLF = true;
            break;
         case 'a':
            gAppendOnFile = true;
            break;

         case 'h':
            usage();
            exit(0);
            break;
         case 'V':
            cout << gProgramName << ": "
               << DESCRIPTION << " " << VERSION     << " "
               << DATE        << endl
               << endl;
            exit(0);
            break;
         case 'v':
               gVerbosityLevel++;
            break;
         case '?':
            cout << "Try `" << gProgramName
               << " --help' for more information.\n";
         default:
            exit(1);
      }
   }

   // Check if there are others arguments or if the operation mode is not well
   // defined
   if (optind +1 != argc || operationModeCheck != 1) {
      cout << gProgramName << ": invalid option\n"
         << "Try `" << gProgramName << " --help' for more information.\n";
      exit(1);
   } else {
      gConfigFileName=argv[optind];
   }

   return 0;
}

// help message
void usage()
{
    cout << gProgramName << ": " << DESCRIPTION << " "
	      <<  VERSION << " " << DATE
	      << endl
	      << endl
              << "Usage: " << gProgramName << " [OPERATION MODE] [OPTIONS]...  [CONFIG FILE]"
	      << endl
              << endl
	      << " OPERATION MODES:" 
              << endl
	      << "  -i, --interactive-fit           run fit interactively" << endl
	      << endl
	      << "  -b, --batch-fit                 run fit(s) in batch" << endl
              << endl
	      << " OPTIONS: "
	      << endl
	      << endl
	      << "  -f, --data-from-file [FILE]     run fit on external data set" << endl
	      << "                                  [default: generate data set via MC]" << endl
	      << endl
	      << "  -p, --build-profiles            build likelihood profiles" << endl
	      << endl
	      << "  -n, --profile-Npts              approx number of pts in profile" << endl
	      << endl     
	      << "  -n, --profile-CL                approx CL interval to be covered" << endl
	      << endl 
	      << endl 
	      << "  -d, --store-MC-datasets         store MC generated data sets" << endl
	      << endl
	      << "  -t, --store-MLF-plot            store canvases with fit results" << endl
	      << endl
	      << "  -a, --append-to-file            add output to existing file instead of overwriting it"
	      << endl
	      << "  -v, --verbose                   increase verbosity level" << endl
	      << "  -V, --version                   print program version" << endl
	      << endl
	      << "Report bugs to " << EMAIL
	      << endl;
}
