///////////////////////////////////////////////////////////////////////
//
// gbvec.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Thu Nov 16 00:10:45 2000
// written: Wed Mar 19 17:56:06 2003
// $Id$
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

#ifndef GBVEC_H_DEFINED
#define GBVEC_H_DEFINED

#include "util/multivalue.h"

#include "gx/vec3.h"

/// GbVec3 is a field class for holding 3-D vectors/points/distances.
template <class T>
class GbVec3 : public Gfx::Vec3<T>,
               public TMultiValue<T>
{
public:
  /// Default constructor, or set initial 3-D coordinates.
  GbVec3(T x_=T(), T y_=T(), T z_=T());

  /// String conversion constructor.
  GbVec3(const fstring& s) : TMultiValue<T>(3) { setFstring(s); }

  /// Virtual destructor.
  virtual ~GbVec3();

  // These help to disambiguate function calls to set()
        Gfx::Vec3<T>& vec()       { return *this; }
  const Gfx::Vec3<T>& vec() const { return *this; }

  virtual fstring getNativeTypeName() const;

  virtual const T* constBegin() const;
};

static const char vcid_gbvec_h[] = "$Header$";
#endif // !GBVEC_H_DEFINED
