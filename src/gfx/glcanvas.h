///////////////////////////////////////////////////////////////////////
//
// glcanvas.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Dec  6 20:27:48 1999
// written: Mon Dec  6 20:30:51 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCANVAS_H_DEFINED
#define GLCANVAS_H_DEFINED

#ifndef CANVAS_H_DEFINED
#include "canvas.h"
#endif

class GLCanvas : public Canvas {
public:
  static GLCanvas& theCanvas();

  virtual Point<int> getScreenFromWorld(const Point<double>& world_pos) const;
  virtual Point<double> getWorldFromScreen(const Point<int>& screen_pos) const;

  virtual Rect<int> getScreenFromWorld(const Rect<double>& world_pos) const;
  virtual Rect<double> getWorldFromScreen(const Rect<int>& screen_pos) const;

  virtual Rect<int> getScreenViewport() const;
  virtual Rect<double> getWorldViewport() const;


  virtual bool isRgba() const;
  virtual bool isColorIndex() const;
  virtual bool isDoubleBuffered() const;

  virtual void pushState() const;
  virtual void popState() const;

//    virtual void translate(const Vector3<double>& v) const;
//    virtual void scale(const Vector3<double>& v) const;
//    virtual void rotate(const Vector3<double>& v, double angle_in_degrees) const;

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

//    virtual void vertex(const Vector3<double>& v) const;
};

static const char vcid_glcanvas_h[] = "$Header$";
#endif // !GLCANVAS_H_DEFINED
