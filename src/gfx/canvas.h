///////////////////////////////////////////////////////////////////////
//
// canvas.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 15 18:00:27 1999
// written: Fri Nov 10 17:03:51 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CANVAS_H_DEFINED
#define CANVAS_H_DEFINED

template <class V> class Point;
template <class V> class Rect;

namespace GWT {
  class Canvas;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Canvas defines an abstract interface for drawing graphics to a
 * drawable area. Subclasses will implement the interface using
 * various graphics API's, such as OpenGL or X11.
 *
 **/
///////////////////////////////////////////////////////////////////////

class GWT::Canvas {
public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Canvas();

  /// Convert a point from world coordinates to screen coordinates.
  virtual Point<int> getScreenFromWorld(const Point<double>& world_pos) const = 0;

  /// Convert a point from screen coordinates to world coordinates.
  virtual Point<double> getWorldFromScreen(const Point<int>& screen_pos) const = 0;

  /// Convert a rect from screen coordinates to world coordinates.
  virtual Rect<int> getScreenFromWorld(const Rect<double>& world_pos) const = 0;

  /// Convert a rect from world coordinates to screen coordinates.
  virtual Rect<double> getWorldFromScreen(const Rect<int>& screen_pos) const = 0;

  /// Get the viewport rect in screen coordinates.
  virtual Rect<int> getScreenViewport() const = 0;

  /// Get the viewport rect in world coordinates.
  virtual Rect<double> getWorldViewport() const = 0;


  /// Query whether the drawable is in RGBA mode.
  virtual bool isRgba() const = 0;

  /// Query whether the drawable is in color-index mode.
  virtual bool isColorIndex() const = 0;

  /// Query whether the drawable is double-buffered.
  virtual bool isDoubleBuffered() const = 0;

  /// Swap the foreground and background colors.
  virtual void swapForeBack() const = 0;

  /// Flush all pending drawing requests.
  virtual void flushOutput() const = 0;

  /// Clear the color buffer to the clear color.
  virtual void clearColorBuffer() const = 0;

  /// Select the front buffer for future drawing operations.
  virtual void drawOnFrontBuffer() const = 0;

  /// Select the back buffer for future drawing operations.
  virtual void drawOnBackBuffer() const = 0;

  /// Save the current state.
  virtual void pushState() const = 0;

  /// Restore the previously saved state.
  virtual void popState() const = 0;

  /** \c StateSaver handles saving and restoring of state within a
      lexical scope. */
  class StateSaver {
  public:
	 /** Save the state of \a canvas. Its state will be restored to the
        saved state when the \c StateSaver is destroyed. */
	 StateSaver(const Canvas& canvas) : itsCanvas(canvas)
		{ itsCanvas.pushState(); }

	 /// Destroy the \c StateSaver and restore the state of the \c Canvas.
	 ~StateSaver()
		{ itsCanvas.popState(); }

  private:
	 StateSaver(const StateSaver&);
	 StateSaver& operator=(const StateSaver&);

	 const Canvas& itsCanvas;
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
