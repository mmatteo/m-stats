// Copyright (C) 2018 Matteo Agostini <matteo.agostini@ph.tum.de>

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
 * \class mst::MSModelTHnBMLF
 *
 * \brief
 * basic analysis modeling implementing a binned likelihood fit of an histogram
 * 
 * \details 
 * The user must set the pointer to a data histogram and to the pdfBulder. The
 * pdfBuilder is used within the likelihood function to create the proper PDF. Also
 * the expsoure of the data hist must set
 * 
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSModelTHnBMLF_H
#define MST_MSModelTHnBMLF_H

// ROOT libs
#include <THnBase.h>

// m-stats libs
#include "MSModel.h"
#include "MSPDFBuilderTHn.h"

namespace mst {

class MSModelTHnBMLF : public MSModelT<THnBase,MSPDFBuilderTHn>
{
   public:
      //! Constructor
      MSModelTHnBMLF(const std::string& name = ""): MSModelT(name) {}
      //! Destructor
      virtual ~MSModelTHnBMLF() {}

      //! function returning the negative log likelihood function to be 
      //! minimized (NLL)
      double NLogLikelihood(double* par) override;

      //! Check consistency between PDF's and data set
      bool AreInputHistsConsistent () override;
};

} // namespace mst

#endif //MST_MSModelTHnBMLF_H
