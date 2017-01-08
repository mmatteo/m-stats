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
 * \class mst::MSParameter
 *
 * \brief 
 * Parameter class
 *
 * \details 
 * Base class for the parameters. It contains all info concerning the parameter.
 * It is used to pass and store info during the fit
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSParameter_H
#define MST_MSParameter_H

// c/c++ libs
#include <string>
#include <vector>
#include <map>
#include <iostream>

// m-stats libs
#include "MSObject.h"

namespace mst {

class MSParameter : public MSObject
{
   public:
      //! Constructor
      MSParameter(const std::string& name = "");
      //! Destructor
      virtual ~MSParameter();

      //! Enum defining the type of parameter
      enum class EMSParameterType : int {
         kTypeUndefined = 0, kPoi = 1, kNuisance = 2, kInput = 3
      };
      //! Enum defining the type of variable used to represent the parameter
      enum class  EMSVariableType : int {
         kVarUndefined = 0, kShort = 1, kInt = 2, kFloat = 3, kDouble = 4
      };

      //! Set the type of parameter
      void SetParameterType(MSParameter::EMSParameterType parameterType) {
         fParameterType = parameterType;
      }
      //! Get the type of parameter
      inline MSParameter::EMSParameterType GetParameterType() const {
         return fParameterType;
      }
      //! Flag the parameter as parameter of interest
      void SetPoi() { fParameterType = EMSParameterType::kPoi; }
      //! Flag the parameter as parameter of nuisance
      void SetNuisance() { fParameterType = EMSParameterType::kNuisance; }
      //! Flag the parameter as  input parameter of the analysis
      void SetInput() { fParameterType = EMSParameterType::kInput; }
      //! Check if the parameter is of interest
      inline bool IsPoi() const { return (fParameterType == EMSParameterType::kPoi); }
      //! Check if the parameter is nuisance
      inline bool IsNuisance() const { return (fParameterType == EMSParameterType::kNuisance); }
      //! Check if the parameter is an input of the analysis
      inline bool IsInput() const { return (fParameterType == EMSParameterType::kInput); }

      //! Set the type of variables used to represent the parameter
      void SetVariableType(EMSVariableType variableType) {
         fVariableType = variableType;
      }
      //! Get the type of variables used to represent the parameter
      inline MSParameter::EMSVariableType GetVariableType() const {
         return fVariableType;
      }

      //! Set fixed parameter
      void SetFixed(bool fixed = true) { fFixed = fixed; }
      //! Fix parameter
      void FixTo(double value) { fFixed = true; SetFitStartValue(value, false); }
      //! Release the parameter
      void Release() { 
         fFixed = false; 
         fFitStartValueSet = fFitStartValueSetDefault;
         fFitStartValue = fFitStartValueDefault;
      }
      //! Check if the parameter is fixed
      bool IsFixed() const { return fFixed; }
      //! Set gloabal parameter
      void SetGlobal(bool global = true) { fGlobal = global; }
      //! Check if the parameter is global
      bool IsGlobal() const { return fGlobal; }

      //! Free lower edge of the range
      void SetNoRangeMin() { fRangeMinSet = false; }
      //! Free upper edge of the range
      void SetNoRangeMax() { fRangeMaxSet = false; }
      //! Free range
      void SetNoRange()    { SetNoRangeMin(); SetNoRangeMax(); }

      //! Check if the lower edge of the range is set
      inline bool IsRangeMinSet() const { return fRangeMinSet; }
      //! Check if the upper edge of the range is set
      inline bool IsRangeMaxSet() const { return fRangeMaxSet; }
      //! Check if the range is set
      inline bool IsRangeSet() const {
         return IsRangeMinSet() && IsRangeMaxSet();
      }

