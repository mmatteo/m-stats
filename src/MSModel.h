// Copyright (C) 2014 Matteo Agostini <matteo.agostini@ph.tum.de>

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

/*!
 * \class mst::MSModel
 *
 * \brief 
 * Base class for all analysis modules.
 *
 * \details 
 * MSModel is a pure virutal class providing a general interace for  all 
 * analysis models. It provides virtual functions to be used by the minimizer.
 * 
 * MSModelT...
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSModel_H
#define MST_MSModel_H

// c/c++ libs
#include <vector>

// m-stats libs
#include "MSObject.h"
#include "MSParameter.h"

namespace mst {

class MSModel : public MSObject
{
   public:
      //! Constructor
      MSModel(const std::string& name = "");
      //! Destructor
      virtual ~MSModel();

    //
    // Parameters:
    //
   public:
      //! Get  pointer to the vector of parameters
      MSParameterMap* GetParameters() { return fParameters; }
      //! Get const pointer to the vector of parameters
      const MSParameterMap* GetParameters() const { return fParameters; }

      //! Add new parameter (the function takes ownership of the object)
      void AddParameter(MSParameter* parameter);
      //! Get pointer to a parameter (call exit if the parameter is not found)
      MSParameter* GetParameter(const std::string& localName)  const;

      //! Get the vector of parameters registered in an instance of the class
      const std::vector<std::string>* GetLocalParameters() const { return fParNameList; }
      //! Get the number of parameters registered in an instance of the class
      unsigned int GetNLocalParameters() { return fParNameList->size(); }


   protected:
      //! Get iterator to a parameter
      MSParameterMap::iterator GetParameterIterator(const std::string& localName) const;
      //! Get index of a parameter (use names without local/global prefix)
       unsigned int GetParameterIndex(const std::string& localName) const;
      //! Get parameter value from Minuit array (use names without local/global prefix)
       double GetMinuitParameter(double* par, const std::string& localName) const {
          return par[GetParameterIndex(localName)];
       }
      //! Get the local/global name  (the foramt is {global:local}.name)
      std::string GetGlobalName (const std::string& name, bool isGlobal = false) const {
         return (isGlobal ? "global" : GetName()) + "." + name;
      }

    //
    // Functions to overload in the user models
    //
    public:
      //! Virtual unction to define the analysis parameters
      virtual void InitializeParameters() = 0;

      //! Virtual function to set the NLogLikelihood function
      virtual double NLogLikelihood(double* parameters) = 0;

    //
    // Parameter of interest for the model
    //
    public:
      //! Set the expsosure of data set
      void SetExposure (double exposure) {fExposure = exposure;}
      //! Get the expsosure of data set
      double GetExposure () const {return fExposure;}

    //
    // Class members
    //
   protected:
      //! Pointer to the global map of parameters
      static MSParameterMap* fParameters;
      //! names of the parameters registered from an instance of the class
      std::vector<std::string>* fParNameList {nullptr};
      //! Exposure of the data set
      double fExposure = 0.0;
};

// Template class inheriting from MSModel to handle a data set and pdfBuilder
template <typename TData, typename TPDF>
class MSModelT: public MSModel {
   public:
      //! Constructor
      MSModelT(const std::string& name = ""): MSModel(name) {}
      //! Destructor
      virtual ~MSModelT() { delete fDataSet; delete fPDFBuilder;}

      //! Virtual function from MSModel.
      //! To be overloaded in the concrete analysis module.
      virtual void InitializeParameters() override = 0;
      //! Virtual function from MSModel to be overloaded in the concrete class
      //! To be overloaded in the concrete analysis module.
      virtual double NLogLikelihood(double* par) override = 0;

      //! Set data set 
      void SetDataSet(TData* dataSet) { delete fDataSet; fDataSet = dataSet; }
      //! Get pointer to the data set
      const TData* GetDataSet() const { return fDataSet; }

      //! Set pdf builder
      void SetPDFBuilder(TPDF* pdf) {delete fPDFBuilder; fPDFBuilder = pdf;}
      //! Get the pointer to the pdf builder
      TPDF* GetPDFBuilder() const {return fPDFBuilder;}

   protected:
      //! pointer to the data set 
      const TData* fDataSet {nullptr}; 
      //! Pointer to PDFBuilder
      TPDF* fPDFBuilder {nullptr};
};

} // namespace mst

#endif // MST_MSModel_H
