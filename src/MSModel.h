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
 * Pure virutal class providing a general interace for  all analysis models. 
 * It provides virtual functions to be used by the minimizer.
 *
 * \author Matteo Agostini
 */


#ifndef MST_MSModel_H
#define MST_MSModel_H

#include "MSObject.h"
#include "MSParameter.h"
#include "MSDataSet.h"

#include <vector>

#include <THn.h>

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

      //! Get the number of parameters registered in an instance of the class
      unsigned int GetNLocalParameters() { return fParNameList->size(); }

      //! Get the vector of parameters registered in an instance of the class
      const std::vector<std::string>* GetLocalParameters() const { return fParNameList; }

      //! Add new parameter (the function takes ownership of the object)
      void AddParameter(MSParameter* parameter);

      //! Get pointer to a parameter (call exit if the parameter is not found)
      MSParameter* GetParameter(const std::string& localName)  const;

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
    // Fit
    //
   public:
      //! Get the best fit value for a parameter
      double GetBestFitParameter(const std::string& name) const {
         return GetParameter(name) ? GetParameter(name)->GetFitBestValue() : 0;
      }

      //! Get the best fit value error for a parameter
      double GetBestFitParameterErr(const std::string& name) const {
         return GetParameter(name) ? GetParameter(name)->GetFitBestValueErr() : 0;
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
      //! Set the expsosure of data set
      void SetExposure (double exposure) {fExposure = exposure;}
      //! Get the expsosure of data set
      double GetExposure () const {return fExposure;}

   protected:
      //! Pointer to the global map of parameters
      static MSParameterMap* fParameters;

      //! names of the parameters registered from an instance of the class
      std::vector<std::string>* fParNameList;

      //! Exposure of the data set
      double fExposure;

};

template <typename T>
class MSModelT: public MSModel {
   public:
      MSModelT(const std::string& name = ""): MSModel(name), fDataSet(nullptr) {}
      virtual ~MSModelT() { delete fDataSet; }

      //! Virtual function from MSModel.
      //! To be overloaded in the concrete analysis module.
      virtual void InitializeParameters() = 0;

      //! Virtual function from MSModel to be overloaded in the concrete class
      //! To be overloaded in the concrete analysis module.
      virtual double NLogLikelihood(double* par) = 0;

      //! Set data set 
      void SetDataSet(T* dataSet) { delete fDataSet; fDataSet = dataSet; }

      //! Get pointer to the data set
      const T* GetDataSet() const { return fDataSet; }

   protected:
      const T* fDataSet; 
};


using MSModelVector = std::vector<MSModel*>;
using MSModelTHn = MSModelT<THn>;
using MSModelDataSet = MSModelT<MSDataSet>;


} // namespace mst

#endif // MST_MSModel_H
