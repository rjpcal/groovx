///////////////////////////////////////////////////////////////////////
//
// pscanvas.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Aug 27 17:20:09 2001
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

#ifndef PSCANVAS_CC_DEFINED
#define PSCANVAS_CC_DEFINED

#include "gfx/pscanvas.h"

#include "gx/bmapdata.h"
#include "gx/rect.h"
#include "gx/rgbacolor.h"
#include "gx/txform.h"
#include "gx/vec2.h"
#include "gx/vec3.h"

#include "util/dlink_list.h"
#include "util/error.h"

#include <fstream>
#include <vector>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

using namespace Gfx;

class Gfx::PSCanvas::Impl
{
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:

  struct Primitive;

  struct State
  {
    State() :
      txform(),
      lineWidth(1.0),
      polygonFill(false)
    {}

    Txform txform;
    double lineWidth; // in points
    bool polygonFill;
  };

  std::ofstream        itsFstream;
  std::vector<State>   itsStates;
  Primitive*           itsPrimPtr;

  Impl(const char* filename) :
    itsFstream(filename),
    itsStates(),
    itsPrimPtr(0)
  {
    if (!itsFstream.is_open())
      raiseError(fstring("couldn't open '", filename, "' for writing"));

    itsStates.push_back(State());

    itsFstream << "%!\n";
    itsFstream << "%% generated by Gfx::PSCanvas $Revision$\n";
    translate(Vec3d(306.0, 360.0, 0.0));
    scale(Vec3d(72.0, 72.0, 1.0));
  }

  ~Impl()
  {
    itsFstream << "showpage\n";
  }

  State& state()
  {
    Assert(itsStates.size() > 0);
    return itsStates.back();
  }

  const State& state() const
  {
    Assert(itsStates.size() > 0);
    return itsStates.back();
  }

  //
  // Graphics primitive definitions
  //

  struct Primitive
  {
    typedef Gfx::PSCanvas::Impl PS;

    virtual ~Primitive() {}

    virtual void begin(PS* ps)
    {
      itsVcount = 0;
      onBegin(ps);
    }

    virtual void vertex(PS* ps, const Vec2d& v)
    {
      onVertex(ps, v);
      ++itsVcount;
    }

    virtual void end(PS* ps)
    {
      onEnd(ps);
    }

    unsigned int vcount() const { return itsVcount; }

  private:
    unsigned int itsVcount;

    virtual void onBegin(PS* ps) = 0;
    virtual void onVertex(PS* ps, const Vec2d& v) = 0;
    virtual void onEnd(PS* ps) = 0;
  };

  struct PointsPrim : public Primitive
  {
    virtual void onBegin(PS*) {}

