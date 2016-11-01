/** @file gfx/glcanvas.cc Gfx::Canvas subclass that renders to an
    OpenGL context */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Dec  6 20:28:36 1999
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

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "glcanvas.h"

#include "geom/projection.h"
#include "geom/rect.h"
#include "geom/txform.h"
#include "geom/vec2.h"
#include "geom/vec3.h"

#include "gfx/glwindowinterface.h"
#include "gfx/glxopts.h"
#include "gfx/gxrasterfont.h"
#include "gfx/gxvectorfont.h"
#include "gfx/rgbacolor.h"

#include "media/bmapdata.h"

#include "rutz/error.h"
#include "rutz/sfmt.h"
#include "rutz/shared_ptr.h"

#include <vector>

#if defined(GVX_GL_PLATFORM_GLX)
#  include <GL/gl.h>
#  include <GL/glu.h>
#elif defined(GVX_GL_PLATFORM_AGL)
#  include <AGL/agl.h>
#  include <OpenGL/glu.h>
#endif

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using geom::recti;
using geom::rectd;
using geom::txform;
using geom::vec2i;
using geom::vec2st;
using geom::vec2d;
using geom::vec3i;
using geom::vec3d;

using rutz::shared_ptr;

namespace
{
  nub::soft_ref<GLCanvas> theCurrentCanvas;

  GLint attribStackDepth()
  {
    GLint d = -1;
    glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &d);
    return d;
  }

  bool rasterPositionValid() noexcept
  {
    GLboolean value = GL_FALSE;
    glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &value);
    return (value == GL_TRUE);
  }

  txform rawGetModelview()
  {
    GVX_TRACE("<glcanvas.cc>::rawGetModelview");
    GLdouble m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, &m[0]);
    return txform::copy_of(&m[0]);
  }

  txform rawGetProjection()
  {
    GVX_TRACE("<glcanvas.cc>::rawGetProjection");
    GLdouble m[16];
    glGetDoublev(GL_PROJECTION_MATRIX, &m[0]);
    return txform::copy_of(&m[0]);
  }

  vec3d unproject1(const txform& modelview,
                   const txform& projection,
                   const recti& viewport,
                   const vec3d& screen)
  {
    GVX_TRACE("<glcanvas.cc>::unproject1");

    const GLint v[4] = { viewport.left(), viewport.bottom(),
                         viewport.width(), viewport.height() };

    vec3d world_pos;

    GLint status =
      gluUnProject(screen.x(), screen.y(), screen.z(),
                   modelview.col_major_data(),
                   projection.col_major_data(),
                   &v[0],
                   &world_pos.x(), &world_pos.y(), &world_pos.z());

    dbg_eval_nl(3, status);

    if (status == GL_FALSE)
      throw rutz::error("gluUnProject error", SRC_POS);

    return world_pos;
  }

  vec3d project1(const txform& modelview,
                 const txform& projection,
                 const recti& viewport,
                 const vec3d& world_pos)
  {
    GVX_TRACE("<glcanvas.cc>::project1");

    const GLint v[4] = { viewport.left(), viewport.bottom(),
                         viewport.width(), viewport.height() };

    vec3d screen_pos;

    GLint status =
      gluProject(world_pos.x(), world_pos.y(), world_pos.z(),
                 modelview.col_major_data(),
                 projection.col_major_data(),
                 &v[0],
                 &screen_pos.x(), &screen_pos.y(), &screen_pos.z());

    dbg_eval_nl(3, status);

    if (status == GL_FALSE)
      throw rutz::error("GLCanvas::screenFromWorld3(): gluProject error",
                        SRC_POS);

    return screen_pos;
  }
}

class GLCanvas::Impl
{
public:
  Impl(shared_ptr<GlxOpts> opts_, shared_ptr<GlWindowInterface> glx_) :
    opts(opts_),
    glx(glx_),
    modelviewCache(),
    projectionCache(),
    viewportCache(),
    depthrangeCache(0.0, 1.0)
  {
    modelviewCache.push_back(txform::identity());
    projectionCache.push_back(txform::identity());

    // The following glLoadIdentity() calls are implied here; we
    // assume that the modelview and projection matrices are both
    // intialized to the identity matrix upon program
    // startup. However, we CANNOT safely call them here, since the GL
    // context may not yet be current. If it turns out that we do need
    // this explicit initialization, then it probably should go in
    // GlxWrapper::makeCurrent() under a flag that checks whether it's
    // the first time.
#if 0
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
#endif

    dbg_dump(4, modelviewCache.back());
  }

