//////////////////////////////////////////////////////////////////////
//
// fish.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Sep 29 11:44:57 1999
// written: Sat Sep  1 09:56:32 2001
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
#define LOCAL_ASSERT
#include "util/debug.h"

///////////////////////////////////////////////////////////////////////
//
// File scope data
//
///////////////////////////////////////////////////////////////////////

namespace
{
  int dummy_int=0; // We need a dummy int to attach various CPtrField's

  const IO::VersionId FISH_SERIAL_VERSION_ID = 2;

  typedef Gfx::Vec3<GLfloat> Pt3;

  const int DF_0 = 0;
  const int TF_1 = 1;
  const int LF_2 = 2;
  const int MA_3 = 3;

  struct BezData
  {
    Gfx::Vec3<double> pt0;
    Gfx::Vec3<double> pt1;
    Gfx::Vec3<double> pt2;
    Gfx::Vec3<double> pt3;
  };

  void drawBezier(Gfx::Canvas& canvas, const BezData& bz, unsigned int subdiv)
  {
    canvas.drawBezier4(bz.pt0, bz.pt1, bz.pt2, bz.pt3, subdiv);
  }

  void drawNurbs(Gfx::Canvas& canvas,
                 const dynamic_block<GLfloat>& knots,
                 const dynamic_block<Pt3>& pts,
                 unsigned int order)
  {
//      struct bspcontrol   *pts = bsd->pts;
//      float               tx, ty, tw;
    float               d, d1, d2, d3;
//      int                 k;

    int n = pts.size();

    Assert(n > 4);

    int nbz = n - 3;

    const GLfloat* t = &knots[3];

    dynamic_block<BezData> bz(nbz);

//      bz[0].x0 = pts[0].x;
//      bz[0].y0 = pts[0].y;
//      bz[0].w0 = pts[0].w;
    bz[0].pt0 = pts[0];
//      bz[0].x1 = pts[1].x;
//      bz[0].y1 = pts[1].y;
//      bz[0].w1 = pts[1].w;
    bz[0].pt1 = pts[1];

//      d1 = pts[3].t - pts[2].t;
//      d2 = pts[2].t - pts[1].t;
    d1 = t[3] - t[2];
    d2 = t[2] - t[1];

    d = d1 + d2;

//      bz[0].x2 = (d2 * pts[2].x + d1 * pts[1].x) / d;
//      bz[0].y2 = (d2 * pts[2].y + d1 * pts[1].y) / d;
//      bz[0].w2 = (d2 * pts[2].w + d1 * pts[1].w) / d;
    bz[0].pt2 = (pts[2] * d2 + pts[1] * d1) / d;

    int j = 2;
    for ( /* */ ; j < n - 2; j++) {
      d3 = d2;
      d2 = d1;
      if (j == n - 3)
        d1 = 0;
      else
        {
//          d1 = pts[j+2].t - pts[j+1].t;
          d1 = t[j+2] - t[j+1];
        }
      d = d1 + d2 + d3;
//        tx = (d3 * pts[j+1].x + (d1 + d2) * pts[j].x) / d;
//        ty = (d3 * pts[j+1].y + (d1 + d2) * pts[j].y) / d;
//        tw = (d3 * pts[j+1].w + (d1 + d2) * pts[j].w) / d;
      Pt3 txyz = ( (pts[j+1] * d3) + (pts[j] * (d1+d2)) ) / d;

//        bz[j-2].x3 = (d2 * bz[j-2].x2 + d3 * tx) / (d2 + d3);
//        bz[j-2].y3 = (d2 * bz[j-2].y2 + d3 * ty) / (d2 + d3);
//        bz[j-2].w3 = (d2 * bz[j-2].w2 + d3 * tw) / (d2 + d3);
      bz[j-2].pt3 = (bz[j-2].pt2 * d2 + txyz * d3) / (d2+d3);
//        bz[j-1].x0 = bz[j-2].x3;
//        bz[j-1].y0 = bz[j-2].y3;
//        bz[j-1].w0 = bz[j-2].w3;
      bz[j-1].pt0 = bz[j-2].pt3;
//        bz[j-1].x1 = tx;
//        bz[j-1].y1 = ty;
//        bz[j-1].w1 = tw;
      bz[j-1].pt1 = txyz;
//        bz[j-1].x2 = ((d2 + d3) * pts[j+1].x + d1 * pts[j].x) / d;
//        bz[j-1].y2 = ((d2 + d3) * pts[j+1].y + d1 * pts[j].y) / d;
//        bz[j-1].w2 = ((d2 + d3) * pts[j+1].w + d1 * pts[j].w) / d;
      bz[j-1].pt2 = (pts[j+1] * (d2+d3) + pts[j] * d1) / d;
    }
//      bz[j-2].x2 = pts[n-2].x;
//      bz[j-2].y2 = pts[n-2].y;
//      bz[j-2].w2 = pts[n-2].w;
    bz[j-2].pt2 = pts[n-2];
//      bz[j-2].x3 = pts[n-1].x;
//      bz[j-2].y3 = pts[n-1].y;
//      bz[j-2].w3 = pts[n-1].w;
    bz[j-2].pt3 = pts[n-1];

    {
      for (int i = 0; i < bz.size(); ++i)
        {
          drawBezier(canvas, bz[i], 20);
        }
    }
  }
}

