///////////////////////////////////////////////////////////////////////
//
// glcanvas.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Dec  6 20:28:36 1999
// written: Sat Nov 23 14:03:55 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCANVAS_CC_DEFINED
#define GLCANVAS_CC_DEFINED

#include "gfx/glcanvas.h"

#include "gfx/glxopts.h"
#include "gfx/glxwrapper.h"

#include "gx/bmapdata.h"
#include "gx/rect.h"
#include "gx/rgbacolor.h"
#include "gx/txform.h"
#include "gx/vec2.h"
#include "gx/vec3.h"

#include "util/error.h"
#include "util/pointers.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "util/trace.h"
#include "util/debug.h"

class GLCanvas::Impl
{
public:
  Impl(Display* dpy, GlxWrapper* share) :
    opts(new GlxOpts),
    glx(new GlxWrapper(dpy, *opts, share))
  {}

  scoped_ptr<GlxOpts> opts;
  scoped_ptr<GlxWrapper> glx;
};

GLCanvas::GLCanvas(Display* dpy) :
  rep(new Impl(dpy, (GlxWrapper*) 0))
{
DOTRACE("GLCanvas::GLCanvas");
}

GLCanvas* GLCanvas::make(Display* dpy)
{
DOTRACE("GLCanvas::make");
  return new GLCanvas(dpy);
}

GLCanvas::~GLCanvas()
{
DOTRACE("GLCanvas::~GLCanvas");
  delete rep;
  rep = 0;
}

Visual* GLCanvas::visual() const
{
  return rep->glx->visInfo()->visual;
}

int GLCanvas::screen() const
{
  return rep->glx->visInfo()->screen;
}

void GLCanvas::makeCurrent(Window win)
{
DOTRACE("GLCanvas::makeCurrent");

  rep->glx->makeCurrent(win);

  // Check for a single/double buffering snafu
  if (rep->opts->doubleFlag == 0 && isDoubleBuffered())
    {
      // We requested single buffering but had to accept a double buffered
      // visual.  Set the GL draw buffer to be the front buffer to
      // simulate single buffering.
      glDrawBuffer(GL_FRONT);
    }

  Gfx::Canvas::setCurrent(*this);
}

Gfx::Vec2<int> GLCanvas::screenFromWorld(
  const Gfx::Vec2<double>& world_pos
  ) const
{
DOTRACE("GLCanvas::screenFromWorld(Gfx::Vec2)");

  GLdouble current_mv_matrix[16];
  GLdouble current_proj_matrix[16];
  GLint current_viewport[4];

  glGetDoublev(GL_MODELVIEW_MATRIX, current_mv_matrix);
  glGetDoublev(GL_PROJECTION_MATRIX, current_proj_matrix);
  glGetIntegerv(GL_VIEWPORT, current_viewport);

  double temp_screen_x, temp_screen_y, dummy_z;

  GLint status =
    gluProject(world_pos.x(), world_pos.y(), 0.0,
               current_mv_matrix, current_proj_matrix, current_viewport,
               &temp_screen_x, &temp_screen_y, &dummy_z);

  dbgEval(3, status);

  if (status == GL_FALSE)
    throw Util::Error("GLCanvas::screenFromWorld(): gluProject error");

  return Gfx::Vec2<int>(int(temp_screen_x), int(temp_screen_y));
}

Gfx::Vec2<double> GLCanvas::worldFromScreen(
  const Gfx::Vec2<int>& screen_pos
  ) const
{
DOTRACE("GLCanvas::worldFromScreen(Gfx::Vec2)");

  GLdouble current_mv_matrix[16];
  GLdouble current_proj_matrix[16];
  GLint current_viewport[4];

  glGetDoublev(GL_MODELVIEW_MATRIX, current_mv_matrix);
  glGetDoublev(GL_PROJECTION_MATRIX, current_proj_matrix);
  glGetIntegerv(GL_VIEWPORT, current_viewport);

  double dummy_z;

  Gfx::Vec2<double> world_pos;

  GLint status =
    gluUnProject(screen_pos.x(), screen_pos.y(), 0,
                 current_mv_matrix, current_proj_matrix, current_viewport,
                 &world_pos.x(), &world_pos.y(), &dummy_z);

  dbgEval(3, status);

  if (status == GL_FALSE)
    throw Util::Error("GLCanvas::worldFromScreen(): gluUnProject error");

  return world_pos;
}