  const txform& getModelview() const
  {
    GVX_TRACE("GLCanvas::getModelview");
    GVX_ASSERT(modelviewCache.size() > 0);
    if (GVX_DBG_LEVEL() >= 8)
      {
        const txform ref = rawGetModelview();
        const double sse = ref.debug_sse(modelviewCache.back());
        if (sse > 1e-10)
          {
            dbg_eval_nl(0, sse);
            dbg_dump(0, ref);
            dbg_dump(0, modelviewCache.back());
            GVX_PANIC("numerical error in modelview matrix cache");
          }
      }
    return modelviewCache.back();
  }

  const txform& getProjection() const
  {
    GVX_TRACE("GLCanvas::getProjection");
    GVX_ASSERT(projectionCache.size() > 0);
    if (GVX_DBG_LEVEL() >= 8)
      {
        const txform ref = rawGetProjection();
        const double sse = ref.debug_sse(projectionCache.back());
        if (sse > 1e-10)
          {
            dbg_eval_nl(0, sse);
            dbg_dump(0, ref);
            dbg_dump(0, projectionCache.back());
            GVX_PANIC("numerical error in projection matrix cache");
          }
      }
    return projectionCache.back();
  }

  shared_ptr<GlxOpts>           opts;
  shared_ptr<GlWindowInterface> glx;
  std::vector<txform>           modelviewCache;
  std::vector<txform>           projectionCache;
  recti                         viewportCache;
  const geom::span<double>      depthrangeCache;
};

GLCanvas::GLCanvas(shared_ptr<GlxOpts> opts,
                   shared_ptr<GlWindowInterface> glx) :
  rep(new Impl(opts, glx))
{
GVX_TRACE("GLCanvas::GLCanvas");
}

GLCanvas* GLCanvas::make(shared_ptr<GlxOpts> opts,
                         shared_ptr<GlWindowInterface> glx)
{
GVX_TRACE("GLCanvas::make");
  return new GLCanvas(opts, glx);
}

GLCanvas::~GLCanvas() noexcept
{
GVX_TRACE("GLCanvas::~GLCanvas");
  delete rep;
  rep = 0;
}

void GLCanvas::makeCurrent()
{
GVX_TRACE("GLCanvas::makeCurrent");
  rep->glx->makeCurrent();
  if (!rep->opts->doubleFlag && this->isDoubleBuffered())
    {
      // We requested single buffering but had to accept a double
      // buffered visual. Set the GL draw buffer to be the front
      // buffer to simulate single buffering.
      this->drawBufferFront();
    }
  theCurrentCanvas = nub::soft_ref<GLCanvas>(this);
}

nub::soft_ref<GLCanvas> GLCanvas::getCurrent()
{
GVX_TRACE("GLCanvas::getCurrent");
  return theCurrentCanvas;
}

void GLCanvas::drawBufferFront() noexcept
{
GVX_TRACE("GLCanvas::drawBufferFront");
  glDrawBuffer(GL_FRONT);
}

void GLCanvas::drawBufferBack() noexcept
{
GVX_TRACE("GLCanvas::drawBufferBack");
  glDrawBuffer(GL_BACK);
}

vec3d GLCanvas::screenFromWorld3(const vec3d& world_pos) const
{
GVX_TRACE("GLCanvas::screenFromWorld3");

  const txform m = rep->getModelview();
  const txform p = rep->getProjection();
  const recti v = this->getScreenViewport();

  const vec3d screen_pos = geom::project(m, p, v, world_pos);

  dbg_dump(3, world_pos);
  dbg_dump(3, screen_pos);

  if (GVX_DBG_LEVEL() >= 8)
    {
      const vec3d screen_pos1 = project1(m, p, v, world_pos);
      const vec3d diff = screen_pos - screen_pos1;

      if (diff.length() > 1e-10)
        {
          dbg_eval_nl(0, diff.length());
          dbg_dump(0, p);
          dbg_dump(0, m);
          dbg_dump(0, screen_pos1);
          dbg_dump(0, screen_pos);
          GVX_PANIC("numerical error during world->screen projection");
        }
    }

  return screen_pos;
}

vec3d GLCanvas::worldFromScreen3(const vec3d& screen_pos) const
{
GVX_TRACE("GLCanvas::worldFromScreen3");

  dbg_dump(3, screen_pos);

  const txform m = rep->getModelview();
  const txform p = rep->getProjection();
  const recti v = getScreenViewport();

  dbg_dump(5, m);
  dbg_dump(5, p);

  const vec3d world2 = geom::unproject(m, p, v, screen_pos);

  if (GVX_DBG_LEVEL() >= 8)
    {
      const vec3d world1 = unproject1(m, p, v, screen_pos);
      const vec3d diff = world2 - world1;

      if (diff.length() > 1e-10)
        {
          dbg_eval_nl(0, diff.length());
          dbg_dump(0, p);
          dbg_dump(0, m);
          dbg_dump(0, world1);
          dbg_dump(0, world2);
          GVX_PANIC("numerical error during screen->world reverse projection");
        }
    }

  return world2;
}


