///////////////////////////////////////////////////////////////////////
//
// arrayvalue.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov  4 15:32:47 2002
// written: Tue Nov  5 05:23:32 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ARRAYVALUE_H_DEFINED
#define ARRAYVALUE_H_DEFINED

#include "util/minivec.h"
#include "util/strings.h"
#include "util/value.h"

#include <iomanip>

/// A Value subclass representing a fixed-size set of homogeneous values.
template <class T>
class TArrayValue : public Value
{
public:
  TArrayValue() : itsArray() {}
  TArrayValue(const fstring& s) { setFstring(s); }
  virtual ~TArrayValue() {}

  virtual fstring getNativeTypeName() const
  {
    return fstring("TArrayValue");
  }

  virtual void printTo(STD_IO::ostream& os) const
  {
    for (unsigned int i = 0; i < itsArray.size(); ++i)
      os << itsArray[i] << "   ";
  }

  virtual void scanFrom(STD_IO::istream& is)
  {
    minivec<T> newarray;

    while (!is.eof() && !is.fail())
      {
        T val;
        is >> val >> std::ws;
        newarray.push_back(val);
      }
    itsArray.swap(newarray);
  }

        minivec<T>& arr()       { return itsArray; }
  const minivec<T>& arr() const { return itsArray; }

private:
  minivec<T> itsArray;
};

static const char vcid_arrayvalue_h[] = "$Header$";
#endif // !ARRAYVALUE_H_DEFINED