Gfx::Rect<int> GLCanvas::screenFromWorld(const Gfx::Rect<double>& world_pos) const
{
DOTRACE("GLCanvas::screenFromWorld(Gfx::Rect)");

  Gfx::Rect<int> screen_rect;
  screen_rect.setBottomLeft( screenFromWorld(world_pos.bottomLeft()) );
  screen_rect.setTopRight  ( screenFromWorld(world_pos.topRight()) +
                             Gfx::Vec2<int>(1,1) );
  return screen_rect;
}

Gfx::Rect<double> GLCanvas::worldFromScreen(const Gfx::Rect<int>& screen_pos) const
{
DOTRACE("GLCanvas::worldFromScreen(Gfx::Rect)");

  Gfx::Rect<double> world_rect;
  world_rect.setBottomLeft( worldFromScreen(screen_pos.bottomLeft()) );
  world_rect.setTopRight  ( worldFromScreen(screen_pos.topRight())   );
  return world_rect;
}

Gfx::Rect<int> GLCanvas::getScreenViewport() const
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  Gfx::Rect<int> screen_rect;
  screen_rect.setRectXYWH(viewport[0], viewport[1],
                          viewport[2], viewport[3]);

  return screen_rect;
}

Gfx::Rect<double> GLCanvas::getWorldViewport() const
{
  return worldFromScreen(getScreenViewport());
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

  return rep->glx->visInfo()->depth;
}

void GLCanvas::throwIfError(const char* where) const
{
DOTRACE("GLCanvas::throwIfError");
  GLenum status = glGetError();
  if (status != GL_NO_ERROR)
    {
      const char* msg =
        reinterpret_cast<const char*>(gluErrorString(status));
      throw Util::Error(fstring("GL error: ", msg, " ", where));
    }
}


void GLCanvas::pushAttribs()
{
DOTRACE("GLCanvas::pushAttribs");
  glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void GLCanvas::popAttribs()
{
DOTRACE("GLCanvas::popAttribs");
  glPopAttrib();
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
}

void GLCanvas::orthographic(const Gfx::Rect<double>& bounds,
                            double zNear, double zFar)
{
DOTRACE("GLCanvas::orthographic");

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


void GLCanvas::pushMatrix()
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

void GLCanvas::translate(const Gfx::Vec3<double>& v)
{
DOTRACE("GLCanvas::translate");
  glTranslated(v.x(), v.y(), v.z());
}

void GLCanvas::scale(const Gfx::Vec3<double>& v)
{
DOTRACE("GLCanvas::scale");
  glScaled(v.x(), v.y(), v.z());
}

void GLCanvas::rotate(const Gfx::Vec3<double>& v, double angle_in_degrees)
{
DOTRACE("GLCanvas::rotate");
  glRotated(angle_in_degrees, v.x(), v.y(), v.z());
}

void GLCanvas::transform(const Gfx::Txform& tx)
{
DOTRACE("GLCanvas::transform");
  glMultMatrixd(tx.colMajorData());
}

void GLCanvas::drawPixels(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos,
                          const Gfx::Vec2<double>& zoom)
{
DOTRACE("GLCanvas::drawPixels");

  data.setRowOrder(Gfx::BmapData::BOTTOM_FIRST);

  glRasterPos2d(world_pos.x(), world_pos.y());
  glPixelZoom(zoom.x(), zoom.y());

  glPixelStorei(GL_UNPACK_ALIGNMENT, data.byteAlignment());

  if (data.bitsPerPixel() == 32)
    {
      glDrawPixels(data.width(), data.height(), GL_RGBA, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytesPtr()));
    }
  else if (data.bitsPerPixel() == 24)
    {
      glDrawPixels(data.width(), data.height(), GL_RGB, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytesPtr()));
    }
  else if (data.bitsPerPixel() == 8)
    {
      if (isRgba())
        {
          glDrawPixels(data.width(), data.height(), GL_LUMINANCE,
                       GL_UNSIGNED_BYTE, static_cast<GLvoid*>(data.bytesPtr()));
        }
      else
        {
          glDrawPixels(data.width(), data.height(), GL_COLOR_INDEX,
                       GL_UNSIGNED_BYTE, static_cast<GLvoid*>(data.bytesPtr()));
        }
    }
  else if (data.bitsPerPixel() == 1)
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
                   GL_BITMAP, static_cast<GLvoid*>(data.bytesPtr()));
    }
}