recti GLCanvas::getScreenViewport() const
{
GVX_TRACE("GLCanvas::getScreenViewport");

  return rep->viewportCache;
}


bool GLCanvas::isRgba() const
{
GVX_TRACE("GLCanvas::isRgba");

  return rep->opts->rgbaFlag;
}

bool GLCanvas::isColorIndex() const
{
GVX_TRACE("GLCanvas::isColorIndex");

  return !(rep->opts->rgbaFlag);
}

bool GLCanvas::isDoubleBuffered() const
{
GVX_TRACE("GLCanvas::isDoubleBuffered");

  return rep->glx->isDoubleBuffered();
}

unsigned int GLCanvas::bitsPerPixel() const
{
GVX_TRACE("GLCanvas::bitsPerPixel");

  return rep->glx->bitsPerPixel();
}

void GLCanvas::throwIfError(const char* where,
                            const rutz::file_pos& pos) const
{
GVX_TRACE("GLCanvas::throwIfError");
  GLenum status = glGetError();
  if (status != GL_NO_ERROR)
    {
      const char* msg =
        reinterpret_cast<const char*>(gluErrorString(status));
      throw rutz::error(rutz::sfmt("GL error: %s %s", msg, where), pos);
    }
}

void GLCanvas::pushAttribs(const char* /*comment*/)
{
GVX_TRACE("GLCanvas::pushAttribs");
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  dbg_eval_nl(3, attribStackDepth());
}

void GLCanvas::popAttribs()
{
GVX_TRACE("GLCanvas::popAttribs");
  glPopAttrib();
  dbg_eval_nl(3, attribStackDepth());
}

void GLCanvas::drawOnFrontBuffer()
{
GVX_TRACE("GLCanvas::drawOnFrontBuffer");
  glDrawBuffer(GL_FRONT);
}

void GLCanvas::drawOnBackBuffer()
{
GVX_TRACE("GLCanvas::drawOnBackBuffer");
  glDrawBuffer(GL_BACK);
}

void GLCanvas::setColor(const Gfx::RgbaColor& rgba)
{
GVX_TRACE("GLCanvas::setColor");
  glColor4fv(rgba.data());
}

void GLCanvas::setClearColor(const Gfx::RgbaColor& rgba)
{
GVX_TRACE("GLCanvas::setClearColor");
  glClearColor(rgba.r(), rgba.g(), rgba.b(), rgba.a());
}

void GLCanvas::setColorIndex(unsigned int index)
{
GVX_TRACE("GLCanvas::setColorIndex");
  glIndexi(GLint(index));
}

void GLCanvas::setClearColorIndex(unsigned int index)
{
GVX_TRACE("GLCanvas::setClearColorIndex");
  glClearIndex(index);
}

void GLCanvas::swapForeBack()
{
GVX_TRACE("GLCanvas::swapForeBack");
  if ( this->isRgba() )
    {
      GLfloat foreground[4];
      GLfloat background[4];
      glGetFloatv(GL_CURRENT_COLOR, &foreground[0]);
      glGetFloatv(GL_COLOR_CLEAR_VALUE, &background[0]);

      glColor4f(background[0],
                background[1],
                background[2],
                foreground[3]); // <-- note we keep the foreground alpha value

      glClearColor(foreground[0],
                   foreground[1],
                   foreground[2],
                   background[3]); // <-- note we keep the background alpha value
    }
  else
    {
      GLint foreground, background;
      glGetIntegerv(GL_CURRENT_INDEX, &foreground);
      glGetIntegerv(GL_INDEX_CLEAR_VALUE, &background);
      glIndexi(background);
      glClearIndex(foreground);
    }
}

void GLCanvas::setPolygonFill(bool on)
{
GVX_TRACE("GLCanvas::setPolygonFill");

  if (on)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  else
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void GLCanvas::setPointSize(double size)
{
GVX_TRACE("GLCanvas::setPointSize");

  glPointSize(GLfloat(size));
}

void GLCanvas::setLineWidth(double width)
{
GVX_TRACE("GLCanvas::setLineWidth");
  glLineWidth(GLfloat(width));
}

void GLCanvas::setLineStipple(unsigned short bit_pattern)
{
GVX_TRACE("GLCanvas::setLineStipple");
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, bit_pattern);
}

void GLCanvas::enableAntialiasing()
{
GVX_TRACE("GLCanvas::enableAntialiasing");

  if (isRgba()) // antialiasing does not work well except in RGBA mode
    {
      glEnable(GL_BLEND); // blend incoming RGBA values with old RGBA values

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use transparency

      // use anti-aliasing for lines (but not polygons):
      glEnable(GL_LINE_SMOOTH);
    }
}



