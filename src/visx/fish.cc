///////////////////////////////////////////////////////////////////////
//
// fish.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 11:44:57 1999
// written: Wed Aug 29 12:48:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_CC_DEFINED
#define FISH_CC_DEFINED

#include "fish.h"

#include "gfx/canvas.h"
#include "gfx/rect.h"
#include "gfx/rgbacolor.h"
#include "gfx/vec3.h"

#include "io/ioproxy.h"
#include "io/reader.h"
#include "io/writer.h"

#include "util/arrays.h"
#include "util/error.h"
#include "util/strings.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <fstream.h>

#define DYNAMIC_TRACE_EXPR Fish::tracer.status()
#include "util/trace.h"
#define LOCAL_DEBUG
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int dummy=0; // We need a dummy int to attach various CPtrField's

  const IO::VersionId FISH_SERIAL_VERSION_ID = 2;

  typedef Gfx::Vec3<GLfloat> Pt3;
}

Util::Tracer Fish::tracer;

///////////////////////////////////////////////////////////////////////
//
// FishPart struct --
//
// Describes one of the part of the fish (upper fin, tail, bottom fin
// or mouth). The descripition is in terms of a spline curve, with
// given order, number of knots and x,y coefficients.
//
///////////////////////////////////////////////////////////////////////

struct Fish::FishPart
{
  FishPart() : itsOrder(0), itsKnots() {}

  GLint itsOrder;

  dynamic_block<GLfloat> itsKnots;

  dynamic_block<Pt3> itsCoefs;
};

/////////////////////////////////////////////////////////////////////////
//
// EndPt struct --
//
// Each of the endpoint (four in total) is associated with a
// breakpoint of one of the parts. This is described in this structure
// as well as the x and y coordinates of the two points which define
// the boundaries of the endpoint line
//
///////////////////////////////////////////////////////////////////////

struct Fish::EndPt
{
  int itsPart;
  int itsBkpt;
  Pt3 itsPt0;
  Pt3 itsPt1;
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
    Field("category", &Fish::itsFishCategory, 0, 0, 10, 1, true),
    Field("dorsalFinCoord", &Fish::itsDorsalFinCoord, 0.0, -2.0, 2.0, 0.1),
    Field("tailFinCoord", &Fish::itsTailFinCoord, 0.0, -2.0, 2.0, 0.1),
    Field("lowerFinCoord", &Fish::itsLowerFinCoord, 0.0, -2.0, 2.0, 0.1),
    Field("mouthCoord", &Fish::itsMouthCoord, 0.0, -2.0, 2.0, 0.1),

    Field("currentPart", Field::BoundsCheck(),
          &Fish::itsCurrentPart, 0, 0, 3, 1, true),

    Field("currentEndPt", Field::BoundsCheck(),
          &Fish::itsCurrentEndPt, 0, 0, 3, 1, true),

    Field("endPt_Part", &Fish::itsEndPt_Part, 1, 1, 4, 1),
    Field("endPt_Bkpt", &Fish::itsEndPt_Bkpt, 1, 1, 10, 1)
  };

  static FieldMap FISH_FIELDS(FIELD_ARRAY, &GrObj::classFields());

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
  itsFishCategory(-1),
  itsDorsalFinCoord(&itsCoords[0]),
  itsTailFinCoord(&itsCoords[1]),
  itsLowerFinCoord(&itsCoords[2]),
  itsMouthCoord(&itsCoords[3]),
  itsCurrentPart(0),
  itsCurrentEndPt(0),
  itsEndPt_Part(&dummy),
  itsEndPt_Bkpt(&dummy),
  itsFishParts(new FishPart[4]),
  itsEndPts(new EndPt[4])
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

  this->sigNodeChanged.connect(this, &Fish::updatePtrs);

  this->sigNodeChanged.emit();
}

