/** @file visx/fish.cc bezier-curve outlines of tropical fish */

//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Sep 29 11:44:57 1999
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

#ifndef GROOVX_VISX_FISH_CC_UTC20050626084016_DEFINED
#define GROOVX_VISX_FISH_CC_UTC20050626084016_DEFINED
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

#include "rutz/arrays.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <fstream>

#define GVX_DYNAMIC_TRACE_EXPR Fish::tracer.status()
#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int dummy_int=0; // We need a dummy int to attach various CPtrField's

  const io::version_id FISH_SVID = 4;

  using geom::vec2d;
  using geom::vec3f;
  using geom::vec3d;

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

  rutz::dynamic_block<vec3f> itsCtrlPnts;

  // -- end points --

  // Each endpoint is associated with a breakpoint. This is described
  // in this structure as well as the x and y coordinates of the two
  // points which define the boundaries of the endpoint line
  int itsBkpt;
  vec3f itsPt0;
  vec3f itsPt1;

  float itsCoord;

  template <std::size_t N1, std::size_t N2>
  void reset(float const (&knots)[N1], vec3f const (&points)[N2])
  {
    itsKnots.assign(rutz::array_begin(knots),
                    rutz::array_end(knots));
    itsCtrlPnts.assign(rutz::array_begin(points),
                       rutz::array_end(points));
    itsCoord = 0.0f;
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
    Field("dorsalFinCoord", &Fish::itsDorsalFinCoord, 0.0f, -2.0f, 2.0f, 0.1f),
    Field("tailFinCoord", &Fish::itsTailFinCoord, 0.0f, -2.0f, 2.0f, 0.1f),
    Field("lowerFinCoord", &Fish::itsLowerFinCoord, 0.0f, -2.0f, 2.0f, 0.1f),
    Field("mouthCoord", &Fish::itsMouthCoord, 0.0f, -2.0f, 2.0f, 0.1f),

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
          0.0f, -1.0f, 1.0f, 0.1f, Field::TRANSIENT)
  };

  static FieldMap FISH_FIELDS(FIELD_ARRAY, &GxShapeKit::classFields());

  return FISH_FIELDS;
}

Fish* Fish::make()
{
GVX_TRACE("Fish::make");
  return new Fish;
}

Fish* Fish::makeFromFiles(const char* splinefile,
                          const char* coordfile, int index)
{
GVX_TRACE("Fish::makeFromFiles");
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
GVX_TRACE("Fish::Fish");

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

  nub::floating_ref<Fish> ref(this);

  this->sigNodeChanged.connect(this, &Fish::updatePtrs);

  this->sigNodeChanged.emit();
}

Fish::~Fish () noexcept
{
GVX_TRACE("Fish::~Fish");
  delete [] itsParts;
}

