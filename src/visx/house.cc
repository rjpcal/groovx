///////////////////////////////////////////////////////////////////////
//
// house.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Sep 13 12:43:16 1999
// written: Wed Aug 15 19:42:08 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef HOUSE_CC_DEFINED
#define HOUSE_CC_DEFINED

#ifdef ACC_COMPILER
#pragma OPT_LEVEL 1
#endif

#include "house.h"

#include "gfx/rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"

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

namespace
{
  const IO::VersionId HOUSE_SERIAL_VERSION_ID = 2;

  void drawWindow(int num_vert_bars, int num_horiz_bars)
  {
    // Draw 1x1 window centered on (0,0)

    glBegin(GL_LINES);
    {
      for (int x = 0; x < (num_vert_bars+2); ++x)
        {
          double xpos = double(x)/(num_vert_bars+1) - 0.5;
          glVertex2d(xpos, -0.5);
          glVertex2d(xpos,  0.5);
        }
      for (int y = 0; y < (num_horiz_bars+2); ++y)
        {
          double ypos = double(y)/(num_horiz_bars+1) - 0.5;
          glVertex2d(-0.5, ypos);
          glVertex2d( 0.5, ypos);
        }
    }
    glEnd();
  }

  void drawDoor()
  {
    // Draw 1x1 door with bottom line centered on (0,0)
    glRectd(-0.5, 0.0, 0.5, 1.0);
    glBegin(GL_LINES);
      glVertex2d(0.25, 0.35);
      glVertex2d(0.25, 0.45);
    glEnd();
  }

  void drawStoryFrame()
  {
    // Draw 1x1 story frame centered on (0,0)
    glRectd(-0.5, -0.5, 0.5, 0.5);
  }

