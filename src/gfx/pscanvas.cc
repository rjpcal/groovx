///////////////////////////////////////////////////////////////////////
//
// pscanvas.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Aug 27 17:20:09 2001
// written: Thu Nov 21 15:49:12 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PSCANVAS_CC_DEFINED
#define PSCANVAS_CC_DEFINED

#include "gfx/pscanvas.h"

#include "gx/bmapdata.h"
#include "gx/rect.h"
#include "gx/rgbacolor.h"
#include "gx/vec2.h"
#include "gx/vec3.h"

#include "util/dlink_list.h"
#include "util/error.h"

#include <fstream>

#include "util/trace.h"
#include "util/debug.h"

class Gfx::PSCanvas::Impl
{
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:

  struct Primitive;

  std::ofstream                     itsFstream;
  unsigned int                      itsIndentLevel;
  dlink_list<Gfx::Vec2<double> >    itsScales;
  double                            itsLineWidth;
  Primitive*                        itsPrimPtr;
  unsigned int                      itsVcount;
  bool                              itsPolygonFill;

  Impl(const char* filename) :
    itsFstream(filename),
    itsIndentLevel(0),
    itsScales(),
    itsLineWidth(1.0), // in points
    itsPrimPtr(0),
    itsVcount(0),
    itsPolygonFill(false)
  {
    if (!itsFstream.is_open())
      raiseError(fstring("couldn't open '", filename, "' for writing"));

    itsScales.push_back(Gfx::Vec2<double>(1.0, 1.0));

    itsFstream << "%!\n";
    itsFstream << "%% generated by Gfx::PSCanvas\n";
    itsFstream << "%% $Revision$ \n";
    translate(Gfx::Vec3<double>(306.0, 360.0, 0.0));
    scale(Gfx::Vec3<double>(72.0, 72.0, 1.0));
  }

  ~Impl()
  {
    itsFstream << "showpage\n";
  }

  //
  // Graphics primitive definitions
  //

  struct Primitive
  {
    typedef Gfx::PSCanvas::Impl PS;

    virtual ~Primitive() {}
    virtual void onBegin(PS* ps) = 0;
    virtual void onVertex(PS* ps, const Gfx::Vec2<double>& v) = 0;
    virtual void onEnd(PS* ps) = 0;
  };

  struct PointsPrim : public Primitive
  {
    virtual void onBegin(PS*) {}

    virtual void onVertex(PS* ps, const Gfx::Vec2<double>& v)
    {
      ps->newpath(); ps->moveto(v); ps->stroke();
    }

    virtual void onEnd(PS*) {}
  };

  struct LinesPrim : public Primitive
  {
    virtual void onBegin(PS* ps)
    {
      ps->newpath();
    }
    virtual void onVertex(PS* ps, const Gfx::Vec2<double>& v)
    {
      if (ps->itsVcount % 2)
        {
          ps->lineto(v); ps->stroke();
        }
      else
        {
          ps->moveto(v);
        }
    }
    virtual void onEnd(PS*) {}
  };

  struct LineStripPrim : public Primitive
  {
    virtual void onBegin(PS* ps)
    {
      ps->newpath();
    }
    virtual void onVertex(PS* ps, const Gfx::Vec2<double>& v)
    {
      if (ps->itsVcount == 0)
        {
          ps->moveto(v);
        }
      else
        {
          ps->lineto(v);
        }
    }
    virtual void onEnd(PS* ps)
    {
      ps->stroke();
    }
  };

  struct LineLoopPrim : public Primitive
  {
    virtual void onBegin(PS*) {}
    virtual void onVertex(PS* ps, const Gfx::Vec2<double>& v)
    {
      if (ps->itsVcount == 0)
        {
          ps->newpath(); ps->moveto(v);
        }
      else
        {
          ps->lineto(v);
        }
    }
    virtual void onEnd(PS* ps)
    {
      ps->closepath(); ps->stroke();
    }
  };

