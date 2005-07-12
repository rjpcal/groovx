/** @file visx/jitter.cc GxTransform subclass with spatial randomness */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Apr  7 13:46:41 1999
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

#ifndef GROOVX_VISX_JITTER_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_JITTER_CC_UTC20050626084016_DEFINED

#include "visx/jitter.h"

#include "geom/vec3.h"

#include "gfx/canvas.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "rutz/rand.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

namespace
{
  const io::version_id JITTER_SVID = 3;
}

///////////////////////////////////////////////////////////////////////
//
// Jitter member functions
//
///////////////////////////////////////////////////////////////////////

Jitter* Jitter::make()
{
GVX_TRACE("Jitter::make");
  return new Jitter;
}

Jitter::Jitter () :
  GxTransform(),
  itsXJitter(0.0), itsYJitter(0.0), itsRJitter(0.0),
  itsXShift(0.0), itsYShift(0.0), itsRShift(0.0)
{
GVX_TRACE("Jitter::Jitter");
  // empty
}

Jitter::~Jitter () throw()
{
GVX_TRACE("Jitter::~Jitter");
  // empty
}

io::version_id Jitter::class_version_id() const
{
GVX_TRACE("Jitter::class_version_id");
  return JITTER_SVID;
}

void Jitter::read_from(io::reader& reader)
{
GVX_TRACE("Jitter::read_from");

  reader.ensure_version_id("Jitter", 3,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  reader.read_value("jitterX", itsXJitter);
  reader.read_value("jitterY", itsYJitter);
  reader.read_value("jitterR", itsRJitter);

  reader.read_base_class("GxTransform", io::make_proxy<GxTransform>(this));
}

void Jitter::write_to(io::writer& writer) const
{
GVX_TRACE("Jitter::write_to");

  writer.ensure_output_version_id("Jitter", JITTER_SVID, 3,
                              "Try groovx0.8a4", SRC_POS);

  writer.write_value("jitterX", itsXJitter);
  writer.write_value("jitterY", itsYJitter);
  writer.write_value("jitterR", itsRJitter);

  writer.write_base_class("GxTransform", io::make_const_proxy<GxTransform>(this));
}

/////////////
// actions //
/////////////

void Jitter::rejitter() const
{
GVX_TRACE("Jitter::rejitter");
  itsXShift = rutz::rand_range(-itsXJitter, itsXJitter);
  itsYShift = rutz::rand_range(-itsYJitter, itsYJitter);
  itsRShift = rutz::rand_range(-itsRJitter, itsRJitter);
}

void Jitter::draw(Gfx::Canvas& canvas) const
{
GVX_TRACE("Jitter::draw");
  rejitter();
  // Translate
  canvas.translate(translation +
                   geom::vec3<double>(itsXShift, itsYShift, 0.0));

  // Scale
  canvas.scale(scaling);

  // Rotate
  canvas.rotate(rotationAxis, itsRotationAngle+itsRShift);
}

static const char vcid_groovx_visx_jitter_cc_utc20050626084016[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_JITTER_CC_UTC20050626084016_DEFINED
