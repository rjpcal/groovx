///////////////////////////////////////////////////////////////////////
//
// canvas.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Nov 15 18:00:38 1999
// written: Mon Nov 15 18:22:52 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CANVAS_CC_DEFINED
#define CANVAS_CC_DEFINED

#include "canvas.h"

#include <GL/gl.h>

#include "error.h"

Canvas::~Canvas() {}



class GLCanvas : public Canvas {
public:
  virtual Point<int> getScreenFromWorld(const Point<double>& world_pos) const;
  virtual Point<double> getWorldFromScreen(const Point<int>& screen_pos) const;

  virtual Rect<int> getScreenFromWorld(const Rect<double>& world_pos) const;
  virtual Rect<double> getWorldFromScreen(const Rect<int>& screen_pos) const;

  virtual Rect<int> getScreenViewport() const;
  virtual Rect<double> getWorldViewport() const;


  virtual void pushState() const;
  virtual void popState() const;

  virtual void translate(const Vector3<double>& v) const;
  virtual void scale(const Vector3<double>& v) const;
  virtual void rotate(const Vector3<double>& v, double angle_in_degrees) const;

  virtual void beginPoints() const;
  virtual void beginLines() const;
  virtual void beginLineStrip() const;
  virtual void beginLineLoop() const;
  virtual void beginTriangles() const;
  virtual void beginTriangleStrip() const;
  virtual void beginTriangleFan() const;
  virtual void beginQuads() const;
  virtual void beginQuadStrip() const;
  virtual void beginPolygon() const;

  virtual void end() const;

  virtual void vertex(const Vector3<double>& v) const;
};

namespace {
  Canvas* theSingleton = 0;
}

Canvas& Canvas::theCanvas() {
  if ( theSingleton == 0 ) {
	 theSingleton = new GLCanvas();
  }

  return *theSingleton;
}

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
}


Rect<int> GLCanvas::getScreenFromWorld(const Rect<double>& world_pos) const {
  Rect<int> screen_rect;
  screen_rect.setBottomLeft( getScreenFromWorld(world_pos.bottomLeft())      );
  screen_rect.setTopRight  ( getScreenFromWorld(world_pos.topRight(), false) );
  return screen_rect;
}

Rect<double> GLCanvas::getWorldFromScreen(const Rect<int>& screen_pos) const {
  Rect<double> world_rect;
  world_rect.setBottomLeft( getWorldFromScreen(screen_pos.bottomLeft())      );
  world_rect.setTopRight  ( getWorldFromScreen(screen_pos.topRight(), false) );
  return world_rect;
}

Rect<int> GLCanvas::getScreenViewport() const {
  GLint current_viewport[4];
  glGetIntegerv(GL_VIEWPORT, current_viewport);

  Rect<int> screen_rect;
  screen_rect.setBottomLeft(Point<int>(viewport[0], viewport[1]));
  screen_rect.setTopRight(Point<int>(viewport[0]+viewport[2],
												 viewport[1]+viewport[3]));

  return screen_rect;
}

Rect<double> GLCanvas::getWorldViewport() const {
  return getWorldFromScreen(getScreenViewport());
}



void GLCanvas::pushState() const {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
}

void GLCanvas::popState() const {
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}


void GLCanvas::translate(const Vector3<double>& v) const {
  glTranslate3d(v.x(), v.y(), v.z());
}

void GLCanvas::scale(const Vector3<double>& v) const {
  glScale3d(v.x(), v.y(), v.z());
}

void GLCanvas::rotate(const Vector3<double>& v, double angle_in_degrees) const {
  glRotate(angle_in_degrees, v.x(), v.y(), v.z());
}


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


void GLCanvas::vertex(const Vector3<double>& v) const {
  glVertex3d(v.x(), v.y(), v.z());
}


const char vcid_canvas_cc[] = "$Header$";
#endif // !CANVAS_CC_DEFINED
