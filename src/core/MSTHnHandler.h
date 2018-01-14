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

/*!
 * \class mst::MSTHnHandler
 *
 * \brief 
 * class used for handling THn
 * 
 * \details 
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSTHnHandler_H
#define MST_MSTHnHandler_H

// c/c++ libs
#include <map>

// ROOT libs
#include <THn.h>

// m-stats libs
#include "MSObject.h"

namespace mst {

class MSTHnHandler : public MSObject
{
   public:
      //! Constructor
      MSTHnHandler(const std::string& name = "") : MSObject (name) {}
      //! Destructor
      virtual ~MSTHnHandler() {}


      void ProjectToAxis(const std::vector<int>& axisID) {
         for (const auto& i : axisID) fProjectID.push_back(i);
      }
      

      //! Set the user range of a specific axis
      void ProjectToAxis(const int axisID, const double min, const double max) {
         while (axisID >= fAxis.size()) fAxis.resize(axisID);
         fAxis.at(axisID).fMin = min;
         fAxis.at(axisID).fMax = max;
         fAxis.at(axisID).fSetRange = true;
      }

      //! Rebin a specific axis
      void Rebin(const int axisID, const int ngroup) { 
         while (axisID >= fAxis.size()) fAxis.resize(axisID);
         fAxis.at(axisID).fNgroup = ngroup;
      }

      //! Normalize histogram. The normalization is performed in the 
      //! user range if respectAxisUserRange is true. Otherwise by default it
      //! includes all bins, including over- and under-shot bins
      void NormalizeHists(const bool respectAxisUserRange = false) { 
         fNormalize = true;
         fRespectUserRange = respectAxisUserRange;
      }

      //! Load histogram from file, manipulate it and return a copy
      THn* BuildHist(const std::string& fileName, 
                     const std::string& histName,
                     const std::string& newHistName);

      //! Reset settings
      void Reset() { 
         fProjectID.clear(); fAxis.clear(); 
         fNormalize = true; fRespectUserRange = false; 
      }

   protected:
      struct axis {
         bool   fSetRange = {false};
         double fMin      = {0.0};
         double fMax      = {0.0};
         int    fNgroup   = {1};
      };
      std::vector<int> fProjectID;
      std::vector<axis> fAxis;

      bool fNormalize        = {true};
      bool fRespectUserRange = {false};
};

} // namespace mst

#endif //MST_MSTHnHandler_H