Util::Tracer Fish::tracer;

///////////////////////////////////////////////////////////////////////
//
// Fish::Part struct --
//
// Describes one of the part of the fish (upper fin, tail, bottom fin
// or mouth). The descripition is in terms of a spline curve, with
// given order, number of knots and x,y coefficients.
//
///////////////////////////////////////////////////////////////////////

struct Fish::Part
{
  Part() :
    itsOrder(0), itsKnots(), itsCtrlPnts(),
    itsBkpt(1), itsPt0(), itsPt1(),
    itsCoord(0.0)
  {}

  GLint itsOrder;

  dynamic_block<GLfloat> itsKnots;

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
  void reset(int order, GLfloat const (&knots)[N1], Pt3 const (&points)[N2])
  {
    itsOrder = order;
    itsKnots.assign(array_begin(knots), array_end(knots));
    itsCtrlPnts.assign(array_begin(points), array_end(points));
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
          Field::NEW_GROUP | Field::CHECKED),

    Field("currentEndPt", &Fish::itsCurrentEndPt, 0, 0, 3, 1, Field::CHECKED),

    Field("endPt_Part", &Fish::itsEndPt_Part, 1, 1, 4, 1),
    Field("endPt_Bkpt", &Fish::itsEndPt_Bkpt, 1, 1, 10, 1),

