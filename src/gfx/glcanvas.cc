///////////////////////////////////////////////////////////////////////
//
// glcanvas.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Dec  6 20:28:36 1999
// written: Mon Aug 13 14:37:47 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCANVAS_CC_DEFINED
#define GLCANVAS_CC_DEFINED

#include "glcanvas.h"

#include "gfx/bmapdata.h"
#include "gfx/rect.h"
#include "gfx/vec2.h"
#include "gfx/vec3.h"

#include "util/error.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "util/debug.h"
#include "util/trace.h"

GLCanvas::~GLCanvas() {}

Gfx::Vec2<int> GLCanvas::screenFromWorld(
  const Gfx::Vec2<double>& world_pos
  ) const
{
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

  DebugEval(status);

  if (status == GL_FALSE)
    throw Util::Error("GrObj::screenFromWorld(): gluProject error");

  return Gfx::Vec2<int>(int(temp_screen_x), int(temp_screen_y));
}

Gfx::Vec2<double> GLCanvas::worldFromScreen(
  const Gfx::Vec2<int>& screen_pos
  ) const
{
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

  DebugEval(status);

  if (status == GL_FALSE)
    throw Util::Error("GrObj::worldFromScreen(): gluUnProject error");

  return world_pos;
}


Gfx::Rect<int> GLCanvas::screenFromWorld(const Gfx::Rect<double>& world_pos) const
{
  Gfx::Rect<int> screen_rect;
  screen_rect.setBottomLeft( screenFromWorld(world_pos.bottomLeft()) );
  screen_rect.setTopRight  ( screenFromWorld(world_pos.topRight())   );
  return screen_rect;
}

Gfx::Rect<double> GLCanvas::worldFromScreen(const Gfx::Rect<int>& screen_pos) const
{
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
  GLboolean val;
  glGetBooleanv(GL_RGBA_MODE, &val);
  return (val == GL_TRUE);
}

bool GLCanvas::isColorIndex() const
{
DOTRACE("GLCanvas::isColorIndex");
  GLboolean val;
  glGetBooleanv(GL_INDEX_MODE, &val);
  return (val == GL_TRUE);
}

bool GLCanvas::isDoubleBuffered() const
{
DOTRACE("GLCanvas::isDoubleBuffered");
  GLboolean val;
  glGetBooleanv(GL_DOUBLEBUFFER, &val);
  return (val == GL_TRUE);
}

unsigned int GLCanvas::bitsPerPixel() const
{
DOTRACE("GLCanvas::bitsPerPixel");

  if (isRgba()) return 24;
  return 8;
}

void GLCanvas::drawPixels(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos,
                          const Gfx::Vec2<double>& zoom) const
{
DOTRACE("GLCanvas::drawPixels");

  glRasterPos2d(world_pos.x(), world_pos.y());
  glPixelZoom(zoom.x(), zoom.y());

  glPixelStorei(GL_UNPACK_ALIGNMENT, data.byteAlignment());

  if (data.bitsPerPixel() == 24)
    {
      glDrawPixels(data.width(), data.height(), GL_RGB, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytesPtr()));
    }
  else if (data.bitsPerPixel() == 8)
    {
      glDrawPixels(data.width(), data.height(), GL_COLOR_INDEX, GL_UNSIGNED_BYTE,
                   static_cast<GLvoid*>(data.bytesPtr()));
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

      glDrawPixels(data.width(), data.height(), GL_COLOR_INDEX, GL_BITMAP,
                   static_cast<GLvoid*>(data.bytesPtr()));
    }
}

void GLCanvas::drawBitmap(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos) const
{
DOTRACE("GLCanvas::drawBitmap");

  glRasterPos2d(world_pos.x(), world_pos.y());

  glBitmap(data.width(), data.height(), 0.0, 0.0, 0.0, 0.0,
           static_cast<GLubyte*>(data.bytesPtr()));
}

void GLCanvas::grabPixels(const Gfx::Rect<int>& bounds, Gfx::BmapData& data_out) const
{
DOTRACE("GLCanvas::grabPixels");

  const int pixel_alignment = 1;

  Gfx::BmapData new_data(bounds.extent(), bitsPerPixel(), pixel_alignment);

  glPixelStorei(GL_PACK_ALIGNMENT, pixel_alignment);

  glPushAttrib(GL_PIXEL_MODE_BIT);
  {
    glReadBuffer(GL_FRONT);
    glReadPixels(bounds.left(), bounds.bottom(), bounds.width(), bounds.height(),
                 (isRgba() ? GL_RGB : GL_COLOR_INDEX),
                 GL_UNSIGNED_BYTE, new_data.bytesPtr());
  }
  glPopAttrib();

  data_out.swap(new_data);
}

void GLCanvas::swapForeBack() const
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

void GLCanvas::flushOutput() const
{
DOTRACE("GLCanvas::flushOutput");
  glFlush();
}

void GLCanvas::clearColorBuffer() const
{
DOTRACE("GLCanvas::clearColorBuffer");
  glClear(GL_COLOR_BUFFER_BIT);
}

void GLCanvas::clearColorBuffer(const Gfx::Rect<int>& screen_rect) const
{
DOTRACE("GLCanvas::clearColorBuffer(Gfx::Rect)");

  glPushAttrib(GL_SCISSOR_BIT);
  {
    glEnable(GL_SCISSOR_TEST);

    glScissor(screen_rect.left(), screen_rect.bottom(),
              screen_rect.width(), screen_rect.height());

    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
  }
  glPopAttrib();
}

void GLCanvas::drawOnFrontBuffer() const
{
DOTRACE("GLCanvas::drawOnFrontBuffer");
  glDrawBuffer(GL_FRONT);
}

void GLCanvas::drawOnBackBuffer() const
{
DOTRACE("GLCanvas::drawOnBackBuffer");
  glDrawBuffer(GL_BACK);
}

void GLCanvas::pushState() const
{
DOTRACE("GLCanvas::pushState");
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
}

void GLCanvas::popState() const
{
DOTRACE("GLCanvas::popState");
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}


void GLCanvas::translate(const Gfx::Vec3<double>& v) const
{
DOTRACE("GLCanvas::translate");
  glTranslated(v.x(), v.y(), v.z());
}

void GLCanvas::scale(const Gfx::Vec3<double>& v) const
{
DOTRACE("GLCanvas::scale");
  glScaled(v.x(), v.y(), v.z());
}

void GLCanvas::rotate(const Gfx::Vec3<double>& v, double angle_in_degrees) const
{
DOTRACE("GLCanvas::rotate");
  glRotated(angle_in_degrees, v.x(), v.y(), v.z());
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

static const char vcid_glcanvas_cc[] = "$Header$";
#endif // !GLCANVAS_CC_DEFINED