void GLCanvas::viewport(int x, int y, int w, int h)
{
GVX_TRACE("GLCanvas::viewport");

  glViewport(x, y, w, h);
  dbg_eval(2, x); dbg_eval(2, y); dbg_eval(2, w); dbg_eval_nl(2, h);
  if (GVX_DBG_LEVEL() >= 8)
    {
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      dbg_eval(2, viewport[0]);
      dbg_eval(2, viewport[1]);
      dbg_eval(2, viewport[2]);
      dbg_eval_nl(2, viewport[3]);
    }

  rep->viewportCache = recti::lbwh(x, y, w, h);
}

void GLCanvas::orthographic(const rectd& bounds,
                            double zNear, double zFar)
{
GVX_TRACE("GLCanvas::orthographic");

  dbg_eval(3, bounds.left()); dbg_eval_nl(3, bounds.right());
  dbg_eval(3, bounds.bottom()); dbg_eval_nl(3, bounds.top());
  dbg_eval(3, zNear); dbg_eval_nl(3, zFar);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(bounds.left(), bounds.right(),
          bounds.bottom(), bounds.top(),
          zNear, zFar);
  rep->projectionCache.back() = txform::orthographic(bounds, zNear, zFar);
  glMatrixMode(GL_MODELVIEW);
}

void GLCanvas::perspective(double fovy, double aspect,
                           double zNear, double zFar)
{
GVX_TRACE("GLCanvas::perspective");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, aspect, zNear, zFar);
  rep->projectionCache.back() = rawGetProjection();
  glMatrixMode(GL_MODELVIEW);
}

void GLCanvas::pushMatrix(const char* /*comment*/)
{
GVX_TRACE("GLCanvas::pushMatrix");
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  rep->modelviewCache.push_back(rep->modelviewCache.back());
  dbg_dump(4, rep->modelviewCache.back());
}

void GLCanvas::popMatrix()
{
GVX_TRACE("GLCanvas::popMatrix");
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  GVX_ASSERT(rep->modelviewCache.size() > 0);
  rep->modelviewCache.pop_back();
  dbg_dump(4, rep->modelviewCache.back());
}

void GLCanvas::translate(const vec3d& v)
{
GVX_TRACE("GLCanvas::translate");
  glTranslated(v.x(), v.y(), v.z());
  rep->modelviewCache.back().translate(v);
  dbg_dump(4, rep->modelviewCache.back());
}

void GLCanvas::scale(const vec3d& v)
{
GVX_TRACE("GLCanvas::scale");
  if (v.x() == 0.0)
    {
      throw rutz::error("invalid x scaling factor", SRC_POS);
    }
  if (v.y() == 0.0)
    {
      throw rutz::error("invalid y scaling factor", SRC_POS);
    }
  if (v.z() == 0.0)
    {
      throw rutz::error("invalid z scaling factor", SRC_POS);
    }
  glScaled(v.x(), v.y(), v.z());
  rep->modelviewCache.back().scale(v);
  dbg_dump(4, rep->modelviewCache.back());
}

void GLCanvas::rotate(const vec3d& v, double angle_in_degrees)
{
GVX_TRACE("GLCanvas::rotate");
  glRotated(angle_in_degrees, v.x(), v.y(), v.z());
  rep->modelviewCache.back().rotate(v, angle_in_degrees);
  dbg_dump(4, rep->modelviewCache.back());
}

void GLCanvas::transform(const geom::txform& tx)
{
GVX_TRACE("GLCanvas::transform");
  glMultMatrixd(tx.col_major_data());
  rep->modelviewCache.back().transform(tx);
  dbg_dump(4, rep->modelviewCache.back());
}

void GLCanvas::loadMatrix(const geom::txform& tx)
{
GVX_TRACE("GLCanvas::loadMatrix");
  glLoadMatrixd(tx.col_major_data());
  rep->modelviewCache.back() = tx;
  dbg_dump(4, rep->modelviewCache.back());
}

void GLCanvas::rasterPos(const geom::vec3<double>& world_pos)
{
GVX_TRACE("GLCanvas::rasterPos");

  const rectd viewport = rectd(getScreenViewport());

  const geom::span<double> depth = rep->depthrangeCache;

  const vec3d screen_pos = screenFromWorld3(world_pos);

  dbg_dump(3, world_pos);
  dbg_dump(3, screen_pos);

  if (viewport.contains(screen_pos.as_vec2()) &&
      depth.contains(screen_pos.z()))
    {
      GVX_TRACE("GLCanvas::rasterPos::branch-1");
      glRasterPos3d(world_pos.x(), world_pos.y(), world_pos.z());
    }
  else
    {
      GVX_TRACE("GLCanvas::rasterPos::branch-2");
      // OK... in this case, our desired raster position actually
      // falls outside the onscreen viewport. If we just called
      // glRasterPos() with that position, it would recognize it as an
      // invalid point and then subsequent glDrawPixels() calls would
      // fail. To trick OpenGL in using the position we want, we first
      // do a glRasterPos() to some valid position -- in this case, we
      // pick a point near at the center of the viewport and depth
      // range. Then we do a glBitmap() call whose only purpose is to
      // use the "xmove" and "ymove" arguments to adjust the raster
      // position to the desired location.
      const vec3d safe_screen = vec3d(viewport.center_x(),
                                      viewport.center_y(),
                                      depth.center());
      const vec3d safe_world = worldFromScreen3(safe_screen);

      dbg_dump(3, safe_screen);
      dbg_dump(3, safe_world);

      glRasterPos3d(safe_world.x(), safe_world.y(), safe_world.z());

      glBitmap(0, 0, 0.0f, 0.0f,
               GLfloat(screen_pos.x()-safe_screen.x()),
               GLfloat(screen_pos.y()-safe_screen.y()),
               static_cast<const GLubyte*>(0));
    }

  if (GVX_DBG_LEVEL() >= 8)
    {
      // This operation is slow because it involves a glGet*()
      GVX_POSTCONDITION(rasterPositionValid());
    }
}