    Field("inColor", &Fish::inColor,
          false, false, true, true, Field::TRANSIENT),
    Field("showControlPoints", &Fish::showControlPoints,
          false, false, true, true, Field::TRANSIENT)
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
  itsParts(new Fish::Part[4]),
  itsFishCategory(-1),
  itsDorsalFinCoord(&itsParts[DF_0].itsCoord),
  itsTailFinCoord(&itsParts[TF_1].itsCoord),
  itsLowerFinCoord(&itsParts[LF_2].itsCoord),
  itsMouthCoord(&itsParts[MA_3].itsCoord),
  itsCurrentPart(0),
  itsCurrentEndPt(0),
  itsEndPt_Part(&dummy_int),
  itsEndPt_Bkpt(&dummy_int),
  inColor(false),
  showControlPoints(false)
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

Fish::~Fish ()
{
DOTRACE("Fish::~Fish");
  delete [] itsParts;
}

void Fish::restoreToDefault()
{
DOTRACE("Fish::restoreToDefault");

  static const GLfloat knots[] =
  {
    0.0, 0.0, 0.0, 0.0,
    0.1667, 0.3333, 0.5000, 0.6667, 0.8333,
    1.0, 1.0, 1.0, 1.0
  };

  static const Pt3 coefs0[] =
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

  static const Pt3 coefs1[] =
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

  static const Pt3 coefs2[] =
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

  static const Pt3 coefs3[] =
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

  itsParts[DF_0].reset(4, knots, coefs0);
  itsParts[TF_1].reset(4, knots, coefs1);
  itsParts[LF_2].reset(4, knots, coefs2);
  itsParts[MA_3].reset(4, knots, coefs3);

  itsParts[0].itsBkpt = 6;
  itsParts[0].itsPt0.set(0.2380, 0.3416, 0.0);
  itsParts[0].itsPt1.set(-0.0236, 0.2711, 0.0);

  itsParts[1].itsBkpt = 5;
  itsParts[1].itsPt0.set(0.6514, -0.0305, 0.0);
  itsParts[1].itsPt1.set(0.2433, 0.0523, 0.0);

  itsParts[2].itsBkpt = 6;
  itsParts[2].itsPt0.set(-0.2121, 0.0083, 0.0);
  itsParts[2].itsPt1.set(-0.1192, -0.2925, 0.0);

  itsParts[3].itsBkpt = 5;
  itsParts[3].itsPt0.set(-0.7015, 0.1584, 0.0);
  itsParts[3].itsPt1.set(-0.7022, -0.1054, 0.0);

  itsParts[DF_0].itsCoord =
    itsParts[TF_1].itsCoord =
    itsParts[LF_2].itsCoord =
    itsParts[MA_3].itsCoord = 0.0;
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

 DebugPrintNL("updatePtrs!");

  itsEndPt_Bkpt = &(itsParts[itsCurrentPart].itsBkpt);
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
      // spline number and order
      ifs >> dummy >> splnb >> dummy >> itsParts[i].itsOrder;

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
      int which;

      int endptnb;

      // endpt number, associated part and breakpoint
      ifs >> dummy >> endptnb
          >> dummy >> which
          >> dummy >> itsParts[(which-1)].itsBkpt;

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

namespace
{
  class NurbsObj
  {
  private:
    NurbsObj(const NurbsObj&);
    NurbsObj& operator=(const NurbsObj&);

  public:
    GLUnurbsObj* ptr;

    NurbsObj() : ptr(0)
    {
      DOTRACE("Fish::grRender::NurbsObj");
      ptr = gluNewNurbsRenderer();

      if (ptr == 0)
        {
          throw Util::Error("couldn't allocate GLUnurbsObj");
        }

      gluNurbsProperty(ptr, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE);
      gluNurbsProperty(ptr, GLU_U_STEP, 200);
      gluNurbsProperty(ptr, GLU_V_STEP, 200);
    }

    ~NurbsObj()
    {
      DOTRACE("Fish::grRender::~NurbsObj");
      gluDeleteNurbsRenderer(ptr);
    }
  };
}

void Fish::grRender(Gfx::Canvas& canvas) const
{
DOTRACE("Fish::grRender");

  // Create and configure the NURBS object
  NurbsObj theNurb;

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

      {
        DOTRACE("Fish::grRender-gluNurbsCurve");
        // Render the curve
        gluBeginCurve(theNurb.ptr);
        {
          gluNurbsCurve(theNurb.ptr,
                        itsParts[i].itsKnots.size(),
                        &(itsParts[i].itsKnots[0]),
                        3, ctrlpnts[0].data(),
                        itsParts[i].itsOrder, GL_MAP1_VERTEX_3);
        }
        gluEndCurve(theNurb.ptr);
      }

      drawNurbs(canvas, itsParts[i].itsKnots, ctrlpnts, itsParts[i].itsOrder);

      if (showControlPoints)
        {
          Gfx::Canvas::AttribSaver saver(canvas);
          canvas.setPointSize(4.0);

          Gfx::Canvas::PointsBlock block(canvas);
          for (unsigned int i = 0; i < ctrlpnts.size(); ++i)
            {
              canvas.vertex3(ctrlpnts[i]);
            }
        }
    }
}


static const char vcid_fish_cc[] = "$Header$";
#endif // !FISH_CC_DEFINED
