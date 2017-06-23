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
      MSTHnHandler(const std::string& name = "");
      //! Destructor
      virtual ~MSTHnHandler();

      //! Normalize histogram. The normalization is performed in the 
      //! user range if respectAxisUserRange is true. Otherwise by default it
      //! includes all bins, including over- and under-shot bins
      void NormalizeHists(bool respectAxisUserRange) { 
         fNormalize = true;
         fRespectUserRange = respectAxisUserRange;
      }

      //! Set the user range of a specific axis
      void SetRangeUser(int axisID, double min, double max) {
         m[axisID].min = min;
         m[axisID].max = max;
      }

      //! Rebin a specific axis
      void Rebin(int axisID, int ngroup) { m[axisID].ngroup = ngroup; }

      //! Reset tmp PDF 
      void Reset() { m.clear(); }

      //! Load histogram from file, manipulate it and return a copy
      THnBase* BuildHist(const std::string& fileName, 
                         const std::string& histName,
                         const std::string& newHistName);

   protected:
      struct axis {
         double  min = {0.0};
         double  max = {0.0};
         int     ngroup = {1};
      };
      std::map<int,axis> m;
      bool fNormalize        = {true};
      bool fRespectUserRange = {false};
};

} // namespace mst

#endif //MST_MSTHnHandler_H

