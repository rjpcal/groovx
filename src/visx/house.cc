/** @file visx/house.cc graphic object for drawing parameterized house
    outlines */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Sep 13 12:43:16 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_VISX_HOUSE_CC_UTC20050626084015_DEFINED
#define GROOVX_VISX_HOUSE_CC_UTC20050626084015_DEFINED

#include "house.h"

#include "geom/rect.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "rutz/algo.h"

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using geom::rectd;
using geom::vec2d;
using geom::vec3d;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const io::version_id HOUSE_SVID = 3;

  void drawWindow(Gfx::Canvas& canvas, int num_vert_bars, int num_horiz_bars)
  {
    GVX_TRACE("House::drawWindow");
    // Draw 1x1 window centered on (0,0)

    {
      Gfx::LinesBlock block(canvas, "window bars");

      for (int x = 1; x < (num_vert_bars+1); ++x)
        {
          double xpos = double(x)/(num_vert_bars+1) - 0.5;
          canvas.vertex2(vec2d(xpos, -0.5));
          canvas.vertex2(vec2d(xpos,  0.5));
        }
      for (int y = 1; y < (num_horiz_bars+1); ++y)
        {
          double ypos = double(y)/(num_horiz_bars+1) - 0.5;
          canvas.vertex2(vec2d(-0.5, ypos));
          canvas.vertex2(vec2d( 0.5, ypos));
        }
    }

    {
      Gfx::QuadsBlock block(canvas, "window outline");
      canvas.vertex2(vec2d(-0.5, -0.5));
      canvas.vertex2(vec2d( 0.5, -0.5));
      canvas.vertex2(vec2d( 0.5, 0.5));
      canvas.vertex2(vec2d(-0.5, 0.5));
    }
  }

  void drawDoor(Gfx::Canvas& canvas)
  {
    GVX_TRACE("House::drawDoor");
    // Draw 1x1 door with bottom line centered on (0,0)
    canvas.drawRect(rectd::lbwh(-0.5, 0.0, 1.0, 1.0));

    Gfx::LinesBlock block(canvas, "door");

    canvas.vertex2(vec2d(0.25, 0.35));
    canvas.vertex2(vec2d(0.25, 0.45));
  }

  void drawStoryFrame(Gfx::Canvas& canvas)
  {
    GVX_TRACE("House::drawStoryFrame");
    // Draw 1x1 story frame centered on (0,0)
    canvas.drawRect(rectd::lbwh(-0.5, -0.5, 1.0, 1.0));
  }

  void drawTriangleRoof(Gfx::Canvas& canvas)
  {
    GVX_TRACE("House::drawTriangleRoof");
    // Draw 1x1-bounded triangle with bottom line centered on (0,0)

    {
      Gfx::TrianglesBlock block(canvas, "triangle roof");
      canvas.vertex2(vec2d(-0.5, 0.0));
      canvas.vertex2(vec2d( 0.5, 0.0));
      canvas.vertex2(vec2d( 0.0, 1.0));
    }
    {
      Gfx::LinesBlock block(canvas, "triangle roof ledge");
      canvas.vertex2(vec2d(-0.5, 0.0));
      canvas.vertex2(vec2d( 0.5, 0.0));
    }
  }

  void drawTrapezoidRoof(Gfx::Canvas& canvas)
  {
    GVX_TRACE("House::drawTrapezoidRoof");
    {
      Gfx::QuadsBlock block(canvas, "trapezoid roof");
      canvas.vertex2(vec2d(-0.5, 0.0));
      canvas.vertex2(vec2d( 0.5, 0.0));
      canvas.vertex2(vec2d( 0.4, 1.0));
      canvas.vertex2(vec2d(-0.4, 1.0));
    }
    {
      Gfx::LinesBlock block(canvas, "trapezoid roof overhand");
      canvas.vertex2(vec2d(-0.5, 0.0));
      canvas.vertex2(vec2d( 0.5, 0.0));
    }
  }

  void drawSquareRoof(Gfx::Canvas& canvas)
  {
    GVX_TRACE("House::drawSquareRoof");
    {
      Gfx::QuadsBlock block(canvas, "square roof");
      canvas.vertex2(vec2d(-0.5, 0.0));
      canvas.vertex2(vec2d( 0.5, 0.0));
      canvas.vertex2(vec2d( 0.5, 1.0));
      canvas.vertex2(vec2d(-0.5, 1.0));
    }
    {
      Gfx::LinesBlock block(canvas, "square roof overhang");
      canvas.vertex2(vec2d(-0.5, 0.0));
      canvas.vertex2(vec2d( 0.5, 0.0));
    }
  }

  void drawChimney(Gfx::Canvas& canvas)
  {
    GVX_TRACE("House::drawChimney");
    {
      Gfx::QuadsBlock block(canvas, "chimney");
      canvas.vertex2(vec2d( 0.5, 0.0));
      canvas.vertex2(vec2d(-0.5, 0.0));
      canvas.vertex2(vec2d(-0.5, 1.0));
      canvas.vertex2(vec2d( 0.5, 1.0));
    }
  }
}

