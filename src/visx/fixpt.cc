///////////////////////////////////////////////////////////////////////
//
// fixpt.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Jan-99
// written: Thu Jul 19 09:31:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FIXPT_CC_DEFINED
#define FIXPT_CC_DEFINED

#include "fixpt.h"

#include "rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <cstring>
#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const FieldInfo FIXPT_FINFOS[] = {
      FieldInfo("length", &FixPt::length, 0.1, 0.0, 10.0, 0.1, true),
      FieldInfo("width", &FixPt::width, 1, 0, 100, 1)
  };

  const FieldMap FIXPT_FIELDS(FIXPT_FINFOS, FIXPT_FINFOS+2);

  const IO::VersionId FIXPT_SERIAL_VERSION_ID = 2;
}

///////////////////////////////////////////////////////////////////////
//
// FixPt member functions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& FixPt::classFields() { return FIXPT_FIELDS; }

FixPt* FixPt::make() {
DOTRACE("FixPt::make");
  return new FixPt;
}

FixPt::FixPt(double len, int wid) :
  FieldContainer(),
  length(this, len), width(this, wid)
{
  setFieldMap(FIXPT_FIELDS);
}

FixPt::~FixPt() {}

IO::VersionId FixPt::serialVersionId() const {
DOTRACE("FixPt::serialVersionId");
  return FIXPT_SERIAL_VERSION_ID;
}

void FixPt::readFrom(IO::Reader* reader) {
DOTRACE("FixPt::readFrom");

  reader->ensureReadVersionId("FixPt", 2, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  DebugEval(length());  DebugEvalNL(width());

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void FixPt::writeTo(IO::Writer* writer) const {
DOTRACE("FixPt::writeTo");

  writer->ensureWriteVersionId("FixPt", FIXPT_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

Rect<double> FixPt::grGetBoundingBox() const {
DOTRACE("FixPt::grGetBoundingBox");

  Rect<double> bbox;
  bbox.left()  = bbox.bottom() = -length()/2.0;
  bbox.right() = bbox.top()    =  length()/2.0;

  return bbox;
}

void FixPt::grRender(GWT::Canvas&, DrawMode) const {
DOTRACE("FixPt::grRender");
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(width());

  glBegin(GL_LINES);
  glVertex3f(0.0, -length()/2.0, 0.0);
  glVertex3f(0.0, length()/2.0, 0.0);
  glEnd();

  glFlush();

  glBegin(GL_LINES);
  glVertex3f(-length()/2.0, 0.0, 0.0);
  glVertex3f(length()/2.0, 0.0, 0.0);
  glEnd();

  glPopAttrib();
}

static const char vcid_fixpt_cc[] = "$Header$";
#endif // !FIXPT_CC_DEFINED