void Fish::restoreToDefault()
{
GVX_TRACE("Fish::restoreToDefault");

  static const float knots[] =
  {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.1667f, 0.3333f, 0.5000f, 0.6667f, 0.8333f,
    1.0f, 1.0f, 1.0f, 1.0f
  };

  static const vec3f DF_coefs[] =
  {
    vec3f(-0.2856f,  0.2915f,  0.2856f),
    vec3f(-0.2140f,  0.2866f,  0.2140f),
    vec3f(-0.2176f,  0.2863f,  0.2176f),
    vec3f(-0.0735f,  0.4670f,  0.0735f),
    vec3f( 0.0168f,  0.3913f,  0.0168f),
    vec3f( 0.1101f,  0.3071f,  0.1101f),
    vec3f( 0.3049f,  0.1048f,  0.3049f),
    vec3f( 0.0953f,  0.1800f,  0.0953f),
    vec3f( 0.1597f,  0.1538f,  0.1597f),
  };

  static const vec3f TF_coefs[] =
  {
    vec3f( 0.1597f,  0.1538f,  0.1597f),
    vec3f( 0.2992f,  0.1016f,  0.2992f),
    vec3f( 0.2864f,  0.1044f,  0.2864f),
    vec3f( 0.7837f,  0.1590f,  0.7837f),
    vec3f( 0.4247f,  0.0155f,  0.4247f),
    vec3f( 0.6362f, -0.3203f,  0.6362f),
    vec3f( 0.3379f, -0.0706f,  0.3379f),
    vec3f( 0.3137f,  0.0049f,  0.3137f),
    vec3f( 0.1573f, -0.0401f,  0.1573f),
  };

  static const vec3f LF_coefs[] =
  {
    vec3f( 0.1573f, -0.0401f,  0.1573f),
    vec3f( 0.2494f, -0.0294f,  0.2494f),
    vec3f( 0.1822f, -0.0877f,  0.1822f),
    vec3f(-0.0208f, -0.3236f,  0.0208f),
    vec3f(-0.0632f, -0.3412f,  0.0632f),
    vec3f(-0.1749f, -0.1120f,  0.1749f),
    vec3f(-0.1260f, -0.4172f,  0.1260f),
    vec3f(-0.3242f, -0.1818f,  0.3242f),
    vec3f(-0.2844f, -0.1840f,  0.2844f),
  };

  static const vec3f MA_coefs[] =
  {
    vec3f(-0.2844f, -0.1840f,  0.2844f),
    vec3f(-0.3492f, -0.1834f,  0.3492f),
    vec3f(-0.4554f, -0.1489f,  0.4554f),
    vec3f(-0.6135f, -0.0410f,  0.6135f),
    vec3f(-0.7018f,  0.0346f,  0.7018f),
    vec3f(-0.5693f,  0.1147f,  0.5693f),
    vec3f(-0.4507f,  0.2227f,  0.4507f),
    vec3f(-0.3393f,  0.2737f,  0.3393f),
    vec3f(-0.2856f,  0.2915f,  0.2856f),
  };

  itsParts[DF_0].reset(knots, DF_coefs);
  itsParts[TF_1].reset(knots, TF_coefs);
  itsParts[LF_2].reset(knots, LF_coefs);
  itsParts[MA_3].reset(knots, MA_coefs);

  itsParts[DF_0].itsBkpt = 6;
  itsParts[DF_0].itsPt0.set(0.2380f, 0.3416f, 0.2380f);
  itsParts[DF_0].itsPt1.set(-0.0236f, 0.2711f, 0.0236f);

  itsParts[TF_1].itsBkpt = 5;
  itsParts[TF_1].itsPt0.set(0.6514f, -0.0305f, 0.6514f);
  itsParts[TF_1].itsPt1.set(0.2433f, 0.0523f, 0.2433f);

  itsParts[LF_2].itsBkpt = 6;
  itsParts[LF_2].itsPt0.set(-0.2121f, 0.0083f, 0.2121f);
  itsParts[LF_2].itsPt1.set(-0.1192f, -0.2925f, 0.1192f);

  itsParts[MA_3].itsBkpt = 5;
  itsParts[MA_3].itsPt0.set(-0.7015f, 0.1584f, 0.7015f);
  itsParts[MA_3].itsPt1.set(-0.7022f, -0.1054f, 0.7022f);
}

io::version_id Fish::class_version_id() const
{
GVX_TRACE("Fish::class_version_id");
  return FISH_SVID;
}

void Fish::read_from(io::reader& reader)
{
GVX_TRACE("Fish::read_from");

  reader.ensure_version_id("Fish", 4,
                           "Try cvs tag xml_conversion_20040526",
                           SRC_POS);

  readFieldsFrom(reader, classFields());

  reader.read_base_class("GxShapeKit", io::make_proxy<GxShapeKit>(this));
}

void Fish::write_to(io::writer& writer) const
{
GVX_TRACE("Fish::write_to");

  writer.ensure_output_version_id("Fish", FISH_SVID, 4,
                              "Try groovx0.8a7", SRC_POS);

  writeFieldsTo(writer, classFields(), FISH_SVID);

  writer.write_base_class("GxShapeKit", io::make_const_proxy<GxShapeKit>(this));
}

