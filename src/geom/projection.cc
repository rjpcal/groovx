/** @file geom/projection.cc transform 3-d points with 4x4 modelview
    and projection matrices */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Oct 28 16:55:06 2004
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

#ifndef GROOVX_GEOM_PROJECTION_CC_UTC20050626084023_DEFINED
#define GROOVX_GEOM_PROJECTION_CC_UTC20050626084023_DEFINED

#include "projection.h"

#include "geom/rect.h"
#include "geom/txform.h"
#include "geom/vec3.h"

#include "rutz/trace.h"

using geom::recti;
using geom::txform;
using geom::vec3d;

vec3d geom::unproject(const txform& modelview,
                      const txform& projection,
                      const recti& viewport,
                      const vec3d& screen)
{
GVX_TRACE("geom::unproject");

  const txform pm = projection.mtx_mul(modelview);
  const txform pmi = pm.inverted();

  const vec3d screen2(2*(screen.x() - viewport.left())/viewport.width() - 1,
                      2*(screen.y() - viewport.bottom())/viewport.height() - 1,
                      2*(screen.z()) - 1);

  return pmi.apply_to(screen2);
}

vec3d geom::project(const txform& modelview,
                    const txform& projection,
                    const recti& viewport,
                    const vec3d& world_pos)
{
GVX_TRACE("geom::project");

  const txform pm = projection.mtx_mul(modelview);

  const vec3d s1 = pm.apply_to(world_pos);

  return vec3d(viewport.left()   + 0.5 * (s1.x()+1) * viewport.width(),
               viewport.bottom() + 0.5 * (s1.y()+1) * viewport.height(),
               0.5 * (s1.z()+1));
}

static const char __attribute__((used)) vcid_groovx_geom_projection_cc_utc20050626084023[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GEOM_PROJECTION_CC_UTC20050626084023_DEFINED
