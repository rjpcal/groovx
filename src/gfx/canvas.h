///////////////////////////////////////////////////////////////////////
//
// canvas.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Nov 15 18:00:27 1999
// written: Mon Dec  6 22:23:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CANVAS_H_DEFINED
#define CANVAS_H_DEFINED

template <class V> class Point;
template <class V> class Rect;

class Canvas {
public:
  virtual ~Canvas();

  virtual Point<int> getScreenFromWorld(const Point<double>& world_pos) const = 0;
  virtual Point<double> getWorldFromScreen(const Point<int>& screen_pos) const = 0;

  virtual Rect<int> getScreenFromWorld(const Rect<double>& world_pos) const = 0;
  virtual Rect<double> getWorldFromScreen(const Rect<int>& screen_pos) const = 0;

  virtual Rect<int> getScreenViewport() const = 0;
  virtual Rect<double> getWorldViewport() const = 0;


  virtual bool isRgba() const = 0;
  virtual bool isColorIndex() const = 0;
  virtual bool isDoubleBuffered() const = 0;

  virtual void swapForeBack() const = 0;

  virtual void flushOutput() const = 0;

  virtual void pushState() const = 0;
  virtual void popState() const = 0;

  class StateSaver {
  public:
	 StateSaver(Canvas& canvas) : itsCanvas(canvas)
		{ itsCanvas.pushState(); }
	 ~StateSaver()
		{ itsCanvas.popState(); }
  private:
	 StateSaver(const StateSaver&);
	 StateSaver& operator=(const StateSaver&);

	 Canvas& itsCanvas;
  };

//    virtual void translate(const Vector3<double>& v) const = 0;
//    virtual void scale(const Vector3<double>& v) const = 0;
//    virtual void rotate(const Vector3<double>& v, double angle_in_degrees) const = 0;

//    virtual void beginPoints() const = 0;
//    virtual void beginLines() const = 0;
//    virtual void beginLineStrip() const = 0;
//    virtual void beginLineLoop() const = 0;
//    virtual void beginTriangles() const = 0;
//    virtual void beginTriangleStrip() const = 0;
//    virtual void beginTriangleFan() const = 0;
//    virtual void beginQuads() const = 0;
//    virtual void beginQuadStrip() const = 0;
//    virtual void beginPolygon() const = 0;

//    virtual void end() const = 0;

//    virtual void vertex(const Vector3<double>& v) const = 0;
};

static const char vcid_canvas_h[] = "$Header$";
#endif // !CANVAS_H_DEFINED