void GLCanvas::drawBitmap(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos)
{
DOTRACE("GLCanvas::drawBitmap");

  data.setRowOrder(Gfx::BmapData::BOTTOM_FIRST);

  glRasterPos2d(world_pos.x(), world_pos.y());

  glBitmap(data.width(), data.height(), 0.0, 0.0, 0.0, 0.0,
           static_cast<GLubyte*>(data.bytesPtr()));
}

void GLCanvas::grabPixels(const Gfx::Rect<int>& bounds, Gfx::BmapData& data_out)
{
DOTRACE("GLCanvas::grabPixels");

  const int pixel_alignment = 1;

  // NOTE: we can't just use GLCanvas::bitsPerPixel() here, since that won't
  // work in the case of a 16-bit color buffer; in that case, we are still in
  // RGBA mode, so glReadPixels() will return one byte per color component
  // (i.e. 24 bits per pixel) regardless of the actual color buffer depth.
  const int bmap_bits_per_pixel = isRgba() ? 24 : 8;

  Gfx::BmapData new_data(bounds.size(),
                         bmap_bits_per_pixel, pixel_alignment);

  glPixelStorei(GL_PACK_ALIGNMENT, pixel_alignment);

  glPushAttrib(GL_PIXEL_MODE_BIT);
  {
    glReadBuffer(GL_FRONT);
    glReadPixels(bounds.left(), bounds.bottom(), bounds.width(), bounds.height(),
                 (isRgba() ? GL_RGB : GL_COLOR_INDEX),
                 GL_UNSIGNED_BYTE, new_data.bytesPtr());
  }
  glPopAttrib();

  new_data.specifyRowOrder(Gfx::BmapData::BOTTOM_FIRST);

  data_out.swap(new_data);
}

