///////////////////////////////////////////////////////////////////////
//
// face.cc
//
// Copyright (c) 1998-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Dec  1 08:00:00 1998
// commit: $Id$
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

#ifndef FACE_CC_DEFINED
#define FACE_CC_DEFINED

#include "visx/face.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/vec3.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/algo.h"
#include "util/strings.h"

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using namespace Gfx;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const IO::VersionId FACE_SERIAL_VERSION_ID = 2;

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
    Field("partsMask", &Face::itsPartsMask, 0, 0, 15, 1, Field::TRANSIENT),
    Field("isFilled", &Face::isItFilled, false, false, true, true,
          Field::TRANSIENT | Field::BOOLEAN)
  };

  static FieldMap FACE_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

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
  itsPartsMask(0),
  isItFilled(false)
{
DOTRACE("Face::Face");

  setFieldMap(Face::classFields());
}

Face::~Face() throw()
{
DOTRACE("Face::~Face");
  // nothing to do
}

IO::VersionId Face::serialVersionId() const
{
DOTRACE("Face::serialVersionId");
  return FACE_SERIAL_VERSION_ID;
}

void Face::readFrom(IO::Reader& reader)
{
DOTRACE("Face::readFrom");

  reader.ensureReadVersionId("Face", 2,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  readFieldsFrom(reader, classFields());

  reader.readBaseClass("GxShapeKit", IO::makeProxy<GxShapeKit>(this));
}

void Face::writeTo(IO::Writer& writer) const
{
DOTRACE("Face::writeTo");

  writer.ensureWriteVersionId("Face", FACE_SERIAL_VERSION_ID, 2,
                              "Try groovx0.8a4", SRC_POS);

  writeFieldsTo(writer, classFields());

  writer.writeBaseClass("GxShapeKit", IO::makeConstProxy<GxShapeKit>(this));
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
          if (isItFilled)
            {
              canvas.drawBezierFill4(Vec3d(0.0, 0.0, 0.0),
                                     Vec3d(ctrlpnts+i*12+0),
                                     Vec3d(ctrlpnts+i*12+3),
                                     Vec3d(ctrlpnts+i*12+6),
                                     Vec3d(ctrlpnts+i*12+9),
                                     num_subdivisions);
            }
          else
            {
              canvas.drawBezier4(Vec3d(ctrlpnts+i*12+0),
                                 Vec3d(ctrlpnts+i*12+3),
                                 Vec3d(ctrlpnts+i*12+6),
                                 Vec3d(ctrlpnts+i*12+9),
                                 num_subdivisions);
            }
        }
    }

  {
    Gfx::MatrixSaver msaver(canvas);
    Gfx::AttribSaver asaver(canvas);

    if (isItFilled)
      canvas.swapForeBack();

    canvas.translate(Vec3d(0.0, getVertOffset(), 0.0));

    //
    // Set up for drawing eyes.
    //

    // Generate the eyeball scales on the basis of the eye aspect. The
    // eye aspect should range from 0.0 to 1.0 to control the eyeball
    // x and y scales from 0.1 to 0.185. The x and y scales are always
    // at opposite points within this range.
    const double aspect = getEyeAspect();

    const Vec3d eyeball_scale(0.1*aspect     + 0.185*(1-aspect),
                              0.1*(1-aspect) + 0.185*aspect,
                              1.0);

    // The absolute scale of the pupil.
    const Vec3d pupil_scale_abs(0.07, 0.07, 1.0);

    // The scale of the pupil relative to the eyeball scale. These
    // values are computed since it is more efficient in the drawing
    // routine to do an additional scale for the pupils after having
    // drawn the eyeballs, rather than to pop the modelview matrix
    // after the eyeballs and push a new matrix for the pupils. But,
    // maybe this is the sort of optimization that OpenGL would make
    // on its own in a display list.
    const Vec3d pupil_scale(pupil_scale_abs/eyeball_scale);

    // Calculate the x position for the eyes
    const double eye_x = rutz::abs(itsEyeDistance)/2.0;

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

            canvas.translate(Vec3d(eye_pos * eye_x, itsEyeHeight, 0.0));
            canvas.scale(eyeball_scale);

            canvas.drawCircle(0.0, outer_radius, isItFilled,
                              num_slices, num_loops);

            canvas.scale(pupil_scale);

            {
              Gfx::AttribSaver asaver(canvas);
              if (isItFilled)
                canvas.swapForeBack();

              canvas.drawCircle(0.0, outer_radius, isItFilled,
                                num_slices, num_loops);
            }
          }
      }

    //
    // Draw nose and mouth.
    //

    // Calculate the positions for the top and bottom of the nose
    // bottom y always <= 0.0
    // top y always >= 0.0
    const Vec2d nose_bottom(theirNose_x, -rutz::abs(itsNoseLength)/2.0);
    const Vec2d nose_top(theirNose_x, -nose_bottom.y());

    {
      Gfx::LinesBlock block(canvas);

      if ( !(itsPartsMask & MOUTH_PART_MASK) )
        {
          canvas.vertex2(Vec2d(theirMouth_x[0], itsMouthHeight));
          canvas.vertex2(Vec2d(theirMouth_x[1], itsMouthHeight));
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

void Face::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("Face::grGetBoundingBox");

  bbox.vertex2(Vec2d(-0.7, 0.2 + 0.75*(-1.7-0.2)));
  bbox.vertex2(Vec2d(+0.7, 0.2 + 0.75*(+1.4-0.2)));
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
