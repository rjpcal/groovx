//////////////////////////////////////////////////////////////////////
//
// fish.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Sep 29 11:44:57 1999
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

#ifndef FISH_CC_DEFINED
#define FISH_CC_DEFINED

#include "fish.h"

#include "geom/rect.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/canvas.h"
#include "gfx/rgbacolor.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "util/arrays.h"
#include "util/error.h"
#include "util/fstring.h"

#include <fstream>

#define DYNAMIC_TRACE_EXPR Fish::tracer.status()
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using rutz::fstring;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int dummy_int=0; // We need a dummy int to attach various CPtrField's

  const IO::VersionId FISH_SERIAL_VERSION_ID = 4;

  using Gfx::Vec2d;
  using Gfx::Vec3f;
  using Gfx::Vec3d;

  const int DF_0 = 0;
  const int TF_1 = 1;
  const int LF_2 = 2;
  const int MA_3 = 3;
}

rutz::tracer Fish::tracer;

///////////////////////////////////////////////////////////////////////
//
// Fish::Part struct --
//
// Describes one of the part of the fish (upper fin, tail, bottom fin
// or mouth) as a NURBS curves, with a knot sequence and a set of
// control points.
//
///////////////////////////////////////////////////////////////////////

struct Fish::Part
{
  Part() :
    itsKnots(), itsCtrlPnts(),
    itsBkpt(1), itsPt0(), itsPt1(),
    itsCoord(0.0)
  {}

  rutz::dynamic_block<float> itsKnots;

  rutz::dynamic_block<Vec3f> itsCtrlPnts;

  // -- end points --

  // Each endpoint is associated with a breakpoint. This is described
  // in this structure as well as the x and y coordinates of the two
  // points which define the boundaries of the endpoint line
  int itsBkpt;
  Vec3f itsPt0;
  Vec3f itsPt1;

  double itsCoord;

  template <std::size_t N1, std::size_t N2>
  void reset(float const (&knots)[N1], Vec3f const (&points)[N2])
  {
    itsKnots.assign(rutz::array_begin(knots),
                    rutz::array_end(knots));
    itsCtrlPnts.assign(rutz::array_begin(points),
                       rutz::array_end(points));
    itsCoord = 0.0;
  }
};


///////////////////////////////////////////////////////////////////////
//
// Fish member definitions
//
///////////////////////////////////////////////////////////////////////

const FieldMap& Fish::classFields()
{
  static const Field FIELD_ARRAY[] =
  {
    Field("category", &Fish::itsFishCategory, 0, 0, 10, 1, Field::NEW_GROUP),
    Field("dorsalFinCoord", &Fish::itsDorsalFinCoord, 0.0, -2.0, 2.0, 0.1),
    Field("tailFinCoord", &Fish::itsTailFinCoord, 0.0, -2.0, 2.0, 0.1),
    Field("lowerFinCoord", &Fish::itsLowerFinCoord, 0.0, -2.0, 2.0, 0.1),
    Field("mouthCoord", &Fish::itsMouthCoord, 0.0, -2.0, 2.0, 0.1),

    Field("currentPart", &Fish::itsCurrentPart, 0, 0, 3, 1,
          Field::NEW_GROUP | Field::CHECKED | Field::TRANSIENT),

    Field("currentPartBkpt", &Fish::itsCurrentPartBkpt, 1, 1, 10, 1,
          Field::TRANSIENT),

    Field("inColor", &Fish::inColor,
          false, false, true, true, Field::BOOLEAN | Field::TRANSIENT),
    Field("showControlPoints", &Fish::showControlPoints,
          false, false, true, true, Field::BOOLEAN | Field::TRANSIENT),
    Field("partsMask", &Fish::partsMask,
          0, 0, 15, 1, Field::TRANSIENT),
    Field("swimStroke", &Fish::swimStroke,
          0.0, -1.0, 1.0, 0.1, Field::TRANSIENT)
  };

  static FieldMap FISH_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return FISH_FIELDS;
}

Fish* Fish::make()
{
DOTRACE("Fish::make");
  return new Fish;
}

Fish* Fish::makeFromFiles(const char* splinefile,
                          const char* coordfile, int index)
{
DOTRACE("Fish::makeFromFiles");
  return new Fish(splinefile, coordfile, index);
}

