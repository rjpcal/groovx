///////////////////////////////////////////////////////////////////////
//
// multivalue.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 16:45:38 2001
// written: Fri Jan 18 16:06:55 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MULTIVALUE_H_DEFINED
#define MULTIVALUE_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H_DEFINED)
#include "util/value.h"
#endif

template <class T>
class TMultiValue : public Value
{
public:
  virtual fstring getNativeTypeName() const = 0;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

protected:
  virtual unsigned int numValues() const = 0;
  virtual const T*   constBegin() const = 0;
          const T*   constEnd()   const { return constBegin() + numValues(); }
                T* mutableBegin()       { return const_cast<T*>(constBegin()); }
                T* mutableEnd()         { return const_cast<T*>(constEnd()); }
};

static const char vcid_multivalue_h[] = "$Header$";
#endif // !MULTIVALUE_H_DEFINED
