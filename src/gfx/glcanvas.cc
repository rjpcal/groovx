///////////////////////////////////////////////////////////////////////
//
// glcanvas.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Dec  6 20:28:36 1999
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

#ifndef GLCANVAS_CC_DEFINED
#define GLCANVAS_CC_DEFINED

#include "glcanvas.h"

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

#include "util/error.h"
#include "util/sharedptr.h"

#if defined(GL_PLATFORM_GLX)
#  include <GL/gl.h>
#  include <GL/glu.h>
#elif defined(GL_PLATFORM_AGL)
#  include <AGL/gl.h>
#  include <AGL/glu.h>
#endif

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using geom::vec2i;
using geom::vec2d;
using geom::vec3i;
using geom::vec3d;

using rutz::shared_ptr;

class GLCanvas::Impl
{
public:
  Impl(shared_ptr<GlxOpts> opts_, shared_ptr<GlWindowInterface> glx_) :
    opts(opts_),
    glx(glx_)
  {}

  shared_ptr<GlxOpts> opts;
  shared_ptr<GlWindowInterface> glx;
};

GLCanvas::GLCanvas(shared_ptr<GlxOpts> opts,
                   shared_ptr<GlWindowInterface> glx) :
  rep(new Impl(opts, glx))
{
DOTRACE("GLCanvas::GLCanvas");
}

GLCanvas* GLCanvas::make(shared_ptr<GlxOpts> opts,
                         shared_ptr<GlWindowInterface> glx)
{
DOTRACE("GLCanvas::make");
  return new GLCanvas(opts, glx);
}

GLCanvas::~GLCanvas() throw()
{
DOTRACE("GLCanvas::~GLCanvas");
  delete rep;
  rep = 0;
}

void GLCanvas::drawBufferFront() throw()
{
DOTRACE("GLCanvas::drawBufferFront");
  glDrawBuffer(GL_FRONT);
}

void GLCanvas::drawBufferBack() throw()
{
DOTRACE("GLCanvas::drawBufferBack");
  glDrawBuffer(GL_BACK);
}

vec3i GLCanvas::screenFromWorld3(const vec3d& world_pos) const
{
DOTRACE("GLCanvas::screenFromWorld3");

  GLdouble current_mv_matrix[16];
  GLdouble current_proj_matrix[16];
  GLint current_viewport[4];

  glGetDoublev(GL_MODELVIEW_MATRIX, current_mv_matrix);
  glGetDoublev(GL_PROJECTION_MATRIX, current_proj_matrix);
  glGetIntegerv(GL_VIEWPORT, current_viewport);

  double screen_x, screen_y, screen_z;

  GLint status =
    gluProject(world_pos.x(), world_pos.y(), world_pos.z(),
               current_mv_matrix, current_proj_matrix, current_viewport,
               &screen_x, &screen_y, &screen_z);

  dbg_eval_nl(3, status);

  if (status == GL_FALSE)
    throw rutz::error("GLCanvas::screenFromWorld3(): gluProject error",
                      SRC_POS);

  return vec3i(int(screen_x), int(screen_y), int(screen_z));
}

vec3d GLCanvas::worldFromScreen3(const vec3i& screen_pos) const
{
DOTRACE("GLCanvas::worldFromScreen3");

  dbg_eval(3, screen_pos.x());
  dbg_eval(3, screen_pos.y());
  dbg_eval_nl(3, screen_pos.z());

  GLdouble current_mv_matrix[16];
  GLdouble current_proj_matrix[16];
  GLint current_viewport[4];

  glGetDoublev(GL_MODELVIEW_MATRIX, current_mv_matrix);
  glGetDoublev(GL_PROJECTION_MATRIX, current_proj_matrix);
  glGetIntegerv(GL_VIEWPORT, current_viewport);

  vec3d world_pos;

  GLint status =
    gluUnProject(screen_pos.x(), screen_pos.y(), 0,
                 current_mv_matrix, current_proj_matrix, current_viewport,
                 &world_pos.x(), &world_pos.y(), &world_pos.z());

  dbg_eval_nl(3, status);

  if (status == GL_FALSE)
    throw rutz::error("GLCanvas::worldFromScreen3(): gluUnProject error",
                      SRC_POS);

  return world_pos;
}


geom::rect<int> GLCanvas::getScreenViewport() const
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  return geom::rect<int>::lbwh(viewport[0], viewport[1],
                               viewport[2], viewport[3]);
}

