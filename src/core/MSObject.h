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
 * \class mst::MSObject
 *
 * \brief 
 * Base class for all objects. 
 *
 * \details 
 * It provides a general type, a name and an index.
 *
 * \author Matteo Agostini
 */

#ifndef MST_MSObject_H
#define MST_MSObject_H

// c/c++ libs
#include <string>

namespace mst {

class MSObject
{
   public:
      //! Constructor
      MSObject(const std::string& name = "", int index = 0):
         fName(name), fIndex(index) {}
      //! Destructor
      virtual ~MSObject() {}

      //! Set name of the object
      void SetName(const std::string& name) { fName = name; }
      //! Get name of the object
      std::string GetName() const { return fName; }

      //! Set index of the object
      void SetIndex(int index) { fIndex = index; }
      //! Get index of the object
      int GetIndex() const { return fIndex; }

      //! Set verbosity level
      void SetVerbosityLevel(int level) { fVerbosity = level; }
      //! Get verbosity level
      int GetVerbosityLevel() const { return fVerbosity; }

   protected:
      //! Object name
      std::string fName {""};
      //! Object index
      int fIndex {0};
      //! Flag setting the verbosity level
      int fVerbosity {0};
};

} // namespace mst

#endif // MST_MSObject_H