  struct TrianglesPrim : public Primitive
  {
    virtual void onBegin(PS* ps)
    {
      ps->newpath();
    }
    virtual void onVertex(PS* ps, const Gfx::Vec2<double>& v)
    {
      switch(ps->itsVcount % 3)
        {
        case 0:
          ps->moveto(v);
          break;
        case 1:
          ps->lineto(v);
          break;
        case 2:
          ps->lineto(v); ps->closepath(); ps->renderpolygon();
          break;
        }
    }
    virtual void onEnd(PS*) {}
  };

  struct TriangleStripPrim : public Primitive
  {
    virtual void onBegin(PS*) { /* FIXME */ }
    virtual void onVertex(PS*, const Gfx::Vec2<double>&) { /* FIXME */ }
    virtual void onEnd(PS*) { /* FIXME */ }
  };

  struct TriangleFanPrim : public Primitive
  {
    virtual void onBegin(PS*) { /* FIXME */ }
    virtual void onVertex(PS*, const Gfx::Vec2<double>&) { /* FIXME */ }
    virtual void onEnd(PS*) { /* FIXME */ }
  };

  struct QuadsPrim : public Primitive
  {
    virtual void onBegin(PS*) {}

    virtual void onVertex(PS* ps, const Gfx::Vec2<double>& v)
    {
      switch(ps->itsVcount % 3)
        {
        case 0:
          ps->moveto(v);
          break;
        case 1:
        case 2:
          ps->lineto(v);
          break;
        case 3:
          ps->lineto(v); ps->closepath(); ps->renderpolygon();
          break;
        }
    }
    virtual void onEnd(PS*) {}
  };

  struct QuadStripPrim : public Primitive
  {
    dlink_list<Gfx::Vec2<double> > itsPts1;
    dlink_list<Gfx::Vec2<double> > itsPts2;

    QuadStripPrim() : itsPts1(), itsPts2() {}

    virtual void onBegin(PS*)
    {
      itsPts1.clear();
      itsPts2.clear();
    }

    virtual void onVertex(PS* ps, const Gfx::Vec2<double>& v)
    {
      if ((ps->itsVcount %2) == 0)
        itsPts1.push_back(v);
      else
        itsPts2.push_front(v);
    }

    virtual void onEnd(PS* ps)
    {
      ps->newpath();

      dlink_list<Gfx::Vec2<double> >::iterator itr = itsPts1.begin();

      ps->moveto(*itr); ++itr;

      while (!itsPts1.is_empty())
        {
          ps->lineto(itsPts1.front());
          itsPts1.pop_front();
        }

      while (!itsPts2.is_empty())
        {
          ps->lineto(itsPts2.front());
          itsPts2.pop_front();
        }

      ps->closepath();

      ps->renderpolygon();
    }
  };

  struct PolygonPrim : public LineLoopPrim
  {
    virtual void onEnd(PS* ps)
    {
      ps->closepath(); ps->renderpolygon();
    }
  };




  void indent()
  {
    for (unsigned int i = 0; i < itsIndentLevel; ++i)
      {
        itsFstream << '\t';
      }
  }

  void gsave()
  {
    indent(); itsFstream << "gsave\n";
    ++itsIndentLevel;
    itsScales.push_back(itsScales.back());
  }

  void grestore()
  {
    itsScales.pop_back();
    --itsIndentLevel;
    indent(); itsFstream << "grestore\n";
  }

  void translate(const Gfx::Vec3<double>& v)
  {
    indent(); pushxy(v); itsFstream << "translate\n";
  }

  void scale(const Gfx::Vec3<double>& v)
  {
    indent(); pushxy(v); itsFstream << "scale\n";

    itsScales.back() *= Gfx::Vec2<double>(v.x(), v.y());

    setlinewidth(itsLineWidth);
  }

  void rotate(double angle)
  {
    indent(); itsFstream << angle << " rotate\n";
  }

  // NOTE: these next four functions must be non-templates in order to avoid
  // "internal compiler errors" with g++-2.96 on Mac OS X.

