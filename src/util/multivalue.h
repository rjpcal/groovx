///////////////////////////////////////////////////////////////////////
//
// multivalue.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Aug 22 16:45:38 2001
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
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
  TMultiValue(int num);
  virtual ~TMultiValue();

  /// Get a string describing the underlying native type.
  virtual fstring getNativeTypeName() const = 0;

  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

protected:
  /// Returns a const pointer to the start of the underlying storage.
  virtual const T*   constBegin() const = 0;
  /// Returns a const pointer to one-past-the-end of the underlying storage.
          const T*   constEnd()   const { return constBegin() + itsNumValues; }
  /// Returns a non-const pointer to the start of the underlying storage.
                T* mutableBegin()       { return const_cast<T*>(constBegin()); }
  /// Returns a non-const pointer to one-past-the-end of the underlying storage.
                T* mutableEnd()         { return const_cast<T*>(constEnd()); }

private:
  int itsNumValues;
};

static const char vcid_multivalue_h[] = "$Header$";
#endif // !MULTIVALUE_H_DEFINED
