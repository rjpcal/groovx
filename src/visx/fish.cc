///////////////////////////////////////////////////////////////////////
//
// fish.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 11:44:57 1999
// written: Tue Aug 21 16:09:26 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FISH_CC_DEFINED
#define FISH_CC_DEFINED

#include "fish.h"

#include "gfx/rect.h"

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

struct Fish::FishPart {
  FishPart() : itsOrder(0), itsKnots() {}

  GLint itsOrder;

  dynamic_block<GLfloat> itsKnots;

  dynamic_block<GLfloat> itsCoefs[2];
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

struct Fish::EndPt {
  int itsPart;
  int itsBkpt;
  float itsX[2];
  float itsY[2];
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

  static static_block<GLfloat, 13> default_knots =
  {{
    0.0, 0.0, 0.0, 0.0,
    0.1667, 0.3333, 0.5000, 0.6667, 0.8333,
    1.0, 1.0, 1.0, 1.0
  }};
  itsFishParts[0].itsKnots.assign(default_knots.begin(), default_knots.end());
  itsFishParts[1].itsKnots.assign(default_knots.begin(), default_knots.end());
  itsFishParts[2].itsKnots.assign(default_knots.begin(), default_knots.end());
  itsFishParts[3].itsKnots.assign(default_knots.begin(), default_knots.end());

  // coefficients 0
  static static_block<GLfloat, 9> coefs0_0 =
  {{
    -0.2856, -0.2140, -0.2176, -0.0735, 0.0168,
    0.1101, 0.3049, 0.0953, 0.1597
  }};
  static static_block<GLfloat, 9> coefs0_1 =
  {{
    0.2915, 0.2866, 0.2863, 0.4670,
    0.3913, 0.3071, 0.1048, 0.1800, 0.1538
  }};
  itsFishParts[0].itsCoefs[0].assign(coefs0_0.begin(), coefs0_0.end());
  itsFishParts[0].itsCoefs[1].assign(coefs0_1.begin(), coefs0_1.end());

  // coefficients 1
  static static_block<GLfloat, 9> coefs1_0 =
  {{
    0.1597, 0.2992, 0.2864, 0.7837, 0.4247,
    0.6362, 0.3379, 0.3137, 0.1573
  }};
  static static_block<GLfloat, 9> coefs1_1 =
  {{
    0.1538, 0.1016, 0.1044, 0.1590, 0.0155,
    -0.3203, -0.0706, 0.0049, -0.0401
  }};
  itsFishParts[1].itsCoefs[0].assign(coefs1_0.begin(), coefs1_0.end());
  itsFishParts[1].itsCoefs[1].assign(coefs1_1.begin(), coefs1_1.end());

  // coefficients 2
  static static_block<GLfloat, 9> coefs2_0 =
  {{
    0.1573, 0.2494, 0.1822, -0.0208, -0.0632,
    -0.1749, -0.1260, -0.3242, -0.2844
  }};
  static static_block<GLfloat, 9> coefs2_1 =
  {{
    -0.0401, -0.0294, -0.0877, -0.3236, -0.3412,
    -0.1120, -0.4172, -0.1818, -0.1840
  }};
  itsFishParts[2].itsCoefs[0].assign(coefs2_0.begin(), coefs2_0.end());
  itsFishParts[2].itsCoefs[1].assign(coefs2_1.begin(), coefs2_1.end());

  // coefficients 3
  static static_block<GLfloat, 9> coefs3_0 =
  {{
    -0.2844, -0.3492, -0.4554, -0.6135, -0.7018,
    -0.5693, -0.4507, -0.3393, -0.2856
  }};
  static static_block<GLfloat, 9> coefs3_1 =
  {{
    -0.1840, -0.1834, -0.1489, -0.0410, 0.0346,
    0.1147, 0.2227, 0.2737, 0.2915
  }};
  itsFishParts[3].itsCoefs[0].assign(coefs3_0.begin(), coefs3_0.end());
  itsFishParts[3].itsCoefs[1].assign(coefs3_1.begin(), coefs3_1.end());