Fish::Fish(const char* splinefile, const char* coordfile, int index) :
  itsParts(new Fish::Part[4]),
  itsFishCategory(-1),
  itsDorsalFinCoord(&itsParts[DF_0].itsCoord),
  itsTailFinCoord(&itsParts[TF_1].itsCoord),
  itsLowerFinCoord(&itsParts[LF_2].itsCoord),
  itsMouthCoord(&itsParts[MA_3].itsCoord),
  itsCurrentPart(0),
  itsCurrentPartBkpt(&dummy_int),
  inColor(false),
  showControlPoints(false),
  partsMask(0),
  swimStroke(0.0)
{
DOTRACE("Fish::Fish");

  setFieldMap(Fish::classFields());

  if (splinefile != 0 && coordfile != 0)
    {
      readSplineFile(splinefile);
      readCoordFile(coordfile, index);
    }
  else
    {
      restoreToDefault();
    }

  Nub::FloatingRef<Fish> ref(this);

  this->sigNodeChanged.connect(this, &Fish::updatePtrs);

  this->sigNodeChanged.emit();
}

Fish::~Fish () throw()
{
DOTRACE("Fish::~Fish");
  delete [] itsParts;
}

void Fish::restoreToDefault()
{
DOTRACE("Fish::restoreToDefault");

  static const float knots[] =
  {
    0.0, 0.0, 0.0, 0.0,
    0.1667, 0.3333, 0.5000, 0.6667, 0.8333,
    1.0, 1.0, 1.0, 1.0
  };

  static const Vec3f DF_coefs[] =
  {
    Vec3f(-0.2856,  0.2915,  0.2856),
    Vec3f(-0.2140,  0.2866,  0.2140),
    Vec3f(-0.2176,  0.2863,  0.2176),
    Vec3f(-0.0735,  0.4670,  0.0735),
    Vec3f( 0.0168,  0.3913,  0.0168),
    Vec3f( 0.1101,  0.3071,  0.1101),
    Vec3f( 0.3049,  0.1048,  0.3049),
    Vec3f( 0.0953,  0.1800,  0.0953),
    Vec3f( 0.1597,  0.1538,  0.1597),
  };

  static const Vec3f TF_coefs[] =
  {
    Vec3f( 0.1597,  0.1538,  0.1597),
    Vec3f( 0.2992,  0.1016,  0.2992),
    Vec3f( 0.2864,  0.1044,  0.2864),
    Vec3f( 0.7837,  0.1590,  0.7837),
    Vec3f( 0.4247,  0.0155,  0.4247),
    Vec3f( 0.6362, -0.3203,  0.6362),
    Vec3f( 0.3379, -0.0706,  0.3379),
    Vec3f( 0.3137,  0.0049,  0.3137),
    Vec3f( 0.1573, -0.0401,  0.1573),
  };

  static const Vec3f LF_coefs[] =
  {
    Vec3f( 0.1573, -0.0401,  0.1573),
    Vec3f( 0.2494, -0.0294,  0.2494),
    Vec3f( 0.1822, -0.0877,  0.1822),
    Vec3f(-0.0208, -0.3236,  0.0208),
    Vec3f(-0.0632, -0.3412,  0.0632),
    Vec3f(-0.1749, -0.1120,  0.1749),
    Vec3f(-0.1260, -0.4172,  0.1260),
    Vec3f(-0.3242, -0.1818,  0.3242),
    Vec3f(-0.2844, -0.1840,  0.2844),
  };

  static const Vec3f MA_coefs[] =
  {
    Vec3f(-0.2844, -0.1840,  0.2844),
    Vec3f(-0.3492, -0.1834,  0.3492),
    Vec3f(-0.4554, -0.1489,  0.4554),
    Vec3f(-0.6135, -0.0410,  0.6135),
    Vec3f(-0.7018,  0.0346,  0.7018),
    Vec3f(-0.5693,  0.1147,  0.5693),
    Vec3f(-0.4507,  0.2227,  0.4507),
    Vec3f(-0.3393,  0.2737,  0.3393),
    Vec3f(-0.2856,  0.2915,  0.2856),
  };

  itsParts[DF_0].reset(knots, DF_coefs);
  itsParts[TF_1].reset(knots, TF_coefs);
  itsParts[LF_2].reset(knots, LF_coefs);
  itsParts[MA_3].reset(knots, MA_coefs);

  itsParts[DF_0].itsBkpt = 6;
  itsParts[DF_0].itsPt0.set(0.2380, 0.3416, 0.2380);
  itsParts[DF_0].itsPt1.set(-0.0236, 0.2711, 0.0236);

  itsParts[TF_1].itsBkpt = 5;
  itsParts[TF_1].itsPt0.set(0.6514, -0.0305, 0.6514);
  itsParts[TF_1].itsPt1.set(0.2433, 0.0523, 0.2433);

  itsParts[LF_2].itsBkpt = 6;
  itsParts[LF_2].itsPt0.set(-0.2121, 0.0083, 0.2121);
  itsParts[LF_2].itsPt1.set(-0.1192, -0.2925, 0.1192);

  itsParts[MA_3].itsBkpt = 5;
  itsParts[MA_3].itsPt0.set(-0.7015, 0.1584, 0.7015);
  itsParts[MA_3].itsPt1.set(-0.7022, -0.1054, 0.7022);
}

