///////////////////////////////////////////////////////////////////////
//
// canvas.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Nov 15 18:00:27 1999
// written: Wed Mar 19 17:56:06 2003
// $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef CANVAS_H_DEFINED
#define CANVAS_H_DEFINED

#include "util/object.h"

template <class T> class dynamic_block;

namespace Gfx
{
  class BmapData;
  class Canvas;
  class RgbaColor;
  class Txform;
  template <class V> class Box;
  template <class V> class Rect;
  template <class V> class Vec2;
  template <class V> class Vec3;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Canvas defines an abstract interface for drawing graphics to a
 * drawable area. Subclasses will implement the interface using
 * various graphics API's, such as OpenGL or X11.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Canvas : public virtual Util::Object
{
public:
  /// Set the current Canvas.
  static void setCurrent(Gfx::Canvas& canvas);

  /// Get the current Canvas.
  static Canvas& current();


  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Canvas();

  ///////////////////////////////////////////////////////////////////////
  //
  // Accessors
  //
  ///////////////////////////////////////////////////////////////////////


  /// Convert a point from world coordinates to screen coordinates.
  virtual Gfx::Vec2<int> screenFromWorld(const Gfx::Vec2<double>& world_pos) const = 0;

  /// Convert a point from screen coordinates to world coordinates.
  virtual Gfx::Vec2<double> worldFromScreen(const Gfx::Vec2<int>& screen_pos) const = 0;

  /// Convert a rect from screen coordinates to world coordinates.
  virtual Gfx::Rect<int> screenFromWorld(const Gfx::Rect<double>& world_pos) const = 0;

  /// Convert a rect from world coordinates to screen coordinates.
  virtual Gfx::Rect<double> worldFromScreen(const Gfx::Rect<int>& screen_pos) const = 0;

  /// Get the viewport rect in screen coordinates.
  virtual Gfx::Rect<int> getScreenViewport() const = 0;

  /// Get the viewport rect in world coordinates.
  virtual Gfx::Rect<double> getWorldViewport() const = 0;


  /// Query whether the drawable is in RGBA mode.
  virtual bool isRgba() const = 0;

  /// Query whether the drawable is in color-index mode.
  virtual bool isColorIndex() const = 0;

  /// Query whether the drawable is double-buffered.
  virtual bool isDoubleBuffered() const = 0;

  /// Query the number of bytes per pixel.
  virtual unsigned int bitsPerPixel() const = 0;

  virtual void throwIfError(const char* where) const = 0;



  typedef void (Canvas::* Manip)();

  /** \c MatrixSaver handles saving and restoring of some part of the
      matrix state within a lexical scope, in an exception-safe manner. */
  template <Manip doit, Manip undoit>
  class Saver
  {
  public:
    /** Save the state of \a canvas. Its state will be restored to the
        saved state when the \c MatrixSaver is destroyed. */
    Saver(Canvas& canvas) :
      itsCanvas(canvas)
    { (itsCanvas.*doit)(); }

    /** Save the state of \a canvas. Its state will be restored to the
        saved state when the \c MatrixSaver is destroyed. */
    template <class Arg>
    Saver(Canvas& canvas, Arg a) :
      itsCanvas(canvas)
    { (itsCanvas.*doit)(a); }

    /// Destroy the \c MatrixSaver and restore the state of the \c Canvas.
    ~Saver()
    { (itsCanvas.*undoit)(); }

  private:
    Saver(const Saver&);
    Saver& operator=(const Saver&);

    Canvas& itsCanvas;
  };

  ///////////////////////////////////////////////////////////////////////
  //
  // Attribs (saved with an AttribSaver)
  //
  ///////////////////////////////////////////////////////////////////////

  /// Save the entire current attrib set.
  virtual void pushAttribs() = 0;

  /// Restore the previously saved entire attrib set.
  virtual void popAttribs() = 0;

  /// Select the front buffer for future drawing operations.
  virtual void drawOnFrontBuffer() = 0;

  /// Select the back buffer for future drawing operations.
  virtual void drawOnBackBuffer() = 0;

  virtual void setColor(const Gfx::RgbaColor& rgba) = 0;
  virtual void setClearColor(const Gfx::RgbaColor& rgba) = 0;

  virtual void setColorIndex(unsigned int index) = 0;
  virtual void setClearColorIndex(unsigned int index) = 0;

  /// Swap the foreground and background colors.
  virtual void swapForeBack() = 0;

  /** Control whether polygons will be filled (if true), or outlined
      (if false). */
  virtual void setPolygonFill(bool on) = 0;

  /// Set the radius of rendered points.
  virtual void setPointSize(double size) = 0;

  /// Set the current line width.
  virtual void setLineWidth(double width) = 0;

  /// Set the current line stipple pattern
  virtual void setLineStipple(unsigned short bit_pattern = 0xFFFF) = 0;

  virtual void enableAntialiasing() = 0;

  ///////////////////////////////////////////////////////////////////////
  //
  // Viewport and projection
  //
  ///////////////////////////////////////////////////////////////////////

  virtual void viewport(int x, int y, int w, int h) = 0;

  virtual void orthographic(const Gfx::Rect<double>& bounds,
                            double zNear, double zFar) = 0;

  virtual void perspective(double fovy, double aspect,
                           double zNear, double zFar) = 0;

  ///////////////////////////////////////////////////////////////////////
  //
  // Transformation matrix (saved with a MatrixSaver)
  //
  ///////////////////////////////////////////////////////////////////////

  /// Save the current transformation matrix.
  virtual void pushMatrix() = 0;

  /// Restore the previously saved transformation matrix.
  virtual void popMatrix() = 0;

  virtual void translate(const Gfx::Vec3<double>& v) = 0;
  virtual void scale(const Gfx::Vec3<double>& v) = 0;
  virtual void rotate(const Gfx::Vec3<double>& v, double degrees) = 0;

  virtual void transform(const Gfx::Txform& tx) = 0;

  ///////////////////////////////////////////////////////////////////////
  //
  // Drawing
  //
  ///////////////////////////////////////////////////////////////////////

  /// Draw pixmap data at the specified position.
  virtual void drawPixels(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos,
                          const Gfx::Vec2<double>& zoom) = 0;

  /// Draw bitmap data at the specified position.
  virtual void drawBitmap(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos) = 0;

  /// Read pixel data from the screen rect \a bounds into \a data_out.
  virtual void grabPixels(const Gfx::Rect<int>& bounds,
                          Gfx::BmapData& data_out) = 0;

  /// Clear the color buffer to the clear color.
  virtual void clearColorBuffer() = 0;

  /// Clear a region of the color buffer to the clear color.
  virtual void clearColorBuffer(const Gfx::Rect<int>& screen_rect) = 0;

  virtual void drawRect(const Gfx::Rect<double>& rect) = 0;

  virtual void drawBox(const Gfx::Box<double>& box);

  virtual void drawCircle(double inner_radius, double outer_radius, bool fill,
                          unsigned int slices, unsigned int loops) = 0;

  virtual void drawCylinder(double base_radius, double top_radius,
                            double height, int slices, int stacks,
                            bool fill) = 0;

  virtual void drawSphere(double radius, int slices, int stacks,
                          bool fill) = 0;

  virtual void drawBezier4(const Gfx::Vec3<double>& p1,
                           const Gfx::Vec3<double>& p2,
                           const Gfx::Vec3<double>& p3,
                           const Gfx::Vec3<double>& p4,
                           unsigned int subdivisions);

  virtual void drawBezierFill4(const Gfx::Vec3<double>& center,
                               const Gfx::Vec3<double>& p1,
                               const Gfx::Vec3<double>& p2,
                               const Gfx::Vec3<double>& p3,
                               const Gfx::Vec3<double>& p4,
                               unsigned int subdivisions);

  /** Draw a NURBS curve. The default implementation splits the NURBS
      curve into 4-pt Bezier curve components, and then draws those
      with drawBezier4(). */
  virtual void drawNurbsCurve(const dynamic_block<float>& knots,
                              const dynamic_block<Gfx::Vec3<float> >& pts);

  virtual void beginPoints() = 0;
  virtual void beginLines() = 0;
  virtual void beginLineStrip() = 0;
  virtual void beginLineLoop() = 0;
  virtual void beginTriangles() = 0;
  virtual void beginTriangleStrip() = 0;
  virtual void beginTriangleFan() = 0;
  virtual void beginQuads() = 0;
  virtual void beginQuadStrip() = 0;
  virtual void beginPolygon() = 0;

  enum VertexStyle
    {
      POINTS,
      LINES,
      LINE_STRIP,
      LINE_LOOP,
      TRIANGLES,
      TRIANGLE_STRIP,
      TRIANGLE_FAN,
      QUADS,
      QUAD_STRIP,
      POLYGON
    };

  void begin(VertexStyle s);

  virtual void vertex2(const Gfx::Vec2<double>& v) = 0;
  virtual void vertex3(const Gfx::Vec3<double>& v) = 0;

  virtual void end() = 0;

  /// Flush all pending drawing requests.
  virtual void flushOutput() = 0;
};

#define BLOCK_TYPEDEF(name) \
  typedef Canvas::Saver<&Canvas::begin##name, &Canvas::end> name##Block;

  BLOCK_TYPEDEF(Points);
  BLOCK_TYPEDEF(Lines);
  BLOCK_TYPEDEF(LineStrip);
  BLOCK_TYPEDEF(LineLoop);
  BLOCK_TYPEDEF(Triangles);
  BLOCK_TYPEDEF(TriangleStrip);
  BLOCK_TYPEDEF(TriangleFan);
  BLOCK_TYPEDEF(Quads);
  BLOCK_TYPEDEF(QuadStrip);
  BLOCK_TYPEDEF(Polygon);

#undef BLOCK_TYPEDEF

  /** \c MatrixSaver handles saving and restoring of the matrix within
      a lexical scope. */
  typedef Canvas::Saver<&Canvas::pushMatrix, &Canvas::popMatrix>
  MatrixSaver;

  /** \c MatrixSaver handles saving and restoring of all of the canvas
      attribs within a lexical scope. */
  typedef Canvas::Saver<&Canvas::pushAttribs, &Canvas::popAttribs>
  AttribSaver;

} // end namespace Gfx

static const char vcid_canvas_h[] = "$Header$";
#endif // !CANVAS_H_DEFINED
