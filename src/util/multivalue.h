///////////////////////////////////////////////////////////////////////
//
// multivalue.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 16:45:38 2001
// written: Wed Sep 25 18:59:01 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MULTIVALUE_H_DEFINED
#define MULTIVALUE_H_DEFINED

#include "util/value.h"

/// A Value subclass representing a (small) set of homogeneous values.
template <class T>
class TMultiValue : public Value
{
public:
  virtual ~TMultiValue() {}

  virtual fstring getNativeTypeName() const = 0;

  void doPrintTo(STD_IO::ostream& os) const;
  void doScanFrom(STD_IO::istream& is);

protected:
  virtual unsigned int numValues() const = 0;
  virtual const T*   constBegin() const = 0;
          const T*   constEnd()   const { return constBegin() + numValues(); }
                T* mutableBegin()       { return const_cast<T*>(constBegin()); }
                T* mutableEnd()         { return const_cast<T*>(constEnd()); }
};

static const char vcid_multivalue_h[] = "$Header$";
#endif // !MULTIVALUE_H_DEFINED