void Fish::updatePtrs()
{
GVX_TRACE("Fish::updatePtrs");

  itsCurrentPartBkpt = &(itsParts[itsCurrentPart].itsBkpt);
}

void Fish::readSplineFile(const char* splinefile)
{
GVX_TRACE("Fish::readSplineFile");
  size_t i, j;
  int k, splnb;
  fstring dummy;

  // read in the spline knots and coefficient
  std::ifstream ifs(splinefile);
  if (ifs.fail())
    {
      throw rutz::error(rutz::sfmt("error opening file '%s'",
                                   splinefile), SRC_POS);
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
          throw rutz::error(rutz::sfmt("error reading file '%s'",
                                       splinefile), SRC_POS);
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
          throw rutz::error(rutz::sfmt("bad 'which' value '%d' while "
                                       "reading fish coords", which),
                            SRC_POS);
        }

      // skip the next line
      ifs >> dummy >> dummy;

      // x&y coordinates
      ifs >> itsParts[(which-1)].itsPt0.x() >> itsParts[(which-1)].itsPt1.x()
          >> itsParts[(which-1)].itsPt0.y() >> itsParts[(which-1)].itsPt1.y();

      if (ifs.fail())
        {
          throw rutz::error(rutz::sfmt("error reading file '%s'",
                                       splinefile), SRC_POS);
        }
    }
}

void Fish::readCoordFile(const char* coordfile, int index)
{
GVX_TRACE("Fish::readCoordFile");
  fstring dummy;

  std::ifstream ifs(coordfile);
  if (ifs.fail())
    {
      throw rutz::error(rutz::sfmt("error opening file '%s'",
                                   coordfile), SRC_POS);
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
      throw rutz::error(rutz::sfmt("error reading file '%s'",
                                   coordfile), SRC_POS);
    }
}

void Fish::grGetBoundingBox(Gfx::Bbox& bbox) const
{
GVX_TRACE("Fish::grGetBoundingBox");

  bbox.vertex2(vec2d(-0.75, -0.5));
  bbox.vertex2(vec2d(+0.75, +0.5));
}

void Fish::grRender(Gfx::Canvas& canvas) const
{
GVX_TRACE("Fish::grRender");

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

      rutz::dynamic_block<vec3f> ctrlpnts(itsParts[i].itsCtrlPnts);

      // Set the coefficients at the break point to a linear sum of
      // the two corresponding endpoints

      float alpha = 0.5f*(1-itsParts[i].itsCoord);
      float beta  = 0.5f*(1+itsParts[i].itsCoord);

      vec3f pt = itsParts[i].itsPt0 * alpha + itsParts[i].itsPt1 * beta;

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
          if (/* DISABLES CODE*/ (0))
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
              for (unsigned int p = 0; p < ctrlpnts.size(); ++p)
                {
                  canvas.vertex3(vec3d(ctrlpnts[p]));
                }
            }
          else
            {
              Gfx::AttribSaver saver(canvas);
              canvas.setPolygonFill(true);
              geom::rect<double> rect;
              for (unsigned int p = 0; p < ctrlpnts.size(); ++p)
                {
                  Gfx::MatrixSaver msaver(canvas);
                  canvas.translate(vec3d(0.0, 0.0, ctrlpnts[p].z()));
                  rect.set_lbwh(ctrlpnts[p].x(), ctrlpnts[p].y(),
                                0.03, 0.03);
                  canvas.drawRect(rect);
                }
            }
          {
            Gfx::LinesBlock block(canvas);
            canvas.vertex2(vec2d(itsParts[i].itsPt0.as_vec2()));
            canvas.vertex2(vec2d(itsParts[i].itsPt1.as_vec2()));
          }
        }
    }
}


#endif // !GROOVX_VISX_FISH_CC_UTC20050626084016_DEFINED
