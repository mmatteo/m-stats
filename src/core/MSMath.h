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
 * \class mst::MSMath
 *
 * \brief 
 * Namespace containing useful mathematical functions, typically used to define
 * the negative log likelihood of statistical models
 *
 * \details 
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSMath_H
#define MST_MSMath_H

namespace mst {

namespace MSMath {

   //! Log of a Gaussian distribution
   double LogGaus (double x, double mean, double sigma);
   //! Log of a Poissonian distribution
   double LogPoisson (double x, double lambda);
   //! Log of an exponential distribution
   double LogExp (double x, double limit, double quantile=.9, double offset =0);

} // namespace MSMath

} // namespace mst

#endif // MST_MSMath_H
