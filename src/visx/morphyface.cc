///////////////////////////////////////////////////////////////////////
//
// morphyface.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Sep  8 15:38:42 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef MORPHYFACE_CC_DEFINED
#define MORPHYFACE_CC_DEFINED

#include "visx/morphyface.h"

#include "geom/bezier4.h"
#include "geom/rect.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/gxscaler.h"
#include "gfx/linestrip.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using geom::vec2d;
using geom::vec3d;

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId MFACE_SERIAL_VERSION_ID = 2;

  const unsigned int NUM_HAIR_POINTS = 15; // can't change this without
                                           // also changing "hair_widths"
                                           // below
  const double* getHairVertices(double top_width, double hair_width)
  {
    DOTRACE("getHairVertices");

    static double hair_vertices[NUM_HAIR_POINTS*4];

    static const double hair_widths[NUM_HAIR_POINTS] =
    {
      0.40, 0.90, 0.95,
      1.00, 0.97, 0.94,
      0.91, 0.88, 0.85,
      0.81, 0.78, 0.75,
      0.7, 0.6, 0.0
    };

    geom::bezier4 xbezier(-1.0, -top_width, top_width, 1.0);
    geom::bezier4 ybezier( 0.0,  4.0/3.0  , 4.0/3.0  , 0.0);

    for (unsigned int i = 0; i < NUM_HAIR_POINTS; ++i)
      {
        const double u = 0.5 + double(i)/double(2*NUM_HAIR_POINTS-2);

        const double x = xbezier.eval(u);
        const double y = ybezier.eval(u);

        const double tang_x = xbezier.eval_deriv(u);
        const double tang_y = ybezier.eval_deriv(u);

        // tangent vector = (tang_x, tang_y)
        // ==> normal vector = (-tang_y, tang_x)
        const double norm_x = -tang_y;
        const double norm_y = tang_x;

        // compute the factor needed to make a unit normal vector
        const double norm_factor = 1.0 / sqrt(norm_x*norm_x + norm_y*norm_y);

        const double hair_x_normal = norm_x * norm_factor;
        const double hair_y_normal = norm_y * norm_factor;

        const double width_factor = hair_widths[i]*hair_width;

        hair_vertices[4*i]   = x - hair_x_normal*width_factor; // inner x value
        hair_vertices[4*i+1] = y - hair_y_normal*width_factor; // inner y value
        hair_vertices[4*i+2] = x + hair_x_normal*width_factor; // outer x value
        hair_vertices[4*i+3] = y + hair_y_normal*width_factor; // outer y value
      }

    return &hair_vertices[0];
  }

  void drawHairStrip(Gfx::Canvas& canvas,
                     const double* vertices, double x_scale, double y_scale)
  {
    Gfx::MatrixSaver saver(canvas);

    canvas.scale(vec3d(x_scale, y_scale, 1.0));

    Gfx::QuadStripBlock block(canvas);

    for (unsigned int i = 0; i < NUM_HAIR_POINTS; ++i)
      {
        canvas.vertex2(vec2d(vertices[4*i  ], vertices[4*i+1]));
        canvas.vertex2(vec2d(vertices[4*i+2], vertices[4*i+3]));
      }
  }

}

///////////////////////////////////////////////////////////////////////
//
// MorphyFace member functions
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Creators
///////////////////////////////////////////////////////////////////////

