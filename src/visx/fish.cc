//////////////////////////////////////////////////////////////////////
//
// fish.cc
//
// Copyright (c) 1999-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Sep 29 11:44:57 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_CC_DEFINED
#define FISH_CC_DEFINED

#include "visx/fish.h"

#include "gfx/canvas.h"

#include "gx/bbox.h"
#include "gx/rgbacolor.h"
#include "gx/vec3.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/arrays.h"
#include "util/error.h"
#include "util/ref.h"
#include "util/strings.h"

#include <fstream>

#define DYNAMIC_TRACE_EXPR Fish::tracer.status()
#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int dummy_int=0; // We need a dummy int to attach various CPtrField's

  const IO::VersionId FISH_SERIAL_VERSION_ID = 4;

  typedef Gfx::Vec3<float> Pt3;

  const int DF_0 = 0;
  const int TF_1 = 1;
  const int LF_2 = 2;
  const int MA_3 = 3;
}

Util::Tracer Fish::tracer;

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

  dynamic_block<float> itsKnots;

  dynamic_block<Pt3> itsCtrlPnts;

  // -- end points --

  // Each endpoint is associated with a breakpoint. This is described
  // in this structure as well as the x and y coordinates of the two
  // points which define the boundaries of the endpoint line
  int itsBkpt;
  Pt3 itsPt0;
  Pt3 itsPt1;

  double itsCoord;

  template <std::size_t N1, std::size_t N2>
  void reset(float const (&knots)[N1], Pt3 const (&points)[N2])
  {
    itsKnots.assign(array_begin(knots), array_end(knots));
    itsCtrlPnts.assign(array_begin(points), array_end(points));
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
          0, 0, 15, 1, Field::TRANSIENT)
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
  partsMask(0)
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

  Util::FloatingRef<Fish> ref(this);

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

  static const Pt3 DF_coefs[] =
  {
    Pt3(-0.2856,  0.2915, 0.0),
    Pt3(-0.2140,  0.2866, 0.0),
    Pt3(-0.2176,  0.2863, 0.0),
    Pt3(-0.0735,  0.4670, 0.0),
    Pt3( 0.0168,  0.3913, 0.0),
    Pt3( 0.1101,  0.3071, 0.0),
    Pt3( 0.3049,  0.1048, 0.0),
    Pt3( 0.0953,  0.1800, 0.0),
    Pt3( 0.1597,  0.1538, 0.0),
  };

  static const Pt3 TF_coefs[] =
  {
    Pt3( 0.1597,  0.1538, 0.0),
    Pt3( 0.2992,  0.1016, 0.0),
    Pt3( 0.2864,  0.1044, 0.0),
    Pt3( 0.7837,  0.1590, 0.0),
    Pt3( 0.4247,  0.0155, 0.0),
    Pt3( 0.6362, -0.3203, 0.0),
    Pt3( 0.3379, -0.0706, 0.0),
    Pt3( 0.3137,  0.0049, 0.0),
    Pt3( 0.1573, -0.0401, 0.0),
  };

  static const Pt3 LF_coefs[] =
  {
    Pt3( 0.1573, -0.0401, 0.0),
    Pt3( 0.2494, -0.0294, 0.0),
    Pt3( 0.1822, -0.0877, 0.0),
    Pt3(-0.0208, -0.3236, 0.0),
    Pt3(-0.0632, -0.3412, 0.0),
    Pt3(-0.1749, -0.1120, 0.0),
    Pt3(-0.1260, -0.4172, 0.0),
    Pt3(-0.3242, -0.1818, 0.0),
    Pt3(-0.2844, -0.1840, 0.0),
  };

  static const Pt3 MA_coefs[] =
  {
    Pt3(-0.2844, -0.1840, 0.0),
    Pt3(-0.3492, -0.1834, 0.0),
    Pt3(-0.4554, -0.1489, 0.0),
    Pt3(-0.6135, -0.0410, 0.0),
    Pt3(-0.7018,  0.0346, 0.0),
    Pt3(-0.5693,  0.1147, 0.0),
    Pt3(-0.4507,  0.2227, 0.0),
    Pt3(-0.3393,  0.2737, 0.0),
    Pt3(-0.2856,  0.2915, 0.0),
  };

  itsParts[DF_0].reset(knots, DF_coefs);
  itsParts[TF_1].reset(knots, TF_coefs);
  itsParts[LF_2].reset(knots, LF_coefs);
  itsParts[MA_3].reset(knots, MA_coefs);

  itsParts[DF_0].itsBkpt = 6;
  itsParts[DF_0].itsPt0.set(0.2380, 0.3416, 0.0);
  itsParts[DF_0].itsPt1.set(-0.0236, 0.2711, 0.0);

  itsParts[TF_1].itsBkpt = 5;
  itsParts[TF_1].itsPt0.set(0.6514, -0.0305, 0.0);
  itsParts[TF_1].itsPt1.set(0.2433, 0.0523, 0.0);

  itsParts[LF_2].itsBkpt = 6;
  itsParts[LF_2].itsPt0.set(-0.2121, 0.0083, 0.0);
  itsParts[LF_2].itsPt1.set(-0.1192, -0.2925, 0.0);

  itsParts[MA_3].itsBkpt = 5;
  itsParts[MA_3].itsPt0.set(-0.7015, 0.1584, 0.0);
  itsParts[MA_3].itsPt1.set(-0.7022, -0.1054, 0.0);
}

