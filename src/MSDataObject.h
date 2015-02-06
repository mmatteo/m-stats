/*
 * Base class for all objects. It provides a name and an index.
 */

#ifndef _MSDataObject_HH
#define _MSDataObject_HH

// c++ libs
#include <string>

namespace mst {

class MSDataObject
{
   public:
      //! Constructor
      MSDataObject(const std::string& name = "", int index = 0):
         fName(name), fIndex(index), fVerbosity(0) {}
      //! Destructor
      virtual ~MSDataObject() {}

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
      std::string fName;
      //! Object index
      int fIndex;
      //! Flag setting the verbosity level
      int fVerbosity;
};

} // namespace mst

#endif // _MSDataObject_HH
