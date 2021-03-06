/** @file gfx/pscanvas.cc Gfx::Canvas subclass that writes to a
    PostScript file */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Aug 27 17:20:09 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "pscanvas.h"

#include "geom/rect.h"
#include "geom/span.h"
#include "geom/txform.h"
#include "geom/vec2.h"
#include "geom/vec3.h"

#include "gfx/rgbacolor.h"

#include "media/bmapdata.h"

#include "rutz/error.h"
#include "rutz/sfmt.h"
#include "rutz/time.h"
#include "rutz/timeformat.h"

#include <cmath> // for floor() and ceil()
#include <cstdlib> // for getenv()
#include <fstream>
#include <list>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using geom::recti;
using geom::rectd;
using geom::vec2i;
using geom::vec2d;
using geom::vec3i;
using geom::vec3d;

class Gfx::PSCanvas::Impl
{
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:

  struct Primitive;

  struct State
  {
    State() :
      txform(geom::txform::identity()),
      lineWidth(1.0),
      polygonFill(false)
    {}

    geom::txform txform;
    double lineWidth; // in points
    bool polygonFill;
  };

  std::ofstream        itsFstream;
  std::vector<State>   itsStates;
  Primitive*           itsPrimPtr;
  bool                 itsSpanFirst;
  geom::span<double>   itsSpanX;
  geom::span<double>   itsSpanY;

  Impl(const char* filename) :
    itsFstream(filename),
    itsStates(),
    itsPrimPtr(nullptr),
    itsSpanFirst(true),
    itsSpanX(),
    itsSpanY()
  {
    if (!itsFstream.is_open())
      raiseError(rutz::sfmt("couldn't open '%s' for writing", filename),
                 SRC_POS);

    itsStates.push_back(State());

    const rutz::fstring timestamp =
      rutz::format_time(rutz::time::wall_clock_now());

    itsFstream << "%!PS-Adobe-2.0 EPSF-1.2\n"
               << "%%Title: " << filename << "\n"
               << "%%Creator: Gfx::PSCanvas $Revision$\n"
               << "%%CreationDate: " << timestamp << "\n";

    const char* username = getenv("USER");

    if (username != nullptr)
      itsFstream << "%%For: " << username << "\n";

    itsFstream << "%%Pages: 1\n"
               << "%%DocumentFonts:\n"
               << "%%BoundingBox: (atend)\n"
               << "%%EndComments\n"
               << "\n"
               << "%%Page: 1 1\n"
               << "%%BeginDocument: " << filename << "\n"
               << "%%DocumentFonts:\n"
               << "\n";
    translate(vec3d(306.0, 360.0, 0.0));
    scale(vec3d(72.0, 72.0, 1.0));
  }

  ~Impl()
  {
    itsFstream << "showpage\n"
               << "\n"
               << "%%EndDocument\n"
               << "\n"
               << "%%Trailer\n"
               << "%%BoundingBox: "
               << floor(itsSpanX.lo) << ' ' << floor(itsSpanY.lo) << ' '
               << ceil(itsSpanX.hi) << ' ' << ceil(itsSpanY.hi) << '\n'
               << "%%EOF\n"
               << std::flush;
  }

  State& current_state()
  {
    GVX_ASSERT(itsStates.size() > 0);
    return itsStates.back();
  }

  const State& current_state() const
  {
    GVX_ASSERT(itsStates.size() > 0);
    return itsStates.back();
  }

  //
  // Graphics primitive definitions
  //

  struct Primitive
  {
    typedef Gfx::PSCanvas::Impl PS;

    virtual ~Primitive() {}

    void begin(PS* ps)
    {
      itsVcount = 0;
      onBegin(ps);
    }

    void vertex(PS* ps, const vec3d& v)
    {
      onVertex(ps, v);
      ++itsVcount;
    }

    void end(PS* ps)
    {
      onEnd(ps);
    }

    unsigned int vcount() const { return itsVcount; }

  private:
    unsigned int itsVcount;

