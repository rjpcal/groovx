///////////////////////////////////////////////////////////////////////
//
// glcanvas.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Dec  6 20:28:36 1999
// written: Fri Nov 10 17:04:00 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCANVAS_CC_DEFINED
#define GLCANVAS_CC_DEFINED

#include "glcanvas.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "util/error.h"
#include "point.h"
#include "rect.h"

#include "util/debug.h"
#include "util/trace.h"

GLCanvas::~GLCanvas() {}

Point<int> GLCanvas::getScreenFromWorld(const Point<double>& world_pos) const {
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
	 throw ErrorWithMsg("GrObj::getScreenFromWorld(): gluProject error");

  return Point<int>(int(temp_screen_x), int(temp_screen_y));
}

Point<double> GLCanvas::getWorldFromScreen(const Point<int>& screen_pos) const {
  GLdouble current_mv_matrix[16];
  GLdouble current_proj_matrix[16];
  GLint current_viewport[4];

  glGetDoublev(GL_MODELVIEW_MATRIX, current_mv_matrix);
  glGetDoublev(GL_PROJECTION_MATRIX, current_proj_matrix);
  glGetIntegerv(GL_VIEWPORT, current_viewport);

  double dummy_z;

  Point<double> world_pos;

  GLint status =
	 gluUnProject(screen_pos.x(), screen_pos.y(), 0,
					  current_mv_matrix, current_proj_matrix, current_viewport,
					  &world_pos.x(), &world_pos.y(), &dummy_z);

  DebugEval(status);

  if (status == GL_FALSE)
	 throw ErrorWithMsg("GrObj::getWorldFromScreen(): gluUnProject error");

  return world_pos;
}


Rect<int> GLCanvas::getScreenFromWorld(const Rect<double>& world_pos) const {
  Rect<int> screen_rect;
  screen_rect.setBottomLeft( getScreenFromWorld(world_pos.bottomLeft()) );
  screen_rect.setTopRight  ( getScreenFromWorld(world_pos.topRight())   );
  return screen_rect;
}

Rect<double> GLCanvas::getWorldFromScreen(const Rect<int>& screen_pos) const {
  Rect<double> world_rect;
  world_rect.setBottomLeft( getWorldFromScreen(screen_pos.bottomLeft()) );
  world_rect.setTopRight  ( getWorldFromScreen(screen_pos.topRight())   );
  return world_rect;
}

Rect<int> GLCanvas::getScreenViewport() const {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  Rect<int> screen_rect;
  screen_rect.setBottomLeft(Point<int>(viewport[0], viewport[1]));
  screen_rect.setTopRight(Point<int>(viewport[0]+viewport[2],
												 viewport[1]+viewport[3]));

  return screen_rect;
}

Rect<double> GLCanvas::getWorldViewport() const {
  return getWorldFromScreen(getScreenViewport());
}


bool GLCanvas::isRgba() const {
DOTRACE("GLCanvas::isRgba");
  GLboolean val;
  glGetBooleanv(GL_RGBA_MODE, &val);
  return (val == GL_TRUE);
}

bool GLCanvas::isColorIndex() const {
DOTRACE("GLCanvas::isColorIndex");
  GLboolean val;
  glGetBooleanv(GL_INDEX_MODE, &val);
  return (val == GL_TRUE);
}

bool GLCanvas::isDoubleBuffered() const {
DOTRACE("GLCanvas::isDoubleBuffered");
  GLboolean val;
  glGetBooleanv(GL_DOUBLEBUFFER, &val);
  return (val == GL_TRUE);
}

void GLCanvas::swapForeBack() const {
DOTRACE("GLCanvas::swapForeBack");
  if ( this->isRgba() ) {
	 GLdouble foreground[4];
	 GLdouble background[4];
	 glGetDoublev(GL_CURRENT_COLOR, &foreground[0]);
	 glGetDoublev(GL_COLOR_CLEAR_VALUE, &background[0]);
	 glColor4dv(background);
	 glClearColor(foreground[0], foreground[1],
					  foreground[2], foreground[3]);
  }
  else {
	 GLint foreground, background;
	 glGetIntegerv(GL_CURRENT_INDEX, &foreground);
	 glGetIntegerv(GL_INDEX_CLEAR_VALUE, &background);
	 glIndexi(background);
	 glClearIndex(foreground);
  }
}

void GLCanvas::flushOutput() const {
DOTRACE("GLCanvas::flushOutput");
  glFlush();
}

void GLCanvas::clearColorBuffer() const {
DOTRACE("GLCanvas::clearColorBuffer");
  glClear(GL_COLOR_BUFFER_BIT); 
}

void GLCanvas::drawOnFrontBuffer() const {
DOTRACE("GLCanvas::drawOnFrontBuffer");
  glDrawBuffer(GL_FRONT);
}

void GLCanvas::drawOnBackBuffer() const {
DOTRACE("GLCanvas::drawOnBackBuffer");
  glDrawBuffer(GL_BACK);
}

void GLCanvas::pushState() const {
DOTRACE("GLCanvas::pushState");
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
}

void GLCanvas::popState() const {
DOTRACE("GLCanvas::popState");
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}


//  void GLCanvas::translate(const Vector3<double>& v) const {
//    glTranslate3d(v.x(), v.y(), v.z());
//  }

//  void GLCanvas::scale(const Vector3<double>& v) const {
//    glScale3d(v.x(), v.y(), v.z());
//  }

//  void GLCanvas::rotate(const Vector3<double>& v, double angle_in_degrees) const {
//    glRotate(angle_in_degrees, v.x(), v.y(), v.z());
//  }

#if 0

void GLCanvas::beginPoints() const {
  glBegin(GL_POINTS);
}

void GLCanvas::beginLines() const {
  glBegin(GL_LINES);
}

void GLCanvas::beginLineStrip() const {
  glBegin(GL_LINE_STRIP);
}

void GLCanvas::beginLineLoop() const {
  glBegin(GL_LINE_LOOP);
}

void GLCanvas::beginTriangles() const {
  glBegin(GL_TRIANGLES);
}

void GLCanvas::beginTriangleStrip() const {
  glBegin(GL_TRIANGLE_STRIP);
}

void GLCanvas::beginTriangleFan() const {
  glBegin(GL_TRIANGLE_FAN);
}

void GLCanvas::beginQuads() const {
  glBegin(GL_QUADS);
}

void GLCanvas::beginQuadStrip() const {
  glBegin(GL_QUAD_STRIP);
}

void GLCanvas::beginPolygon() const {
  glBegin(GL_POLYGON);
}


void GLCanvas::end() const {
  glEnd();
}

#endif

//  void GLCanvas::vertex(const Vector3<double>& v) const {
//    glVertex3d(v.x(), v.y(), v.z());
//  }

static const char vcid_glcanvas_cc[] = "$Header$";
#endif // !GLCANVAS_CC_DEFINED
