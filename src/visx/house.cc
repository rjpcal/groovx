///////////////////////////////////////////////////////////////////////
//
// house.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 12:43:16 1999
// written: Sat Mar  4 03:18:02 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSE_CC_DEFINED
#define HOUSE_CC_DEFINED

#ifdef ACC_COMPILER
#pragma OPT_LEVEL 1
#endif

#include "house.h"

#include "reader.h"
#include "rect.h"
#include "writer.h"

#include <iostream.h>
#include <string>
#include <GL/gl.h>
#include <vector>

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

  void makeIoList(House* h, vector<IO *>& vec);
  void makeIoList(const House* h, vector<const IO *>& vec);

  void makeIoList(House* h, vector<IO *>& vec) {
  DOTRACE("House::makeIoList");
	 makeIoList(h, reinterpret_cast<vector<const IO *> &>(vec));
  }

  void makeIoList(const House* h, vector<const IO *>& vec) {
	 DOTRACE("House::makeIoList const");
	 vec.clear();

	 vec.push_back(&h->storyAspectRatio);
	 vec.push_back(&h->numStories);

	 vec.push_back(&h->doorPosition);
	 vec.push_back(&h->doorWidth);  // fraction of avail. space
	 vec.push_back(&h->doorHeight); // fraction of one story
	 vec.push_back(&h->doorOrientation); // left or right

	 vec.push_back(&h->numWindows);
	 vec.push_back(&h->windowWidth);	// fraction of avail. space
	 vec.push_back(&h->windowHeight); // fraction of one story
	 vec.push_back(&h->windowVertBars);
	 vec.push_back(&h->windowHorizBars);

	 vec.push_back(&h->roofShape);
	 vec.push_back(&h->roofOverhang);
	 vec.push_back(&h->roofHeight);
	 vec.push_back(&h->roofColor);

	 vec.push_back(&h->chimneyXPosition);
	 vec.push_back(&h->chimneyYPosition);
	 vec.push_back(&h->chimneyWidth);
	 vec.push_back(&h->chimneyHeight);
  }


  const vector<House::PInfo>& getPropertyInfos() {
  DOTRACE("House::getPropertyInfos");

	 static vector<House::PInfo> p;

	 typedef House H;
	 typedef House::PInfo P;

	 if (p.size() == 0) {
		p.push_back(P("storyAspectRatio", &H::storyAspectRatio, 0.5, 10.0, 0.05, true));
		p.push_back(P("numStories", &H::numStories, 1, 5, 1));

		p.push_back(P("doorPosition", &H::doorPosition, 0, 5, 1, true));
		p.push_back(P("doorWidth", &H::doorWidth, 0.05, 1.0, 0.05));
		p.push_back(P("doorHeight", &H::doorHeight, 0.05, 1.0, 0.05));
		p.push_back(P("doorOrientation", &H::doorOrientation, 0, 1, 1));

		p.push_back(P("numWindows", &H::numWindows, 2, 6, 1, true));
		p.push_back(P("windowWidth", &H::windowWidth, 0.05, 1.0, 0.05));
		p.push_back(P("windowHeight", &H::windowHeight, 0.05, 1.0, 0.05));
		p.push_back(P("windowVertBars", &H::windowVertBars, 0, 5, 1));
		p.push_back(P("windowHorizBars", &H::windowHorizBars, 0, 5, 1));

		p.push_back(P("roofShape", &H::roofShape, 0, 2, 1, true));
		p.push_back(P("roofHeight", &H::roofHeight, 0.05, 2.0, 0.05));
		p.push_back(P("roofOverhang", &H::roofOverhang, 0.0, 0.5, 0.05));
		p.push_back(P("roofColor", &H::roofColor, 0, 1, 1));

		p.push_back(P("chimneyXPosition", &H::chimneyXPosition, -0.5, 0.5, 0.05, true));
		p.push_back(P("chimneyYPosition", &H::chimneyYPosition, 0.0, 1.0, 0.05));
		p.push_back(P("chimneyWidth", &H::chimneyWidth, 0.01, 0.30, 0.01));
		p.push_back(P("chimneyHeight", &H::chimneyHeight, 0.05, 2.0, 0.1));
	 }
	 return p;
  }

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
  storyAspectRatio() = 3.0; 
  numStories() = 2;

  doorPosition() = 2;
  doorHeight() = 0.75;
  doorWidth() = 0.75;

  numWindows() = 5;
  windowHeight() = 0.5;
  windowWidth() = 0.75;
  windowVertBars() = 1;
  windowHorizBars() = 1;

  roofShape() = 0;
  roofOverhang() = 0.05;
  roofHeight() = 0.75;
  roofColor() = 1;

  chimneyXPosition() = 0.2;
  chimneyYPosition() = 0.5;
  chimneyWidth() = 0.06;
  chimneyHeight() = 0.5;
}

House::~House () {
DOTRACE("House::~House ");
}

void House::serialize(ostream& os, IOFlag flag) const {
DOTRACE("House::serialize");

  char sep = ' ';
  if (flag & TYPENAME) { os << ioTag << sep; }

  vector<const IO *> ioList;
  makeIoList(this, ioList);
  for (vector<const IO *>::const_iterator ii = ioList.begin();
		 ii != ioList.end(); ++ii) {
	 (*ii)->serialize(os, flag);
  }

  if (os.fail()) throw OutputError(ioTag);

  if (flag & BASES) { GrObj::serialize(os, flag | TYPENAME); }
}

void House::deserialize(istream& is, IOFlag flag) {
DOTRACE("House::deserialize");
  if (flag & TYPENAME) { IO::readTypename(is, ioTag); }

  vector<IO *> ioList;
  makeIoList(this, ioList);
  for (vector<IO *>::iterator ii = ioList.begin(); ii != ioList.end(); ii++) {
	 (*ii)->deserialize(is, flag);
  }

  try {
	 if (is.fail()) throw InputError(ioTag);
  }
  catch (IoError&) { 
	 throw;
  }

  if (flag & BASES) { GrObj::deserialize(is, flag | TYPENAME); }

  sendStateChangeMsg();
}

void House::readFrom(Reader* reader) {
DOTRACE("House::readFrom");
  const vector<PInfo>& infos = getPropertyInfos();
  for (size_t i = 0; i < infos.size(); ++i) {
	 reader->readValueObj(infos[i].name_cstr(),
								 const_cast<Value&>(get(infos[i].property())));
  }

  GrObj::readFrom(reader);
}

void House::writeTo(Writer* writer) const {
DOTRACE("House::writeTo");
  const vector<PInfo>& infos = getPropertyInfos();
  for (size_t i = 0; i < infos.size(); ++i) {
	 writer->writeValueObj(infos[i].name_cstr(),
								  get(infos[i].property()));
  }

  GrObj::writeTo(writer);
}

int House::charCount() const {
DOTRACE("House::charCount");
  return 128 + GrObj::charCount();
}

///////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////

unsigned int House::numPropertyInfos() {
DOTRACE("House::numPropertyInfos");
  return getPropertyInfos().size();
}

const House::PInfo& House::getPropertyInfo(unsigned int i) {
DOTRACE("House::getPropertyInfo");
  return getPropertyInfos()[i];
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

void House::grRender(Canvas&) const {
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
