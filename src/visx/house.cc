///////////////////////////////////////////////////////////////////////
//
// house.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Sep 13 12:43:16 1999
// written: Wed Sep 27 11:48:24 2000
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

#include "io/iolegacy.h"
#include "io/reader.h"
#include "io/writer.h"

#include <GL/gl.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#ifdef MIPSPRO_COMPILER
#  define SGI_IDIOT_CAST(to, from) reinterpret_cast<to>(from)
#else
#  define SGI_IDIOT_CAST(to, from) (from)
#endif

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace {
  template <class T>
  inline T max(const T& t1, const T& t2) { return (t1 > t2) ? t1 : t2; }

  const char* ioTag = "House";

  typedef Property House::* IoMember;

  const IoMember IO_MEMBERS[] = {
	 SGI_IDIOT_CAST(Property House::*, &House::storyAspectRatio),
	 SGI_IDIOT_CAST(Property House::*, &House::numStories),

	 SGI_IDIOT_CAST(Property House::*, &House::doorPosition),
	 SGI_IDIOT_CAST(Property House::*, &House::doorWidth),  // fraction of avail. space
	 SGI_IDIOT_CAST(Property House::*, &House::doorHeight), // fraction of one story
	 SGI_IDIOT_CAST(Property House::*, &House::doorOrientation), // left or right

	 SGI_IDIOT_CAST(Property House::*, &House::numWindows),
	 SGI_IDIOT_CAST(Property House::*, &House::windowWidth),	// fraction of avail. space
	 SGI_IDIOT_CAST(Property House::*, &House::windowHeight), // fraction of one story
	 SGI_IDIOT_CAST(Property House::*, &House::windowVertBars),
	 SGI_IDIOT_CAST(Property House::*, &House::windowHorizBars),

	 SGI_IDIOT_CAST(Property House::*, &House::roofShape),
	 SGI_IDIOT_CAST(Property House::*, &House::roofOverhang),
	 SGI_IDIOT_CAST(Property House::*, &House::roofHeight),
	 SGI_IDIOT_CAST(Property House::*, &House::roofColor),

	 SGI_IDIOT_CAST(Property House::*, &House::chimneyXPosition),
	 SGI_IDIOT_CAST(Property House::*, &House::chimneyYPosition),
	 SGI_IDIOT_CAST(Property House::*, &House::chimneyWidth),
	 SGI_IDIOT_CAST(Property House::*, &House::chimneyHeight)
  };

  const unsigned int NUM_IO_MEMBERS = sizeof(IO_MEMBERS)/sizeof(IoMember);

  const House::PInfo PINFOS[] = {
	 House::PInfo("storyAspectRatio",
					  SGI_IDIOT_CAST(Property House::*, &House::storyAspectRatio), 
					  0.5, 10.0, 0.05, true),
	 House::PInfo("numStories",
					  SGI_IDIOT_CAST(Property House::*, &House::numStories), 
					  1, 5, 1),

	 House::PInfo("doorPosition",
					  SGI_IDIOT_CAST(Property House::*, &House::doorPosition), 
					  0, 5, 1, true),
	 House::PInfo("doorWidth",
					  SGI_IDIOT_CAST(Property House::*, &House::doorWidth), 
					  0.05, 1.0, 0.05),
	 House::PInfo("doorHeight",
					  SGI_IDIOT_CAST(Property House::*, &House::doorHeight), 
					  0.05, 1.0, 0.05),
	 House::PInfo("doorOrientation",
					  SGI_IDIOT_CAST(Property House::*, &House::doorOrientation), 
					  0, 1, 1),

	 House::PInfo("numWindows",
					  SGI_IDIOT_CAST(Property House::*, &House::numWindows), 
					  2, 6, 1, true),
	 House::PInfo("windowWidth",
					  SGI_IDIOT_CAST(Property House::*, &House::windowWidth), 
					  0.05, 1.0, 0.05),
	 House::PInfo("windowHeight",
					  SGI_IDIOT_CAST(Property House::*, &House::windowHeight), 
					  0.05, 1.0, 0.05),
	 House::PInfo("windowVertBars",
					  SGI_IDIOT_CAST(Property House::*, &House::windowVertBars), 
					  0, 5, 1),
	 House::PInfo("windowHorizBars",
					  SGI_IDIOT_CAST(Property House::*, &House::windowHorizBars), 
					  0, 5, 1),

	 House::PInfo("roofShape",
					  SGI_IDIOT_CAST(Property House::*, &House::roofShape), 
					  0, 2, 1, true),
	 House::PInfo("roofHeight",
					  SGI_IDIOT_CAST(Property House::*, &House::roofHeight), 
					  0.05, 2.0, 0.05),
	 House::PInfo("roofOverhang",
					  SGI_IDIOT_CAST(Property House::*, &House::roofOverhang), 
					  0.0, 0.5, 0.05),
	 House::PInfo("roofColor",
					  SGI_IDIOT_CAST(Property House::*, &House::roofColor), 
					  0, 1, 1),

	 House::PInfo("chimneyXPosition",
					  SGI_IDIOT_CAST(Property House::*, &House::chimneyXPosition), 
					  -0.5, 0.5, 0.05, true),
	 House::PInfo("chimneyYPosition",
					  SGI_IDIOT_CAST(Property House::*, &House::chimneyYPosition), 
					  0.0, 1.0, 0.05),
	 House::PInfo("chimneyWidth",
					  SGI_IDIOT_CAST(Property House::*, &House::chimneyWidth), 
					  0.01, 0.30, 0.01),
	 House::PInfo("chimneyHeight",
					  SGI_IDIOT_CAST(Property House::*, &House::chimneyHeight), 
					  0.05, 2.0, 0.1)
  };

  const unsigned int NUM_PINFOS = sizeof(PINFOS)/sizeof(House::PInfo);

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

House::House() :
  storyAspectRatio(3.0), 
  numStories(2),

  doorPosition(2),
  doorWidth(0.75),
  doorHeight(0.75),
  doorOrientation(false),

  numWindows(5),
  windowWidth(0.75),
  windowHeight(0.5),
  windowVertBars(1),
  windowHorizBars(1),

  roofShape(0),
  roofOverhang(0.05),
  roofHeight(0.75),
  roofColor(1),

  chimneyXPosition(0.2),
  chimneyYPosition(0.5),
  chimneyWidth(0.06),
  chimneyHeight(0.5)
{
DOTRACE("House::House");
}

House::~House() {
DOTRACE("House::~House");
}

void House::legacySrlz(IO::Writer* writer) const {
DOTRACE("House::legacySrlz");
  IO::LegacyWriter* lwriter = dynamic_cast<IO::LegacyWriter*>(writer);
  if (lwriter != 0) {
	 ostream& os = lwriter->output();

	 char sep = ' ';
	 if (lwriter->flags() & IO::TYPENAME) { os << ioTag << sep; }

	 for (unsigned int i = 0; i < NUM_IO_MEMBERS; ++i) {
		(this->*IO_MEMBERS[i]).legacySrlz(writer);
	 }

	 if (os.fail()) throw IO::OutputError(ioTag);

	 if (lwriter->flags() & IO::BASES) {
		IO::LWFlagJanitor jtr_(*lwriter, lwriter->flags() | IO::TYPENAME);
		GrObj::legacySrlz(writer);
	 }
  }
}

void House::legacyDesrlz(IO::Reader* reader) {
DOTRACE("House::legacyDesrlz");
  IO::LegacyReader* lreader = dynamic_cast<IO::LegacyReader*>(reader); 
  if (lreader != 0) {
	 istream& is = lreader->input();
	 if (lreader->flags() & IO::TYPENAME) { IO::IoObject::readTypename(is, ioTag); }

	 for (unsigned int i = 0; i < NUM_IO_MEMBERS; ++i) {
		(this->*IO_MEMBERS[i]).legacyDesrlz(reader);
	 }

	 try {
		if (is.fail()) throw IO::InputError(ioTag);
	 }
	 catch (IO::IoError&) { 
		throw;
	 }

	 if (lreader->flags() & IO::BASES) {
		IO::LRFlagJanitor jtr_(*lreader, lreader->flags() | IO::TYPENAME);
		GrObj::legacyDesrlz(reader);
	 }

	 sendStateChangeMsg();
  }
}

void House::readFrom(IO::Reader* reader) {
DOTRACE("House::readFrom");
  for (unsigned int i = 0; i < NUM_PINFOS; ++i) {
	 reader->readValueObj(PINFOS[i].name_cstr(),
								 const_cast<Value&>(get(PINFOS[i].property())));
  }

  GrObj::readFrom(reader);
}

void House::writeTo(IO::Writer* writer) const {
DOTRACE("House::writeTo");
  for (unsigned int i = 0; i < NUM_PINFOS; ++i) {
	 writer->writeValueObj(PINFOS[i].name_cstr(),
								  get(PINFOS[i].property()));
  }

  GrObj::writeTo(writer);
}

///////////////////////////////////////////////////////////////////////
//
// Properties
//
///////////////////////////////////////////////////////////////////////

unsigned int House::numPropertyInfos() {
DOTRACE("House::numPropertyInfos");
  return NUM_PINFOS;
}

const House::PInfo& House::getPropertyInfo(unsigned int i) {
DOTRACE("House::getPropertyInfo");
  return PINFOS[i];
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