///////////////////////////////////////////////////////////////////////
//
// House member definitions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& House::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("storyAspectRatio",
          &House::itsStoryAspectRatio, 3.0, 0.5, 10.0, 0.05, Field::NEW_GROUP),
    Field("numStories", &House::itsNumStories, 2, 1, 5, 1),

    Field("doorPosition", &House::itsDoorPosition, 2, 0, 5, 1, Field::NEW_GROUP),
    Field("doorWidth", &House::itsDoorWidth, 0.75, 0.05, 1.0, 0.05),
    Field("doorHeight", &House::itsDoorHeight, 0.75, 0.05, 1.0, 0.05),
    Field("doorOrientation",
          &House::itsDoorOrientation, false, false, true, true),

    Field("numWindows", &House::itsNumWindows, 5, 2, 6, 1, Field::NEW_GROUP),
    Field("windowWidth", &House::itsWindowWidth, 0.75, 0.05, 1.0, 0.05),
    Field("windowHeight", &House::itsWindowHeight, 0.5, 0.05, 1.0, 0.05),
    Field("windowVertBars", &House::itsWindowVertBars, 1, 0, 5, 1),
    Field("windowHorizBars", &House::itsWindowHorizBars, 1, 0, 5, 1),

    Field("roofShape", &House::itsRoofShape, 0, 0, 2, 1, Field::NEW_GROUP),
    Field("roofHeight", &House::itsRoofHeight, 0.05, 0.05, 2.0, 0.05),
    Field("roofOverhang", &House::itsRoofOverhang, 0.75, 0.0, 0.5, 0.05),
    Field("roofColor", &House::itsRoofColor, 1, 0, 1, 1),

    Field("chimneyXPosition",
          &House::itsChimneyXPosition, 0.2, -0.5, 0.5, 0.05, Field::NEW_GROUP),
    Field("chimneyYPosition",
          &House::itsChimneyYPosition, 0.5, 0.0, 1.0, 0.05),
    Field("chimneyWidth", &House::itsChimneyWidth, 0.06, 0.01, 0.30, 0.01),
    Field("chimneyHeight", &House::itsChimneyHeight, 0.5, 0.05, 2.0, 0.1)
  };

  static FieldMap HOUSE_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return HOUSE_FIELDS;
}

House* House::make()
{
GVX_TRACE("House::make");
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
GVX_TRACE("House::House");

  setFieldMap(House::classFields());
}

House::~House() throw()
{
GVX_TRACE("House::~House");
}

io::version_id House::class_version_id() const
{
GVX_TRACE("House::class_version_id");
  return HOUSE_SVID;
}