  void moveto(const Gfx::Vec2<double>& v)
  {
    indent(); pushxy(v); itsFstream << "moveto\n";
  }

  void moveto(const Gfx::Vec3<double>& v)
  {
    indent(); pushxy(v); itsFstream << "moveto\n";
  }

  void lineto(const Gfx::Vec2<double>& v)
  {
    indent(); pushxy(v); itsFstream << "lineto\n";
  }

  void lineto(const Gfx::Vec3<double>& v)
  {
    indent(); pushxy(v); itsFstream << "lineto\n";
  }

  void newpath()
  {
    itsFstream << "\n"; indent(); itsFstream << "newpath\n";
  }

  void closepath()
  {
    indent(); itsFstream << "closepath\n";
  }

  void renderpolygon()
  {
    if (itsPolygonFill)
      fill();
    else
      stroke();
  }

  void arc(double x, double y, double r, double start, double end,
           bool reverse=false)
  {
    indent(); push1(x); push1(y); push1(r); push1(start); push1(end);
    if (reverse)
      itsFstream << "arcn\n";
    else
      itsFstream << "arc\n";
    //      indent(); itsFstream << "4 { pop } repeat\n";
  }

  void arcn(double x, double y, double r, double start, double end)
  {
    arc(x, y, r, start, end, true);
  }

  void bezier(const Gfx::Vec3<double>& p1,
              const Gfx::Vec3<double>& p2,
              const Gfx::Vec3<double>& p3,
              const Gfx::Vec3<double>& p4)
  {
    newpath();
    moveto(p1);
    indent(); pushxy(p2); pushxy(p3); pushxy(p4);
    itsFstream << "curveto\n";
    stroke();
  }

  void fill()
  {
    indent(); itsFstream << "fill\n\n";
  }

  void stroke()
  {
    indent(); itsFstream << "stroke\n\n";
  }

  double getLineScaleFactor()
  {
    double factor =
      (itsScales.back().abs().x() + itsScales.back().abs().y()) / 2.0;

    dbgEvalNL(3, factor);

    return factor;
  }

  void setlinewidth(double w)
  {
    itsLineWidth = w;

    indent(); push1(w/getLineScaleFactor()); itsFstream << "setlinewidth\n";
  }

  void setdash(unsigned short bit_pattern)
  {
    dbgEvalNL(3, (void*) bit_pattern);

    bool prev_bit = (0x8000 & bit_pattern);

    // will hold the lengths of the individual bit runs
    int lengths[16] = { 0 };
    int pos = 0;

    for (unsigned short mask = 0x8000; mask != 0; mask >>= 1)
      {
        bool bit = (mask & bit_pattern);
        bool same = (bit == prev_bit);

        if (!same)
          {
            ++pos; lengths[pos] = 0;
          }

        ++lengths[pos];

        prev_bit = bit;
      }

    ++pos; // so we point at one-past-the-end

    int offset = 0;

    if ((pos % 2) == 1)
      {
        offset = lengths[--pos];
        lengths[0] += offset;
      }

    double factor = getLineScaleFactor();

    indent();

    push1("[");
    for (int i = 0; i < pos; ++i)
      {
        push1(lengths[i] / factor);
      }
    push1("]");

    push1(offset); itsFstream << "setdash\n";
  }

  void setrgbcolor(const Gfx::RgbaColor& col)
  {
    indent(); push1(col.r()); push1(col.g()); push1(col.b());
    itsFstream << "setrgbcolor\n";
  }

  // The push1() and pushxy() functions must be non-templates to placate
  // g++-2.96 on Mac OS X.

  void push1(double v)
  {
    itsFstream << v << " ";
  }

  void push1(int v)
  {
    itsFstream << v << " ";
  }

  void push1(const char* v)
  {
    itsFstream << v << " ";
  }

  void pushxy(const Gfx::Vec2<double>& v)
  {
    itsFstream << v.x() << " " << v.y() << " ";
  }

