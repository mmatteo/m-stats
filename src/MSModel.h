/*
 * Base class for analysis modules.
 */

#ifndef MST_MSModel_H
#define MST_MSModel_H

#include "MSDataObject.h"

#include "MSParameter.h"
#include "MSDataSet.h"

namespace mst {

class MSModel : public MSDataObject
{
   public:
      //! Constructor
      MSModel(const std::string& name = "");
      //! Destructor
      ~MSModel();

    //
    // Parameters:
    //
   public:
      //! Get  pointer to the vector of parameters
      MSParameterMap* GetParameters() { return fParameters; }

      //! Get const pointer to the vector of parameters
      const MSParameterMap * GetParameters() const { return fParameters; }

      //! Get the number of parameters
      unsigned int GetNParameters() const { return fParameters->size(); }

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
    // Data set
    //
   public:
      //! Set data set (the function takes ownership of the object)
      void SetDataSet(MSDataSet* dataSet);

      //! Get pointer to the data set
      const MSDataSet* GetDataSet() const { return fDataSet; }

      //! Get number of points in the data set
      unsigned int GetNDataPoints() const {
         return (fDataSet ? fDataSet->GetNDataPoints() : 0);
      }

      //! Get pointer to a data point
      const MSDataPoint* GetDataPoint(int index) const {
           return (fDataSet ? fDataSet->GetDataPoint(index) : 0);
      }

      //! Get value of an observable in a data point
      double GetDataPoint(int index, const std::string& field) const {
         return (fDataSet ? fDataSet->GetDataPoint(index)->Get(field) : 0);
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


   protected:
      //! Pointer to the global map of parameters
      static MSParameterMap* fParameters;
      //! Pointer to the data set
      MSDataSet* fDataSet;
};

typedef std::vector<MSModel*> MSModelVector;

} // namespace mst

#endif // MST_MSModel_H