void House::read_from(io::reader& reader)
{
GVX_TRACE("House::read_from");

  reader.ensure_version_id("House", 3,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  readFieldsFrom(reader, classFields());

  reader.read_base_class("GxShapeKit", io::make_proxy<GxShapeKit>(this));
}

void House::write_to(io::writer& writer) const
{
GVX_TRACE("House::write_to");

  writer.ensure_output_version_id("House", HOUSE_SVID, 3,
                              "Try groovx0.8a4", SRC_POS);

  writeFieldsTo(writer, classFields(), HOUSE_SVID);

  writer.write_base_class("GxShapeKit", io::make_const_proxy<GxShapeKit>(this));
}

///////////////////////////////////////////////////////////////////////
//
// Actions
//
///////////////////////////////////////////////////////////////////////

void House::grGetBoundingBox(Gfx::Bbox& bbox) const
{
GVX_TRACE("House::grGetBoundingBox");

  const double main_width = itsStoryAspectRatio;
  const double main_height = itsNumStories + itsRoofHeight;

  using rutz::max;

  const double max_dim = max(main_height, main_width);

  const double extra_chimney_height =
    max(0.0, itsChimneyYPosition + itsChimneyHeight - itsRoofHeight);

  const double l = -main_width/2.0 * (1 + max(itsRoofOverhang, 0.0)) / max_dim;
  const double r =  main_width/2.0 * (1 + max(itsRoofOverhang, 0.0)) / max_dim;
  const double b = -main_height/2.0 / max_dim;
  const double t =  (main_height/2.0 + extra_chimney_height) / max_dim;

  bbox.drawRect(rectd::ltrb(l,t,r,b));
}

void House::grRender(Gfx::Canvas& canvas) const
{
GVX_TRACE("House::grRender");
  const double total_width = itsStoryAspectRatio;
  const double total_height = itsNumStories + itsRoofHeight;

  const double max_dimension =
    (total_height > total_width) ? total_height : total_width;

  Gfx::MatrixSaver msaver(canvas, "house");
  Gfx::AttribSaver asaver(canvas);

  // Scale so that the larger dimension (of width and height)
  // extends across 1.0 units in world coordinates. The smaller
  // dimension will extend across less than 1.0 world units.
  canvas.scale(vec3d(itsStoryAspectRatio/max_dimension,
                     1.0/max_dimension, 1.0));

  // Translate down by half the height of the house, then up by 0.5
  // units, so that we are positioned in the center of the first
  // story to be drawn.
  canvas.translate(vec3d(0.0, -total_height/2.0 + 0.5, 0.0));

  canvas.setPolygonFill(false);


  // Loop over house stories.
  for (int s = 0; s < itsNumStories; ++s)
    {
      drawStoryFrame(canvas);

      // Loop over window positions.
      for (int w = 0; w < itsNumWindows; ++w)
        {
          Gfx::MatrixSaver msaver(canvas, "window");

          double x_pos = double(w+0.5)/itsNumWindows - 0.5;
          if (s == 0 && w == itsDoorPosition )
            {
              // Draw door.
              canvas.translate(vec3d(x_pos, -0.5, 0.0));
              canvas.scale(vec3d(itsDoorWidth/itsNumWindows,
                                 itsDoorHeight, 1.0));
              if (itsDoorOrientation)
                {
                  canvas.scale(vec3d(-1.0, 1.0, 1.0));
                }
              drawDoor(canvas);
            }
          else
            {
              // Draw window.
              canvas.translate(vec3d(x_pos, 0.0, 0.0));
              canvas.scale(vec3d(itsWindowWidth/itsNumWindows,
                                 itsWindowHeight, 1.0));
              drawWindow(canvas, itsWindowVertBars, itsWindowHorizBars);
            }
        }

      canvas.translate(vec3d(0.0, 1.0, 0.0));
    }


  // Draw roof.
  canvas.translate(vec3d(0.0, -0.5, 0.0));

  {
    Gfx::MatrixSaver msaver(canvas, "roof");
    Gfx::AttribSaver asaver(canvas);

    canvas.scale(vec3d(1.0+itsRoofOverhang, itsRoofHeight, 1.0));

    canvas.setPolygonFill((itsRoofColor == 0));

    if (itsRoofShape == 0)
      {
        drawTriangleRoof(canvas);
      }
    else if (itsRoofShape == 1)
      {
        drawTrapezoidRoof(canvas);
      }
    else
      {
        drawSquareRoof(canvas);
      }
  }

  // Draw chimney.
  {
    Gfx::MatrixSaver msaver(canvas, "chimney");
    Gfx::AttribSaver asaver(canvas);

    canvas.translate(vec3d(itsChimneyXPosition,
                           itsChimneyYPosition, 0.0));
    canvas.scale(vec3d(itsChimneyWidth,
                       itsChimneyHeight, 1.0));

    canvas.setPolygonFill(true);
    drawChimney(canvas);
  }
}

static const char vcid_groovx_visx_house_cc_utc20050626084015[] = "$Id$ $HeadURL$";
#endif // !GROOVX_VISX_HOUSE_CC_UTC20050626084015_DEFINED
