/** @file gfx/gbvec.cc a field class for 2-D vectors/points */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Nov 16 00:11:19 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GFX_GBVEC_CC_UTC20050626084025_DEFINED
#define GROOVX_GFX_GBVEC_CC_UTC20050626084025_DEFINED

#include "gfx/gbvec.h"

#include "rutz/fstring.h"

template <class T>
GbVec2<T>::GbVec2(T x_, T y_) :
  geom::vec2<T>(x_, y_),
  rutz::multi_value<T>(2)
{}

template <class T>
GbVec2<T>::~GbVec2() {}

template <class T>
rutz::fstring GbVec2<T>::value_typename() const
{
  return rutz::fstring("GbVec2");
}

template <class T>
const T* GbVec2<T>::const_begin() const { return &geom::vec2<T>::x(); }

template <class T>
GbVec3<T>::GbVec3(T x_, T y_, T z_) :
  geom::vec3<T>(x_, y_, z_),
  rutz::multi_value<T>(3)
{}

template <class T>
GbVec3<T>::~GbVec3() {}

template <class T>
rutz::fstring GbVec3<T>::value_typename() const
{
  return rutz::fstring("GbVec3");
}

template <class T>
const T* GbVec3<T>::const_begin() const { return geom::vec3<T>::data(); }

template class GbVec2<int>;
template class GbVec2<double>;

template class GbVec3<int>;
template class GbVec3<double>;

static const char __attribute__((used)) vcid_groovx_gfx_gbvec_cc_utc20050626084025[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_GBVEC_CC_UTC20050626084025_DEFINED
