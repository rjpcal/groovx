///////////////////////////////////////////////////////////////////////
//
// jitter.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Apr  7 13:46:41 1999
// written: Wed Mar 19 12:46:31 2003
// $Id$
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

#include "util/randutils.h"

#include "util/trace.h"
#include "util/debug.h"

namespace
{
  const IO::VersionId JITTER_SERIAL_VERSION_ID = 2;
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

Jitter::~Jitter ()
{
DOTRACE("Jitter::~Jitter");
  // empty
}

IO::VersionId Jitter::serialVersionId() const
{
DOTRACE("Jitter::serialVersionId");
  return JITTER_SERIAL_VERSION_ID;
}

void Jitter::readFrom(IO::Reader* reader)
{
DOTRACE("Jitter::readFrom");

  reader->ensureReadVersionId("Jitter", 2, "Try grsh0.8a4");

  reader->readValue("jitterX", itsXJitter);
  reader->readValue("jitterY", itsYJitter);
  reader->readValue("jitterR", itsRJitter);

  // FIXME change to "GxTransform" with next version
  reader->readBaseClass("Position", IO::makeProxy<GxTransform>(this));
}

void Jitter::writeTo(IO::Writer* writer) const
{
DOTRACE("Jitter::writeTo");

  writer->ensureWriteVersionId("Jitter", JITTER_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writer->writeValue("jitterX", itsXJitter);
  writer->writeValue("jitterY", itsYJitter);
  writer->writeValue("jitterR", itsRJitter);

  // FIXME change to "GxTransform" with next version
  writer->writeBaseClass("Position", IO::makeConstProxy<GxTransform>(this));
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