  void pushxy(const Gfx::Vec3<double>& v)
  {
    itsFstream << v.x() << " " << v.y() << " ";
  }

  void raiseError(const fstring& msg)
  {
    throw Util::Error(fstring("PSCanvas error: ", msg));
  }

  void beginPrimitive(Primitive* ptr)
  {
    if (itsPrimPtr != 0)
      raiseError("in beginPrimitive, already in graphics primitive");

    itsPrimPtr = ptr;

    Assert(itsVcount == 0);
  }

  void endPrimitive()
  {
    if (itsPrimPtr == 0)
      {
        raiseError("called endPrimitive outside graphics primitive");
      }

    itsPrimPtr->onEnd(this);

    itsPrimPtr = 0;
    itsVcount = 0;
  }
};

namespace
{
  Gfx::PSCanvas::Impl::PointsPrim thePointsPrim;
  Gfx::PSCanvas::Impl::LinesPrim theLinesPrim;
  Gfx::PSCanvas::Impl::LineStripPrim theLineStripPrim;
  Gfx::PSCanvas::Impl::LineLoopPrim theLineLoopPrim;
  Gfx::PSCanvas::Impl::TrianglesPrim theTrianglesPrim;
  Gfx::PSCanvas::Impl::TriangleStripPrim theTriangleStripPrim;
  Gfx::PSCanvas::Impl::TriangleFanPrim theTriangleFanPrim;
  Gfx::PSCanvas::Impl::QuadsPrim theQuadsPrim;
  Gfx::PSCanvas::Impl::QuadStripPrim theQuadStripPrim;
  Gfx::PSCanvas::Impl::PolygonPrim thePolygonPrim;
}

Gfx::PSCanvas::PSCanvas(const char* filename) :
  itsImpl(new Impl(filename))
{
DOTRACE("Gfx::PSCanvas::PSCanvas");
}

Gfx::PSCanvas::~PSCanvas()
{
DOTRACE("Gfx::PSCanvas::~PSCanvas");
  delete itsImpl;
}

Gfx::Vec2<int> Gfx::PSCanvas::screenFromWorld(
  const Gfx::Vec2<double>& /*world_pos*/
  ) const
{
DOTRACE("Gfx::PSCanvas::screenFromWorld(Gfx::Vec2)");
// FIXME
  itsImpl->raiseError("not implemented");
  return Gfx::Vec2<int>();
}

Gfx::Vec2<double> Gfx::PSCanvas::worldFromScreen(
  const Gfx::Vec2<int>& /*screen_pos*/
  ) const
{
DOTRACE("Gfx::PSCanvas::worldFromScreen(Gfx::Vec2)");
// FIXME
  itsImpl->raiseError("not implemented");
  return Gfx::Vec2<double>();
}


Gfx::Rect<int> Gfx::PSCanvas::screenFromWorld(
  const Gfx::Rect<double>& /*world_pos*/) const
{
DOTRACE("Gfx::PSCanvas::screenFromWorld(Gfx::Rect)");
// FIXME
  itsImpl->raiseError("not implemented");
  return Gfx::Rect<int>();
}

Gfx::Rect<double> Gfx::PSCanvas::worldFromScreen(
  const Gfx::Rect<int>& /*screen_pos*/) const
{
DOTRACE("Gfx::PSCanvas::worldFromScreen(Gfx::Rect)");
// FIXME
  itsImpl->raiseError("not implemented");
  return Gfx::Rect<double>();
}

Gfx::Rect<int> Gfx::PSCanvas::getScreenViewport() const
{
DOTRACE("Gfx::PSCanvas::getScreenViewport()");
// FIXME
  itsImpl->raiseError("not implemented");
  return Gfx::Rect<int>();
}

Gfx::Rect<double> Gfx::PSCanvas::getWorldViewport() const
{
DOTRACE("Gfx::PSCanvas::getWorldViewport()");
// FIXME
  itsImpl->raiseError("not implemented");
  return Gfx::Rect<double>();
}