    virtual void onBegin(PS* ps) = 0;
    virtual void onVertex(PS* ps, const vec3d& v) = 0;
    virtual void onEnd(PS* ps) = 0;
  };

  struct PointsPrim : public Primitive
  {
    virtual void onBegin(PS*) override {}

    virtual void onVertex(PS* ps, const vec3d& v) override
    {
      ps->newpath(); ps->moveto(v); ps->stroke();
    }

    virtual void onEnd(PS*) override {}
  };

  struct LinesPrim : public Primitive
  {
    virtual void onBegin(PS* ps) override
    {
      ps->newpath();
    }
    virtual void onVertex(PS* ps, const vec3d& v) override
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
    virtual void onEnd(PS*) override {}
  };

  struct LineStripPrim : public Primitive
  {
    virtual void onBegin(PS* ps) override
    {
      ps->newpath();
    }
    virtual void onVertex(PS* ps, const vec3d& v) override
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
    virtual void onEnd(PS* ps) override
    {
      ps->stroke();
    }
  };

  struct LineLoopPrim : public Primitive
  {
    virtual void onBegin(PS*) override {}
    virtual void onVertex(PS* ps, const vec3d& v) override
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
    virtual void onEnd(PS* ps) override
    {
      ps->closepath(); ps->stroke();
    }
  };

  struct TrianglesPrim : public Primitive
  {
    virtual void onBegin(PS* ps) override
    {
      ps->newpath();
    }
    virtual void onVertex(PS* ps, const vec3d& v) override
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
    virtual void onEnd(PS*) override {}
  };

  struct TriangleStripPrim : public Primitive
  {
    virtual void onBegin(PS*) override { /* FIXME */ }
    virtual void onVertex(PS*, const vec3d&) override { /* FIXME */ }
    virtual void onEnd(PS*) override { /* FIXME */ }
  };

  struct TriangleFanPrim : public Primitive
  {
    virtual void onBegin(PS*) override { /* FIXME */ }
    virtual void onVertex(PS*, const vec3d&) override { /* FIXME */ }
    virtual void onEnd(PS*) override { /* FIXME */ }
  };

  struct QuadsPrim : public Primitive
  {
    virtual void onBegin(PS*) override {}

    virtual void onVertex(PS* ps, const vec3d& v) override
    {
      switch(vcount() % 4)
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
    virtual void onEnd(PS*) override {}
  };

  struct QuadStripPrim : public Primitive
  {
    std::list<vec3d> itsEvenPts;
    std::list<vec3d> itsOddPts;

    QuadStripPrim() : itsEvenPts(), itsOddPts() {}

    virtual void onBegin(PS*) override
    {
      itsEvenPts.clear();
      itsOddPts.clear();
    }

    virtual void onVertex(PS*, const vec3d& v) override
    {
      if ((vcount() % 2) == 0)
        itsEvenPts.push_back(v);
      else
        itsOddPts.push_front(v);
    }

    virtual void onEnd(PS* ps) override
    {
      ps->newpath();

      ps->moveto(itsEvenPts.front());

      while (!itsEvenPts.empty())
        {
          ps->lineto(itsEvenPts.front());
          itsEvenPts.pop_front();
        }

      while (!itsOddPts.empty())
        {
          ps->lineto(itsOddPts.front());
          itsOddPts.pop_front();
        }

      ps->closepath();

      ps->renderpolygon();
    }
  };

  struct PolygonPrim : public LineLoopPrim
  {
    virtual void onEnd(PS* ps) override
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

  void gsave(const char* comment)
  {
    indent(); itsFstream << "gsave";
    if (comment != nullptr && comment[0] != '\0')
      itsFstream << " % " << comment;
    itsFstream << '\n';
    itsStates.push_back(current_state());
  }

  void grestore()
  {
    itsStates.pop_back();
    indent(); itsFstream << "grestore\n";
  }

  void translate(const vec3d& v)
  {
    current_state().txform.translate(v);
  }

  void scale(const vec3d& v)
  {
    current_state().txform.scale(v);
  }

