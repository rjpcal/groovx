///////////////////////////////////////////////////////////////////////
//
// morphyface.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep  8 15:38:42 1999
// written: Thu Aug 16 11:03:24 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MORPHYFACE_CC_DEFINED
#define MORPHYFACE_CC_DEFINED

#ifdef ACC_COMPILER
#pragma OPT_LEVEL 1
#endif

#include "morphyface.h"

#include "bezier.h"

#include "gfx/canvas.h"
#include "gfx/rect.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"

#include <cstring>
#include <GL/gl.h>
#include <GL/glu.h>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
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
    Bezier4 ybezier( 0.0,     4.0/3.0   ,   4.0/3.0  , 0.0);

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

  void drawHairStrip(const double* vertices, double x_scale, double y_scale)
  {
    glPushMatrix();
    {
      glScalef(x_scale, y_scale, 1.0);

      glBegin(GL_QUAD_STRIP);
      {
        for (unsigned int i = 0; i < NUM_HAIR_POINTS; ++i)
          {
            glVertex2d(vertices[4*i  ], vertices[4*i+1]);
            glVertex2d(vertices[4*i+2], vertices[4*i+3]);
          }
      }
      glEnd();

    }
    glPopMatrix();
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

  static const Field FINFOS[] =
  {
    Field("category", &MF::itsMfaceCategory, 0, 0, 10, 1, true),

    Field("faceWidth", &MF::itsFaceWidth, 2.75, 1.5, 3.5, 0.1),
    Field("topWidth", &MF::itsTopWidth, 1.15, 0.05, 2.0, 0.05),
    Field("bottomWidth", &MF::itsBottomWidth, 1.0, 0.05, 2.0, 0.05),
    Field("topHeight", &MF::itsTopHeight, 3.8, 0.5, 5.0, 0.25),
    Field("bottomHeight", &MF::itsBottomHeight, -3.0, -5.0, -0.5, 0.25),

    Field("hairWidth", &MF::itsHairWidth, 0.20, 0.00, 0.5, 0.02, true),
    Field("hairStyle", &MF::itsHairStyle, 0, 0, 1, 1),

    Field("eyeYpos", &MF::itsEyeYpos, 0.375, -2.0, 2.0, 0.1, true),
    Field("eyeDistance", &MF::itsEyeDistance, 2.25, 0.0, 5.0, 0.25),
    Field("eyeHeight", &MF::itsEyeHeight, 0.9, 0.0, 2.0, 0.1),
    Field("eyeAspectRatio", &MF::itsEyeAspectRatio, 1.555556, 0.1, 5.0, 0.1),

    Field("pupilXpos", &MF::itsPupilXpos, 0.0, -0.5, 0.5, 0.05, true),
    Field("pupilYpos", &MF::itsPupilYpos, 0.0, -0.5, 0.5, 0.05),
    Field("pupilSize", &MF::itsPupilSize, 0.6, 0.0, 1.0, 0.05),
    Field("pupilDilation", Field::BoundsCheck(),
          &MF::itsPupilDilation, 0.5, 0.05, 0.95, 0.05),

    Field("eyebrowXpos", &MF::itsEyebrowXpos, 0.0, -0.5, 0.5, 0.02, true),
    Field("eyebrowYpos", &MF::itsEyebrowYpos, 0.5, 0.0, 1.5, 0.05),
    Field("eyebrowCurvature", &MF::itsEyebrowCurvature, 0.8, -2.0, 2.0, 0.1),
    Field("eyebrowAngle", &MF::itsEyebrowAngle, -5, -50, 50, 1),
    Field("eyebrowThickness", &MF::itsEyebrowThickness, 2.0, 0.1, 4.0, 0.1),

    Field("noseXpos", &MF::itsNoseXpos, 0.0, -1.0, 1.0, 0.05, true),
    Field("noseYpos", &MF::itsNoseYpos, -0.825, -2.0, 2.0, 0.1),
    Field("noseLength", &MF::itsNoseLength, 0.75, 0.0, 2.0, 0.1),
    Field("noseWidth", &MF::itsNoseWidth, 1.5, 0.0, 3.0, 0.1),

    Field("mouthXpos", &MF::itsMouthXpos, 0.0, -2.0, 2.0, 0.1, true),
    Field("mouthYpos", &MF::itsMouthYpos, -2.0, -3.0, 1.0, 0.1),
    Field("mouthWidth", &MF::itsMouthWidth, 2.5, 0.0, 5.0, 0.25),
    Field("mouthCurvature", &MF::itsMouthCurvature, 0.6, -2.0, 2.0, 0.1)
  };

  const unsigned int NUM_FINFOS = sizeof(FINFOS)/sizeof(Field);

  static FieldMap MFACE_FIELDS(FINFOS, FINFOS+NUM_FINFOS,
                               &GrObj::classFields());

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

  // Enable antialiasing, if it is available
  const bool have_antialiasing = canvas.isRgba();

  if (have_antialiasing)
    {
      glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
      glEnable(GL_BLEND); // blend incoming RGBA values with old RGBA values

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use transparency

      glEnable(GL_LINE_SMOOTH);   // use anti-aliasing
    }

  // Prepare to work with modelview matrices.
  glMatrixMode(GL_MODELVIEW);


  //
  // Draw eyes
  //

  // Create a quadric obj to use for calling gluDisk(). This disk will
  // be used to render the eyeballs and the pupils.
  GLUquadricObj* qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_FILL);

  const double eye_ctrlpnts[] =
  {
    -3.0/7.0, 0.0,     0.0,
    -2.0/7.0, 2.0/3.0, 0.0,
     2.0/7.0, 2.0/3.0, 0.0,
     4.0/7.0, 0.0,     0.0,
  };
  const int eye_subdivisions = 10;

  for (int left_right = -1; left_right < 2; left_right += 2)
    {
      glPushMatrix();
      {
        // Do appropriate reflection
        glScalef(left_right*1.0, 1.0, 1.0);

        // Move to the eye position
        glTranslatef(Util::abs(itsEyeDistance)/2.0, itsEyeYpos, 0.0);

        // Draw eye outline
        for (int top_bottom = -1; top_bottom < 2; top_bottom += 2)
          {
            glPushMatrix();
            {
              glScalef(itsEyeHeight*itsEyeAspectRatio,
                       itsEyeHeight*top_bottom,
                       1.0);
              glEnable(GL_MAP1_VERTEX_3);
              glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, eye_ctrlpnts);
              // Evaluate the 1-d Bezier curve
              glMapGrid1d(eye_subdivisions, 0.0, 1.0);
              glEvalMesh1(GL_LINE, 0, eye_subdivisions);
              glPopMatrix();
            }
          }

        // Draw eyebrow
        glPushMatrix();
        {
          glTranslatef(itsEyebrowXpos, itsEyebrowYpos, 0.0);
          glRotatef(itsEyebrowAngle, 0.0, 0.0, 1.0);
          glScalef(itsEyeHeight*itsEyeAspectRatio,
                   itsEyeHeight*itsEyebrowCurvature,
                   1.0);
          glPushAttrib(GL_LINE_BIT);
          {
            GLdouble line_width;
            glGetDoublev(GL_LINE_WIDTH, &line_width);
            glLineWidth(itsEyebrowThickness*line_width);
            glEnable(GL_MAP1_VERTEX_3);
            glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, eye_ctrlpnts);
            // Evaluate the 1-d Bezier curve
            glMapGrid1d(eye_subdivisions, 0.0, 1.0);
            glEvalMesh1(GL_LINE, 0, eye_subdivisions);
          }
          glPopAttrib();
        }
        glPopMatrix();

        // Draw pupil
        glPushMatrix();
        {
          glTranslatef(left_right*itsPupilXpos, itsPupilYpos, 0.0);
          glScalef(itsPupilSize*itsEyeHeight,
                   itsPupilSize*itsEyeHeight,
                   1.0);
          static const int num_slices = 20;
          static const int num_loops = 1;
          gluDisk(qobj, 0.5*Util::abs(itsPupilDilation), 0.5,
                  num_slices, num_loops);
        }
        glPopMatrix();
      }
      glPopMatrix();
    }

  gluDeleteQuadric(qobj);

  //
  // Draw face outline.
  //

  // These parameters control the generation of the Bezier curve for
  // the face outline.
  static const int num_subdivisions = 30;
  static const int nctrlsets = 2;
  const double ctrlpnts[] =
  {
    -itsFaceWidth, 0.0,        0,        // first 4 control points
    -itsTopWidth*itsFaceWidth,  itsTopHeight*4.0/3.0, 0,
    itsTopWidth*itsFaceWidth,   itsTopHeight*4.0/3.0, 0,
    itsFaceWidth,  0.0,        0,
    itsFaceWidth, 0.0, 0, // second 4 control points
    itsBottomWidth*itsFaceWidth, itsBottomHeight*4.0/3.0, 0,
    -itsBottomWidth*itsFaceWidth, itsBottomHeight*4.0/3.0, 0,
    -itsFaceWidth, 0.0, 0
  };

  glEnable(GL_MAP1_VERTEX_3);
  for (int i = 1; i < nctrlsets; ++i)
    {
      glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpnts[i*12]);
      // Evaluate the 1-d Bezier curve
      glMapGrid1d(num_subdivisions, 0.0, 1.0);
      glEvalMesh1(GL_LINE, 0, num_subdivisions);
    }

  //
  // Draw nose.
  //

  glPushMatrix();
  {
    glTranslatef(itsNoseXpos, itsNoseYpos, 0.0);
    glScalef(Util::abs(itsNoseWidth)/2.0, Util::abs(itsNoseLength), 1.0);
    glBegin(GL_LINE_STRIP);
      glVertex2d(-0.75, 0.5);
      glVertex2d(-1.0,  0.0);
      glVertex2d(-0.75, -0.333333);
      glVertex2d(-0.25, -0.333333);
      glVertex2d( 0.0, -0.5);   // CENTER
      glVertex2d( 0.25, -0.333333);
      glVertex2d( 0.75, -0.333333);
      glVertex2d( 1.0,  0.0);
      glVertex2d( 0.75, 0.5);
    glEnd();
  }
  glPopMatrix();

  //
  // Draw mouth.
  //

  const double mouth_ctrlpnts[] =
  {
    -0.5,  0.5,      0.0,
    -0.2, -0.833333, 0.0,
     0.2, -0.833333, 0.0,
     0.5,  0.5,      0.0
  };

  glPushMatrix();
  {
    glTranslatef(itsMouthXpos, itsMouthYpos, 0.0);
    glScalef(itsMouthWidth, itsMouthCurvature, 1.0);
    glEnable(GL_MAP1_VERTEX_3);
    glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, mouth_ctrlpnts);
    // Evaluate the 1-d Bezier curve
    glMapGrid1d(num_subdivisions, 0.0, 1.0);
    glEvalMesh1(GL_LINE, 0, num_subdivisions);
  }
  glPopMatrix();

  //
  // Draw hair.
  //

  glPushAttrib(GL_POLYGON_BIT);
  {
    if (itsHairStyle == 0)
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }
    else
      {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }

    const double* hair_vertices = getHairVertices(itsTopWidth, itsHairWidth);

    // Draw right side
    drawHairStrip(hair_vertices, itsFaceWidth, itsTopHeight);

    // Draw left side
    drawHairStrip(hair_vertices, -itsFaceWidth, itsTopHeight);
  }
  glPopAttrib();

  // Undo pushes
  if (have_antialiasing)
    {
      glPopAttrib();
    }
}

///////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////

Gfx::Rect<double> MorphyFace::grGetBoundingBox() const
{
DOTRACE("MorphyFace::grGetBoundingBox");

  Gfx::Rect<double> bbox;

  Bezier4 xbezier_top(-1.0, -itsTopWidth, itsTopWidth, 1.0);
  Bezier4 xbezier_bottom(1.0, itsBottomWidth, -itsBottomWidth, -1.0);

  double top_width = xbezier_top.evalMax();
  double bottom_width = xbezier_bottom.evalMax();

  DebugEval(top_width);   DebugEvalNL(bottom_width);

  double max_width = max(1.0, max(top_width, bottom_width));

  DebugEvalNL(max_width);

  bbox.left()   = -max_width      * itsFaceWidth * (1 + itsHairWidth);
  bbox.right()  =  max_width      * itsFaceWidth * (1 + itsHairWidth);
  bbox.top()    =  itsTopHeight * (1 + itsHairWidth);
  bbox.bottom() =  itsBottomHeight;

  return bbox;
}


static const char vcid_morphyface_cc[] = "$Header$";
#endif // !MORPHYFACE_CC_DEFINED
