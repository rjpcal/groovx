///////////////////////////////////////////////////////////////////////
//
// house.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Sep 13 12:43:16 1999
// written: Wed Nov 15 07:42:05 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSE_CC_DEFINED
#define HOUSE_CC_DEFINED

#ifdef ACC_COMPILER
#pragma OPT_LEVEL 1
#endif

#include "house.h"

#include "rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const IO::VersionId HOUSE_SERIAL_VERSION_ID = 2;

  template <class T>
  inline T max(const T& t1, const T& t2) { return (t1 > t2) ? t1 : t2; }

  const FieldInfo FINFOS[] = {
	 FieldInfo("storyAspectRatio",
				  &House::storyAspectRatio, 3.0, 0.5, 10.0, 0.05, true),
	 FieldInfo("numStories", &House::numStories, 2, 1, 5, 1),

	 FieldInfo("doorPosition", &House::doorPosition, 2, 0, 5, 1, true),
	 FieldInfo("doorWidth", &House::doorWidth, 0.75, 0.05, 1.0, 0.05),
	 FieldInfo("doorHeight", &House::doorHeight, 0.75, 0.05, 1.0, 0.05),
	 FieldInfo("doorOrientation",
				  &House::doorOrientation, false, false, true, true),

	 FieldInfo("numWindows", &House::numWindows, 5, 2, 6, 1, true),
	 FieldInfo("windowWidth", &House::windowWidth, 0.75, 0.05, 1.0, 0.05),
	 FieldInfo("windowHeight", &House::windowHeight, 0.5, 0.05, 1.0, 0.05),
	 FieldInfo("windowVertBars", &House::windowVertBars, 1, 0, 5, 1),
	 FieldInfo("windowHorizBars", &House::windowHorizBars, 1, 0, 5, 1),

	 FieldInfo("roofShape", &House::roofShape, 0, 0, 2, 1, true),
	 FieldInfo("roofHeight", &House::roofHeight, 0.05, 0.05, 2.0, 0.05),
	 FieldInfo("roofOverhang", &House::roofOverhang, 0.75, 0.0, 0.5, 0.05),
	 FieldInfo("roofColor", &House::roofColor, 1, 0, 1, 1),

	 FieldInfo("chimneyXPosition",
				  &House::chimneyXPosition, 0.2, -0.5, 0.5, 0.05, true),
	 FieldInfo("chimneyYPosition",
				  &House::chimneyYPosition, 0.5, 0.0, 1.0, 0.05),
	 FieldInfo("chimneyWidth", &House::chimneyWidth, 0.06, 0.01, 0.30, 0.01),
	 FieldInfo("chimneyHeight", &House::chimneyHeight, 0.5, 0.05, 2.0, 0.1)
  };

  const unsigned int NUM_FINFOS = sizeof(FINFOS)/sizeof(FieldInfo);

  const FieldMap HOUSE_FIELDS(FINFOS, FINFOS+NUM_FINFOS);

  void drawWindow(int num_vert_bars, int num_horiz_bars) {
	 // Draw 1x1 window centered on (0,0)

	 glBegin(GL_LINES);
	 {
		for (int x = 0; x < (num_vert_bars+2); ++x) {
		  double xpos = double(x)/(num_vert_bars+1) - 0.5;
		  glVertex2d(xpos, -0.5);
		  glVertex2d(xpos,  0.5);
		}
		for (int y = 0; y < (num_horiz_bars+2); ++y) {
		  double ypos = double(y)/(num_horiz_bars+1) - 0.5;
		  glVertex2d(-0.5, ypos);
		  glVertex2d( 0.5, ypos);
		}
	 }
	 glEnd();
  }

  void drawDoor() {
	 // Draw 1x1 door with bottom line centered on (0,0)
	 glRectd(-0.5, 0.0, 0.5, 1.0);
	 glBegin(GL_LINES);
	   glVertex2d(0.25, 0.35);
		glVertex2d(0.25, 0.45);
	 glEnd();
  }

  void drawStoryFrame() {
	 // Draw 1x1 story frame centered on (0,0)
	 glRectd(-0.5, -0.5, 0.5, 0.5);
  }

  void drawTriangleRoof() {
	 // Draw 1x1-bounded triangle with bottom line centered on (0,0)
	 glBegin(GL_TRIANGLES);
	   glVertex2d(-0.5, 0.0);
		glVertex2d( 0.5, 0.0);
		glVertex2d( 0.0, 1.0);
	 glEnd();
	 glBegin(GL_LINES);
	   glVertex2d(-0.5, 0.0);
		glVertex2d( 0.5, 0.0);
	 glEnd();
  }

  void drawTrapezoidRoof() {
	 glBegin(GL_QUADS);
	   glVertex2d(-0.5, 0.0);
		glVertex2d( 0.5, 0.0);
		glVertex2d( 0.4, 1.0);
		glVertex2d(-0.4, 1.0);
	 glEnd();
	 glBegin(GL_LINES);
	   glVertex2d(-0.5, 0.0);
		glVertex2d( 0.5, 0.0);
	 glEnd();
  }

  void drawSquareRoof() {
	 glBegin(GL_QUADS);
	   glVertex2d(-0.5, 0.0);
		glVertex2d( 0.5, 0.0);
		glVertex2d( 0.5, 1.0);
		glVertex2d(-0.5, 1.0);
	 glEnd();
	 glBegin(GL_LINES);
	   glVertex2d(-0.5, 0.0);
		glVertex2d( 0.5, 0.0);
	 glEnd();
  }

  void drawChimney () {
	 glBegin(GL_QUADS);
	   glEdgeFlag(GL_FALSE);
		glVertex2d( 0.5, 0.0);
		glEdgeFlag(GL_TRUE);
		glVertex2d(-0.5, 0.0);
		glVertex2d(-0.5, 1.0);
		glVertex2d( 0.5, 1.0);
	 glEnd();
  }
}