geom::rect<double> GLCanvas::getWorldViewport() const
{
  return worldFromScreenRect(getScreenViewport());
}


bool GLCanvas::isRgba() const
{
DOTRACE("GLCanvas::isRgba");

  return rep->opts->rgbaFlag;
}

bool GLCanvas::isColorIndex() const
{
DOTRACE("GLCanvas::isColorIndex");

  return !(rep->opts->rgbaFlag);
}

bool GLCanvas::isDoubleBuffered() const
{
DOTRACE("GLCanvas::isDoubleBuffered");

  return rep->glx->isDoubleBuffered();
}

unsigned int GLCanvas::bitsPerPixel() const
{
DOTRACE("GLCanvas::bitsPerPixel");

  return rep->glx->bitsPerPixel();
}

void GLCanvas::throwIfError(const char* where,
                            const rutz::file_pos& pos) const
{
DOTRACE("GLCanvas::throwIfError");
  GLenum status = glGetError();
  if (status != GL_NO_ERROR)
    {
      const char* msg =
        reinterpret_cast<const char*>(gluErrorString(status));
      throw rutz::error(rutz::fstring("GL error: ", msg, " ", where), pos);
    }
}


namespace
{
  GLint attribStackDepth()
  {
    GLint d = -1;
    glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &d);
    return d;
  }
}

void GLCanvas::pushAttribs(const char* /*comment*/)
{
DOTRACE("GLCanvas::pushAttribs");
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  dbg_eval_nl(3, attribStackDepth());
}

void GLCanvas::popAttribs()
{
DOTRACE("GLCanvas::popAttribs");
  glPopAttrib();
  dbg_eval_nl(3, attribStackDepth());
}

void GLCanvas::drawOnFrontBuffer()
{
DOTRACE("GLCanvas::drawOnFrontBuffer");
  glDrawBuffer(GL_FRONT);
}

void GLCanvas::drawOnBackBuffer()
{
DOTRACE("GLCanvas::drawOnBackBuffer");
  glDrawBuffer(GL_BACK);
}

void GLCanvas::setColor(const Gfx::RgbaColor& rgba)
{
DOTRACE("GLCanvas::setColor");
  glColor4dv(rgba.data());
}

void GLCanvas::setClearColor(const Gfx::RgbaColor& rgba)
{
DOTRACE("GLCanvas::setClearColor");
  glClearColor(rgba.r(), rgba.g(), rgba.b(), rgba.a());
}

void GLCanvas::setColorIndex(unsigned int index)
{
DOTRACE("GLCanvas::setColorIndex");
  glIndexi(index);
}

void GLCanvas::setClearColorIndex(unsigned int index)
{
DOTRACE("GLCanvas::setClearColorIndex");
  glClearIndex(index);
}

void GLCanvas::swapForeBack()
{
DOTRACE("GLCanvas::swapForeBack");
  if ( this->isRgba() )
    {
      GLdouble foreground[4];
      GLdouble background[4];
      glGetDoublev(GL_CURRENT_COLOR, &foreground[0]);
      glGetDoublev(GL_COLOR_CLEAR_VALUE, &background[0]);

      glColor4d(background[0],
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
DOTRACE("GLCanvas::setPolygonFill");

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
DOTRACE("GLCanvas::setPointSize");

  glPointSize(size);
}

void GLCanvas::setLineWidth(double width)
{
DOTRACE("GLCanvas::setLineWidth");
  glLineWidth(width);
}

void GLCanvas::setLineStipple(unsigned short bit_pattern)
{
DOTRACE("GLCanvas::setLineStipple");
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, bit_pattern);
}

void GLCanvas::enableAntialiasing()
{
DOTRACE("GLCanvas::enableAntialiasing");

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
DOTRACE("GLCanvas::viewport");

  glViewport(x, y, w, h);
  dbg_eval(2, x); dbg_eval(2, y); dbg_eval(2, w); dbg_eval_nl(2, h);
  if (GET_DBG_LEVEL() >= 8)
    {
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      dbg_eval(2, viewport[0]);
      dbg_eval(2, viewport[1]);
      dbg_eval(2, viewport[2]);
      dbg_eval_nl(2, viewport[3]);
    }
}

void GLCanvas::orthographic(const geom::rect<double>& bounds,
                            double zNear, double zFar)
{
DOTRACE("GLCanvas::orthographic");

  dbg_eval(3, bounds.left()); dbg_eval_nl(3, bounds.right());
  dbg_eval(3, bounds.bottom()); dbg_eval_nl(3, bounds.top());
  dbg_eval(3, zNear); dbg_eval_nl(3, zFar);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(bounds.left(), bounds.right(),
          bounds.bottom(), bounds.top(),
          zNear, zFar);
}

void GLCanvas::perspective(double fovy, double aspect,
                           double zNear, double zFar)
{
DOTRACE("GLCanvas::perspective");

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, aspect, zNear, zFar);
}