void GLCanvas::drawPixels(const media::bmap_data& data,
                          const vec3d& world_pos,
                          const vec2d& zoom)
{
GVX_TRACE("GLCanvas::drawPixels");

  data.set_row_order(media::bmap_data::BOTTOM_FIRST);

  rasterPos(world_pos);

  glPixelZoom(GLfloat(zoom.x()), GLfloat(zoom.y()));

  glPixelStorei(GL_UNPACK_ALIGNMENT, GLint(data.byte_alignment()));

  if (data.bits_per_pixel() == 32)
    {
      GVX_TRACE("GLCanvas::drawPixels[32]");
      glDrawPixels(GLsizei(data.width()), GLsizei(data.height()),
                   GL_RGBA, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytes_ptr()));
    }
  else if (data.bits_per_pixel() == 24)
    {
      GVX_TRACE("GLCanvas::drawPixels[24]");
      glDrawPixels(GLsizei(data.width()), GLsizei(data.height()),
                   GL_RGB, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytes_ptr()));
    }
  else if (data.bits_per_pixel() == 8)
    {
      GVX_TRACE("GLCanvas::drawPixels[8]");
      if (isRgba())
        {
          glDrawPixels(GLsizei(data.width()), GLsizei(data.height()),
                       GL_LUMINANCE, GL_UNSIGNED_BYTE,
                       static_cast<GLvoid*>(data.bytes_ptr()));
        }
      else
        {
          glDrawPixels(GLsizei(data.width()), GLsizei(data.height()),
                       GL_COLOR_INDEX, GL_UNSIGNED_BYTE,
                       static_cast<GLvoid*>(data.bytes_ptr()));
        }
    }
  else if (data.bits_per_pixel() == 1)
    {
      GVX_TRACE("GLCanvas::drawPixels[1]");

#if 0
      if (isRgba())
        {
          GLfloat simplemap[] = {0.0, 1.0};

          glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, simplemap);
          glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, simplemap);
          glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, simplemap);
          glPixelMapfv(GL_PIXEL_MAP_I_TO_A, 2, simplemap);
        }

      glDrawPixels(GLsizei(data.width()), GLsizei(data.height()), GL_COLOR_INDEX,
                   GL_BITMAP, static_cast<GLvoid*>(data.bytes_ptr()));
#else

      // for nvidia driver; it's much faster (~125x faster) to first
      // unpack the 1-bit-per-pixel bitmap into an 8-bits-per-pixel
      // image before calling glDrawPixels()

      media::bmap_data unpacked(data.size(), 8, 1);

      const unsigned char* sptr = data.bytes_ptr();
      const unsigned char* sstop = sptr + data.byte_count();
      unsigned char* dptr = unpacked.bytes_ptr();
      unsigned char* dstop = unpacked.bytes_ptr() + unpacked.byte_count();

      while (sptr != sstop)
        {
          GVX_ASSERT((dptr+7) < dstop);

          *dptr++ = ((*sptr) & (1 << 7)) ? 255 : 0;
          *dptr++ = ((*sptr) & (1 << 6)) ? 255 : 0;
          *dptr++ = ((*sptr) & (1 << 5)) ? 255 : 0;
          *dptr++ = ((*sptr) & (1 << 4)) ? 255 : 0;
          *dptr++ = ((*sptr) & (1 << 3)) ? 255 : 0;
          *dptr++ = ((*sptr) & (1 << 2)) ? 255 : 0;
          *dptr++ = ((*sptr) & (1 << 1)) ? 255 : 0;
          *dptr++ = ((*sptr) & (1 << 0)) ? 255 : 0;

          ++sptr;
        }

      glPixelStorei(GL_UNPACK_ALIGNMENT, GLint(unpacked.byte_alignment()));

      glDrawPixels(GLsizei(unpacked.width()), GLsizei(unpacked.height()),
                   GL_LUMINANCE, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(unpacked.bytes_ptr()));
