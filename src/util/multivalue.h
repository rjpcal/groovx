///////////////////////////////////////////////////////////////////////
//
// multivalue.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Aug 22 16:45:38 2001
// written: Sun Aug 26 08:35:09 2001
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
  virtual Value* clone() const = 0;
  virtual fstring getNativeTypeName() const = 0;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual const char* get(Util::TypeCue<const char*>) const;

  virtual void assignTo(Value& other) const;
  virtual void assignFrom(const Value& other);

protected:
  virtual unsigned int numValues() const = 0;
  virtual const T*   constBegin() const = 0;
          const T*   constEnd()   const { return constBegin() + numValues(); }
                T* mutableBegin()       { return const_cast<T*>(constBegin()); }
                T* mutableEnd()         { return const_cast<T*>(constEnd()); }
};

static const char vcid_multivalue_h[] = "$Header$";
#endif // !MULTIVALUE_H_DEFINED