const FieldMap& MorphyFace::classFields()
{
  typedef MorphyFace MF;

  static const Field FIELD_ARRAY[] =
  {
    Field("category", &MF::itsMfaceCategory, 0, 0, 10, 1, Field::NEW_GROUP),

    Field("faceWidth", &MF::itsFaceWidth, 2.75, 1.5, 3.5, 0.1),
    Field("topWidth", &MF::itsTopWidth, 1.15, 0.05, 2.0, 0.05),
    Field("bottomWidth", &MF::itsBottomWidth, 1.0, 0.05, 2.0, 0.05),
    Field("topHeight", &MF::itsTopHeight, 3.8, 0.5, 5.0, 0.25),
    Field("bottomHeight", &MF::itsBottomHeight, -3.0, -5.0, -0.5, 0.25),
    Field("strokeWidth", &MF::itsStrokeWidth, 1.0, 0.1, 10.0, 0.1,
          Field::TRANSIENT),
    Field("lineJoin", &MF::itsLineJoin, true, false, true, true,
          Field::TRANSIENT | Field::BOOLEAN),

    Field("hairWidth", &MF::itsHairWidth, 0.20, 0.00, 0.5, 0.02, Field::NEW_GROUP),
    Field("hairStyle", &MF::itsHairStyle, 0, 0, 1, 1),

    Field("eyeYpos", &MF::itsEyeYpos, 0.375, -2.0, 2.0, 0.1, Field::NEW_GROUP),
    Field("eyeDistance", &MF::itsEyeDistance, 2.25, 0.0, 5.0, 0.25),
    Field("eyeHeight", &MF::itsEyeHeight, 0.9, 0.0, 2.0, 0.1),
    Field("eyeAspectRatio", &MF::itsEyeAspectRatio, 1.555556, 0.1, 5.0, 0.1),

    Field("pupilXpos", &MF::itsPupilXpos, 0.0, -0.5, 0.5, 0.05, Field::NEW_GROUP),
    Field("pupilYpos", &MF::itsPupilYpos, 0.0, -0.5, 0.5, 0.05),
    Field("pupilSize", &MF::itsPupilSize, 0.6, 0.0, 1.0, 0.05),
    Field("pupilDilation", &MF::itsPupilDilation,
          0.5, 0.05, 0.95, 0.05, Field::CHECKED),

    Field("eyebrowXpos", &MF::itsEyebrowXpos, 0.0, -0.5, 0.5, 0.02,
          Field::NEW_GROUP),
    Field("eyebrowYpos", &MF::itsEyebrowYpos, 0.5, 0.0, 1.5, 0.05),
    Field("eyebrowCurvature", &MF::itsEyebrowCurvature, 0.8, -2.0, 2.0, 0.1),
    Field("eyebrowAngle", &MF::itsEyebrowAngle, -5.0, -50.0, 50.0, 1.0),
    Field("eyebrowThickness", &MF::itsEyebrowThickness, 2.0, 0.1, 4.0, 0.1),

    Field("noseXpos", &MF::itsNoseXpos, 0.0, -1.0, 1.0, 0.05, Field::NEW_GROUP),
    Field("noseYpos", &MF::itsNoseYpos, -0.825, -2.0, 2.0, 0.1),
    Field("noseLength", &MF::itsNoseLength, 0.75, 0.0, 2.0, 0.1),
    Field("noseWidth", &MF::itsNoseWidth, 1.5, 0.0, 3.0, 0.1),

    Field("mouthXpos", &MF::itsMouthXpos, 0.0, -2.0, 2.0, 0.1, Field::NEW_GROUP),
    Field("mouthYpos", &MF::itsMouthYpos, -2.0, -3.0, 1.0, 0.1),
    Field("mouthWidth", &MF::itsMouthWidth, 2.5, 0.0, 5.0, 0.25),
    Field("mouthCurvature", &MF::itsMouthCurvature, 0.6, -2.0, 2.0, 0.1)
  };

  static FieldMap MFACE_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return MFACE_FIELDS;
}

MorphyFace* MorphyFace::make()
{
DOTRACE("MorphyFace::make");
  return new MorphyFace;
}

MorphyFace::MorphyFace() :
  GxShapeKit(),

  itsMfaceCategory(0),

  itsFaceWidth(2.75),
  itsTopWidth(1.15),
  itsBottomWidth(1.0),
  itsTopHeight(3.8),
  itsBottomHeight(-3.0),

  itsHairWidth(0.15),
  itsHairStyle(0),

  itsEyeYpos(0.375),
  itsEyeDistance(2.25),
  itsEyeHeight(0.9),
  itsEyeAspectRatio(1.555556),

  itsPupilXpos(0.0),
  itsPupilYpos(0.0),
  itsPupilSize(0.6),
  itsPupilDilation(0.5),

  itsEyebrowXpos(0.0),
  itsEyebrowYpos(0.5),
  itsEyebrowCurvature(0.8),
  itsEyebrowAngle(-5),
  itsEyebrowThickness(2.0),

  itsNoseXpos(0.0),
  itsNoseYpos(-0.825),
  itsNoseLength(0.75),
  itsNoseWidth(1.5),

  itsMouthXpos(0.0),
  itsMouthYpos(-2.0),
  itsMouthWidth(2.5),
  itsMouthCurvature(0.6),

  itsStrokeWidth(3.0),
  itsLineJoin(true)
{
DOTRACE("MorphyFace::MorphyFace");

  setFieldMap(MorphyFace::classFields());

  setScalingMode(GxScaler::MAINTAIN_ASPECT_SCALING);
  setMaxDimension(1.0);
}

MorphyFace::~MorphyFace() throw()
{
DOTRACE("MorphyFace::~MorphyFace");
  // nothing to do
}

IO::VersionId MorphyFace::serialVersionId() const
{
DOTRACE("MorphyFace::serialVersionId");
  return MFACE_SERIAL_VERSION_ID;
}