#endif
    }
}

void GLCanvas::drawBitmap(const media::bmap_data& data,
                          const vec3d& world_pos)
{
GVX_TRACE("GLCanvas::drawBitmap");

  data.set_row_order(media::bmap_data::BOTTOM_FIRST);

  rasterPos(world_pos);

  glBitmap(GLsizei(data.width()), GLsizei(data.height()), 0.0, 0.0, 0.0, 0.0,
           static_cast<GLubyte*>(data.bytes_ptr()));
}

void GLCanvas::grabPixels(const recti& bounds,
                          media::bmap_data& data_out)
{
GVX_TRACE("GLCanvas::grabPixels");

  const int pixel_alignment = 1;

  // NOTE: we can't just use GLCanvas::bitsPerPixel() here, since that
  // won't work in the case of a 16-bit color buffer; in that case, we
  // are still in RGBA mode, so glReadPixels() will return one byte
  // per color component (i.e. 24 bits per pixel) regardless of the
  // actual color buffer depth.
  const unsigned int bmap_bits_per_pixel = isRgba() ? 24 : 8;

  media::bmap_data new_data(vec2st(bounds.size()),
                            bmap_bits_per_pixel, pixel_alignment);

  glPixelStorei(GL_PACK_ALIGNMENT, pixel_alignment);

  glPushAttrib(GL_PIXEL_MODE_BIT);
  {
    glReadBuffer(GL_FRONT);
    glReadPixels(bounds.left(), bounds.bottom(),
                 bounds.width(), bounds.height(),
                 (isRgba() ? GL_RGB : GL_COLOR_INDEX),
                 GL_UNSIGNED_BYTE, new_data.bytes_ptr());
  }
  glPopAttrib();

  new_data.specify_row_order(media::bmap_data::BOTTOM_FIRST);

  data_out.swap(new_data);
}

void GLCanvas::clearColorBuffer()
{
GVX_TRACE("GLCanvas::clearColorBuffer");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLCanvas::clearColorBuffer(const recti& screen_rect)
{
GVX_TRACE("GLCanvas::clearColorBuffer(geom::recti)");

  glPushAttrib(GL_SCISSOR_BIT);
  {
    glEnable(GL_SCISSOR_TEST);

    glScissor(screen_rect.left(), screen_rect.bottom(),
              screen_rect.width(), screen_rect.height());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
  }
  glPopAttrib();
}

void GLCanvas::drawRect(const rectd& rect)
{
GVX_TRACE("GLCanvas::drawRect");

  glRectd(rect.left(),
          rect.bottom(),
          rect.right(),
          rect.top());
}

void GLCanvas::drawCircle(double inner_radius, double outer_radius,
                          bool fill,
                          unsigned int slices, unsigned int loops)
{
GVX_TRACE("GLCanvas::drawCircle");

  GLUquadricObj* qobj = gluNewQuadric();

  if (qobj == 0)
    throw rutz::error("couldn't allocate GLUquadric object", SRC_POS);

  gluQuadricDrawStyle(qobj, fill ? GLU_FILL : GLU_SILHOUETTE);
  gluDisk(qobj, inner_radius, outer_radius, GLint(slices), GLint(loops));
  gluDeleteQuadric(qobj);
}

void GLCanvas::drawCylinder(double base_radius, double top_radius,
                            double height, int slices, int stacks,
                            bool fill)
{
GVX_TRACE("GLCanvas::drawCylinder");

  GLUquadric* qobj = gluNewQuadric();

  if (qobj == 0)
    throw rutz::error("couldn't allocate GLUquadric object", SRC_POS);

  gluQuadricDrawStyle(qobj, fill ? GLU_FILL : GLU_LINE);
  gluCylinder(qobj, base_radius, top_radius, height, slices, stacks);
  gluDeleteQuadric(qobj);
}

void GLCanvas::drawSphere(double radius, int slices, int stacks,
                          bool fill)
{
GVX_TRACE("GLCanvas::drawSphere");

  GLUquadric* qobj = gluNewQuadric();

  if (qobj == 0)
    throw rutz::error("couldn't allocate GLUquadric object", SRC_POS);

  gluQuadricDrawStyle(qobj, fill ? GLU_FILL : GLU_LINE);
  gluSphere(qobj, radius, slices, stacks);
  gluDeleteQuadric(qobj);
}

void GLCanvas::drawBezier4(const vec3d& p1,
                           const vec3d& p2,
                           const vec3d& p3,
                           const vec3d& p4,
                           unsigned int subdivisions)
{
GVX_TRACE("GLCanvas::drawBezier4");

#if 1
  // Use the generic version, since it seems to actually be faster than the
  // glEvalMesh() approach anyway
  Canvas::drawBezier4(p1, p2, p3, p4, subdivisions);

#else

  vec3d points[] =
  {
    p1, p2, p3, p4
  };

  glEnable(GL_MAP1_VERTEX_3);
  glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, points[0].data());

  glMapGrid1d(subdivisions, 0.0, 1.0);
  glEvalMesh1(GL_LINE, 0, subdivisions);
#endif
}