    virtual void onVertex(PS* ps, const Vec2d& v)
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
    virtual void onVertex(PS* ps, const Vec2d& v)
    {
      if (vcount() % 2)
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
    virtual void onVertex(PS* ps, const Vec2d& v)
    {
      if (vcount() == 0)
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
    virtual void onVertex(PS* ps, const Vec2d& v)
    {
      if (vcount() == 0)
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
    virtual void onVertex(PS* ps, const Vec2d& v)
    {
      switch(vcount() % 3)
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
    virtual void onVertex(PS*, const Vec2d&) { /* FIXME */ }
    virtual void onEnd(PS*) { /* FIXME */ }
  };

  struct TriangleFanPrim : public Primitive
  {
    virtual void onBegin(PS*) { /* FIXME */ }
    virtual void onVertex(PS*, const Vec2d&) { /* FIXME */ }
    virtual void onEnd(PS*) { /* FIXME */ }
  };

  struct QuadsPrim : public Primitive
  {
    virtual void onBegin(PS*) {}

    virtual void onVertex(PS* ps, const Vec2d& v)
    {
      switch(vcount() % 3)
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
    dlink_list<Vec2d> itsPts1;
    dlink_list<Vec2d> itsPts2;

    QuadStripPrim() : itsPts1(), itsPts2() {}

    virtual void onBegin(PS*)
    {
      itsPts1.clear();
      itsPts2.clear();
    }

    virtual void onVertex(PS*, const Vec2d& v)
    {
      if ((vcount() % 2) == 0)
        itsPts1.push_back(v);
      else
        itsPts2.push_front(v);
    }

    virtual void onEnd(PS* ps)
    {
      ps->newpath();

      ps->moveto(itsPts1.front());

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
    for (unsigned int i = 1; i < itsStates.size(); ++i)
      {
        itsFstream << '\t';
      }
  }

  void gsave()
  {
    indent(); itsFstream << "gsave\n";
    itsStates.push_back(state());
  }

  void grestore()
  {
    itsStates.pop_back();
    indent(); itsFstream << "grestore\n";
  }

  void translate(const Vec3d& v)
  {
    state().txform.translate(v);
  }

  void scale(const Vec3d& v)
  {
    state().txform.scale(v);
  }

  void rotate(const Vec3d& axis, double angle)
  {
    state().txform.rotate(axis, angle);
  }

  // NOTE: these next four functions must be non-templates in order to avoid
  // "internal compiler errors" with g++-2.96 on Mac OS X.

  void moveto(const Vec2d& v)
  {
    indent(); pushxy(v); itsFstream << "moveto\n";
  }

  void moveto(const Vec3d& v)
  {
    indent(); pushxy(v); itsFstream << "moveto\n";
  }

  void lineto(const Vec2d& v)
  {
    indent(); pushxy(v); itsFstream << "lineto\n";
  }

  void lineto(const Vec3d& v)
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
    if (state().polygonFill)
      fill();
    else
      stroke();
  }

#if 0
  // WARNING! The arc() and arcn() functions won't work properly with
  // maintaining our own 3-D coordinate transformation.
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
#endif

  void circle(double x, double y, double r, bool reverse=false)
  {
    // A good cubic Bezier approximation for a radius-r 90 degree arc is:
    //
    //   (r,0) (r,r*a) (r*a,r) (0,r)
    //
    // where a = 4/3*(sqrt(2)-1).

    const double a = 4.0/3.0 * (sqrt(2.0)-1.0);

    const Vec2d pt1 (x+r   , y);      // right
    const Vec2d pt2 (x+r   , y+r*a);
    const Vec2d pt3 (x+r*a , y+r);
    const Vec2d pt4 (x     , y+r);    // top
    const Vec2d pt5 (x-r*a , y+r);
    const Vec2d pt6 (x-r   , y+r*a);
    const Vec2d pt7 (x-r   , y);      // left
    const Vec2d pt8 (x-r   , y-r*a);
    const Vec2d pt9 (x-r*a , y-r);
    const Vec2d pt10(x     , y-r);    // bottom
    const Vec2d pt11(x+r*a , y-r);
    const Vec2d pt12(x+r   , y-r*a);

    if (!reverse)
      {
        moveto(pt1);
        indent(); pushxy(pt2); pushxy(pt3); pushxy(pt4); itsFstream << "curveto\n";
        indent(); pushxy(pt5); pushxy(pt6); pushxy(pt7); itsFstream << "curveto\n";
        indent(); pushxy(pt8); pushxy(pt9); pushxy(pt10); itsFstream << "curveto\n";
        indent(); pushxy(pt11); pushxy(pt12); pushxy(pt1); itsFstream << "curveto\n";
      }
    else
      {
        moveto(pt1);
        indent(); pushxy(pt12); pushxy(pt11); pushxy(pt10); itsFstream << "curveto\n";
        indent(); pushxy(pt9); pushxy(pt8); pushxy(pt7); itsFstream << "curveto\n";
        indent(); pushxy(pt6); pushxy(pt5); pushxy(pt4); itsFstream << "curveto\n";
        indent(); pushxy(pt3); pushxy(pt2); pushxy(pt1); itsFstream << "curveto\n";
      }
  }

  void bezier(const Vec3d& p1,
              const Vec3d& p2,
              const Vec3d& p3,
              const Vec3d& p4)
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

  void setlinewidth(double w)
  {
    state().lineWidth = w;

    indent(); push1(w); itsFstream << "setlinewidth\n";
  }

  void setdash(unsigned short bit_pattern)
  {
    dbgEvalNL(3, (void*) int(bit_pattern));

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

    indent();

    push1("[");
    for (int i = 0; i < pos; ++i)
      {
        push1(lengths[i]);
      }
    push1("]");

    push1(offset); itsFstream << "setdash\n";
  }

  void setrgbcolor(const RgbaColor& col)
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

  void pushxy(const Vec2d& v)
  {
    Vec2d t = state().txform.applyTo(v);
    itsFstream << t.x() << " " << t.y() << " ";
  }

  void pushxy(const Vec3d& v)
  {
    Vec3d t = state().txform.applyTo(v);
    itsFstream << t.x() << " " << t.y() << " ";
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
    itsPrimPtr->begin(this);
  }

  void endPrimitive()
  {
    if (itsPrimPtr == 0)
      {
        raiseError("called endPrimitive outside graphics primitive");
      }

    itsPrimPtr->end(this);
    itsPrimPtr = 0;
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
  rep(new Impl(filename))
{
DOTRACE("Gfx::PSCanvas::PSCanvas");
}

Gfx::PSCanvas::~PSCanvas() throw()
{
DOTRACE("Gfx::PSCanvas::~PSCanvas");
  delete rep;
}

Vec2i Gfx::PSCanvas::screenFromWorld(
  const Vec2d& /*world_pos*/
  ) const
{
DOTRACE("Gfx::PSCanvas::screenFromWorld(Vec2d)");
// FIXME
  rep->raiseError("not implemented");
  return Vec2i();
}

Vec2d Gfx::PSCanvas::worldFromScreen(
  const Vec2i& /*screen_pos*/
  ) const
{
DOTRACE("Gfx::PSCanvas::worldFromScreen(Vec2i)");
// FIXME
  rep->raiseError("not implemented");
  return Vec2d();
}


Rect<int> Gfx::PSCanvas::screenFromWorld(
  const Rect<double>& /*world_pos*/) const
{
DOTRACE("Gfx::PSCanvas::screenFromWorld(Rect)");
// FIXME
  rep->raiseError("not implemented");
  return Rect<int>();
}

Rect<double> Gfx::PSCanvas::worldFromScreen(
  const Rect<int>& /*screen_pos*/) const
{
DOTRACE("Gfx::PSCanvas::worldFromScreen(Rect)");
// FIXME
  rep->raiseError("not implemented");
  return Rect<double>();
}

Rect<int> Gfx::PSCanvas::getScreenViewport() const
{
DOTRACE("Gfx::PSCanvas::getScreenViewport()");
// FIXME
  rep->raiseError("not implemented");
  return Rect<int>();
}

Rect<double> Gfx::PSCanvas::getWorldViewport() const
{
DOTRACE("Gfx::PSCanvas::getWorldViewport()");
// FIXME
  rep->raiseError("not implemented");
  return Rect<double>();
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
  if (rep->itsFstream.fail())
    rep->raiseError(where);
}


void Gfx::PSCanvas::pushAttribs()
{
DOTRACE("Gfx::PSCanvas::pushAttribs");
  rep->gsave();
}

void Gfx::PSCanvas::popAttribs()
{
DOTRACE("Gfx::PSCanvas::popAttribs");
  rep->grestore();
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

void Gfx::PSCanvas::setColor(const RgbaColor& col)
{
DOTRACE("Gfx::PSCanvas::setColor");
  rep->setrgbcolor(col);
}

void Gfx::PSCanvas::setClearColor(const RgbaColor&)
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

  rep->state().polygonFill = on;
}

void Gfx::PSCanvas::setPointSize(double /*size*/)
{
DOTRACE("Gfx::PSCanvas::setPointSize");
// FIXME
}

void Gfx::PSCanvas::setLineWidth(double width)
{
DOTRACE("Gfx::PSCanvas::setLineWidth");

  rep->setlinewidth(width);
}

void Gfx::PSCanvas::setLineStipple(unsigned short bit_pattern)
{
DOTRACE("Gfx::PSCanvas::setLineStipple");

  rep->setdash(bit_pattern);
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

void Gfx::PSCanvas::orthographic(const Rect<double>& /*bounds*/,
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
  rep->gsave();
}

void Gfx::PSCanvas::popMatrix()
{
DOTRACE("Gfx::PSCanvas::popMatrix");
  rep->grestore();
}

void Gfx::PSCanvas::translate(const Vec3d& v)
{
DOTRACE("Gfx::PSCanvas::translate");
  rep->translate(v);
}

void Gfx::PSCanvas::scale(const Vec3d& v)
{
DOTRACE("Gfx::PSCanvas::scale");
  rep->scale(v);
}

void Gfx::PSCanvas::rotate(const Vec3d& axis, double angle_in_degrees)
{
DOTRACE("Gfx::PSCanvas::rotate");
  rep->rotate(axis, angle_in_degrees);
}

void Gfx::PSCanvas::transform(const Txform& tx)
{
DOTRACE("Gfx::PSCanvas::transform");
  rep->state().txform.transform(tx);
}


void Gfx::PSCanvas::drawPixels(const BmapData& /*data*/,
                               const Vec2d& /*world_pos*/,
                               const Vec2d& /*zoom*/)
{
DOTRACE("Gfx::PSCanvas::drawPixels");
// FIXME
}

void Gfx::PSCanvas::drawBitmap(const BmapData& /*data*/,
                               const Vec2d& /*world_pos*/)
{
DOTRACE("Gfx::PSCanvas::drawBitmap");
// FIXME
}

void Gfx::PSCanvas::grabPixels(const Rect<int>&, BmapData&)
{
DOTRACE("Gfx::PSCanvas::grabPixels");
  rep->raiseError("grabPixels not possible");
}

void Gfx::PSCanvas::clearColorBuffer()
{
DOTRACE("Gfx::PSCanvas::clearColorBuffer");
// FIXME
}

void Gfx::PSCanvas::clearColorBuffer(const Rect<int>& /*screen_rect*/)
{
DOTRACE("Gfx::PSCanvas::clearColorBuffer(Rect)");
// FIXME
}

void Gfx::PSCanvas::drawRect(const Rect<double>& rect)
{
DOTRACE("Gfx::PSCanvas::drawRect");

  rep->newpath();
  rep->moveto(rect.bottomLeft());
  rep->lineto(rect.bottomRight());
  rep->lineto(rect.topRight());
  rep->lineto(rect.topLeft());
  rep->closepath();
  rep->stroke();
}

void Gfx::PSCanvas::drawCircle(double inner_radius, double outer_radius,
                               bool fill, unsigned int, unsigned int)
{
DOTRACE("Gfx::PSCanvas::drawCircle");

  rep->newpath();
  rep->circle(0.0, 0.0, outer_radius);
  if (fill)
    {
      if (inner_radius > 0)
        rep->circle(0.0, 0.0, inner_radius, true);
      rep->fill();
    }
  else
    {
      rep->stroke();
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

void Gfx::PSCanvas::drawBezier4(const Vec3d& p1,
                                const Vec3d& p2,
                                const Vec3d& p3,
                                const Vec3d& p4,
                                unsigned int /*subdivisions*/)
{
DOTRACE("Gfx::PSCanvas::drawBezier4");
  rep->bezier(p1, p2, p3, p4);
}

void Gfx::PSCanvas::drawBezierFill4(const Vec3d& /*center*/,
                                    const Vec3d& /*p1*/,
                                    const Vec3d& /*p2*/,
                                    const Vec3d& /*p3*/,
                                    const Vec3d& /*p4*/,
                                    unsigned int /*subdivisions*/)
{
DOTRACE("Gfx::PSCanvas::drawBezierFill4");
  // FIXME
}

void Gfx::PSCanvas::beginPoints()
{
DOTRACE("Gfx::PSCanvas::beginPoints");
  rep->beginPrimitive(&thePointsPrim);
}

void Gfx::PSCanvas::beginLines()
{
DOTRACE("Gfx::PSCanvas::beginLines");
  rep->beginPrimitive(&theLinesPrim);
}

void Gfx::PSCanvas::beginLineStrip()
{
DOTRACE("Gfx::PSCanvas::beginLineStrip");
  rep->beginPrimitive(&theLineStripPrim);
}

void Gfx::PSCanvas::beginLineLoop()
{
DOTRACE("Gfx::PSCanvas::beginLineLoop");
  rep->beginPrimitive(&theLineLoopPrim);
}

void Gfx::PSCanvas::beginTriangles()
{
DOTRACE("Gfx::PSCanvas::beginTriangles");
  rep->beginPrimitive(&theTrianglesPrim);
}

void Gfx::PSCanvas::beginTriangleStrip()
{
DOTRACE("Gfx::PSCanvas::beginTriangleStrip");
  rep->beginPrimitive(&theTriangleStripPrim);
}

void Gfx::PSCanvas::beginTriangleFan()
{
DOTRACE("Gfx::PSCanvas::beginTriangleFan");
  rep->beginPrimitive(&theTriangleFanPrim);
}

void Gfx::PSCanvas::beginQuads()
{
DOTRACE("Gfx::PSCanvas::beginQuads");
  rep->beginPrimitive(&theQuadsPrim);
}

void Gfx::PSCanvas::beginQuadStrip()
{
DOTRACE("Gfx::PSCanvas::beginQuadStrip");
  rep->beginPrimitive(&theQuadStripPrim);
}

void Gfx::PSCanvas::beginPolygon()
{
DOTRACE("Gfx::PSCanvas::beginPolygon");
  rep->beginPrimitive(&thePolygonPrim);
}

void Gfx::PSCanvas::vertex2(const Vec2d& v)
{
DOTRACE("Gfx::PSCanvas::vertex2");

  if (rep->itsPrimPtr == 0)
    {
      rep->raiseError("called vertex() outside graphics primitive");
    }

  rep->itsPrimPtr->vertex(rep, v);
}

void Gfx::PSCanvas::vertex3(const Vec3d& v)
{
DOTRACE("Gfx::PSCanvas::vertex3");
  vertex2(Vec2d(v.x(), v.y()));
}

void Gfx::PSCanvas::end()
{
DOTRACE("Gfx::PSCanvas::end");
  rep->endPrimitive();
}

void Gfx::PSCanvas::drawRasterText(const fstring& /*text*/,
                                   const GxRasterFont& /*font*/)
{
DOTRACE("Gfx::PSCanvas::drawRasterText");
  throw Util::Error("PSCanvas::drawRasterText not implemented");
}

void Gfx::PSCanvas::drawVectorText(const fstring& /*text*/,
                                   const GxVectorFont& /*font*/)
{
DOTRACE("Gfx::PSCanvas::drawVectorText");
  throw Util::Error("PSCanvas::drawVectorText not implemented");
}

void Gfx::PSCanvas::flushOutput()
{
DOTRACE("Gfx::PSCanvas::flushOutput");
  rep->itsFstream << std::flush;
}

static const char vcid_pscanvas_cc[] = "$Header$";
#endif // !PSCANVAS_CC_DEFINED
