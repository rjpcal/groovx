///////////////////////////////////////////////////////////////////////
//
// projection.cc
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Oct 28 16:55:06 2004
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

#ifndef PROJECTION_CC_DEFINED
#define PROJECTION_CC_DEFINED

#include "projection.h"

#include "geom/rect.h"
#include "geom/txform.h"
#include "geom/vec3.h"

#include "util/trace.h"

using geom::recti;
using geom::txform;
using geom::vec3d;

vec3d geom::unproject(const txform& modelview,
                      const txform& projection,
                      const recti& viewport,
                      const vec3d& screen)
{
DOTRACE("geom::unproject");

  const txform pm = projection.mtx_mul(modelview);
  const txform pmi = pm.inverted();

  const vec3d screen2(2*(screen.x() - viewport.left())/viewport.width() - 1,
                      2*(screen.y() - viewport.bottom())/viewport.height() - 1,
                      2*(screen.z()) - 1);

  return pmi.apply_to(screen2);
}

static const char vcid_projection_cc[] = "$Header$";
#endif // !PROJECTION_CC_DEFINED