bool Gfx::PSCanvas::isRgba() const
{
DOTRACE("Gfx::PSCanvas::isRgba");
  return true;
}

bool Gfx::PSCanvas::isColorIndex() const
{
DOTRACE("Gfx::PSCanvas::isColorIndex");
  return false;
}

bool Gfx::PSCanvas::isDoubleBuffered() const
{
DOTRACE("Gfx::PSCanvas::isDoubleBuffered");
  return false;
}

unsigned int Gfx::PSCanvas::bitsPerPixel() const
{
DOTRACE("Gfx::PSCanvas::bitsPerPixel");
  return 24;
}

void Gfx::PSCanvas::throwIfError(const char* where) const
{
DOTRACE("Gfx::PSCanvas::throwIfError");
  if (itsImpl->itsFstream.fail())
    itsImpl->raiseError(where);
}


void Gfx::PSCanvas::pushAttribs()
{
DOTRACE("Gfx::PSCanvas::pushAttribs");
  itsImpl->gsave();
}

void Gfx::PSCanvas::popAttribs()
{
DOTRACE("Gfx::PSCanvas::popAttribs");
  itsImpl->grestore();
}

void Gfx::PSCanvas::drawOnFrontBuffer()
{
DOTRACE("Gfx::PSCanvas::drawOnFrontBuffer");
  ;// nothing
}

void Gfx::PSCanvas::drawOnBackBuffer()
{
DOTRACE("Gfx::PSCanvas::drawOnBackBuffer");
  ;// nothing
}

void Gfx::PSCanvas::setColor(const Gfx::RgbaColor& col)
{
DOTRACE("Gfx::PSCanvas::setColor");
  itsImpl->setrgbcolor(col);
}

void Gfx::PSCanvas::setClearColor(const Gfx::RgbaColor&)
{
DOTRACE("Gfx::PSCanvas::setClearColor");
  ;//nothing
}

void Gfx::PSCanvas::setColorIndex(unsigned int /*index*/)
{
DOTRACE("Gfx::PSCanvas::setColorIndex");
// FIXME
}

void Gfx::PSCanvas::setClearColorIndex(unsigned int /*index*/)
{
DOTRACE("Gfx::PSCanvas::setClearColorIndex");
// FIXME
}

void Gfx::PSCanvas::swapForeBack()
{
DOTRACE("Gfx::PSCanvas::swapForeBack");
// FIXME
}

void Gfx::PSCanvas::setPolygonFill(bool on)
{
DOTRACE("Gfx::PSCanvas::setPolygonFill");

  itsImpl->itsPolygonFill = on;
}

void Gfx::PSCanvas::setPointSize(double /*size*/)
{
DOTRACE("Gfx::PSCanvas::setPointSize");
// FIXME
}

void Gfx::PSCanvas::setLineWidth(double width)
{
DOTRACE("Gfx::PSCanvas::setLineWidth");

  itsImpl->setlinewidth(width);
}

void Gfx::PSCanvas::setLineStipple(unsigned short bit_pattern)
{
DOTRACE("Gfx::PSCanvas::setLineStipple");

  itsImpl->setdash(bit_pattern);
}

void Gfx::PSCanvas::enableAntialiasing()
{
DOTRACE("Gfx::PSCanvas::enableAntialiasing");
  // nothing, antialiasing is default in postscript
}



void Gfx::PSCanvas::viewport(int /*x*/, int /*y*/, int /*w*/, int /*h*/)
{
DOTRACE("Gfx::PSCanvas::viewport");
// FIXME
}

void Gfx::PSCanvas::orthographic(const Gfx::Rect<double>& /*bounds*/,
                                 double /*zNear*/, double /*zFar*/)
{
DOTRACE("Gfx::PSCanvas::orthographic");
// FIXME
}