void Fish::restoreToDefault()
{
DOTRACE("Fish::restoreToDefault");
  itsFishParts[3].itsOrder = itsFishParts[2].itsOrder = itsFishParts[1].itsOrder = itsFishParts[0].itsOrder = 4;

  static const GLfloat def_knots[] =
  {
    0.0, 0.0, 0.0, 0.0,
    0.1667, 0.3333, 0.5000, 0.6667, 0.8333,
    1.0, 1.0, 1.0, 1.0
  };
  itsFishParts[0].itsKnots.assign(array_begin(def_knots), array_end(def_knots));
  itsFishParts[1].itsKnots.assign(array_begin(def_knots), array_end(def_knots));
  itsFishParts[2].itsKnots.assign(array_begin(def_knots), array_end(def_knots));
  itsFishParts[3].itsKnots.assign(array_begin(def_knots), array_end(def_knots));

  static Pt3 coefs0[] =
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

  static Pt3 coefs1[] =
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

  static Pt3 coefs2[] =
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

  static Pt3 coefs3[] =
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

  itsFishParts[0].itsCoefs.assign(array_begin(coefs0), array_end(coefs0));
  itsFishParts[1].itsCoefs.assign(array_begin(coefs1), array_end(coefs1));
  itsFishParts[2].itsCoefs.assign(array_begin(coefs2), array_end(coefs2));
  itsFishParts[3].itsCoefs.assign(array_begin(coefs3), array_end(coefs3));

  itsEndPts[0].itsPart = 1;
  itsEndPts[0].itsBkpt = 6;
  itsEndPts[0].itsPt0.set(0.2380, 0.3416, 0.0);
  itsEndPts[0].itsPt1.set(-0.0236, 0.2711, 0.0);

  itsEndPts[1].itsPart = 2;
  itsEndPts[1].itsBkpt = 5;
  itsEndPts[1].itsPt0.set(0.6514, -0.0305, 0.0);
  itsEndPts[1].itsPt1.set(0.2433, 0.0523, 0.0);

  itsEndPts[2].itsPart = 3;
  itsEndPts[2].itsBkpt = 6;
  itsEndPts[2].itsPt0.set(-0.2121, 0.0083, 0.0);
  itsEndPts[2].itsPt1.set(-0.1192, -0.2925, 0.0);

  itsEndPts[3].itsPart = 4;
  itsEndPts[3].itsBkpt = 5;
  itsEndPts[3].itsPt0.set(-0.7015, 0.1584, 0.0);
  itsEndPts[3].itsPt1.set(-0.7022, -0.1054, 0.0);

  itsCoords[0] = itsCoords[1] = itsCoords[2] = itsCoords[3] = 0.0;
}

Fish::~Fish ()
{
DOTRACE("Fish::~Fish");
  delete [] itsFishParts;
  delete [] itsEndPts;
}

IO::VersionId Fish::serialVersionId() const
{
DOTRACE("Fish::serialVersionId");
  return FISH_SERIAL_VERSION_ID;
}

void Fish::readFrom(IO::Reader* reader)
{
DOTRACE("Fish::readFrom");

  reader->ensureReadVersionId("Fish", 2, "Try grsh0.8a4");

  readFieldsFrom(reader, classFields());

  reader->readBaseClass("GrObj", IO::makeProxy<GrObj>(this));
}

void Fish::writeTo(IO::Writer* writer) const
{
DOTRACE("Fish::writeTo");

  writer->ensureWriteVersionId("Fish", FISH_SERIAL_VERSION_ID, 2,
                               "Try grsh0.8a4");

  writeFieldsTo(writer, classFields());

  writer->writeBaseClass("GrObj", IO::makeConstProxy<GrObj>(this));
}

void Fish::updatePtrs()
{
DOTRACE("Fish::updatePtrs");

  itsEndPt_Part = &(itsEndPts[itsCurrentEndPt].itsPart);
  itsEndPt_Bkpt = &(itsEndPts[itsCurrentEndPt].itsBkpt);
}