  itsEndPts[0].itsPart = 1;
  itsEndPts[0].itsBkpt = 6;
  itsEndPts[0].itsX[0] = 0.2380;
  itsEndPts[0].itsX[1] = -0.0236;
  itsEndPts[0].itsY[0] = 0.3416;
  itsEndPts[0].itsY[1] = 0.2711;

  itsEndPts[1].itsPart = 2;
  itsEndPts[1].itsBkpt = 5;
  itsEndPts[1].itsX[0] = 0.6514;
  itsEndPts[1].itsX[1] = 0.2433;
  itsEndPts[1].itsY[0] = -0.0305;
  itsEndPts[1].itsY[1] = 0.0523;

  itsEndPts[2].itsPart = 3;
  itsEndPts[2].itsBkpt = 6;
  itsEndPts[2].itsX[0] = -0.2121;
  itsEndPts[2].itsX[1] = -0.1192;
  itsEndPts[2].itsY[0] = 0.0083;
  itsEndPts[2].itsY[1] = -0.2925;

  itsEndPts[3].itsPart = 4;
  itsEndPts[3].itsBkpt = 5;
  itsEndPts[3].itsX[0] = -0.7015;
  itsEndPts[3].itsX[1] = -0.7022;
  itsEndPts[3].itsY[0] = 0.1584;
  itsEndPts[3].itsY[1] = -0.1054;

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

  // reads in the spline knots and coefficient
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

      // allocates space and reads in the successive knots
      itsFishParts[i].itsKnots.resize(nknots);
      for (j = 0; j < itsFishParts[i].itsKnots.size(); ++j)
        {
          ifs >> itsFishParts[i].itsKnots[j];
        }

      // number of coefficients
      int ncoefs;
      ifs >> dummy >> ncoefs;

      // allocates space and reads in the successive coefficients
      for (j = 0; j < 2; ++j)
        {
          itsFishParts[i].itsCoefs[j].resize(ncoefs);

          for (k = 0; k < ncoefs; ++k)
            {
              ifs >> itsFishParts[i].itsCoefs[j][k];
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
      ifs >> itsEndPts[i].itsX[0] >> itsEndPts[i].itsX[1]
          >> itsEndPts[i].itsY[0] >> itsEndPts[i].itsY[1];

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

void Fish::grRender(Gfx::Canvas&) const
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

  // Loop over fish parts
  for (int i = 0; i < 4; ++i)
    {
      int totcoefs = 3*(itsFishParts[i].itsCoefs[0].size());

      fixed_block<GLfloat> coefs(totcoefs);

      {
        for (size_t j = 0; j < itsFishParts[i].itsCoefs[0].size(); ++j)
          {
            for (int k = 0; k < 2; ++k)
              {
                coefs.at(3*j+k) = (GLfloat) itsFishParts[i].itsCoefs[k][j];
              }
            coefs.at(3*j+2) = (GLfloat) 0.0;
          }
      }

      // Fix up the coefficients at the end points
      for (int j = 0; j < 4; ++j)
        {
          if ( itsEndPts[j].itsPart == (i+1) )
            {
              // indexing in the coefs array starts at 0
              int bkpt = itsEndPts[j].itsBkpt - 1;

              float alpha = 0.5*(1-itsCoords[j]);
              float beta  = 0.5*(1+itsCoords[j]);
              GLfloat x = alpha*itsEndPts[j].itsX[0] + beta*itsEndPts[j].itsX[1];
              GLfloat y = alpha*itsEndPts[j].itsY[0] + beta*itsEndPts[j].itsY[1];
              coefs.at(3*bkpt)   = x;
              coefs.at(3*bkpt+1) = y;
            }
        }

      // Render the curve
      gluBeginCurve(theNurb);
      {
        gluNurbsCurve(theNurb,
                      itsFishParts[i].itsKnots.size(),
                      &(itsFishParts[i].itsKnots[0]),
                      3, &coefs[0],
                      itsFishParts[i].itsOrder, GL_MAP1_VERTEX_3);
      }
      gluEndCurve(theNurb);

    } // end loop over fish parts

  gluDeleteNurbsRenderer(theNurb);
}


static const char vcid_fish_cc[] = "$Header$";
#endif // !FISH_CC_DEFINED