void GLCanvas::pushMatrix(const char* /*comment*/)
{
DOTRACE("GLCanvas::pushMatrix");
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
}

void GLCanvas::popMatrix()
{
DOTRACE("GLCanvas::popMatrix");
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void GLCanvas::translate(const vec3d& v)
{
DOTRACE("GLCanvas::translate");
  glTranslated(v.x(), v.y(), v.z());
}

void GLCanvas::scale(const vec3d& v)
{
DOTRACE("GLCanvas::scale");
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
}

void GLCanvas::rotate(const vec3d& v, double angle_in_degrees)
{
DOTRACE("GLCanvas::rotate");
  glRotated(angle_in_degrees, v.x(), v.y(), v.z());
}

void GLCanvas::transform(const geom::txform& tx)
{
DOTRACE("GLCanvas::transform");
  glMultMatrixd(tx.col_major_data());
}

namespace
{
  bool rasterPositionValid() throw()
  {
    GLboolean value = GL_FALSE;
    glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &value);
    return (value == GL_TRUE);
  }
}

void GLCanvas::rasterPos(const geom::vec2<double>& world_pos)
{
DOTRACE("GLCanvas::rasterPos");

  const geom::rect<int> viewport = getScreenViewport();

  const vec2i screen_pos = screenFromWorld2(world_pos);

  dbg_eval(3, world_pos.x()); dbg_eval_nl(3, world_pos.y());
  dbg_eval(3, screen_pos.x()); dbg_eval_nl(3, screen_pos.y());

  if (viewport.contains(screen_pos))
    {
      glRasterPos2d(world_pos.x(), world_pos.y());
    }
  else
    {
      // OK... in this case, our desired raster position actually
      // falls outside the onscreen viewport. If we just called
      // glRasterPos() with that position, it would recognize it as an
      // invalid point and then subsequent glDrawPixels() calls would
      // fail. To trick OpenGL in using the position we want, we first
      // do a glRasterPos() to some valid position -- in this case, we
      // pick the lower left corner of the viewport with coords
      // (0,0). Then we do a glBitmap() call whose only purpose is to
      // use the "xmove" and "ymove" arguments to adjust the raster
      // position.
      const vec2d lower_left = worldFromScreen2(vec2i(0,0));
      dbg_eval(3, lower_left.x()); dbg_eval_nl(3, lower_left.y());
      glRasterPos2d(lower_left.x(), lower_left.y());

      if (!rasterPositionValid())
        throw rutz::error("couldn't set valid raster position", SRC_POS);

      glBitmap(0, 0, 0.0f, 0.0f,
               screen_pos.x(),
               screen_pos.y(),
               static_cast<const GLubyte*>(0));
    }

  POSTCONDITION(rasterPositionValid());
}

void GLCanvas::drawPixels(const media::bmap_data& data,
                          const vec2d& world_pos,
                          const vec2d& zoom)
{
DOTRACE("GLCanvas::drawPixels");

  data.set_row_order(media::bmap_data::BOTTOM_FIRST);

  rasterPos(world_pos);

  glPixelZoom(zoom.x(), zoom.y());

  glPixelStorei(GL_UNPACK_ALIGNMENT, data.byte_alignment());

  if (data.bits_per_pixel() == 32)
    {
      glDrawPixels(data.width(), data.height(), GL_RGBA, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytes_ptr()));
    }
  else if (data.bits_per_pixel() == 24)
    {
      glDrawPixels(data.width(), data.height(), GL_RGB, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytes_ptr()));
    }
  else if (data.bits_per_pixel() == 8)
    {
      if (isRgba())
        {
          glDrawPixels(data.width(), data.height(),
                       GL_LUMINANCE, GL_UNSIGNED_BYTE,
                       static_cast<GLvoid*>(data.bytes_ptr()));
        }
      else
        {
          glDrawPixels(data.width(), data.height(),
                       GL_COLOR_INDEX, GL_UNSIGNED_BYTE,
                       static_cast<GLvoid*>(data.bytes_ptr()));
        }
    }
  else if (data.bits_per_pixel() == 1)
    {
      if (isRgba())
        {
          GLfloat simplemap[] = {0.0, 1.0};

          glPixelMapfv(GL_PIXEL_MAP_I_TO_R, 2, simplemap);
          glPixelMapfv(GL_PIXEL_MAP_I_TO_G, 2, simplemap);
          glPixelMapfv(GL_PIXEL_MAP_I_TO_B, 2, simplemap);
          glPixelMapfv(GL_PIXEL_MAP_I_TO_A, 2, simplemap);
        }

      glDrawPixels(data.width(), data.height(), GL_COLOR_INDEX,
                   GL_BITMAP, static_cast<GLvoid*>(data.bytes_ptr()));
    }
}