IO::VersionId Fish::serialVersionId() const
{
DOTRACE("Fish::serialVersionId");
  return FISH_SERIAL_VERSION_ID;
}

void Fish::readFrom(IO::Reader& reader)
{
DOTRACE("Fish::readFrom");

  reader.ensureReadVersionId("Fish", 4,
                             "Try cvs tag xml_conversion_20040526",
                             SRC_POS);

  readFieldsFrom(reader, classFields());

  reader.readBaseClass("GxShapeKit", IO::makeProxy<GxShapeKit>(this));
}

void Fish::writeTo(IO::Writer& writer) const
{
DOTRACE("Fish::writeTo");

  writer.ensureWriteVersionId("Fish", FISH_SERIAL_VERSION_ID, 4,
                              "Try groovx0.8a7", SRC_POS);

  writeFieldsTo(writer, classFields());

  writer.writeBaseClass("GxShapeKit", IO::makeConstProxy<GxShapeKit>(this));
}

void Fish::updatePtrs()
{
DOTRACE("Fish::updatePtrs");

  itsCurrentPartBkpt = &(itsParts[itsCurrentPart].itsBkpt);
}

void Fish::readSplineFile(const char* splinefile)
{
DOTRACE("Fish::readSplineFile");
  size_t i, j;
  int k, splnb;
  fstring dummy;

  // read in the spline knots and coefficient
  STD_IO::ifstream ifs(splinefile);
  if (ifs.fail())
    {
      throw rutz::error(fstring("error opening file '",
                                splinefile, "'"), SRC_POS);
    }

  for(i = 0; i < 4; ++i)
    {
      int dummy_order;

      // spline number and order
      ifs >> dummy >> splnb >> dummy >> dummy_order;

      // number of knots
      int nknots;
      ifs >> dummy >> nknots;

      // allocate space and read in the successive knots
      itsParts[i].itsKnots.resize(nknots);
      for (j = 0; j < itsParts[i].itsKnots.size(); ++j)
        {
          ifs >> itsParts[i].itsKnots[j];
        }

      // number of coefficients
      int ncoefs;
      ifs >> dummy >> ncoefs;

      // allocate space and read in the successive coefficients
      itsParts[i].itsCtrlPnts.resize(ncoefs);
      {
        for (k = 0; k < ncoefs; ++k)
          {
            ifs >> itsParts[i].itsCtrlPnts[k].x();
          }
      }
      {
        for (k = 0; k < ncoefs; ++k)
          {
            ifs >> itsParts[i].itsCtrlPnts[k].y();
          }
      }
      {
        for (k = 0; k < ncoefs; ++k)
          {
            itsParts[i].itsCtrlPnts[k].z() = 0.0;
          }
      }

      if (ifs.fail())
        {
          throw rutz::error(fstring("error reading file '",
                                    splinefile, "'"), SRC_POS);
        }
    }


  for(i = 0; i < 4; ++i)
    {
      int which = 0;

      int endptnb = 0;

      // endpt number, associated part and breakpoint
      ifs >> dummy >> endptnb
          >> dummy >> which
          >> dummy >> itsParts[(which-1)].itsBkpt;

      if (which <= 0)
        {
          throw rutz::error(fstring("bad 'which' value '", which,
                                    "' while reading fish coords"),
                            SRC_POS);
        }

      // skip the next line
      ifs >> dummy >> dummy;

      // x&y coordinates
      ifs >> itsParts[(which-1)].itsPt0.x() >> itsParts[(which-1)].itsPt1.x()
          >> itsParts[(which-1)].itsPt0.y() >> itsParts[(which-1)].itsPt1.y();

      if (ifs.fail())
        {
          throw rutz::error(fstring("error reading file '",
                                    splinefile, "'"), SRC_POS);
        }
    }
}

