#include "MSMinimizer.h"

// root libs
#include "TString.h"

namespace mst {

const bool rootWrapper = false;

MSMinimizer* MSMinimizer::global_pointer = 0;

MSMinimizer::MSMinimizer(const std::string& name) : MSDataObject(name),
   fModelVector(0), fGlobalParMap(0), fLocalParMap(0),
   fMinuit(0), fMinuitErrorFlag(0), fNMigradFails(0),
   fMinuitMaxCalls(2000), fMinuitTollerance(1e-6),
   fMinNLL(0.0), fEDM(0.0), fCovQual(0)
{
   fMinuitArglist[0] = 0;
   fMinuitArglist[1] = 0;
   fModelVector = new MSModelVector();
   fLocalParMap = new MSParameterMap();
}

MSMinimizer::~MSMinimizer()
{
   if (fModelVector) {
      for (size_t i = 0; i < fModelVector->size(); i++)
         delete fModelVector->at(i);
      delete fModelVector;
      fModelVector   = 0;
   }

   if (fLocalParMap) {
      for (MSParameterMap::iterator it = fLocalParMap->begin();
            it != fLocalParMap->end(); ++it)
         delete it->second;
      delete fLocalParMap;
      fLocalParMap = 0;
   }

   if (fMinuit) {
      delete fMinuit;
      fMinuit = 0;
   }
}

TMinuit* MSMinimizer::InitializeMinuit()
{
   // Check if module list contains at least one model
   if (fModelVector->size() == 0) {
      std::cerr << "MSMinimizer::InitializeMinuit: module list empty"
                << std::endl;
      exit(1);
   }
   // Set global pointer
   global_pointer = this;

   // Fixed the pointer to the global parameter list
   fGlobalParMap = fModelVector->at(0)->GetParameters();

   // Initiazlie minuit
   if (fMinuit) delete fMinuit;
   fMinuit = new TMinuit(fGlobalParMap->size());
   fMinuit->SetFCN(&MSMinimizer::FCNNLLLikelihood);

   return fMinuit;
}

void MSMinimizer::SetMinuitErrVal(double errVal)
{
   if (!fMinuit) {
      std::cerr << "MSMinimizer::SetMinuitErrVal: minuit not initialized yet"
                << std::endl;
   }
   fMinuitArglist[0] = errVal;
   fMinuit->mnexcm("SET ERR", fMinuitArglist, 1, fMinuitErrorFlag);
}

void MSMinimizer::SetMinuitVerbosity(int level)
{
   if (!fMinuit) {
      std::cerr << "MSMinimizer::SetMinuitVerbosity: minuit not initialized yet"
                << std::endl;
   }
   fMinuit->SetPrintLevel(level);
}

void MSMinimizer::SyncFitParameters()
{
   // Check whether minuit has been last sync against this minimizer
   // and define value of global pointer
   bool forceUpdateAll = false;
   if (global_pointer != this) {
      forceUpdateAll = true;
      global_pointer = this;
   }

   // Initialize minuit if not done manually
   if (!fMinuit) InitializeMinuit();

   const MSParameterMap::const_iterator gItB = fGlobalParMap->begin();
   const MSParameterMap::const_iterator gItE = fGlobalParMap->end();

   // Parse parameter to minuit (only if different from previous call)
   for (MSParameterMap::const_iterator gIt = gItB; gIt != gItE; ++gIt) {
      const int d = distance(gItB, gIt);
      MSParameterMap::const_iterator lIt = fLocalParMap->find(gIt->first);

      // Intialize all parameters the first time the function is called or
      // if minuit was synced against a different MSMinimizer
      if (lIt == fLocalParMap->end() || forceUpdateAll) {
         if (fVerbosity) std::cerr << "MSMinimizer::SyncFitParameters: "
                                   << "par[" << d
                                   << "] \"" << gIt->first << "\""
                                   << " -> synced all fields"
                                   << std::endl;

         fMinuit->mnparm ( d, gIt->second->GetName().data(),
               gIt->second->GetFitStartValue(),
               gIt->second->GetFitStartStep(),
               gIt->second->GetRangeMin(),
               gIt->second->GetRangeMax(),
               fMinuitErrorFlag);

         if (gIt->second->IsFixed()) {
            if (fVerbosity) std::cerr << "MSMinimizer::SyncFitParameters: "
                                      << "par[" << d
                                      << "] \"" << gIt->first << "\""
                                      << " -> fixed"
                                      << std::endl;
            fMinuitArglist[0] = d+1;
            fMinuit->mnexcm("FIX", fMinuitArglist , 1, fMinuitErrorFlag);
         }

      // otherwise update only changed field
      } else if (gIt->second->IsFixed()) {

         if (gIt->second->GetFitStartValue() != lIt->second->GetFitStartValue()) {
            if (fVerbosity) std::cerr << "MSMinimizer::SyncFitParameters: "
                                      << "par[" << d
                                      << "] \"" << gIt->first << "\""
                                      << " -> synced starting value"
                                      << std::endl;
            fMinuitArglist[0] = d+1;
            fMinuitArglist[1] = gIt->second->GetFitStartValue();
            fMinuit->mnexcm("SET PAR",fMinuitArglist,2, fMinuitErrorFlag);
         }

         if (!lIt->second->IsFixed()) {
            if (fVerbosity) std::cerr << "MSMinimizer::SyncFitParameters: "
                                      << "par[" << d
                                      << "] \"" << gIt->first << "\""
                                      << " -> fixed"
                                      << std::endl;
            fMinuitArglist[0] = d+1;
            fMinuit->mnexcm("FIX", fMinuitArglist , 1, fMinuitErrorFlag);
         }

      } else {
         if (lIt->second->IsFixed()) {
            if (fVerbosity) std::cerr << "MSMinimizer::SyncFitParameters: "
                                      << "par[" << d
                                      << "] \"" << gIt->first << "\""
                                      << " -> released"
                                      << std::endl;
            fMinuitArglist[0] = d+1;
            fMinuit->mnexcm("RELEASE", fMinuitArglist , 1, fMinuitErrorFlag);
         }

         if (gIt->second->GetFitStartValue() != lIt->second->GetFitStartValue()||
               gIt->second->GetFitStartStep()  != lIt->second->GetFitStartStep() ||
               gIt->second->GetRangeMin()      != lIt->second->GetRangeMin() ||
               gIt->second->GetRangeMax()      != lIt->second->GetRangeMax() ) {
            if (fVerbosity) std::cerr << "MSMinimizer::SyncFitParameters: "
                                      << "par[" << d
                                      << "] \"" << gIt->first << "\""
                                      << " -> synced all fields"
                                      << std::endl;

            fMinuit->mnparm ( d, gIt->second->GetName().data(),
                                 gIt->second->GetFitStartValue(),
                                 gIt->second->GetFitStartStep(),
                                 gIt->second->GetRangeMin(),
                                 gIt->second->GetRangeMax(),
                                 fMinuitErrorFlag);
         }
      }
   }

   // Clear and Make local copy of the gloabal parameter map
   for (MSParameterMap::iterator it = fLocalParMap->begin();
         it != fLocalParMap->end(); ++it)
      delete it->second;
   fLocalParMap->clear();

   for (MSParameterMap::const_iterator gIt = gItB; gIt != gItE; ++gIt) {
      MSParameter* newPar = new MSParameter(*(gIt->second));
      fLocalParMap->insert(MSParameterPair(newPar->GetName(), newPar));
   }

}

void MSMinimizer::Migrad()
{
   // Sync parameters
   SyncFitParameters();
   // Set maxcalls
   fMinuitArglist[0] = fMinuitMaxCalls;
   // Set tolerance
   fMinuitArglist[1] = fMinuitTollerance;
   // Run actual minimization
   fMinuit->mnexcm("MINIMIZE", fMinuitArglist, 2, fMinuitErrorFlag);

   if (GetMinuitStatus()) fNMigradFails++;

   // Retrive fit results from minuit and store info
   MSParameterMap::const_iterator gIt0 = fGlobalParMap->begin();
   for (MSParameterMap::const_iterator gIt = fGlobalParMap->begin();
         gIt != fGlobalParMap->end(); ++gIt) {
      int d = distance(gIt0, gIt);
      double fitBestValue, fitBestValueErr;
      double fitRangeMin, fitRangeMax;
      TString name;
      int index;
      fMinuit->mnpout(d, name, fitBestValue, fitBestValueErr,
                      fitRangeMin, fitRangeMax, index);
      gIt->second->SetFitBestValue(fitBestValue);
      gIt->second->SetFitBestValueErr(fitBestValueErr);
   }

   // Retrive info about the status of the minimation
   double errdef;
   int npari, nparx;
   fMinuit->mnstat(fMinNLL,fEDM,errdef,npari,nparx,fCovQual);
}

void MSMinimizer::FCNNLLLikelihood(int & /*npar*/, double * /*grad*/,
      double &fval, double * par, int /*flag*/)
{
   fval = 0.0 ;
   MSModelVector* modelVector = global_pointer->fModelVector;

   for (unsigned int i = 0; i < modelVector->size(); i++)
      fval += modelVector->at(i)->NLogLikelihood(par);
}

} // namespace mst