      //! Set lower edge of the range
      void SetRangeMin(double min) { fRangeMinSet = true; fRangeMin = min; }
      //! Set upper edge of the range
      void SetRangeMax(double max) { fRangeMaxSet = true; fRangeMax = max; }
      //! Set range
      void SetRange(double min, double max) {
         SetRangeMin(min); SetRangeMax(max);
      }

      //! Get range lower edge
      inline double GetRangeMin() const { return fRangeMinSet ? fRangeMin : 0; }
      //! Get range upper edge
      inline double GetRangeMax() const { return fRangeMaxSet ? fRangeMax : 0; }
      //! Get range width
      inline double GetRangeWidth() const {
         return (GetRangeMax() - GetRangeMin());
      }

      //! Set fit starting value of the parameter
      void SetFitStartValue(double value, bool setDefault = true) {
         fFitStartValueSet = true; fFitStartValue = value;
         if (setDefault) { 
            fFitStartValueSetDefault = true; 
            fFitStartValueDefault = value; 
         }
      }
      //! Check if the fit starting value of the par is set
      inline bool IsFitStartValueSet() const { return fFitStartValueSet; }
      //! Set fit starting value of the parameter
      inline double GetFitStartValue() const {
         return fFitStartValueSet ?  fFitStartValue : (fRangeMax - fRangeMin)/2;
      }

      void SetFitStartStep(double step)     { fFitStartStep = step; }
      inline double GetFitStartStep() const {
         return fFitStartStep ? fFitStartStep : GetRangeWidth()/100.;
      }
      //! Check if the value is at the range limits
      inline bool IsAtLimit(double value) const {
         return IsRangeSet() && (value==GetRangeMin() || value==GetRangeMax());
      }

      //! Print parameter info
      void PrintSummary() const;

      //! Set best fit value
      void SetFitBestValue(double value) { fFitBestValue = value; }
      //! Set fit lower limit
      void SetFitLowerLimit(double limit) { fFitLowerLimit = limit; }
      //! Set fit upper limit
      void SetFitUpperLimit(double limit) { fFitUpperLimit = limit; }
      //! Set best fit value error
      void SetFitBestValueErr(double error) { fFitBestValueErr = error;  }

      //! Get best fit value
      inline double GetFitBestValue()  const { return fFitBestValue; }
      //! Get best fit value error
      inline double GetFitBestValueErr()  const { return fFitBestValueErr; }
      //! Get fit lower limit
      inline double GetFitLowerLimit() const { return fFitLowerLimit; }
      //! Get fit upper limit
      inline double GetFitUpperLimit() const { return fFitUpperLimit; }

      //! Reset fit results
      void ResetFitResult();

   private:
      //! Flag storing the type of parameter
      EMSParameterType fParameterType {EMSParameterType::kTypeUndefined};
      //! Flag of type of variable used to represent the parameter
      EMSVariableType fVariableType {EMSVariableType::kVarUndefined};

      //! Flag identifying fixed parameter
      bool fFixed {false};
      //! Flag identifying global parameter
      bool fGlobal {false};

      //! Range edges set
      bool fRangeMinSet {false}, fRangeMaxSet {false};
      //! Range edges value
      double fRangeMin {0.0}, fRangeMax {0.0};

      //! Temporary starting value for the fit
      double fFitStartValue {0.0};
      //! Temporary starting value for the fit set
      bool fFitStartValueSet {false};
      //! Default starting value for the fit
      double fFitStartValueDefault {0.0};
      //! Default starting value for the fit set
      bool fFitStartValueSetDefault {false};
      //! Starting fit steps
      double fFitStartStep {0.1};

      //! Fit results
      double fFitBestValue    {0.0}, 
             fFitBestValueErr {0.0}, 
             fFitLowerLimit   {0.0}, 
             fFitUpperLimit   {0.0};

};

//! Vector of parameters used to build up a model
using MSParameterMap =  std::map <std::string, MSParameter*>;
using MSParameterPair = std::pair<std::string, MSParameter*>;

} // namespace mst

#endif  // MST_MSParameter_H
