///////////////////////////////////////////////////////////////////////
//
// face.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Dec  1 08:00:00 1998
// written: Sat Feb  9 12:16:03 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FACE_CC_DEFINED
#define FACE_CC_DEFINED

#include "visx/face.h"

#include "gfx/canvas.h"

#include "gx/rect.h"
#include "gx/vec3.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"
#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId FACE_SERIAL_VERSION_ID = 1;

  const double theirNose_x = 0.0;
  const double theirMouth_x[2] = {-0.2, 0.2};

  const int OUTLINE_PART_MASK   = 1 << 0;
  const int EYES_PART_MASK      = 1 << 1;
  const int NOSE_PART_MASK      = 1 << 2;
  const int MOUTH_PART_MASK     = 1 << 3;
}

///////////////////////////////////////////////////////////////////////
//
// Face member functions
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Creators
///////////////////////////////////////////////////////////////////////

const FieldMap& Face::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("category", &Face::itsFaceCategory, 0, 0, 10, 1, Field::NEW_GROUP),
    Field("eyeHeight", &Face::itsEyeHeight, 0.6, -1.2, 1.2, 0.01),
    Field("eyeDistance", &Face::itsEyeDistance, 0.4, 0.0, 1.8, 0.01),
    Field("noseLength", &Face::itsNoseLength, 0.4, -0.0, 3.0, 0.01),
    Field("mouthHeight", &Face::itsMouthHeight, 0.8, -1.2, 1.2, 0.01),
    Field("partsMask", &Face::itsPartsMask, 0, 0, 15, 1, Field::TRANSIENT)
  };

  static FieldMap FACE_FIELDS(FIELD_ARRAY, &GrObj::classFields());

  return FACE_FIELDS;
}

Face* Face::make()
{
DOTRACE("Face::make");
  return new Face;
}

Face::Face(double eh, double es, double nl, double mh, int categ) :
  itsFaceCategory(categ),
  itsEyeHeight(eh),
  itsEyeDistance(es),
  itsNoseLength(nl),
  itsMouthHeight(mh),
  itsPartsMask(0)
{
DOTRACE("Face::Face");

  setFieldMap(Face::classFields());
}

Face::~Face()
{
DOTRACE("Face::~Face");
  // nothing to do
}

IO::VersionId Face::serialVersionId() const
{
DOTRACE("Face::serialVersionId");
  return FACE_SERIAL_VERSION_ID;
}