void GLCanvas::drawBezierFill4(const vec3d& center,
                               const vec3d& p1,
                               const vec3d& p2,
                               const vec3d& p3,
                               const vec3d& p4,
                               unsigned int subdivisions)
{
GVX_TRACE("GLCanvas::drawBezierFill4");

  // Looks like the the OpenGL-specific version beats the generic version
  // here, unlike for drawBezier4().

#if 0
  Canvas::drawBezierFill4(center, p1, p2, p3, p4, subdivisions);
#else
  vec3d points[] =
  {
    p1, p2, p3, p4
  };

  glEnable(GL_MAP1_VERTEX_3);
  glMap1d(GL_MAP1_VERTEX_3,
          0.0,                  // beginning of domain range
          double(subdivisions), // end of domain range
          3,                    // stride (i.e. skip between array points)
          4,                    // order (i.e. number of control points)
          points[0].data());

  glMapGrid1d(GLint(subdivisions), 0.0, 1.0);

  glBegin(GL_TRIANGLE_FAN);
  vertex3(center);
  for (unsigned int d = 0; d <= subdivisions; ++d)
    {
      glEvalCoord1d(double(d));
    }
  glEnd();
#endif
}

void GLCanvas::beginPoints(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginPoints"); glBegin(GL_POINTS); }

void GLCanvas::beginLines(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginLines"); glBegin(GL_LINES); }

void GLCanvas::beginLineStrip(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginLineStrip"); glBegin(GL_LINE_STRIP); }

void GLCanvas::beginLineLoop(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginLineLoop"); glBegin(GL_LINE_LOOP); }

void GLCanvas::beginTriangles(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginTriangles"); glBegin(GL_TRIANGLES); }

void GLCanvas::beginTriangleStrip(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginTriangleStrip"); glBegin(GL_TRIANGLE_STRIP); }

void GLCanvas::beginTriangleFan(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginTriangleFan"); glBegin(GL_TRIANGLE_FAN); }

void GLCanvas::beginQuads(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginQuads"); glBegin(GL_QUADS); }

void GLCanvas::beginQuadStrip(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginQuadStrip"); glBegin(GL_QUAD_STRIP); }

void GLCanvas::beginPolygon(const char* /*comment*/)
{ GVX_TRACE("GLCanvas::beginPolygon"); glBegin(GL_POLYGON); }

void GLCanvas::vertex2(const vec2d& v)
{
GVX_TRACE("GLCanvas::vertex2");
  glVertex2d(v.x(), v.y());
}

void GLCanvas::vertex3(const vec3d& v)
{
GVX_TRACE("GLCanvas::vertex3");
  glVertex3d(v.x(), v.y(), v.z());
}

void GLCanvas::end()
{
GVX_TRACE("GLCanvas::end");
  glEnd();
}

void GLCanvas::drawRasterText(const rutz::fstring& text,
                              const GxRasterFont& font)
{
GVX_TRACE("GLCanvas::drawRasterText");

  glListBase( font.listBase() );

  const char* p = text.c_str();

  int line = 0;

  while (1)
    {
      int len = 0;
      while (p[len] != '\0' && p[len] != '\n')
        ++len;

      dbg_eval(3, len); dbg_eval_nl(3, p);

      rasterPos( vec3d::zeros() );
      if (line > 0)
        {
          // this is a workaround to shift the raster position by a given
          // number of pixels
          glBitmap(0, 0, 0.0f, 0.0f,
                   0,                               // x shift
                   -1 * font.rasterHeight() * line, // y shift
                   static_cast<const GLubyte*>(0));
        }

      glCallLists( len, GL_BYTE, p );

      p += len;

      if (*p == '\0')
        break;

      // else...
      GVX_ASSERT(*p == '\n');
      ++p;
      ++line;
    }
}

void GLCanvas::drawVectorText(const rutz::fstring& text,
                              const GxVectorFont& font)
{
GVX_TRACE("GLCanvas::drawVectorText");

  glListBase( font.listBase() );

  const char* p = text.c_str();

  glMatrixMode(GL_MODELVIEW);

  int line = 0;

  while (1)
    {
      int len = 0;
      while (p[len] != '\0' && p[len] != '\n')
        ++len;

      dbg_eval(3, len); dbg_eval_nl(3, p);

      glPushMatrix();

      if (line > 0)
        glTranslated( 0.0,
                      -1.0 * font.vectorHeight() * line,
                      0.0 );

      glCallLists( len, GL_BYTE, p );
      glPopMatrix();

      p += len;

      if (*p == '\0')
        break;

      // else...
      GVX_ASSERT(*p == '\n');
      ++p;
      ++line;
    }
}

void GLCanvas::flushOutput()
{
GVX_TRACE("GLCanvas::flushOutput");

  if (rep->opts->doubleFlag)
    rep->glx->swapBuffers();
  else
    glFlush();
}

void GLCanvas::finishDrawing()
{
GVX_TRACE("GLCanvas::finishDrawing");
  glFinish();
}

unsigned int GLCanvas::genLists(unsigned int num)
{
GVX_TRACE("GLCanvas::genLists");
  const unsigned int i = glGenLists(GLsizei(num));

  if (i == 0)
    throw rutz::error("Couldn't allocate GL display list", SRC_POS);

  return i;
}

void GLCanvas::deleteLists(unsigned int start, unsigned int num)
{
GVX_TRACE("GLCanvas::deleteLists");
  glDeleteLists(start, GLsizei(num));
}

void GLCanvas::newList(unsigned int i, bool do_execute)
{
GVX_TRACE("GLCanvas::newList");

 glNewList(i,
           do_execute
           ? GL_COMPILE_AND_EXECUTE
           : GL_COMPILE);
}

void GLCanvas::endList()
{
GVX_TRACE("GLCanvas::endList");
  glEndList();
}

bool GLCanvas::isList(unsigned int i)
{
GVX_TRACE("GLCanvas::isList");
  return i != 0 && glIsList(i) == GL_TRUE;
}

void GLCanvas::callList(unsigned int i)
{
GVX_TRACE("GLCanvas::callList");
  glCallList(i);
}

void GLCanvas::light(unsigned int lightnum,
                     const Gfx::RgbaColor* spec,
                     const Gfx::RgbaColor* diff,
                     const Gfx::RgbaColor* ambi,
                     const vec3d* posi,
                     const vec3d* sdir,
                     double attenuation,
                     double spotExponent,
                     double spotCutoff)
{
GVX_TRACE("GLCanvas::light");

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0+lightnum);
  glEnable(GL_DEPTH_TEST);

  if (spec != 0)
    {
      const GLfloat fspecular[] = { spec->r(), spec->g(), spec->b(), spec->a() };
      glLightfv(GL_LIGHT0+lightnum, GL_SPECULAR, fspecular);
    }

  if (diff != 0)
    {
      const GLfloat fdiffuse[] = { diff->r(), diff->g(), diff->b(), diff->a() };
      glLightfv(GL_LIGHT0+lightnum, GL_DIFFUSE, fdiffuse);
    }

  if (ambi != 0)
    {
      const GLfloat fambient[] =  { ambi->r(), ambi->g(), ambi->b(), ambi->a() };
      glLightfv(GL_LIGHT0+lightnum, GL_AMBIENT, fambient);
    }

  if (posi != 0)
    {
      const GLfloat w = (attenuation == 0.0) ? 0.0f : 1.0f;
      const GLfloat m = (attenuation == 0.0) ? 1.0f : GLfloat(1.0/attenuation);

      const GLfloat fposition[] = { GLfloat(m*posi->x()), GLfloat(m*posi->y()), GLfloat(m*posi->z()), w };
      glLightfv(GL_LIGHT0+lightnum, GL_POSITION, fposition);
    }

  if (sdir != 0)
    {
      const GLfloat fdirection[] = { (GLfloat) sdir->x(), (GLfloat) sdir->y(), (GLfloat) sdir->z(), 0.0 };

      glLightfv(GL_LIGHT0+lightnum, GL_SPOT_DIRECTION, fdirection);
      glLightf(GL_LIGHT0+lightnum, GL_SPOT_EXPONENT, GLfloat(spotExponent));
      glLightf(GL_LIGHT0+lightnum, GL_SPOT_CUTOFF, GLfloat(spotCutoff));
    }
}

void GLCanvas::material(const Gfx::RgbaColor* spec,
                        const Gfx::RgbaColor* diff,
                        const Gfx::RgbaColor* ambi,
                        const double* shininess)
{
  glEnable(GL_DEPTH_TEST);

  if (spec != 0)
    {
      const GLfloat specular[] = { spec->r(), spec->g(), spec->b(), spec->a() };
      glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    }

  if (diff != 0)
    {
      const GLfloat diffuse[] =  { diff->r(), diff->g(), diff->b(), diff->a() };
      glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    }

  if (ambi != 0)
    {
      const GLfloat ambient[] =  { ambi->r(), ambi->g(), ambi->b(), ambi->a() };
      glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    }

  if (shininess != 0)
    glMaterialf(GL_FRONT, GL_SHININESS, GLfloat(*shininess));
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
