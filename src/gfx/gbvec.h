///////////////////////////////////////////////////////////////////////
//
// gbvec.h
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov 16 00:10:45 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GBVEC_H_DEFINED
#define GBVEC_H_DEFINED

#include "util/multivalue.h"

#include "geom/vec2.h"
#include "geom/vec3.h"

/// GbVec2 is a field class for holding @-D vectors/points/distances.
template <class T>
class GbVec2 : public geom::vec2<T>,
               public rutz::multi_value<T>
{
public:
  /// Default constructor, or set initial 2-D coordinates.
  GbVec2(T x_=T(), T y_=T());

  /// String conversion constructor.
  GbVec2(const rutz::fstring& s)
    : rutz::multi_value<T>(2)
  { rutz::multi_value<T>::set_string(s); }

  /// Virtual destructor.
  virtual ~GbVec2();

  /// Get the underlying geom::vec2 value.
        geom::vec2<T>& vec()       { return *this; }
  /// Get the underlying geom::vec2 value.
  const geom::vec2<T>& vec() const { return *this; }

  virtual rutz::fstring value_typename() const;

  virtual const T* const_begin() const;
};

/// GbVec3 is a field class for holding 3-D vectors/points/distances.
template <class T>
class GbVec3 : public geom::vec3<T>,
               public rutz::multi_value<T>
{
public:
  /// Default constructor, or set initial 3-D coordinates.
  GbVec3(T x_=T(), T y_=T(), T z_=T());

  /// String conversion constructor.
  GbVec3(const rutz::fstring& s)
    : rutz::multi_value<T>(3)
  { rutz::multi_value<T>::set_string(s); }

  /// Virtual destructor.
  virtual ~GbVec3();

  /// Get the underlying geom::vec3 value.
        geom::vec3<T>& vec()       { return *this; }
  /// Get the underlying geom::vec3 value.
  const geom::vec3<T>& vec() const { return *this; }

  virtual rutz::fstring value_typename() const;

  virtual const T* const_begin() const;
};

static const char vcid_gbvec_h[] = "$Id$ $URL$";
#endif // !GBVEC_H_DEFINED