IO::VersionId Fish::serialVersionId() const
{
DOTRACE("Fish::serialVersionId");
  return FISH_SERIAL_VERSION_ID;
}

void Fish::readFrom(IO::Reader& reader)
{
DOTRACE("Fish::readFrom");

  int svid = reader.ensureReadVersionId("Fish", 3, "Try groovx0.8a7");

  readFieldsFrom(reader, classFields());

  if (svid < 4)
    {
      reader.readBaseClass("GrObj", IO::makeProxy<GxShapeKit>(this));
    }
  else
    {
      reader.readBaseClass("GxShapeKit", IO::makeProxy<GxShapeKit>(this));
    }
}

void Fish::writeTo(IO::Writer& writer) const
{
DOTRACE("Fish::writeTo");

  writer.ensureWriteVersionId("Fish", FISH_SERIAL_VERSION_ID, 4,
                              "Try groovx0.8a7");

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
      throw Util::Error(fstring("error opening file '", splinefile, "'"));
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
          throw Util::Error(fstring("error reading file '", splinefile, "'"));
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
          throw Util::Error(fstring("bad 'which' value '", which,
                                    "' while reading fish coords"));
        }

      // skip the next line
      ifs >> dummy >> dummy;

      // x&y coordinates
      ifs >> itsParts[(which-1)].itsPt0.x() >> itsParts[(which-1)].itsPt1.x()
          >> itsParts[(which-1)].itsPt0.y() >> itsParts[(which-1)].itsPt1.y();

      if (ifs.fail())
        {
          throw Util::Error(fstring("error reading file '", splinefile, "'"));
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
      throw Util::Error(fstring("error opening file '", coordfile, "'"));
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
      throw Util::Error(fstring("error reading file '", coordfile, "'"));
    }
}

void Fish::grGetBoundingBox(Gfx::Bbox& bbox) const
{
DOTRACE("Fish::grGetBoundingBox");

  bbox.vertex2(Gfx::Vec2<double>(-0.75, -0.5));
  bbox.vertex2(Gfx::Vec2<double>(+0.75, +0.5));
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

      dynamic_block<Pt3> ctrlpnts(itsParts[i].itsCtrlPnts);

      // Set the coefficients at the break point to a linear sum of
      // the two corresponding endpoints

      float alpha = 0.5*(1-itsParts[i].itsCoord);
      float beta  = 0.5*(1+itsParts[i].itsCoord);

      Pt3 pt = itsParts[i].itsPt0 * alpha + itsParts[i].itsPt1 * beta;

      // indexing in the coefs array starts at 0
      unsigned int bkpt = itsParts[i].itsBkpt - 1;

      if (bkpt < ctrlpnts.size())
        ctrlpnts[bkpt] = pt;

      canvas.drawNurbsCurve(itsParts[i].itsKnots, ctrlpnts);

      if (showControlPoints)
        {
          {
            Gfx::AttribSaver saver(canvas);
            canvas.setPointSize(4.0);

            Gfx::PointsBlock block(canvas);
            for (unsigned int i = 0; i < ctrlpnts.size(); ++i)
              {
                canvas.vertex3(ctrlpnts[i]);
              }
          }
          {
            Gfx::LinesBlock block(canvas);
            canvas.vertex3(itsParts[i].itsPt0);
            canvas.vertex3(itsParts[i].itsPt1);
          }
        }
    }
}


static const char vcid_fish_cc[] = "$Header$";
#endif // !FISH_CC_DEFINED