void Face::readFrom(IO::Reader* reader)
{
DOTRACE("Face::readFrom");

  reader->ensureReadVersionId("Face", 1, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void Face::writeTo(IO::Writer* writer) const
{
DOTRACE("Face::writeTo");

  writer->ensureWriteVersionId("Face", FACE_SERIAL_VERSION_ID, 1,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

///////////////////////////////////////////////////////////////////////
// Actions
///////////////////////////////////////////////////////////////////////

void Face::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Face::grRender");

  //
  // Drawing commands begin here...
  //

  Gfx::AttribSaver attribSaver(canvas);

  canvas.enableAntialiasing();

  //
  // Draw face outline.
  //

  if ( !(itsPartsMask & OUTLINE_PART_MASK) )
    {
      // These parameters control the generation of the Bezier curve for
      // the face outline.
      const int num_subdivisions = 30;
      const int nctrlsets = 2;
      const double* const ctrlpnts = getCtrlPnts();

      for (int i = 0; i < nctrlsets; ++i)
        {
          canvas.drawBezier4(Gfx::Vec3<double>(ctrlpnts+i*12+0),
                             Gfx::Vec3<double>(ctrlpnts+i*12+3),
                             Gfx::Vec3<double>(ctrlpnts+i*12+6),
                             Gfx::Vec3<double>(ctrlpnts+i*12+9),
                             num_subdivisions);
        }
    }

  {
    Gfx::MatrixSaver saver(canvas);

    canvas.translate(Gfx::Vec3<double>(0.0, getVertOffset(), 0.0));

    //
    // Set up for drawing eyes.
    //

    // Generate the eyeball scales on the basis of the eye aspect. The
    // eye aspect should range from 0.0 to 1.0 to control the eyeball
    // x and y scales from 0.1 to 0.185. The x and y scales are always
    // at opposite points within this range.
    const double aspect = getEyeAspect();

    const Gfx::Vec3<double> eyeball_scale(0.1*aspect     + 0.185*(1-aspect),
                                          0.1*(1-aspect) + 0.185*aspect,
                                          1.0);

    // The absolute scale of the pupil.
    const Gfx::Vec3<double> pupil_scale_abs(0.07, 0.07, 1.0);

    // The scale of the pupil relative to the eyeball scale. These
    // values are computed since it is more efficient in the drawing
    // routine to do an additional scale for the pupils after having
    // drawn the eyeballs, rather than to pop the modelview matrix
    // after the eyeballs and push a new matrix for the pupils. But,
    // maybe this is the sort of optimization that OpenGL would make
    // on its own in a display list.
    const Gfx::Vec3<double> pupil_scale(pupil_scale_abs/eyeball_scale);

    // Calculate the x position for the eyes
    const double eye_x = Util::abs(itsEyeDistance)/2.0;

    // Parameters for the circles for the eyeballs and the pupils.
    const int num_slices = 20;
    const int num_loops = 1;
    const double outer_radius = 0.5;

    // Draw eyes.
    if ( !(itsPartsMask & EYES_PART_MASK) )
      {
        for (int eye_pos = -1; eye_pos < 2; eye_pos += 2)
          {
            Gfx::MatrixSaver eyesaver(canvas);

            canvas.translate(Gfx::Vec3<double>(eye_pos * eye_x,
                                               itsEyeHeight,
                                               0.0));
            canvas.scale(eyeball_scale);

            canvas.drawCircle(0.0, outer_radius, false, num_slices, num_loops);

            canvas.scale(pupil_scale);

            canvas.drawCircle(0.0, outer_radius, false, num_slices, num_loops);
          }
      }

    //
    // Draw nose and mouth.
    //

    // Calculate the positions for the top and bottom of the nose
    // bottom y always <= 0.0
    // top y always >= 0.0
    const Gfx::Vec2<double> nose_bottom(theirNose_x,
                                        -Util::abs(itsNoseLength)/2.0);
    const Gfx::Vec2<double> nose_top(theirNose_x,
                                     -nose_bottom.y());

    {
      Gfx::LinesBlock block(canvas);

      if ( !(itsPartsMask & MOUTH_PART_MASK) )
        {
          canvas.vertex2(Gfx::Vec2<double>(theirMouth_x[0], itsMouthHeight));
          canvas.vertex2(Gfx::Vec2<double>(theirMouth_x[1], itsMouthHeight));
        }

      if ( !(itsPartsMask & NOSE_PART_MASK) )
        {
          canvas.vertex2(nose_bottom);
          canvas.vertex2(nose_top);
        }
    }
  }
}

///////////////////////////////////////////////////////////////////////
// Accessors
///////////////////////////////////////////////////////////////////////

Gfx::Rect<double> Face::grGetBoundingBox(Gfx::Canvas&) const
{
DOTRACE("Face::grGetBoundingBox");

  Gfx::Rect<double> bounding_box;
  bounding_box.left() = -0.7;
  bounding_box.right() = 0.7;
  bounding_box.bottom() = 0.2 + 0.75*(-1.7-0.2);
  bounding_box.top() = 0.2 + 0.75*(1.4-0.2);

  return bounding_box;
}

int Face::category() const
{
DOTRACE("Face::category");
  return itsFaceCategory;
}

void Face::setCategory(int val)
{
DOTRACE("Face::setCategory");
  itsFaceCategory = val;
}

const double* Face::getCtrlPnts() const
{
DOTRACE("Face::getCtrlPnts");
  static const double ctrlpnts[] =
  {
    -.7, 0.2, 0.0, // first 4 control points
    -.7, 1.4, 0.0,
    .7, 1.4, 0.0,
    .7, 0.2, 0.0,
    .7, 0.2, 0.0, // second 4 control points
    .6, -1.7, 0.0,
    -.6, -1.7, 0.0,
    -.7, 0.2, 0.0
  };
  return ctrlpnts;
}

double Face::getEyeAspect() const
{
DOTRACE("Face::getEyeAspect");
  return 0.0;
}

double Face::getVertOffset() const
{
DOTRACE("Face::getVertOffset");
  return 0.0;
}

static const char vcid_face_cc[] = "$Header$";
#endif // !FACE_CC_DEFINED