void Gfx::PSCanvas::perspective(double /*fovy*/, double /*aspect*/,
                                double /*zNear*/, double /*zFar*/)
{
DOTRACE("Gfx::PSCanvas::perspective");
// FIXME
}


void Gfx::PSCanvas::pushMatrix()
{
DOTRACE("Gfx::PSCanvas::pushMatrix");
  itsImpl->gsave();
}

void Gfx::PSCanvas::popMatrix()
{
DOTRACE("Gfx::PSCanvas::popMatrix");
  itsImpl->grestore();
}

void Gfx::PSCanvas::translate(const Gfx::Vec3<double>& v)
{
DOTRACE("Gfx::PSCanvas::translate");
  itsImpl->translate(v);
}

void Gfx::PSCanvas::scale(const Gfx::Vec3<double>& v)
{
DOTRACE("Gfx::PSCanvas::scale");
  itsImpl->scale(v);
}

void Gfx::PSCanvas::rotate(const Gfx::Vec3<double>&, double angle_in_degrees)
{
DOTRACE("Gfx::PSCanvas::rotate");
  itsImpl->rotate(angle_in_degrees);
}

void Gfx::PSCanvas::transform(const Gfx::Txform& /*tx*/)
{
DOTRACE("Gfx::PSCanvas::transform");
  itsImpl->raiseError("transform() operation not supported");
}


void Gfx::PSCanvas::drawPixels(const Gfx::BmapData& /*data*/,
                               const Gfx::Vec2<double>& /*world_pos*/,
                               const Gfx::Vec2<double>& /*zoom*/)
{
DOTRACE("Gfx::PSCanvas::drawPixels");
// FIXME
}

void Gfx::PSCanvas::drawBitmap(const Gfx::BmapData& /*data*/,
                               const Gfx::Vec2<double>& /*world_pos*/)
{
DOTRACE("Gfx::PSCanvas::drawBitmap");
// FIXME
}

void Gfx::PSCanvas::grabPixels(const Gfx::Rect<int>&, Gfx::BmapData&)
{
DOTRACE("Gfx::PSCanvas::grabPixels");
  itsImpl->raiseError("grabPixels not possible");
}

void Gfx::PSCanvas::clearColorBuffer()
{
DOTRACE("Gfx::PSCanvas::clearColorBuffer");
// FIXME
}

void Gfx::PSCanvas::clearColorBuffer(const Gfx::Rect<int>& /*screen_rect*/)
{
DOTRACE("Gfx::PSCanvas::clearColorBuffer(Gfx::Rect)");
// FIXME
}

void Gfx::PSCanvas::drawRect(const Gfx::Rect<double>& rect)
{
DOTRACE("Gfx::PSCanvas::drawRect");

  itsImpl->newpath();
  itsImpl->moveto(rect.bottomLeft());
  itsImpl->lineto(rect.bottomRight());
  itsImpl->lineto(rect.topRight());
  itsImpl->lineto(rect.topLeft());
  itsImpl->closepath();
  itsImpl->stroke();
}

void Gfx::PSCanvas::drawCircle(double inner_radius, double outer_radius,
                               bool fill, unsigned int, unsigned int)
{
DOTRACE("Gfx::PSCanvas::drawCircle");

  itsImpl->newpath();
  itsImpl->arc(0.0, 0.0, outer_radius, 0.0, 360.0);
  if (fill)
    {
      itsImpl->arcn(0.0, 0.0, inner_radius, 360.0, 0.0);
      itsImpl->fill();
    }
  else
    {
      itsImpl->stroke();
    }
}

void Gfx::PSCanvas::drawCylinder(double /*base_radius*/, double /*top_radius*/,
                                 double /*height*/, int /*slices*/, int /*stacks*/,
                                 bool /*fill*/)
{
  // FIXME
}

void Gfx::PSCanvas::drawSphere(double /*radius*/, int /*slices*/, int /*stacks*/,
                               bool /*fill*/)
{
  // FIXME
}

