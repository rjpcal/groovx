///////////////////////////////////////////////////////////////////////
//
// multivalue.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 16:45:38 2001
// written: Mon Jan 13 11:08:25 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MULTIVALUE_H_DEFINED
#define MULTIVALUE_H_DEFINED

#include "util/value.h"

/// A Value subclass representing a fixed-size set of homogeneous values.
template <class T>
class TMultiValue : public Value
{
public:
  TMultiValue(int num) : itsNumValues(num) {}
  virtual ~TMultiValue() {}

  virtual fstring getNativeTypeName() const = 0;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

protected:
  virtual const T*   constBegin() const = 0;
          const T*   constEnd()   const { return constBegin() + itsNumValues; }
                T* mutableBegin()       { return const_cast<T*>(constBegin()); }
                T* mutableEnd()         { return const_cast<T*>(constEnd()); }

private:
  int itsNumValues;
};

static const char vcid_multivalue_h[] = "$Header$";
#endif // !MULTIVALUE_H_DEFINED
