///////////////////////////////////////////////////////////////////////
//
// span.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Oct 25 12:49:03 2004
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

#ifndef SPAN_H_DEFINED
#define SPAN_H_DEFINED

namespace geom
{
  template <class V>
  class span
  {
  public:
    span()         : lo(),  hi()  {} // init to zero
    span(V l, V h) : lo(l), hi(h) {}

    span<V>& operator=(const span<V>& i)
    { lo = i.lo; hi = i.hi; return *this; }

    V lo;
    V hi;
  };
}

static const char vcid_span_h[] = "$Header$";
#endif // !SPAN_H_DEFINED