void Gfx::PSCanvas::drawBezier4(const Gfx::Vec3<double>& p1,
                                const Gfx::Vec3<double>& p2,
                                const Gfx::Vec3<double>& p3,
                                const Gfx::Vec3<double>& p4,
                                unsigned int /*subdivisions*/)
{
DOTRACE("Gfx::PSCanvas::drawBezier4");
  itsImpl->bezier(p1, p2, p3, p4);
}

void Gfx::PSCanvas::drawBezierFill4(const Gfx::Vec3<double>& /*center*/,
                                    const Gfx::Vec3<double>& /*p1*/,
                                    const Gfx::Vec3<double>& /*p2*/,
                                    const Gfx::Vec3<double>& /*p3*/,
                                    const Gfx::Vec3<double>& /*p4*/,
                                    unsigned int /*subdivisions*/)
{
DOTRACE("Gfx::PSCanvas::drawBezierFill4");
  // FIXME
}

void Gfx::PSCanvas::beginPoints()
{
DOTRACE("Gfx::PSCanvas::beginPoints");
  itsImpl->beginPrimitive(&thePointsPrim);
}

void Gfx::PSCanvas::beginLines()
{
DOTRACE("Gfx::PSCanvas::beginLines");
  itsImpl->beginPrimitive(&theLinesPrim);
}

void Gfx::PSCanvas::beginLineStrip()
{
DOTRACE("Gfx::PSCanvas::beginLineStrip");
  itsImpl->beginPrimitive(&theLineStripPrim);
}

void Gfx::PSCanvas::beginLineLoop()
{
DOTRACE("Gfx::PSCanvas::beginLineLoop");
  itsImpl->beginPrimitive(&theLineLoopPrim);
}

void Gfx::PSCanvas::beginTriangles()
{
DOTRACE("Gfx::PSCanvas::beginTriangles");
  itsImpl->beginPrimitive(&theTrianglesPrim);
}

void Gfx::PSCanvas::beginTriangleStrip()
{
DOTRACE("Gfx::PSCanvas::beginTriangleStrip");
  itsImpl->beginPrimitive(&theTriangleStripPrim);
}

void Gfx::PSCanvas::beginTriangleFan()
{
DOTRACE("Gfx::PSCanvas::beginTriangleFan");
  itsImpl->beginPrimitive(&theTriangleFanPrim);
}

void Gfx::PSCanvas::beginQuads()
{
DOTRACE("Gfx::PSCanvas::beginQuads");
  itsImpl->beginPrimitive(&theQuadsPrim);
}

void Gfx::PSCanvas::beginQuadStrip()
{
DOTRACE("Gfx::PSCanvas::beginQuadStrip");
  itsImpl->beginPrimitive(&theQuadStripPrim);
}

void Gfx::PSCanvas::beginPolygon()
{
DOTRACE("Gfx::PSCanvas::beginPolygon");
  itsImpl->beginPrimitive(&thePolygonPrim);
}

void Gfx::PSCanvas::vertex2(const Gfx::Vec2<double>& v)
{
DOTRACE("Gfx::PSCanvas::vertex2");

  if (itsImpl->itsPrimPtr == 0)
    {
      itsImpl->raiseError("called vertex() outside graphics primitive");
    }

  itsImpl->itsPrimPtr->onVertex(itsImpl, v);
  ++(itsImpl->itsVcount);
}

void Gfx::PSCanvas::vertex3(const Gfx::Vec3<double>& v)
{
DOTRACE("Gfx::PSCanvas::vertex3");
  vertex2(Gfx::Vec2<double>(v.x(), v.y()));
}

void Gfx::PSCanvas::end()
{
DOTRACE("Gfx::PSCanvas::end");
  itsImpl->endPrimitive();
}

void Gfx::PSCanvas::flushOutput()
{
DOTRACE("Gfx::PSCanvas::flushOutput");
  itsImpl->itsFstream << std::flush;
}

static const char vcid_pscanvas_cc[] = "$Header$";
#endif // !PSCANVAS_CC_DEFINED
