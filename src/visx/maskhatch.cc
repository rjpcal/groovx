///////////////////////////////////////////////////////////////////////
//
// maskhatch.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Sep 23 15:49:58 1999
// written: Wed Sep 27 14:37:15 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKHATCH_CC_DEFINED
#define MASKHATCH_CC_DEFINED

#include "maskhatch.h"

#include "rect.h"

#include "io/iolegacy.h"
#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <cstring>
#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

#ifdef MIPSPRO_COMPILER
#  define SGI_IDIOT_CAST(to, from) reinterpret_cast<to>(from)
#else
#  define SGI_IDIOT_CAST(to, from) (from)
#endif

namespace {
  const char* ioTag = "MaskHatch";

  const MaskHatch::PInfo PINFOS[] = {
	 MaskHatch::PInfo("numLines",
							SGI_IDIOT_CAST(Property MaskHatch::*, &MaskHatch::numLines),
							0, 25, 1, true),
	 MaskHatch::PInfo("lineWidth",
							SGI_IDIOT_CAST(Property MaskHatch::*, &MaskHatch::lineWidth),
							0, 25, 1)
  };

  const unsigned int NUM_PINFOS = sizeof(PINFOS)/sizeof(MaskHatch::PInfo);
}

MaskHatch::MaskHatch () :
  GrObj(GROBJ_GL_COMPILE, GROBJ_CLEAR_BOUNDING_BOX),
  numLines(10),
  lineWidth(1)
{
DOTRACE("MaskHatch::MaskHatch ");

  GrObj::setAlignmentMode(GrObj::CENTER_ON_CENTER);
  DebugEval(getAlignmentMode());
}

MaskHatch::~MaskHatch () {
DOTRACE("MaskHatch::~MaskHatch ");
  
}

void MaskHatch::legacySrlz(IO::Writer* writer) const {
DOTRACE("MaskHatch::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {

	 lwriter->writeTypename(ioTag);

	 ostream& os = lwriter->output();

	 IO::ConstIoProxy<GrObj> baseclass(this);
	 lwriter->writeBaseClass("GrObj", &baseclass);
  }
}

void MaskHatch::legacyDesrlz(IO::Reader* reader) {
DOTRACE("MaskHatch::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 lreader->readTypename(ioTag);

	 IO::IoProxy<GrObj> baseclass(this);
	 lreader->readBaseClass("GrObj", &baseclass);
  }
}

void MaskHatch::readFrom(IO::Reader* reader) {
DOTRACE("MaskHatch::readFrom");
  for (size_t i = 0; i < numPropertyInfos(); ++i) {
	 reader->readValueObj(PINFOS[i].name_cstr(),
								 const_cast<Value&>(get(PINFOS[i].property())));
  }

  GrObj::readFrom(reader);
}

void MaskHatch::writeTo(IO::Writer* writer) const {
DOTRACE("MaskHatch::writeTo");
  for (size_t i = 0; i < numPropertyInfos(); ++i) {
	 writer->writeValueObj(PINFOS[i].name_cstr(), get(PINFOS[i].property()));
  }

  GrObj::writeTo(writer);
}

///////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////

unsigned int MaskHatch::numPropertyInfos() {
DOTRACE("MaskHatch::numPropertyInfos");
//   return getPropertyInfos().size();
  return NUM_PINFOS;
}

const MaskHatch::PInfo& MaskHatch::getPropertyInfo(unsigned int i) {
DOTRACE("MaskHatch::getPropertyInfo");
//   return getPropertyInfos()[i];
  return PINFOS[i];
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