void Fish::readCoordFile(const char* coordfile, int index)
{
DOTRACE("Fish::readCoordFile");
  fstring dummy;

  STD_IO::ifstream ifs(coordfile);
  if (ifs.fail())
    {
      throw rutz::error(fstring("error opening file '",
                                coordfile, "'"), SRC_POS);
    }

  // Skip (index-1) lines
  for (int j = 0; j < index-1; ++j)
    {
      getline(ifs, dummy, '\n');
    }

  ifs >> itsFishCategory;

  for (int i = 0; i < 4; ++i)
    {
      ifs >> itsParts[i].itsCoord;
    }

  if (ifs.fail())
    {
      throw rutz::error(fstring("error reading file '",
                                coordfile, "'"), SRC_POS);
    }
}

void Fish::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("Fish::grGetBoundingBox");

  bbox.vertex2(Vec2d(-0.75, -0.5));
  bbox.vertex2(Vec2d(+0.75, +0.5));
}

void Fish::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Fish::grRender");

  static Gfx::RgbaColor colors[4] =
  {
    Gfx::RgbaColor(1.0, 0.0, 0.0, 1.0),
    Gfx::RgbaColor(0.0, 1.0, 0.0, 1.0),
    Gfx::RgbaColor(0.0, 0.0, 1.0, 1.0),
    Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0)
  };

  // Loop over fish parts
  for (int i = 0; i < 4; ++i)
    {
      if ((1 << i) & partsMask)
        continue;

      if (inColor)
        canvas.setColor(colors[i]);

      rutz::dynamic_block<Vec3f> ctrlpnts(itsParts[i].itsCtrlPnts);

      // Set the coefficients at the break point to a linear sum of
      // the two corresponding endpoints

      float alpha = 0.5*(1-itsParts[i].itsCoord);
      float beta  = 0.5*(1+itsParts[i].itsCoord);

      Vec3f pt = itsParts[i].itsPt0 * alpha + itsParts[i].itsPt1 * beta;

      // indexing in the coefs array starts at 0
      unsigned int bkpt = itsParts[i].itsBkpt - 1;

      if (bkpt < ctrlpnts.size())
        ctrlpnts[bkpt] = pt;

      for (unsigned int j = 0; j < ctrlpnts.size(); ++j)
        {
          ctrlpnts[j].z() =
            ctrlpnts[j].z() * ctrlpnts[j].z() * swimStroke;
        }

      canvas.drawNurbsCurve(itsParts[i].itsKnots, ctrlpnts);

      if (showControlPoints)
        {
          if (0)
            {
              // This version (using GL_POINTS) doesn't seem to work
              // properly with Mesa under Linux; the problem is that
              // setPointSize() (i.e. glPointSize()) doesn't actually
              // change the size of the points that get drawn... they all
              // end up at 1 pixel... instead we can draw the points as
              // filled rects.
              Gfx::AttribSaver saver(canvas);
              canvas.setPointSize(4.0);

              Gfx::PointsBlock block(canvas);
              for (unsigned int i = 0; i < ctrlpnts.size(); ++i)
                {
                  canvas.vertex3(ctrlpnts[i]);
                }
            }
          else
            {
              Gfx::AttribSaver saver(canvas);
              canvas.setPolygonFill(true);
              Gfx::Rect<double> rect;
              for (unsigned int i = 0; i < ctrlpnts.size(); ++i)
                {
                  Gfx::MatrixSaver msaver(canvas);
                  canvas.translate(Vec3d(0.0, 0.0, ctrlpnts[i].z()));
                  rect.setXYWH(ctrlpnts[i].x(), ctrlpnts[i].y(), 0.03, 0.03);
                  canvas.drawRect(rect);
                }
            }
          {
            Gfx::LinesBlock block(canvas);
            canvas.vertex2(Vec2d(itsParts[i].itsPt0.vec2()));
            canvas.vertex2(Vec2d(itsParts[i].itsPt1.vec2()));
          }
        }
    }
}


static const char vcid_fish_cc[] = "$Header$";
#endif // !FISH_CC_DEFINED