  void rotate(const vec3d& axis, double angle)
  {
    current_state().txform.rotate(axis, angle);
  }

  // NOTE: these next four functions must be non-templates in order to avoid
  // "internal compiler errors" with g++-2.96 on Mac OS X.

  void moveto(const vec2d& v)
  {
    indent(); pushxy(v); itsFstream << "moveto\n";
  }

  void moveto(const vec3d& v)
  {
    indent(); pushxy(v); itsFstream << "moveto\n";
  }

  void lineto(const vec2d& v)
  {
    indent(); pushxy(v); itsFstream << "lineto\n";
  }

  void lineto(const vec3d& v)
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
    if (current_state().polygonFill)
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

    const vec3d pt1 (x+r   , y ,     0.0); // right
    const vec3d pt2 (x+r   , y+r*a , 0.0);
    const vec3d pt3 (x+r*a , y+r ,   0.0);
    const vec3d pt4 (x     , y+r ,   0.0); // top
    const vec3d pt5 (x-r*a , y+r ,   0.0);
    const vec3d pt6 (x-r   , y+r*a , 0.0);
    const vec3d pt7 (x-r   , y ,     0.0); // left
    const vec3d pt8 (x-r   , y-r*a , 0.0);
    const vec3d pt9 (x-r*a , y-r ,   0.0);
    const vec3d pt10(x     , y-r ,   0.0); // bottom
    const vec3d pt11(x+r*a , y-r ,   0.0);
    const vec3d pt12(x+r   , y-r*a , 0.0);

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

  void bezier(const vec3d& p1,
              const vec3d& p2,
              const vec3d& p3,
              const vec3d& p4)
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
    current_state().lineWidth = w;

    indent(); push1(w); itsFstream << "setlinewidth\n";
  }