///////////////////////////////////////////////////////////////////////
//
// House member definitions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& House::classFields() { return HOUSE_FIELDS; }

House* House::make() {
DOTRACE("House::make");
  return new House;
}

House::House() :
  FieldContainer(),
  storyAspectRatio(this, 3.0),
  numStories(this, 2),

  doorPosition(this, 2),
  doorWidth(this, 0.75),
  doorHeight(this, 0.75),
  doorOrientation(this, false),

  numWindows(this, 5),
  windowWidth(this, 0.75),
  windowHeight(this, 0.5),
  windowVertBars(this, 1),
  windowHorizBars(this, 1),

  roofShape(this, 0),
  roofOverhang(this, 0.05),
  roofHeight(this, 0.75),
  roofColor(this, 1),

  chimneyXPosition(this, 0.2),
  chimneyYPosition(this, 0.5),
  chimneyWidth(this, 0.06),
  chimneyHeight(this, 0.5)
{
DOTRACE("House::House");

  setFieldMap(HOUSE_FIELDS);
}

House::~House() {
DOTRACE("House::~House");
}

IO::VersionId House::serialVersionId() const {
DOTRACE("House::serialVersionId");
  return HOUSE_SERIAL_VERSION_ID;
}

void House::readFrom(IO::Reader* reader) {
DOTRACE("House::readFrom");

  reader->ensureReadVersionId("House", 2, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  IO::IoProxy<GrObj> baseclass(this);
  reader->readBaseClass("GrObj", &baseclass);
}

void House::writeTo(IO::Writer* writer) const {
DOTRACE("House::writeTo");

  writer->ensureWriteVersionId("House", HOUSE_SERIAL_VERSION_ID, 2,
										 "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  IO::ConstIoProxy<GrObj> baseclass(this);
  writer->writeBaseClass("GrObj", &baseclass);
}

///////////////////////////////////////////////////////////////////////
//
// Actions
//
///////////////////////////////////////////////////////////////////////

void House::grGetBoundingBox(Rect<double>& bbox,
									  int& border_pixels) const {
DOTRACE("House::grGetBoundingBox");
  GLdouble main_width = storyAspectRatio();
  GLdouble main_height = numStories() + roofHeight();

  GLdouble max_dimension = max(main_height, main_width);

  bbox.left()   = -main_width/2.0 * (1 + max(roofOverhang(), 0.0)) / max_dimension;
  bbox.right()  =  main_width/2.0 * (1 + max(roofOverhang(), 0.0)) / max_dimension;
  bbox.bottom() = -main_height/2.0 / max_dimension;

  bbox.top()    =  main_height/2.0;
  if ( (chimneyYPosition() + chimneyHeight()) > roofHeight() ) {
	 bbox.top() += chimneyYPosition() + chimneyHeight() - roofHeight();
  }

  bbox.top()   /= max_dimension;

  border_pixels = 4;
}

bool House::grHasBoundingBox() const {
DOTRACE("House::grHasBoundingBox");
  return true;
}

void House::grRender(GWT::Canvas&) const {
DOTRACE("House::grRender");
  GLdouble total_width = storyAspectRatio();
  GLdouble total_height = numStories() + roofHeight();

  GLdouble max_dimension = 
 	 (total_height > total_width) ? total_height : total_width;

  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  {
 	 // Scale so that the larger dimension (of width and height)
 	 // extends across 1.0 units in GL coordinates. The smaller
 	 // dimension will extend across less than 1.0 GL units.
	 glScaled(storyAspectRatio()/max_dimension, 1.0/max_dimension, 1.0);

	 // Translate down by half the height of the house, then up by 0.5
	 // units, so that we are positioned in the center of the first
	 // story to be drawn.
	 glTranslated(0.0, -total_height/2.0 + 0.5, 0.0);

	 glPushAttrib(GL_POLYGON_BIT);
	 {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		
		// Loop over house stories.
		for (int s = 0; s < numStories(); ++s) {
		  drawStoryFrame();

		  // Loop over window positions.
		  for (int w = 0; w < numWindows(); ++w) {
			 glPushMatrix();
			 {
				GLdouble x_pos = double(w+0.5)/numWindows() - 0.5;
				if (s == 0 && w == doorPosition() ) {
				  // Draw door.
				  glTranslated(x_pos, -0.5, 0.0);
				  glScaled(doorWidth()/numWindows(), doorHeight(), 1.0);
				  if (doorOrientation()) {
					 glScaled(-1.0, 1.0, 1.0);
				  }
				  drawDoor();
				}
				else {
				  // Draw window.
				  glTranslated(x_pos, 0.0, 0.0);
				  glScaled(windowWidth()/numWindows(), windowHeight(), 1.0);
				  drawWindow(windowVertBars(), windowHorizBars());
				}
			 }
			 glPopMatrix();
		  }

		  glTranslated(0.0, 1.0, 0.0);
		}
		

		// Draw roof.
		glTranslated(0.0, -0.5, 0.0);

		glPushMatrix();
		{
		  glScaled(1.0+roofOverhang(), roofHeight(), 1.0);

		  glPushAttrib(GL_POLYGON_BIT);
		  {
			 if (roofColor() == 0) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			 }
			 else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			 }
			 
			 if (roofShape() == 0) {
				drawTriangleRoof();
			 }
			 else if (roofShape() == 1) {
				drawTrapezoidRoof();
			 }
			 else {
				drawSquareRoof();
			 }
		  }
		  glPopAttrib();

		}
		glPopMatrix();

		// Draw chimney.
		glPushMatrix();
		{
		  glTranslated(chimneyXPosition(), chimneyYPosition(), 0.0);
		  glScaled(chimneyWidth(), chimneyHeight(), 0.0);
			 
		  glPushAttrib(GL_POLYGON_BIT);
		  {
			 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			 drawChimney();
		  }
		  glPopAttrib();

		}
		glPopMatrix();
		
	 }
	 glPopAttrib();

  }
  glPopMatrix();
}

static const char vcid_house_cc[] = "$Header$";
#endif // !HOUSE_CC_DEFINED