void GLCanvas::drawBitmap(const media::bmap_data& data,
                          const vec2d& world_pos)
{
DOTRACE("GLCanvas::drawBitmap");

  data.set_row_order(media::bmap_data::BOTTOM_FIRST);

  rasterPos(world_pos);

  glBitmap(data.width(), data.height(), 0.0, 0.0, 0.0, 0.0,
           static_cast<GLubyte*>(data.bytes_ptr()));
}

void GLCanvas::grabPixels(const geom::rect<int>& bounds,
                          media::bmap_data& data_out)
{
DOTRACE("GLCanvas::grabPixels");

  const int pixel_alignment = 1;

  // NOTE: we can't just use GLCanvas::bitsPerPixel() here, since that
  // won't work in the case of a 16-bit color buffer; in that case, we
  // are still in RGBA mode, so glReadPixels() will return one byte
  // per color component (i.e. 24 bits per pixel) regardless of the
  // actual color buffer depth.
  const int bmap_bits_per_pixel = isRgba() ? 24 : 8;

  media::bmap_data new_data(bounds.size(),
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
DOTRACE("GLCanvas::clearColorBuffer");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLCanvas::clearColorBuffer(const geom::rect<int>& screen_rect)
{
DOTRACE("GLCanvas::clearColorBuffer(geom::rect)");

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

void GLCanvas::drawRect(const geom::rect<double>& rect)
{
DOTRACE("GLCanvas::drawRect");

  glRectd(rect.left(),
          rect.bottom(),
          rect.right(),
          rect.top());
}

void GLCanvas::drawCircle(double inner_radius, double outer_radius,
                          bool fill,
                          unsigned int slices, unsigned int loops)
{
DOTRACE("GLCanvas::drawCircle");

  GLUquadricObj* qobj = gluNewQuadric();

  if (qobj == 0)
    throw rutz::error("couldn't allocate GLUquadric object", SRC_POS);

  gluQuadricDrawStyle(qobj, fill ? GLU_FILL : GLU_SILHOUETTE);
  gluDisk(qobj, inner_radius, outer_radius, slices, loops);
  gluDeleteQuadric(qobj);
}

void GLCanvas::drawCylinder(double base_radius, double top_radius,
                            double height, int slices, int stacks,
                            bool fill)
{
DOTRACE("GLCanvas::drawCylinder");

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
DOTRACE("GLCanvas::drawSphere");

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
DOTRACE("GLCanvas::drawBezier4");

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
DOTRACE("GLCanvas::drawBezierFill4");

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

  glMapGrid1d(subdivisions, 0.0, 1.0);

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
{ DOTRACE("GLCanvas::beginPoints"); glBegin(GL_POINTS); }

void GLCanvas::beginLines(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginLines"); glBegin(GL_LINES); }

void GLCanvas::beginLineStrip(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginLineStrip"); glBegin(GL_LINE_STRIP); }

void GLCanvas::beginLineLoop(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginLineLoop"); glBegin(GL_LINE_LOOP); }

void GLCanvas::beginTriangles(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginTriangles"); glBegin(GL_TRIANGLES); }

void GLCanvas::beginTriangleStrip(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginTriangleStrip"); glBegin(GL_TRIANGLE_STRIP); }

void GLCanvas::beginTriangleFan(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginTriangleFan"); glBegin(GL_TRIANGLE_FAN); }

void GLCanvas::beginQuads(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginQuads"); glBegin(GL_QUADS); }

void GLCanvas::beginQuadStrip(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginQuadStrip"); glBegin(GL_QUAD_STRIP); }

void GLCanvas::beginPolygon(const char* /*comment*/)
{ DOTRACE("GLCanvas::beginPolygon"); glBegin(GL_POLYGON); }

void GLCanvas::vertex2(const vec2d& v)
{
DOTRACE("GLCanvas::vertex2");
  glVertex2d(v.x(), v.y());
}

void GLCanvas::vertex3(const vec3d& v)
{
DOTRACE("GLCanvas::vertex3");
  glVertex3d(v.x(), v.y(), v.z());
}

void GLCanvas::end()
{
DOTRACE("GLCanvas::end");
  glEnd();
}

void GLCanvas::drawRasterText(const rutz::fstring& text,
                              const GxRasterFont& font)
{
DOTRACE("GLCanvas::drawRasterText");

  glListBase( font.listBase() );

  const char* p = text.c_str();

  int line = 0;

  while (1)
    {
      int len = 0;
      while (p[len] != '\0' && p[len] != '\n')
        ++len;

      dbg_eval(3, len); dbg_eval_nl(3, p);

      rasterPos( vec2d(0.0, 0.0) );
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
      ASSERT(*p == '\n');
      ++p;
      ++line;
    }
}

void GLCanvas::drawVectorText(const rutz::fstring& text,
                              const GxVectorFont& font)
{
DOTRACE("GLCanvas::drawVectorText");

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
      ASSERT(*p == '\n');
      ++p;
      ++line;
    }
}

void GLCanvas::flushOutput()
{
DOTRACE("GLCanvas::flushOutput");

  if (rep->opts->doubleFlag)
    rep->glx->swapBuffers();
  else
    glFlush();
}

void GLCanvas::finishDrawing()
{
DOTRACE("GLCanvas::finishDrawing");
  glFinish();
}

int GLCanvas::genLists(int num)
{
DOTRACE("GLCanvas::genLists");
  const int i = glGenLists(num);

  if (i == 0)
    throw rutz::error("Couldn't allocate GL display list", SRC_POS);

  return i;
}

void GLCanvas::deleteLists(int start, int num)
{
DOTRACE("GLCanvas::deleteLists");
  glDeleteLists(start, num);
}

void GLCanvas::newList(int i, bool do_execute)
{
DOTRACE("GLCanvas::newList");

 glNewList(i,
           do_execute
           ? GL_COMPILE_AND_EXECUTE
           : GL_COMPILE);
}

void GLCanvas::endList()
{
DOTRACE("GLCanvas::endList");
  glEndList();
}

bool GLCanvas::isList(int i)
{
DOTRACE("GLCanvas::isList");
  return i != 0 && glIsList(i) == GL_TRUE;
}

void GLCanvas::callList(int i)
{
DOTRACE("GLCanvas::callList");
  glCallList(i);
}

void GLCanvas::light(int lightnum,
                     const Gfx::RgbaColor* spec,
                     const Gfx::RgbaColor* diff,
                     const Gfx::RgbaColor* ambi,
                     const vec3d* posi,
                     const vec3d* sdir,
                     double attenuation,
                     double spotExponent,
                     double spotCutoff)
{
DOTRACE("GLCanvas::light");

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
      const GLfloat w = (attenuation == 0.0) ? 0.0 : 1.0;
      const GLfloat m = (attenuation == 0.0) ? 1.0 : (1.0/attenuation);

      const GLfloat fposition[] = { m*posi->x(), m*posi->y(), m*posi->z(), w };
      glLightfv(GL_LIGHT0+lightnum, GL_POSITION, fposition);
    }

  if (sdir != 0)
    {
      const GLfloat fdirection[] = { sdir->x(), sdir->y(), sdir->z(), 0.0 };

      glLightfv(GL_LIGHT0+lightnum, GL_SPOT_DIRECTION, fdirection);
      glLightf(GL_LIGHT0+lightnum, GL_SPOT_EXPONENT, spotExponent);
      glLightf(GL_LIGHT0+lightnum, GL_SPOT_CUTOFF, spotCutoff);
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
    glMaterialf(GL_FRONT, GL_SHININESS, *shininess);
}

static const char vcid_glcanvas_cc[] = "$Header$";
#endif // !GLCANVAS_CC_DEFINED
