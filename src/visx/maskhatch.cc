///////////////////////////////////////////////////////////////////////
//
// maskhatch.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Sep 23 15:49:58 1999
// written: Tue Nov 14 15:09:01 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKHATCH_CC_DEFINED
#define MASKHATCH_CC_DEFINED

#include "maskhatch.h"

#include "rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <cstring>
#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

namespace {
  const FieldInfo MASK_FINFOS[] = {
	 FieldInfo("numLines", &MaskHatch::numLines, 5, 0, 25, 1, true),
	 FieldInfo("lineWidth", &MaskHatch::lineWidth, 1, 0, 25, 1)
  };

  const unsigned int NUM_FINFOS = sizeof(MASK_FINFOS)/sizeof(FieldInfo);

  const FieldMap MASK_FIELDS(MASK_FINFOS, MASK_FINFOS+NUM_FINFOS);

  const IO::VersionId MASKHATCH_SERIAL_VERSION_ID = 2;
}

const FieldMap& MaskHatch::classFields() { return MASK_FIELDS; }

MaskHatch* MaskHatch::make() {
DOTRACE("MaskHatch::make");
  return new MaskHatch;
}

MaskHatch::MaskHatch () :
  GrObj(GROBJ_GL_COMPILE, GROBJ_CLEAR_BOUNDING_BOX),
  FieldContainer(MASK_FIELDS),
  numLines(this, 10, 0, 1000),
  lineWidth(this, 1, 0, 1000)
{
DOTRACE("MaskHatch::MaskHatch");

  GrObj::setAlignmentMode(GrObj::CENTER_ON_CENTER);
  DebugEval(getAlignmentMode());
}

MaskHatch::~MaskHatch() {
DOTRACE("MaskHatch::~MaskHatch");
}

IO::VersionId MaskHatch::serialVersionId() const {
DOTRACE("MaskHatch::serialVersionId");
 return MASKHATCH_SERIAL_VERSION_ID;
}

void MaskHatch::readFrom(IO::Reader* reader) {
DOTRACE("MaskHatch::readFrom");

  reader->ensureReadVersionId("MaskHatch", 2, "Try grsh0.8a4");

  readFieldsFrom(reader);

  IO::IoProxy<GrObj> baseclass(this);
  reader->readBaseClass("GrObj", &baseclass);
}

void MaskHatch::writeTo(IO::Writer* writer) const {
DOTRACE("MaskHatch::writeTo");

  writer->ensureWriteVersionId("MaskHatch", MASKHATCH_SERIAL_VERSION_ID, 2,
										 "Try grsh0.8a4");

  writeFieldsTo(writer);

  IO::ConstIoProxy<GrObj> baseclass(this);
  writer->writeBaseClass("GrObj", &baseclass);
}

void MaskHatch::grGetBoundingBox(Rect<double>& bbox,
											int& border_pixels) const {
DOTRACE("MaskHatch::grGetBoundingBox");

  bbox.left() = bbox.bottom() = 0.0;
  bbox.right() = bbox.top() = 1.0;
  border_pixels = lineWidth()/2 + 2;
}

bool MaskHatch::grHasBoundingBox() const {
DOTRACE("MaskHastch::grHasBoundingBox");
  return true;
}

void MaskHatch::grRender(GWT::Canvas&) const {
DOTRACE("MaskHatch::grRender");

  if (numLines() == 0) return; 

  glPushAttrib(GL_LINE_BIT);
  { 
	 glLineWidth(lineWidth());

	 glBegin(GL_LINES);
	 {
		for (int i = 0; i < numLines(); ++i) {
		  GLdouble position = double(i)/numLines();
		  
		  // horizontal line
		  glVertex2d(0.0, position);
		  glVertex2d(1.0, position);
		  
		  // vertical line
		  glVertex2d(position, 0.0);
		  glVertex2d(position, 1.0);
		  
		  // lines with slope = 1 
		  glVertex2d(0.0, position);
		  glVertex2d(1.0-position, 1.0);
		  
		  glVertex2d(position, 0.0);
		  glVertex2d(1.0, 1.0-position);
		  
		  // lines with slope = -1
		  glVertex2d(0.0, 1.0-position);
		  glVertex2d(1.0-position, 0.0);
		  
		  glVertex2d(position, 1.0);
		  glVertex2d(1.0, position);
		}
		
		// final closing lines
		glVertex2d(0.0, 1.0);
		glVertex2d(1.0, 1.0);
		
		glVertex2d(1.0, 0.0);
		glVertex2d(1.0, 1.0);
	 }
	 glEnd();
  }
  glPopAttrib();
}

static const char vcid_maskhatch_cc[] = "$Header$";
#endif // !MASKHATCH_CC_DEFINED
