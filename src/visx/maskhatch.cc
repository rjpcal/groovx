///////////////////////////////////////////////////////////////////////
//
// maskhatch.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Sep 23 15:49:58 1999
// written: Mon Oct  4 13:30:47 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MASKHATCH_CC_DEFINED
#define MASKHATCH_CC_DEFINED

#include "maskhatch.h"

#include <GL/gl.h>
#include <iostream.h>

#include "trace.h"
#include "debug.h"

MaskHatch::MaskHatch () :
  GrObj(GROBJ_GL_COMPILE, GROBJ_CLEAR_BOUNDING_BOX),
  numLines(10),
  lineWidth(1)
{
DOTRACE("MaskHatch::MaskHatch ");
  GrObj::setAlignmentMode(GrObj::CENTER_ON_CENTER);
}

MaskHatch::~MaskHatch () {
DOTRACE("MaskHatch::~MaskHatch ");
  
}

void MaskHatch::serialize(ostream &os, IOFlag flag) const {
DOTRACE("MaskHatch::serialize");
}

void MaskHatch::deserialize(istream &is, IOFlag flag) {
DOTRACE("MaskHatch::deserialize");
}

int MaskHatch::charCount() const { return 0; }

///////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////

const vector<MaskHatch::PInfo>& MaskHatch::getPropertyInfos() {
DOTRACE("MaskHatch::getPropertyInfos");

  static vector<PInfo> p;

  typedef MaskHatch MH;

  if (p.size() == 0) {
	 p.push_back(PInfo("numLines", &MH::numLines, 0, 25, 1, true));
	 p.push_back(PInfo("lineWidth", &MH::lineWidth, 0, 25, 1));
  }

  return p;
}

bool MaskHatch::grGetBoundingBox(double& left, double& top,
											double& right, double& bottom,
											int& border_pixels) const {
DOTRACE("MaskHatch::grGetBoundingBox");

  left = bottom = 0.0;
  right = top = 1.0;
  border_pixels = lineWidth()/2 + 2;

  return true;
}

void MaskHatch::grRender() const {
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