void GLCanvas::clearColorBuffer()
{
DOTRACE("GLCanvas::clearColorBuffer");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLCanvas::clearColorBuffer(const Gfx::Rect<int>& screen_rect)
{
DOTRACE("GLCanvas::clearColorBuffer(Gfx::Rect)");

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

void GLCanvas::drawRect(const Gfx::Rect<double>& rect)
{
DOTRACE("GLCanvas::drawRect");

  Gfx::LineLoopBlock block(*this);

  vertex2(rect.bottomLeft());
  vertex2(rect.bottomRight());
  vertex2(rect.topRight());
  vertex2(rect.topLeft());
}

void GLCanvas::drawCircle(double inner_radius, double outer_radius, bool fill,
                          unsigned int slices, unsigned int loops)
{
DOTRACE("GLCanvas::drawCircle");

  GLUquadricObj* qobj = gluNewQuadric();

  if (qobj == 0)
    throw Util::Error("couldn't allocate GLUquadric object");

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
    throw Util::Error("couldn't allocate GLUquadric object");

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
    throw Util::Error("couldn't allocate GLUquadric object");

  gluQuadricDrawStyle(qobj, fill ? GLU_FILL : GLU_LINE);
  gluSphere(qobj, radius, slices, stacks);
  gluDeleteQuadric(qobj);
}

void GLCanvas::drawBezier4(const Gfx::Vec3<double>& p1,
                           const Gfx::Vec3<double>& p2,
                           const Gfx::Vec3<double>& p3,
                           const Gfx::Vec3<double>& p4,
                           unsigned int subdivisions)
{
DOTRACE("GLCanvas::drawBezier4");

#if 1
  // Use the generic version, since it seems to actually be faster than the
  // glEvalMesh() approach anyway
  Canvas::drawBezier4(p1, p2, p3, p4, subdivisions);

#else

  Gfx::Vec3<double> points[] =
  {
    p1, p2, p3, p4
  };

  glEnable(GL_MAP1_VERTEX_3);
  glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, points[0].data());

  glMapGrid1d(subdivisions, 0.0, 1.0);
  glEvalMesh1(GL_LINE, 0, subdivisions);
#endif
}

void GLCanvas::drawBezierFill4(const Gfx::Vec3<double>& center,
                               const Gfx::Vec3<double>& p1,
                               const Gfx::Vec3<double>& p2,
                               const Gfx::Vec3<double>& p3,
                               const Gfx::Vec3<double>& p4,
                               unsigned int subdivisions)
{
DOTRACE("GLCanvas::drawBezierFill4");

  // Looks like the the OpenGL-specific version beats the generic version
  // here, unlike for drawBezier4().

#if 0
  Canvas::drawBezierFill4(center, p1, p2, p3, p4, subdivisions);
#else
  Gfx::Vec3<double> points[] =
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

void GLCanvas::beginPoints()
{ DOTRACE("GLCanvas::beginPoints"); glBegin(GL_POINTS); }

void GLCanvas::beginLines()
{ DOTRACE("GLCanvas::beginLines"); glBegin(GL_LINES); }

void GLCanvas::beginLineStrip()
{ DOTRACE("GLCanvas::beginLineStrip"); glBegin(GL_LINE_STRIP); }

void GLCanvas::beginLineLoop()
{ DOTRACE("GLCanvas::beginLineLoop"); glBegin(GL_LINE_LOOP); }

void GLCanvas::beginTriangles()
{ DOTRACE("GLCanvas::beginTriangles"); glBegin(GL_TRIANGLES); }

void GLCanvas::beginTriangleStrip()
{ DOTRACE("GLCanvas::beginTriangleStrip"); glBegin(GL_TRIANGLE_STRIP); }

void GLCanvas::beginTriangleFan()
{ DOTRACE("GLCanvas::beginTriangleFan"); glBegin(GL_TRIANGLE_FAN); }

void GLCanvas::beginQuads()
{ DOTRACE("GLCanvas::beginQuads"); glBegin(GL_QUADS); }

void GLCanvas::beginQuadStrip()
{ DOTRACE("GLCanvas::beginQuadStrip"); glBegin(GL_QUAD_STRIP); }

void GLCanvas::beginPolygon()
{ DOTRACE("GLCanvas::beginPolygon"); glBegin(GL_POLYGON); }

void GLCanvas::vertex2(const Gfx::Vec2<double>& v)
{
DOTRACE("GLCanvas::vertex2");
  glVertex2d(v.x(), v.y());
}

void GLCanvas::vertex3(const Gfx::Vec3<double>& v)
{
DOTRACE("GLCanvas::vertex3");
  glVertex3d(v.x(), v.y(), v.z());
}

void GLCanvas::end()
{
DOTRACE("GLCanvas::end");
  glEnd();
}

void GLCanvas::flushOutput()
{
DOTRACE("GLCanvas::flushOutput");

  if (rep->opts->doubleFlag)
    rep->glx->swapBuffers();
  else
    glFlush();
}

static const char vcid_glcanvas_cc[] = "$Header$";
#endif // !GLCANVAS_CC_DEFINED
