///////////////////////////////////////////////////////////////////////
//
// morphyface.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 15:38:42 1999
// written: Wed Nov 13 12:55:21 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_CC_DEFINED
#define MORPHYFACE_CC_DEFINED

#include "visx/morphyface.h"

#include "visx/bezier.h"

#include "gfx/canvas.h"

#include "gx/rect.h"
#include "gx/vec3.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"

#include "visx/gmodes.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId MFACE_SERIAL_VERSION_ID = 1;

  const unsigned int NUM_HAIR_POINTS = 15;
  const double* getHairVertices(double top_width, double hair_width)
  {
    DOTRACE("getHairVertices");

    static double hair_vertices[NUM_HAIR_POINTS*4];

    double hair_widths[NUM_HAIR_POINTS] =
    {
      0.40, 0.90, 0.95,
      1.00, 0.97, 0.94,
      0.91, 0.88, 0.85,
      0.81, 0.78, 0.75,
      0.7, 0.6, 0.0
    };

    Bezier4 xbezier(-1.0, -top_width, top_width, 1.0);
    Bezier4 ybezier( 0.0,  4.0/3.0  , 4.0/3.0  , 0.0);

    for (unsigned int i = 0; i < NUM_HAIR_POINTS; ++i)
      {
        const double u = 0.5 + double(i)/double(2*NUM_HAIR_POINTS-2);

        const double x = xbezier.eval(u);
        const double y = ybezier.eval(u);

        const double tang_x = xbezier.evalDeriv(u);
        const double tang_y = ybezier.evalDeriv(u);

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

    canvas.scale(Gfx::Vec3<double>(x_scale, y_scale, 1.0));

    Gfx::QuadStripBlock block(canvas);

    for (unsigned int i = 0; i < NUM_HAIR_POINTS; ++i)
      {
        canvas.vertex2(Gfx::Vec2<double>(vertices[4*i  ], vertices[4*i+1]));
        canvas.vertex2(Gfx::Vec2<double>(vertices[4*i+2], vertices[4*i+3]));
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

  static FieldMap MFACE_FIELDS(FIELD_ARRAY, &GrObj::classFields());

  return MFACE_FIELDS;
}

MorphyFace* MorphyFace::make()
{
DOTRACE("MorphyFace::make");
  return new MorphyFace;
}

MorphyFace::MorphyFace() :
  GrObj(),

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
  itsMouthCurvature(0.6)
{
DOTRACE("MorphyFace::MorphyFace");

  setFieldMap(MorphyFace::classFields());

  setScalingMode(Gmodes::MAINTAIN_ASPECT_SCALING);
  setMaxDimension(1.0);
}

MorphyFace::~MorphyFace()
{
DOTRACE("MorphyFace::~MorphyFace");
  // nothing to do
}

IO::VersionId MorphyFace::serialVersionId() const
{
DOTRACE("MorphyFace::serialVersionId");
  return MFACE_SERIAL_VERSION_ID;
}

void MorphyFace::readFrom(IO::Reader* reader)
{
DOTRACE("MorphyFace::readFrom");

  reader->ensureReadVersionId("MorphyFace", 1, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void MorphyFace::writeTo(IO::Writer* writer) const
{
DOTRACE("MorphyFace::writeTo");

  writer->ensureWriteVersionId("MorphyFace", MFACE_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}


///////////////////////////////////////////////////////////////////////
// Actions
///////////////////////////////////////////////////////////////////////

void MorphyFace::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("MorphyFace::grRender");

  Gfx::AttribSaver attribSaver(canvas);

  canvas.enableAntialiasing();

  //
  // Draw eyes
  //

  const Gfx::Vec3<double> eye_ctrlpnts[4] =
  {
    Gfx::Vec3<double>(-3.0/7.0, 0.0,     0.0),
    Gfx::Vec3<double>(-2.0/7.0, 2.0/3.0, 0.0),
    Gfx::Vec3<double>( 2.0/7.0, 2.0/3.0, 0.0),
    Gfx::Vec3<double>( 4.0/7.0, 0.0,     0.0)
  };

  const int eye_subdivisions = 10;

  for (int left_right = -1; left_right < 2; left_right += 2)
    {
      Gfx::MatrixSaver msaver(canvas);

      // Do appropriate reflection
      canvas.scale(Gfx::Vec3<double>(left_right*1.0, 1.0, 1.0));

      // Move to the eye position
      canvas.translate(Gfx::Vec3<double>(Util::abs(itsEyeDistance)/2.0,
                                         itsEyeYpos, 0.0));

      // Draw eye outline
      for (int top_bottom = -1; top_bottom < 2; top_bottom += 2)
        {
          DOTRACE("MorphyFace::grRender-draw eye outline");

          Gfx::MatrixSaver msaver2(canvas);

          canvas.scale(Gfx::Vec3<double>(itsEyeHeight*itsEyeAspectRatio,
                                         itsEyeHeight*top_bottom,
                                         1.0));

          canvas.drawBezier4(eye_ctrlpnts[0], eye_ctrlpnts[1],
                             eye_ctrlpnts[2], eye_ctrlpnts[3],
                             eye_subdivisions);
        }

      // Draw eyebrow
      {
        DOTRACE("MorphyFace::grRender-draw eyebrow");

        Gfx::MatrixSaver msaver3(canvas);

        canvas.translate(Gfx::Vec3<double>(itsEyebrowXpos, itsEyebrowYpos, 0.0));
        canvas.rotate(Gfx::Vec3<double>::unitZ(), itsEyebrowAngle);
        canvas.scale(Gfx::Vec3<double>(itsEyeHeight*itsEyeAspectRatio,
                                       itsEyeHeight*itsEyebrowCurvature,
                                       1.0));

        Gfx::AttribSaver asaver(canvas);

        canvas.setLineWidth(itsEyebrowThickness);

        canvas.drawBezier4(eye_ctrlpnts[0], eye_ctrlpnts[1],
                           eye_ctrlpnts[2], eye_ctrlpnts[3],
                           eye_subdivisions);
      }

      // Draw pupil
      {
        DOTRACE("MorphyFace::grRender-draw pupil");

        Gfx::MatrixSaver msaver4(canvas);

        canvas.translate(Gfx::Vec3<double>(left_right*itsPupilXpos,
                                           itsPupilYpos, 0.0));

        double radius = 0.5 * itsPupilSize * itsEyeHeight;

        static const int num_slices = 20;
        static const int num_loops = 1;

        canvas.drawCircle(radius*Util::abs(itsPupilDilation), radius, true,
                          num_slices, num_loops);
      }
    }

  //
  // Draw face outline.
  //

  canvas.drawBezier4(Gfx::Vec3<double>(itsFaceWidth, 0.0, 0.0),
                     Gfx::Vec3<double>(itsBottomWidth*itsFaceWidth,
                                       itsBottomHeight*4.0/3.0, 0.0),
                     Gfx::Vec3<double>(-itsBottomWidth*itsFaceWidth,
                                       itsBottomHeight*4.0/3.0, 0.0),
                     Gfx::Vec3<double>(-itsFaceWidth, 0.0, 0.0),
                     30);

  //
  // Draw nose.
  //

  {
    DOTRACE("MorphyFace::grRender-draw nose");

    Gfx::MatrixSaver msaver5(canvas);

    canvas.translate(Gfx::Vec3<double>(itsNoseXpos, itsNoseYpos, 0.0));
    canvas.scale(Gfx::Vec3<double>(Util::abs(itsNoseWidth)/2.0,
                                   Util::abs(itsNoseLength), 1.0));

    Gfx::LineStripBlock block(canvas);

    canvas.vertex2(Gfx::Vec2<double>(-0.75, 0.5));
    canvas.vertex2(Gfx::Vec2<double>(-1.0,  0.0));
    canvas.vertex2(Gfx::Vec2<double>(-0.75, -0.333333));
    canvas.vertex2(Gfx::Vec2<double>(-0.25, -0.333333));
    canvas.vertex2(Gfx::Vec2<double>( 0.0, -0.5));   // CENTER
    canvas.vertex2(Gfx::Vec2<double>( 0.25, -0.333333));
    canvas.vertex2(Gfx::Vec2<double>( 0.75, -0.333333));
    canvas.vertex2(Gfx::Vec2<double>( 1.0,  0.0));
    canvas.vertex2(Gfx::Vec2<double>( 0.75, 0.5));
  }

  //
  // Draw mouth.
  //

  {
    DOTRACE("MorphyFace::grRender-draw mouth");

    Gfx::MatrixSaver msaver6(canvas);

    canvas.translate(Gfx::Vec3<double>(itsMouthXpos, itsMouthYpos, 0.0));
    canvas.scale(Gfx::Vec3<double>(itsMouthWidth, itsMouthCurvature, 1.0));

    canvas.drawBezier4(Gfx::Vec3<double>(-0.5,  0.5,      0.0),
                       Gfx::Vec3<double>(-0.2, -0.833333, 0.0),
                       Gfx::Vec3<double>( 0.2, -0.833333, 0.0),
                       Gfx::Vec3<double>( 0.5,  0.5,      0.0),
                       30);
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

Gfx::Rect<double> MorphyFace::grGetBoundingBox(Gfx::Canvas&) const
{
DOTRACE("MorphyFace::grGetBoundingBox");

  Gfx::Rect<double> bbox;

  Bezier4 xbezier_top(-1.0, -itsTopWidth, itsTopWidth, 1.0);
  Bezier4 xbezier_bottom(1.0, itsBottomWidth, -itsBottomWidth, -1.0);

  double top_width = xbezier_top.evalMax();
  double bottom_width = xbezier_bottom.evalMax();

  dbgEval(3, top_width);   dbgEvalNL(3, bottom_width);

  double max_width = Util::max(1.0, Util::max(top_width, bottom_width));

  dbgEvalNL(3, max_width);

  bbox.left()   = -max_width      * itsFaceWidth * (1 + itsHairWidth);
  bbox.right()  =  max_width      * itsFaceWidth * (1 + itsHairWidth);
  bbox.top()    =  itsTopHeight * (1 + itsHairWidth);
  bbox.bottom() =  itsBottomHeight;

  return bbox;
}

static const char vcid_morphyface_cc[] = "$Header$";
#endif // !MORPHYFACE_CC_DEFINED