void MorphyFace::readFrom(IO::Reader& reader)
{
DOTRACE("MorphyFace::readFrom");

  reader.ensureReadVersionId("MorphyFace", 2,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  readFieldsFrom(reader, classFields());

  reader.readBaseClass("GxShapeKit", IO::makeProxy<GxShapeKit>(this));
}

void MorphyFace::writeTo(IO::Writer& writer) const
{
DOTRACE("MorphyFace::writeTo");

  writer.ensureWriteVersionId("MorphyFace", MFACE_SERIAL_VERSION_ID, 2,
                              "Try groovx0.8a4", SRC_POS);

  writeFieldsTo(writer, classFields());

  writer.writeBaseClass("GxShapeKit", IO::makeConstProxy<GxShapeKit>(this));
}


///////////////////////////////////////////////////////////////////////
// Actions
///////////////////////////////////////////////////////////////////////

void MorphyFace::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("MorphyFace::grRender");

  Gfx::AttribSaver attribSaver(canvas);

  //canvas.enableAntialiasing();

  Gfx::LineStrip ls;
  ls.lineJoin(itsLineJoin);

  //
  // Draw eyes
  //

  const vec3d eye_ctrlpnts[4] =
  {
    vec3d(-3.0/7.0, 0.0,     0.0),
    vec3d(-2.0/7.0, 2.0/3.0, 0.0),
    vec3d( 2.0/7.0, 2.0/3.0, 0.0),
    vec3d( 4.0/7.0, 0.0,     0.0)
  };

  const int eye_subdivisions = 20;

  for (int left_right = -1; left_right < 2; left_right += 2)
    {
      Gfx::MatrixSaver msaver(canvas);

      // Do appropriate reflection
      canvas.scale(vec3d(left_right*1.0, 1.0, 1.0));

      // Move to the eye position
      canvas.translate(vec3d(rutz::abs(itsEyeDistance)/2.0,
                             itsEyeYpos, 0.0));

      // Draw eye outline
      {
        DOTRACE("MorphyFace::grRender-draw eye outline");

        const vec3d s1(itsEyeHeight*itsEyeAspectRatio,
                       -itsEyeHeight,
                       1.0);
        const vec3d s2(itsEyeHeight*itsEyeAspectRatio,
                       itsEyeHeight,
                       1.0);

        ls.closeLoop(true);
        ls.begin(canvas, 0.01*itsStrokeWidth);
        // Two bezier curves end to end, sharing common vertices in the middle
        ls.drawBezier4(s1*eye_ctrlpnts[3], s1*eye_ctrlpnts[2],
                       s1*eye_ctrlpnts[1], s1*eye_ctrlpnts[0],
                       eye_subdivisions, 1 /* skip the first vertex since
                                              it will be wrapped around to
                                              in the next bezier curve */);
        ls.drawBezier4(s2*eye_ctrlpnts[0], s2*eye_ctrlpnts[1],
                       s2*eye_ctrlpnts[2], s2*eye_ctrlpnts[3],
                       eye_subdivisions, 1 /* skip the first vertex since
                                              it was already covered by the
                                              previous bezier curve */);
        ls.end();
        ls.closeLoop(false);
      }

      // Draw eyebrow
      {
        DOTRACE("MorphyFace::grRender-draw eyebrow");

        Gfx::MatrixSaver msaver3(canvas);

        canvas.translate(vec3d(itsEyebrowXpos, itsEyebrowYpos, 0.0));
        canvas.rotate(vec3d::unit_z(), itsEyebrowAngle);

        Gfx::AttribSaver asaver(canvas);

        canvas.setLineWidth(itsEyebrowThickness);

        const vec3d s(itsEyeHeight*itsEyeAspectRatio,
                      itsEyeHeight*itsEyebrowCurvature,
                      1.0);

        ls.begin(canvas, 0.01*itsStrokeWidth*itsEyebrowThickness);
        ls.drawBezier4(s*eye_ctrlpnts[0], s*eye_ctrlpnts[1],
                       s*eye_ctrlpnts[2], s*eye_ctrlpnts[3],
                       eye_subdivisions);
        ls.end();
      }

      // Draw pupil
      {
        DOTRACE("MorphyFace::grRender-draw pupil");

        Gfx::MatrixSaver msaver4(canvas);

        canvas.translate(vec3d(left_right*itsPupilXpos,
                               itsPupilYpos, 0.0));

        double radius = 0.5 * itsPupilSize * itsEyeHeight;

        static const int num_slices = 20;
        static const int num_loops = 1;

        canvas.drawCircle(radius*rutz::abs(itsPupilDilation), radius, true,
                          num_slices, num_loops);
      }
    }

  //
  // Draw face outline.
  //

  ls.closeLoop(true);
  ls.begin(canvas, 0.015*itsStrokeWidth);
  ls.drawBezier4(vec3d(itsFaceWidth, 0.0, 0.0),
                 vec3d(itsBottomWidth*itsFaceWidth,
                       itsBottomHeight*4.0/3.0, 0.0),
                 vec3d(-itsBottomWidth*itsFaceWidth,
                       itsBottomHeight*4.0/3.0, 0.0),
                 vec3d(-itsFaceWidth, 0.0, 0.0),
                 50, 1);
  ls.drawBezier4(vec3d(-itsFaceWidth, 0.0, 0.0),
                 vec3d(-itsTopWidth*itsFaceWidth,
                       itsTopHeight*4.0/3.0, 0.0),
                 vec3d(itsTopWidth*itsFaceWidth,
                       itsTopHeight*4.0/3.0, 0.0),
                 vec3d(itsFaceWidth, 0.0, 0.0),
                 50, 1);
  ls.end();
  ls.closeLoop(false);

  //
  // Draw nose.
  //

  {
    DOTRACE("MorphyFace::grRender-draw nose");

    Gfx::MatrixSaver msaver5(canvas);

    canvas.translate(vec3d(itsNoseXpos, itsNoseYpos, 0.0));

    const vec2d s(rutz::abs(itsNoseWidth)/2.0,
                  rutz::abs(itsNoseLength));

    ls.begin(canvas, 0.01*itsStrokeWidth);
    ls.vertex(s*vec2d(-0.75, 0.5));
    ls.vertex(s*vec2d(-1.0,  0.0));
    ls.vertex(s*vec2d(-0.75, -0.333333));
    ls.vertex(s*vec2d(-0.25, -0.333333));
    ls.vertex(s*vec2d( 0.0, -0.5));   // CENTER
    ls.vertex(s*vec2d( 0.25, -0.333333));
    ls.vertex(s*vec2d( 0.75, -0.333333));
    ls.vertex(s*vec2d( 1.0,  0.0));
    ls.vertex(s*vec2d( 0.75, 0.5));
    ls.end();
  }

  //
  // Draw mouth.
  //

  {
    DOTRACE("MorphyFace::grRender-draw mouth");

    Gfx::MatrixSaver msaver6(canvas);

    canvas.translate(vec3d(itsMouthXpos, itsMouthYpos, 0.0));

    const vec3d s(itsMouthWidth, itsMouthCurvature, 1.0);

    ls.begin(canvas, 0.01*itsStrokeWidth);
    ls.drawBezier4(s*vec3d(-0.5,  0.5,      0.0),
                   s*vec3d(-0.2, -0.833333, 0.0),
                   s*vec3d( 0.2, -0.833333, 0.0),
                   s*vec3d( 0.5,  0.5,      0.0),
                   30);
    ls.end();
  }

  //
  // Draw hair.
  //

  {
    DOTRACE("MorphyFace::grRender-draw hair");

    Gfx::AttribSaver saver(canvas);

    canvas.setPolygonFill((itsHairStyle == 0));

    const double* hair_vertices = getHairVertices(itsTopWidth, itsHairWidth);

    // Draw right side
    drawHairStrip(canvas, hair_vertices, itsFaceWidth, itsTopHeight);

    // Draw left side
    drawHairStrip(canvas, hair_vertices, -itsFaceWidth, itsTopHeight);
  }
}

///////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////

void MorphyFace::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("MorphyFace::grGetBoundingBox");

  geom::rect<double> rect;

  geom::bezier4 xbezier_top(-1.0, -itsTopWidth, itsTopWidth, 1.0);
  geom::bezier4 xbezier_bottom(1.0, itsBottomWidth, -itsBottomWidth, -1.0);

  double top_width = xbezier_top.eval_max();
  double bottom_width = xbezier_bottom.eval_max();

  dbg_eval(3, top_width);   dbg_eval_nl(3, bottom_width);

  double max_width = rutz::max(1.0, rutz::max(top_width, bottom_width));

  dbg_eval_nl(3, max_width);

  rect.left()   = -max_width      * itsFaceWidth * (1 + itsHairWidth);
  rect.right()  =  max_width      * itsFaceWidth * (1 + itsHairWidth);
  rect.top()    =  itsTopHeight * (1 + itsHairWidth);
  rect.bottom() =  itsBottomHeight;

  bbox.drawRect(rect);
}

static const char vcid_morphyface_cc[] = "$Header$";
#endif // !MORPHYFACE_CC_DEFINED