void Fish::readSplineFile(const char* splinefile)
{
DOTRACE("Fish::readSplineFile");
  size_t i, j;
  int k, splnb, endptnb;
  fstring dummy;

  // read in the spline knots and coefficient
  STD_IO::ifstream ifs(splinefile);
  if (ifs.fail())
    {
      throw Util::Error(fstring("error opening file '", splinefile, "'"));
    }

  for(i = 0; i < 4; ++i)
    {
      // spline number and order
      ifs >> dummy >> splnb >> dummy >> itsFishParts[i].itsOrder;

      // number of knots
      int nknots;
      ifs >> dummy >> nknots;

      // allocate space and read in the successive knots
      itsFishParts[i].itsKnots.resize(nknots);
      for (j = 0; j < itsFishParts[i].itsKnots.size(); ++j)
        {
          ifs >> itsFishParts[i].itsKnots[j];
        }

      // number of coefficients
      int ncoefs;
      ifs >> dummy >> ncoefs;

      // allocate space and read in the successive coefficients
      itsFishParts[i].itsCoefs.resize(ncoefs);
      {
        for (k = 0; k < ncoefs; ++k)
          {
            ifs >> itsFishParts[i].itsCoefs[k].x();
          }
      }
      {
        for (k = 0; k < ncoefs; ++k)
          {
            ifs >> itsFishParts[i].itsCoefs[k].y();
          }
      }
      {
        for (k = 0; k < ncoefs; ++k)
          {
            itsFishParts[i].itsCoefs[k].z() = 0.0;
          }
      }

      if (ifs.fail())
        {
          throw Util::Error(fstring("error reading file '", splinefile, "'"));
        }
    }


  for(i = 0; i < 4; ++i)
    {
      // endpt number, associated part and breakpoint
      ifs >> dummy >> endptnb
          >> dummy >> itsEndPts[i].itsPart
          >> dummy >> itsEndPts[i].itsBkpt;

      // skip the next line
      ifs >> dummy >> dummy;

      // x&y coordinates
      ifs >> itsEndPts[i].itsPt0.x() >> itsEndPts[i].itsPt1.x()
          >> itsEndPts[i].itsPt0.y() >> itsEndPts[i].itsPt1.y();

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
      ifs >> itsCoords[i];
    }

  if (ifs.fail())
    {
      throw Util::Error(fstring("error reading file '", coordfile, "'"));
    }
}

Gfx::Rect<double> Fish::grGetBoundingBox() const
{
DOTRACE("Fish::grGetBoundingBox");

  Gfx::Rect<double> bbox;
  bbox.left() = -0.75;
  bbox.right() = 0.75;
  bbox.bottom() = -0.5;
  bbox.top() = 0.5;

  return bbox;
}

void Fish::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Fish::grRender");
  // Create and configure the NURBS object
  GLUnurbsObj* theNurb = gluNewNurbsRenderer();
  if (theNurb == 0)
    {
      throw Util::Error("Fish::grRender: couldn't allocate GLUnurbsObj");
    }
  gluNurbsProperty(theNurb, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
  gluNurbsProperty(theNurb, GLU_U_STEP, 200);
  gluNurbsProperty(theNurb, GLU_V_STEP, 200);

  Gfx::RgbaColor colors[4] =
  {
    Gfx::RgbaColor(1.0, 0.0, 0.0, 1.0),
    Gfx::RgbaColor(0.0, 1.0, 0.0, 1.0),
    Gfx::RgbaColor(0.0, 0.0, 1.0, 1.0),
    Gfx::RgbaColor(0.0, 0.0, 0.0, 1.0)
  };

  // Loop over fish parts
  for (int i = 0; i < 4; ++i)
    {
      canvas.setColor(colors[i]);

      dynamic_block<Pt3> ctrlpnts(itsFishParts[i].itsCoefs);

      // Fix up the coefficients at the end points
      for (int j = 0; j < 4; ++j)
        {
          if ( itsEndPts[j].itsPart == (i+1) )
            {
              // indexing in the coefs array starts at 0
              int bkpt = itsEndPts[j].itsBkpt - 1;

              float alpha = 0.5*(1-itsCoords[j]);
              float beta  = 0.5*(1+itsCoords[j]);

              Pt3 pt = itsEndPts[j].itsPt0 * alpha + itsEndPts[j].itsPt1 * beta;

              ctrlpnts.at(bkpt) = pt;
            }
        }

      // Render the curve
      gluBeginCurve(theNurb);
      {
        gluNurbsCurve(theNurb,
                      itsFishParts[i].itsKnots.size(),
                      &(itsFishParts[i].itsKnots[0]),
                      3, ctrlpnts.at(0).data(),
                      itsFishParts[i].itsOrder, GL_MAP1_VERTEX_3);
      }
      gluEndCurve(theNurb);

    } // end loop over fish parts

  gluDeleteNurbsRenderer(theNurb);
}


static const char vcid_fish_cc[] = "$Header$";
#endif // !FISH_CC_DEFINED
