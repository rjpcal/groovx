/** @file gfx/gbvec.h a field class for 2-D vectors/points */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Thu Nov 16 00:10:45 2000
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_GFX_GBVEC_H_UTC20050626084023_DEFINED
#define GROOVX_GFX_GBVEC_H_UTC20050626084023_DEFINED

#include "rutz/multivalue.h"

#include "geom/vec2.h"
#include "geom/vec3.h"

/// GbVec2 is a field class for holding 2-D vectors/points/distances.
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

  /// Get the underlying geom::vec2 value.
        geom::vec2<T>& vec()       { return *this; }
  /// Get the underlying geom::vec2 value.
  const geom::vec2<T>& vec() const { return *this; }

  virtual rutz::fstring value_typename() const override;

  virtual const T* const_begin() const override;
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

  /// Get the underlying geom::vec3 value.
        geom::vec3<T>& vec()       { return *this; }
  /// Get the underlying geom::vec3 value.
  const geom::vec3<T>& vec() const { return *this; }

  virtual rutz::fstring value_typename() const override;

  virtual const T* const_begin() const override;
};

#endif // !GROOVX_GFX_GBVEC_H_UTC20050626084023_DEFINED
