///////////////////////////////////////////////////////////////////////
//
// projection.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 28 16:53:38 2004
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

#ifndef PROJECTION_H_DEFINED
#define PROJECTION_H_DEFINED

namespace geom
{
  class txform;
  template <class T> class rect;
  template <class T> class vec3;

  vec3<double> unproject(const txform& modelview,
                         const txform& projection,
                         const rect<int>& viewport,
                         const vec3<double>& screen_pos);

  vec3<double> project(const txform& modelview,
                       const txform& projection,
                       const rect<int>& viewport,
                       const vec3<double>& world_pos);
}

static const char vcid_projection_h[] = "$Header$";
#endif // !PROJECTION_H_DEFINED
