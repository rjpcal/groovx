///////////////////////////////////////////////////////////////////////
//
// house.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 12:43:16 1999
// written: Tue Sep 21 15:03:32 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSE_CC_DEFINED
#define HOUSE_CC_DEFINED

#include "house.h"

#include <iostream.h>
#include <string>
#include <GL/gl.h>

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  const string ioTag = "House";

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

House::House () {
DOTRACE("House::House ");
  itsStoryAspectRatio() = 3.0; 
  itsNumStories() = 2;

  itsDoorPosition() = 2;
  itsDoorHeight() = 0.75;
  itsDoorWidth() = 0.75;

  itsNumWindows() = 5;
  itsWindowHeight() = 0.5;
  itsWindowWidth() = 0.75;
  itsWindowVertBars() = 1;
  itsWindowHorizBars() = 1;

  itsRoofShape() = 0;
  itsRoofOverhang() = 0.05;
  itsRoofHeight() = 0.75;
  itsRoofColor() = 1;

  itsChimneyXPosition() = 0.2;
  itsChimneyYPosition() = 0.5;
  itsChimneyWidth() = 0.06;
  itsChimneyHeight() = 0.5;
}

House::~House () {
DOTRACE("House::~House ");
}

void House::serialize(ostream& os, IOFlag flag) const {
DOTRACE("House::serialize");

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  vector<const IO *> ioList;
  makeIoList(ioList);
  for (vector<const IO *>::const_iterator ii = ioList.begin();
		 ii != ioList.end(); ++ii) {
	 (*ii)->serialize(os, flag);
  }

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag); }  
}

void House::deserialize(istream& is, IOFlag flag) {
DOTRACE("House::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  vector<IO *> ioList;
  makeIoList(ioList);
  for (vector<IO *>::iterator ii = ioList.begin(); ii != ioList.end(); ii++) {
	 (*ii)->deserialize(is, flag);
  }

  try {
	 if (is.fail()) throw InputError(ioTag);
  }
  catch (IoError&) { 
	 throw;
  }

  if (flag & BASES) { GrObj::deserialize(is, flag); }

  sendStateChangeMsg();
}

int House::charCount() const {
DOTRACE("House::charCount");
  return 128; 
}

bool House::grGetBoundingBox(double& left, double& top,
									  double& right, double& bottom,
									  int& border_pixels) const {
DOTRACE("House::grGetBoundingBox");
  GLdouble main_width = itsStoryAspectRatio();
  GLdouble main_height = itsNumStories() + itsRoofHeight();

  GLdouble max_dimension = max(main_height, main_width);

  left   = -main_width/2.0 * (1 + max(itsRoofOverhang(), 0.0)) / max_dimension;
  right  =  main_width/2.0 * (1 + max(itsRoofOverhang(), 0.0)) / max_dimension;
  bottom = -main_height/2.0 / max_dimension;

  top    =  main_height/2.0;
  if ( (itsChimneyYPosition() + itsChimneyHeight()) > itsRoofHeight() ) {
	 top += itsChimneyYPosition() + itsChimneyHeight() - itsRoofHeight();
  }

  top   /= max_dimension;

  border_pixels = 4;

  return true;
}

void House::grRender() const {
DOTRACE("House::grRender");
  GLdouble total_width = itsStoryAspectRatio();
  GLdouble total_height = itsNumStories() + itsRoofHeight();

  GLdouble max_dimension = 
 	 (total_height > total_width) ? total_height : total_width;

  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  {
 	 // Scale so that the larger dimension (of width and height)
 	 // extends across 1.0 units in GL coordinates. The smaller
 	 // dimension will extend across less than 1.0 GL units.
	 glScaled(itsStoryAspectRatio()/max_dimension, 1.0/max_dimension, 1.0);

	 // Translate down by half the height of the house, then up by 0.5
	 // units, so that we are positioned in the center of the first
	 // story to be drawn.
	 glTranslated(0.0, -total_height/2.0 + 0.5, 0.0);

	 glPushAttrib(GL_POLYGON_BIT);
	 {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		
		// Loop over house stories.
		for (int s = 0; s < itsNumStories(); ++s) {
		  drawStoryFrame();

		  // Loop over window positions.
		  for (int w = 0; w < itsNumWindows(); ++w) {
			 glPushMatrix();
			 {
				GLdouble x_pos = double(w+0.5)/itsNumWindows() - 0.5;
				if (s == 0 && w == itsDoorPosition() ) {
				  // Draw door.
				  glTranslated(x_pos, -0.5, 0.0);
				  glScaled(itsDoorWidth()/itsNumWindows(), itsDoorHeight(), 1.0);
				  if (itsDoorOrientation()) {
					 glScaled(-1.0, 1.0, 1.0);
				  }
				  drawDoor();
				}
				else {
				  // Draw window.
				  glTranslated(x_pos, 0.0, 0.0);
				  glScaled(itsWindowWidth()/itsNumWindows(), itsWindowHeight(), 1.0);
				  drawWindow(itsWindowVertBars(), itsWindowHorizBars());
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
		  glScaled(1.0+itsRoofOverhang(), itsRoofHeight(), 1.0);

		  glPushAttrib(GL_POLYGON_BIT);
		  {
			 if (itsRoofColor() == 0) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			 }
			 else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			 }
			 
			 if (itsRoofShape() == 0) {
				drawTriangleRoof();
			 }
			 else if (itsRoofShape() == 1) {
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
		  glTranslated(itsChimneyXPosition(), itsChimneyYPosition(), 0.0);
		  glScaled(itsChimneyWidth(), itsChimneyHeight(), 0.0);
			 
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


void House::makeIoList(vector<IO *>& vec) {
DOTRACE("House::makeIoList");
  makeIoList(reinterpret_cast<vector<const IO *> &>(vec));
}

void House::makeIoList(vector<const IO *>& vec) const {
DOTRACE("House::makeIoList const");
  vec.clear();

/*
  PROPERTY([a-z]*, \(.*\));
  vec.push_back(&its\1);
 */

  vec.push_back(&itsStoryAspectRatio);
  vec.push_back(&itsNumStories);

  vec.push_back(&itsDoorPosition);
  vec.push_back(&itsDoorWidth);  // fraction of avail. space
  vec.push_back(&itsDoorHeight); // fraction of one story
  vec.push_back(&itsDoorOrientation); // left or right

  vec.push_back(&itsNumWindows);
  vec.push_back(&itsWindowWidth);	// fraction of avail. space
  vec.push_back(&itsWindowHeight); // fraction of one story
  vec.push_back(&itsWindowVertBars);
  vec.push_back(&itsWindowHorizBars);

  vec.push_back(&itsRoofShape);
  vec.push_back(&itsRoofOverhang);
  vec.push_back(&itsRoofHeight);
  vec.push_back(&itsRoofColor);

  vec.push_back(&itsChimneyXPosition);
  vec.push_back(&itsChimneyYPosition);
  vec.push_back(&itsChimneyWidth);
  vec.push_back(&itsChimneyHeight);
}

static const char vcid_house_cc[] = "$Header$";
#endif // !HOUSE_CC_DEFINED
