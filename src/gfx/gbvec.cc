///////////////////////////////////////////////////////////////////////
//
// gbvec.cc
//
// Copyright (c) 2000-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Nov 16 00:11:19 2000
// commit: $Id$
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

#ifndef GBVEC_CC_DEFINED
#define GBVEC_CC_DEFINED

#include "gfx/gbvec.h"

#include "util/fstring.h"

template <class T>
GbVec2<T>::GbVec2(T x_, T y_) :
  Gfx::Vec2<T>(x_, y_),
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
const T* GbVec2<T>::const_begin() const { return &Gfx::Vec2<T>::x(); }

template <class T>
GbVec3<T>::GbVec3(T x_, T y_, T z_) :
  Gfx::Vec3<T>(x_, y_, z_),
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
const T* GbVec3<T>::const_begin() const { return Gfx::Vec3<T>::data(); }

template class GbVec2<int>;
template class GbVec2<double>;

template class GbVec3<int>;
template class GbVec3<double>;

static const char vcid_gbvec_cc[] = "$Header$";
#endif // !GBVEC_CC_DEFINED