  void setdash(unsigned short bit_pattern)
  {
    dbg_eval_nl(3, reinterpret_cast<void*>(int(bit_pattern)));

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

  void mergespan(const double x, const double y)
  {
    if (itsSpanFirst)
      {
        itsSpanX = geom::span<double>::from_any(x, x);
        itsSpanY = geom::span<double>::from_any(y, y);
        itsSpanFirst = false;
      }
    else
      {
        itsSpanX = itsSpanX.including(x);
        itsSpanY = itsSpanY.including(y);
      }
  }

  void pushxy(const vec2d& v)
  {
    vec2d t = current_state().txform.apply_to(v);
    this->mergespan(t.x(), t.y());
    itsFstream << t.x() << " " << t.y() << " ";
  }

  void pushxy(const vec3d& v)
  {
    vec3d t = current_state().txform.apply_to(v);
    this->mergespan(t.x(), t.y());
    itsFstream << t.x() << " " << t.y() << " ";
  }

  [[noreturn]] void raiseError(const rutz::fstring& msg, const rutz::file_pos& pos)
  {
    throw rutz::error(rutz::sfmt("PSCanvas error: %s", msg.c_str()), pos);
  }

  void beginPrimitive(Primitive* ptr, const char* comment)
  {
    if (itsPrimPtr != nullptr)
      raiseError("in beginPrimitive, already in graphics primitive",
                 SRC_POS);

    if (comment != nullptr && comment[0] != '\0')
      {
        indent(); itsFstream << "% " << comment << " (begin)\n";
      }

    itsPrimPtr = ptr;
    itsPrimPtr->begin(this);
  }

  void endPrimitive()
  {
    if (itsPrimPtr == nullptr)
      {
        raiseError("called endPrimitive outside graphics primitive",
                   SRC_POS);
      }

    itsPrimPtr->end(this);
    itsPrimPtr = nullptr;
  }
};

namespace
{
  Gfx::PSCanvas::Impl::PointsPrim        thePointsPrim;
  Gfx::PSCanvas::Impl::LinesPrim         theLinesPrim;
  Gfx::PSCanvas::Impl::LineStripPrim     theLineStripPrim;
  Gfx::PSCanvas::Impl::LineLoopPrim      theLineLoopPrim;
  Gfx::PSCanvas::Impl::TrianglesPrim     theTrianglesPrim;
  Gfx::PSCanvas::Impl::TriangleStripPrim theTriangleStripPrim;
  Gfx::PSCanvas::Impl::TriangleFanPrim   theTriangleFanPrim;
  Gfx::PSCanvas::Impl::QuadsPrim         theQuadsPrim;
  Gfx::PSCanvas::Impl::QuadStripPrim     theQuadStripPrim;
  Gfx::PSCanvas::Impl::PolygonPrim       thePolygonPrim;
}

Gfx::PSCanvas::PSCanvas(const char* filename) :
  rep(new Impl(filename))
{
GVX_TRACE("Gfx::PSCanvas::PSCanvas");
}

Gfx::PSCanvas::~PSCanvas() noexcept
{
GVX_TRACE("Gfx::PSCanvas::~PSCanvas");
  delete rep;
}

vec3d Gfx::PSCanvas::screenFromWorld3(const vec3d& /*world_pos*/) const
{
GVX_TRACE("Gfx::PSCanvas::screenFromWorld3");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

vec3d Gfx::PSCanvas::worldFromScreen3(const vec3d& /*screen_pos*/) const
{
GVX_TRACE("Gfx::PSCanvas::worldFromScreen3");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}


recti Gfx::PSCanvas::getScreenViewport() const
{
GVX_TRACE("Gfx::PSCanvas::getScreenViewport()");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}


bool Gfx::PSCanvas::isRgba() const
{
GVX_TRACE("Gfx::PSCanvas::isRgba");
  return true;
}

bool Gfx::PSCanvas::isColorIndex() const
{
GVX_TRACE("Gfx::PSCanvas::isColorIndex");
  return false;
}

bool Gfx::PSCanvas::isDoubleBuffered() const
{
GVX_TRACE("Gfx::PSCanvas::isDoubleBuffered");
  return false;
}

unsigned int Gfx::PSCanvas::bitsPerPixel() const
{
GVX_TRACE("Gfx::PSCanvas::bitsPerPixel");
  return 24;
}

void Gfx::PSCanvas::throwIfError(const char* where,
                                 const rutz::file_pos& pos) const
{
GVX_TRACE("Gfx::PSCanvas::throwIfError");
  if (rep->itsFstream.fail())
    rep->raiseError(where, pos);
}


void Gfx::PSCanvas::pushAttribs(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::pushAttribs");
  rep->gsave(comment);
}

void Gfx::PSCanvas::popAttribs()
{
GVX_TRACE("Gfx::PSCanvas::popAttribs");
  rep->grestore();
}

void Gfx::PSCanvas::drawOnFrontBuffer()
{
GVX_TRACE("Gfx::PSCanvas::drawOnFrontBuffer");
  ;// nothing
}

void Gfx::PSCanvas::drawOnBackBuffer()
{
GVX_TRACE("Gfx::PSCanvas::drawOnBackBuffer");
  ;// nothing
}

void Gfx::PSCanvas::setColor(const RgbaColor& col)
{
GVX_TRACE("Gfx::PSCanvas::setColor");
  rep->setrgbcolor(col);
}

void Gfx::PSCanvas::setClearColor(const RgbaColor&)
{
GVX_TRACE("Gfx::PSCanvas::setClearColor");
  ;//nothing
}

void Gfx::PSCanvas::setColorIndex(unsigned int /*index*/)
{
GVX_TRACE("Gfx::PSCanvas::setColorIndex");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::setClearColorIndex(unsigned int /*index*/)
{
GVX_TRACE("Gfx::PSCanvas::setClearColorIndex");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::swapForeBack()
{
GVX_TRACE("Gfx::PSCanvas::swapForeBack");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::setPolygonFill(bool on)
{
GVX_TRACE("Gfx::PSCanvas::setPolygonFill");

  rep->current_state().polygonFill = on;
}

void Gfx::PSCanvas::setPointSize(double size)
{
GVX_TRACE("Gfx::PSCanvas::setPointSize");
// FIXME
  if (size != 1.0)
    rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::setLineWidth(double width)
{
GVX_TRACE("Gfx::PSCanvas::setLineWidth");

  rep->setlinewidth(width);
}

void Gfx::PSCanvas::setLineStipple(unsigned short bit_pattern)
{
GVX_TRACE("Gfx::PSCanvas::setLineStipple");

  rep->setdash(bit_pattern);
}

void Gfx::PSCanvas::enableAntialiasing()
{
GVX_TRACE("Gfx::PSCanvas::enableAntialiasing");
  // nothing, antialiasing is default in postscript
}



void Gfx::PSCanvas::viewport(int /*x*/, int /*y*/, int /*w*/, int /*h*/)
{
GVX_TRACE("Gfx::PSCanvas::viewport");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::orthographic(const rectd& /*bounds*/,
                                 double /*zNear*/, double /*zFar*/)
{
GVX_TRACE("Gfx::PSCanvas::orthographic");
// FIXME
}

void Gfx::PSCanvas::perspective(double /*fovy*/, double /*aspect*/,
                                double /*zNear*/, double /*zFar*/)
{
GVX_TRACE("Gfx::PSCanvas::perspective");
// FIXME
}


void Gfx::PSCanvas::pushMatrix(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::pushMatrix");
  rep->gsave(comment);
}

void Gfx::PSCanvas::popMatrix()
{
GVX_TRACE("Gfx::PSCanvas::popMatrix");
  rep->grestore();
}

void Gfx::PSCanvas::translate(const vec3d& v)
{
GVX_TRACE("Gfx::PSCanvas::translate");
  rep->translate(v);
}

void Gfx::PSCanvas::scale(const vec3d& v)
{
GVX_TRACE("Gfx::PSCanvas::scale");
  rep->scale(v);
}

void Gfx::PSCanvas::rotate(const vec3d& axis, double angle_in_degrees)
{
GVX_TRACE("Gfx::PSCanvas::rotate");
  rep->rotate(axis, angle_in_degrees);
}

void Gfx::PSCanvas::transform(const geom::txform& tx)
{
GVX_TRACE("Gfx::PSCanvas::transform");
  rep->current_state().txform.transform(tx);
}

void Gfx::PSCanvas::loadMatrix(const geom::txform& tx)
{
GVX_TRACE("Gfx::PSCanvas::loadMatrix");
  rep->current_state().txform = tx;
}


void Gfx::PSCanvas::drawPixels(const media::bmap_data& /*data*/,
                               const vec3d& /*world_pos*/,
                               const vec2d& /*zoom*/)
{
GVX_TRACE("Gfx::PSCanvas::drawPixels");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::drawBitmap(const media::bmap_data& /*data*/,
                               const vec3d& /*world_pos*/)
{
GVX_TRACE("Gfx::PSCanvas::drawBitmap");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

media::bmap_data Gfx::PSCanvas::grabPixels(const recti&)
{
GVX_TRACE("Gfx::PSCanvas::grabPixels");
  rep->raiseError("grabPixels not possible in PostScript canvas",
                  SRC_POS);
}

void Gfx::PSCanvas::clearColorBuffer()
{
GVX_TRACE("Gfx::PSCanvas::clearColorBuffer");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::clearColorBuffer(const recti& /*screen_rect*/)
{
GVX_TRACE("Gfx::PSCanvas::clearColorBuffer(geom::rect)");
// FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::drawRect(const rectd& r)
{
GVX_TRACE("Gfx::PSCanvas::drawRect");

  rep->newpath();
  rep->moveto(r.bottom_left());
  rep->lineto(r.bottom_right());
  rep->lineto(r.top_right());
  rep->lineto(r.top_left());
  rep->closepath();
  if (rep->current_state().polygonFill)
    rep->fill();
  else
    rep->stroke();
}

void Gfx::PSCanvas::drawCircle(double inner_radius, double outer_radius,
                               bool fill, unsigned int, unsigned int)
{
GVX_TRACE("Gfx::PSCanvas::drawCircle");

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
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::drawSphere(double /*radius*/, int /*slices*/, int /*stacks*/,
                               bool /*fill*/)
{
  // FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::drawBezier4(const vec3d& p1,
                                const vec3d& p2,
                                const vec3d& p3,
                                const vec3d& p4,
                                unsigned int /*subdivisions*/)
{
GVX_TRACE("Gfx::PSCanvas::drawBezier4");
  rep->bezier(p1, p2, p3, p4);
}

void Gfx::PSCanvas::drawBezierFill4(const vec3d& /*center*/,
                                    const vec3d& /*p1*/,
                                    const vec3d& /*p2*/,
                                    const vec3d& /*p3*/,
                                    const vec3d& /*p4*/,
                                    unsigned int /*subdivisions*/)
{
GVX_TRACE("Gfx::PSCanvas::drawBezierFill4");
  // FIXME
  rep->raiseError("not implemented", SRC_POS);
}

void Gfx::PSCanvas::beginPoints(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginPoints");
  rep->beginPrimitive(&thePointsPrim, comment);
}

void Gfx::PSCanvas::beginLines(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginLines");
  rep->beginPrimitive(&theLinesPrim, comment);
}

void Gfx::PSCanvas::beginLineStrip(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginLineStrip");
  rep->beginPrimitive(&theLineStripPrim, comment);
}

void Gfx::PSCanvas::beginLineLoop(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginLineLoop");
  rep->beginPrimitive(&theLineLoopPrim, comment);
}

void Gfx::PSCanvas::beginTriangles(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginTriangles");
  rep->beginPrimitive(&theTrianglesPrim, comment);
}

void Gfx::PSCanvas::beginTriangleStrip(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginTriangleStrip");
  rep->beginPrimitive(&theTriangleStripPrim, comment);
}

void Gfx::PSCanvas::beginTriangleFan(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginTriangleFan");
  rep->beginPrimitive(&theTriangleFanPrim, comment);
}

void Gfx::PSCanvas::beginQuads(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginQuads");
  rep->beginPrimitive(&theQuadsPrim, comment);
}

void Gfx::PSCanvas::beginQuadStrip(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginQuadStrip");
  rep->beginPrimitive(&theQuadStripPrim, comment);
}

void Gfx::PSCanvas::beginPolygon(const char* comment)
{
GVX_TRACE("Gfx::PSCanvas::beginPolygon");
  rep->beginPrimitive(&thePolygonPrim, comment);
}

void Gfx::PSCanvas::vertex2(const vec2d& v)
{
GVX_TRACE("Gfx::PSCanvas::vertex2");
  vertex3(vec3d(v.x(), v.y(), 0.0));
}

void Gfx::PSCanvas::vertex3(const vec3d& v)
{
GVX_TRACE("Gfx::PSCanvas::vertex3");

  if (rep->itsPrimPtr == nullptr)
    {
      rep->raiseError("called vertex() outside graphics primitive",
                      SRC_POS);
    }

  rep->itsPrimPtr->vertex(rep, v);
}

void Gfx::PSCanvas::end()
{
GVX_TRACE("Gfx::PSCanvas::end");
  rep->endPrimitive();
}

void Gfx::PSCanvas::drawRasterText(const rutz::fstring& /*text*/,
                                   const GxRasterFont& /*font*/)
{
GVX_TRACE("Gfx::PSCanvas::drawRasterText");
  throw rutz::error("PSCanvas::drawRasterText not implemented", SRC_POS);
}

void Gfx::PSCanvas::drawVectorText(const rutz::fstring& /*text*/,
                                   const GxVectorFont& /*font*/)
{
GVX_TRACE("Gfx::PSCanvas::drawVectorText");
  throw rutz::error("PSCanvas::drawVectorText not implemented", SRC_POS);
}

void Gfx::PSCanvas::flushOutput()
{
GVX_TRACE("Gfx::PSCanvas::flushOutput");
  rep->itsFstream << std::flush;
}