  void drawTriangleRoof()
  {
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

  void drawTrapezoidRoof()
  {
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

  void drawSquareRoof()
  {
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

  void drawChimney ()
  {
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

const FieldMap& House::classFields()
{
  static const Field FINFOS[] =
  {
    Field("storyAspectRatio",
          &House::itsStoryAspectRatio, 3.0, 0.5, 10.0, 0.05, true),
    Field("numStories", &House::itsNumStories, 2, 1, 5, 1),

    Field("doorPosition", &House::itsDoorPosition, 2, 0, 5, 1, true),
    Field("doorWidth", &House::itsDoorWidth, 0.75, 0.05, 1.0, 0.05),
    Field("doorHeight", &House::itsDoorHeight, 0.75, 0.05, 1.0, 0.05),
    Field("doorOrientation",
          &House::itsDoorOrientation, false, false, true, true),

    Field("numWindows", &House::itsNumWindows, 5, 2, 6, 1, true),
    Field("windowWidth", &House::itsWindowWidth, 0.75, 0.05, 1.0, 0.05),
    Field("windowHeight", &House::itsWindowHeight, 0.5, 0.05, 1.0, 0.05),
    Field("windowVertBars", &House::itsWindowVertBars, 1, 0, 5, 1),
    Field("windowHorizBars", &House::itsWindowHorizBars, 1, 0, 5, 1),

    Field("roofShape", &House::itsRoofShape, 0, 0, 2, 1, true),
    Field("roofHeight", &House::itsRoofHeight, 0.05, 0.05, 2.0, 0.05),
    Field("roofOverhang", &House::itsRoofOverhang, 0.75, 0.0, 0.5, 0.05),
    Field("roofColor", &House::itsRoofColor, 1, 0, 1, 1),

    Field("chimneyXPosition",
          &House::itsChimneyXPosition, 0.2, -0.5, 0.5, 0.05, true),
    Field("chimneyYPosition",
          &House::itsChimneyYPosition, 0.5, 0.0, 1.0, 0.05),
    Field("chimneyWidth", &House::itsChimneyWidth, 0.06, 0.01, 0.30, 0.01),
    Field("chimneyHeight", &House::itsChimneyHeight, 0.5, 0.05, 2.0, 0.1)
  };

  const unsigned int NUM_FINFOS = sizeof(FINFOS)/sizeof(Field);

  static FieldMap HOUSE_FIELDS(FINFOS, FINFOS+NUM_FINFOS,
                               &GrObj::classFields());

  return HOUSE_FIELDS;
}

House* House::make()
{
DOTRACE("House::make");
  return new House;
}

House::House() :
  itsStoryAspectRatio(3.0),
  itsNumStories(2),

  itsDoorPosition(2),
  itsDoorWidth(0.75),
  itsDoorHeight(0.75),
  itsDoorOrientation(false),

  itsNumWindows(5),
  itsWindowWidth(0.75),
  itsWindowHeight(0.5),
  itsWindowVertBars(1),
  itsWindowHorizBars(1),

  itsRoofShape(0),
  itsRoofOverhang(0.05),
  itsRoofHeight(0.75),
  itsRoofColor(1),

  itsChimneyXPosition(0.2),
  itsChimneyYPosition(0.5),
  itsChimneyWidth(0.06),
  itsChimneyHeight(0.5)
{
DOTRACE("House::House");

  setFieldMap(House::classFields());
}

House::~House()
{
DOTRACE("House::~House");
}

IO::VersionId House::serialVersionId() const
{
DOTRACE("House::serialVersionId");
  return HOUSE_SERIAL_VERSION_ID;
}

void House::readFrom(IO::Reader* reader)
{
DOTRACE("House::readFrom");

  reader->ensureReadVersionId("House", 2, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void House::writeTo(IO::Writer* writer) const
{
DOTRACE("House::writeTo");

  writer->ensureWriteVersionId("House", HOUSE_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

///////////////////////////////////////////////////////////////////////
//
// Actions
//
///////////////////////////////////////////////////////////////////////

Gfx::Rect<double> House::grGetBoundingBox() const
{
DOTRACE("House::grGetBoundingBox");

  Gfx::Rect<double> bbox;

  GLdouble main_width = itsStoryAspectRatio;
  GLdouble main_height = itsNumStories + itsRoofHeight;

  GLdouble max_dimension = max(main_height, main_width);

  bbox.left()   = -main_width/2.0 * (1 + max(itsRoofOverhang, 0.0)) / max_dimension;
  bbox.right()  =  main_width/2.0 * (1 + max(itsRoofOverhang, 0.0)) / max_dimension;
  bbox.bottom() = -main_height/2.0 / max_dimension;

  bbox.top()    =  main_height/2.0;
  if ( (itsChimneyYPosition + itsChimneyHeight) > itsRoofHeight )
    {
      bbox.top() += itsChimneyYPosition + itsChimneyHeight - itsRoofHeight;
    }

  bbox.top()   /= max_dimension;

  return bbox;
}

void House::grRender(Gfx::Canvas&, DrawMode) const
{
DOTRACE("House::grRender");
  GLdouble total_width = itsStoryAspectRatio;
  GLdouble total_height = itsNumStories + itsRoofHeight;

  GLdouble max_dimension =
    (total_height > total_width) ? total_height : total_width;

  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
  {
    // Scale so that the larger dimension (of width and height)
    // extends across 1.0 units in GL coordinates. The smaller
    // dimension will extend across less than 1.0 GL units.
    glScaled(itsStoryAspectRatio/max_dimension, 1.0/max_dimension, 1.0);

    // Translate down by half the height of the house, then up by 0.5
    // units, so that we are positioned in the center of the first
    // story to be drawn.
    glTranslated(0.0, -total_height/2.0 + 0.5, 0.0);

    glPushAttrib(GL_POLYGON_BIT);
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


      // Loop over house stories.
      for (int s = 0; s < itsNumStories; ++s)
        {
          drawStoryFrame();

          // Loop over window positions.
          for (int w = 0; w < itsNumWindows; ++w)
            {
              glPushMatrix();
              {
                GLdouble x_pos = double(w+0.5)/itsNumWindows - 0.5;
                if (s == 0 && w == itsDoorPosition )
                  {
                    // Draw door.
                    glTranslated(x_pos, -0.5, 0.0);
                    glScaled(itsDoorWidth/itsNumWindows, itsDoorHeight, 1.0);
                    if (itsDoorOrientation)
                      {
                        glScaled(-1.0, 1.0, 1.0);
                      }
                    drawDoor();
                  }
                else
                  {
                    // Draw window.
                    glTranslated(x_pos, 0.0, 0.0);
                    glScaled(itsWindowWidth/itsNumWindows, itsWindowHeight, 1.0);
                    drawWindow(itsWindowVertBars, itsWindowHorizBars);
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
        glScaled(1.0+itsRoofOverhang, itsRoofHeight, 1.0);

        glPushAttrib(GL_POLYGON_BIT);
        {
          if (itsRoofColor == 0)
            {
              glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
          else
            {
              glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }

          if (itsRoofShape == 0)
            {
              drawTriangleRoof();
            }
          else if (itsRoofShape == 1)
            {
              drawTrapezoidRoof();
            }
          else
            {
              drawSquareRoof();
            }
        }
        glPopAttrib();

      }
      glPopMatrix();

      // Draw chimney.
      glPushMatrix();
      {
        glTranslated(itsChimneyXPosition, itsChimneyYPosition, 0.0);
        glScaled(itsChimneyWidth, itsChimneyHeight, 0.0);

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
