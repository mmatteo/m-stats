// simple structure to encapsule all input parameter values

#include<TColor.h>
#include<istream>

struct ParInfo {
   std::string fName;
   double      fTrueValue;
   bool        fIsGlobal;
   double      fFitStartValue;
   double      fFitStartStep;
   double      fFitRangeMin;
   double      fFitRangeMax;
   double      fBestFitValue;
   double      fFitError;
   std::string fPdfName;
   EColor      fColor;

   // default constructor
   ParInfo() {}
   // customized constructor
   ParInfo(std::string parName, 
                 double parValue, 
                 bool isGlobal, 
                 double fitBestValue,
                 double fitError,
                 double fitStartValue, 
                 double fitStartStep,
                 double fitRangeMin, 
                 double fitRangeMax,
                 std::string pdfName,
                 EColor color) {
      fName            = parName;
      fTrueValue       = parValue; // in cts/100T/d
      fIsGlobal        = isGlobal;
      fBestFitValue    = fitBestValue;
      fFitError        = fitError;
      fFitStartValue   = fitStartValue; // in cts/100T/d
      fFitStartStep    = fitStartStep;
      fFitRangeMin     = fitRangeMin; // in cts/100T/d
      fFitRangeMax     = fitRangeMax; // in cts/100T/d
      fPdfName         = pdfName;
      fColor           = color;
   };

};

enum EConfig { kUndefined=0, kHist1=1, kHist2=2 };

std::vector<ParInfo> BuildParInfo(EConfig conf) {
   std::vector<ParInfo> v;
   if (conf == EConfig::kHist1) {

      double min = std::numeric_limits<double>::min() ;
      v.push_back(ParInfo(    "Be7",   47,  true, 0.0, 0.0, 0.0, 0.1, min,  100.0,       "Be7_nhn_1", kRed));
      v.push_back(ParInfo(    "CNO",    5,  true, 0.0, 0.0, 0.0, 0.1, min,   30.0,       "CNO_nhn_1", kMagenta));
      v.push_back(ParInfo(    "pep",   10,  true, 0.0, 0.0, 0.0, 0.1, min,   30.0,       "pep_nhn_1", kOrange));
      v.push_back(ParInfo(   "Kr85",    5,  true, 0.0, 0.0, 0.0, 0.1, min,   50.0,      "Kr85_nhn_1", kGray));
      v.push_back(ParInfo(  "Bi210",   20,  true, 0.0, 0.0, 0.0, 0.1, min,  100.0,     "Bi210_nhn_1", kPink));
      v.push_back(ParInfo(  "Po210",  300,  true, 0.0, 0.0, 0.0, 0.1, min, 1000.0,     "Po210_nhn_1", kCyan));
      v.push_back(ParInfo(  "ebK40", 0.15,  true, 0.0, 0.0, 0.0, 0.1, min,    5.0,   "Ext_K40_nhn_1", kSpring));
      v.push_back(ParInfo("ebBi214",  0.4,  true, 0.0, 0.0, 0.0, 0.1, min,    5.0, "Ext_Bi214_nhn_1", kTeal));
      v.push_back(ParInfo("ebTl208",  1.9,  true, 0.0, 0.0, 0.0, 0.1, min,   10.0, "Ext_Tl208_nhn_1", kViolet));
      v.push_back(ParInfo(    "C11",  56 , false, 0.0, 0.0, 0.0, 0.1, min, 1000.0,       "C11_nhn_1", kBlue));

   } else if (conf == EConfig::kHist2) {
      v.push_back(ParInfo(    "Be7",   47,  true, 0.0, 0.0, 0.0, 0.1, min,  100.0,       "Be7_nhn_1", kRed));
      v.push_back(ParInfo(    "CNO",    5,  true, 0.0, 0.0, 0.0, 0.1, min,   30.0,       "CNO_nhn_1", kMagenta));
      v.push_back(ParInfo(    "pep",   10,  true, 0.0, 0.0, 0.0, 0.1, min,   30.0,       "pep_nhn_1", kOrange));
      v.push_back(ParInfo(   "Kr85",    5,  true, 0.0, 0.0, 0.0, 0.1, min,   50.0,      "Kr85_nhn_1", kGray));
      v.push_back(ParInfo(  "Bi210",   20,  true, 0.0, 0.0, 0.0, 0.1, min,  100.0,     "Bi210_nhn_1", kPink));
      v.push_back(ParInfo(  "Po210",  300,  true, 0.0, 0.0, 0.0, 0.1, min, 1000.0,     "Po210_nhn_1", kCyan));
      v.push_back(ParInfo(  "ebK40", 0.15,  true, 0.0, 0.0, 0.0, 0.1, min,    5.0,   "Ext_K40_nhn_1", kSpring));
      v.push_back(ParInfo("ebBi214",  0.4,  true, 0.0, 0.0, 0.0, 0.1, min,    5.0, "Ext_Bi214_nhn_1", kTeal));
      v.push_back(ParInfo("ebTl208",  1.9,  true, 0.0, 0.0, 0.0, 0.1, min,   10.0, "Ext_Tl208_nhn_1", kViolet));
      v.push_back(ParInfo(    "C11",  56 , false, 0.0, 0.0, 0.0, 0.1, min, 1000.0,       "C11_nhn_1", kBlue));
   }
   return v;
}
