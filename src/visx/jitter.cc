///////////////////////////////////////////////////////////////////////
//
// jitter.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Apr  7 13:46:41 1999
// commit: $Id$
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

#ifndef JITTER_CC_DEFINED
#define JITTER_CC_DEFINED

#include "visx/jitter.h"

#include "gfx/canvas.h"

#include "gx/vec3.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/rand.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

namespace
{
  const IO::VersionId JITTER_SERIAL_VERSION_ID = 3;
}

///////////////////////////////////////////////////////////////////////
//
// Jitter member functions
//
///////////////////////////////////////////////////////////////////////

Jitter* Jitter::make()
{
DOTRACE("Jitter::make");
  return new Jitter;
}

Jitter::Jitter () :
  GxTransform(),
  itsXJitter(0.0), itsYJitter(0.0), itsRJitter(0.0),
  itsXShift(0.0), itsYShift(0.0), itsRShift(0.0)
{
DOTRACE("Jitter::Jitter");
  // empty
}

Jitter::~Jitter () throw()
{
DOTRACE("Jitter::~Jitter");
  // empty
}

IO::VersionId Jitter::serialVersionId() const
{
DOTRACE("Jitter::serialVersionId");
  return JITTER_SERIAL_VERSION_ID;
}

void Jitter::readFrom(IO::Reader& reader)
{
DOTRACE("Jitter::readFrom");

  reader.ensureReadVersionId("Jitter", 3,
                             "Try cvs tag xml_conversion_20040526");

  reader.readValue("jitterX", itsXJitter);
  reader.readValue("jitterY", itsYJitter);
  reader.readValue("jitterR", itsRJitter);

  reader.readBaseClass("GxTransform", IO::makeProxy<GxTransform>(this));
}

void Jitter::writeTo(IO::Writer& writer) const
{
DOTRACE("Jitter::writeTo");

  writer.ensureWriteVersionId("Jitter", JITTER_SERIAL_VERSION_ID, 3,
                              "Try groovx0.8a4");

  writer.writeValue("jitterX", itsXJitter);
  writer.writeValue("jitterY", itsYJitter);
  writer.writeValue("jitterR", itsRJitter);

  writer.writeBaseClass("GxTransform", IO::makeConstProxy<GxTransform>(this));
}

/////////////
// actions //
/////////////

void Jitter::rejitter() const
{
DOTRACE("Jitter::rejitter");
  itsXShift = Util::randRange(-itsXJitter, itsXJitter);
  itsYShift = Util::randRange(-itsYJitter, itsYJitter);
  itsRShift = Util::randRange(-itsRJitter, itsRJitter);
}

void Jitter::draw(Gfx::Canvas& canvas) const
{
DOTRACE("Jitter::draw");
  rejitter();
  // Translate
  canvas.translate(translation +
                   Gfx::Vec3<double>(itsXShift, itsYShift, 0.0));

  // Scale
  canvas.scale(scaling);

  // Rotate
  canvas.rotate(rotationAxis, itsRotationAngle+itsRShift);
}

static const char vcid_jitter_cc[] = "$Header$";
#endif // !JITTER_CC_DEFINED
