///////////////////////////////////////////////////////////////////////
//
// canvas.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Nov 15 18:00:27 1999
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

#ifndef CANVAS_H_DEFINED
#define CANVAS_H_DEFINED

#include "nub/object.h"
#include "nub/ref.h"

namespace rutz
{
  class file_pos;
  class fstring;
  template <class T> class dynamic_block;
}

namespace geom
{
  template <class V> class box;
  template <class V> class rect;
  template <class V> class vec2;
  template <class V> class vec3;
  class txform;
}

namespace media
{
  class bmap_data;
}

class GxRasterFont;
class GxVectorFont;

namespace Gfx
{
  class Canvas;
  class RgbaColor;

///////////////////////////////////////////////////////////////////////
/**
 *
 * \c Canvas defines an abstract interface for drawing graphics to a
 * drawable area. Subclasses will implement the interface using
 * various graphics API's, such as OpenGL or X11.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Canvas : public virtual Nub::Object
{
public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Canvas() throw();

  ///////////////////////////////////////////////////////////
  //
  // Accessors
  //
  ///////////////////////////////////////////////////////////


  /// Convert a point from world coordinates to screen coordinates.
  geom::vec2<int> screenFromWorld(const geom::vec2<double>& world_pos) const;

  /// Convert a point from screen coordinates to world coordinates.
  geom::vec2<double> worldFromScreen(const geom::vec2<int>& screen_pos) const;

  /// Convert a point from world coordinates to screen coordinates.
  virtual geom::vec3<int> screenFromWorld(const geom::vec3<double>& world_pos) const = 0;

  /// Convert a point from screen coordinates to world coordinates.
  virtual geom::vec3<double> worldFromScreen(const geom::vec3<int>& screen_pos) const = 0;

  /// Convert a rect from screen coordinates to world coordinates.
  virtual geom::rect<int> screenFromWorld(const geom::rect<double>& world_pos) const = 0;

  /// Convert a rect from world coordinates to screen coordinates.
  virtual geom::rect<double> worldFromScreen(const geom::rect<int>& screen_pos) const = 0;

  /// Get the viewport rect in screen coordinates.
  virtual geom::rect<int> getScreenViewport() const = 0;

  /// Get the viewport rect in world coordinates.
  virtual geom::rect<double> getWorldViewport() const = 0;


  /// Query whether the drawable is in RGBA mode.
  virtual bool isRgba() const = 0;

  /// Query whether the drawable is in color-index mode.
  virtual bool isColorIndex() const = 0;

  /// Query whether the drawable is double-buffered.
  virtual bool isDoubleBuffered() const = 0;

  /// Query the number of bytes per pixel.
  virtual unsigned int bitsPerPixel() const = 0;

  /// Throw an exception if there has been an error.
  virtual void throwIfError(const char* where,
                            const rutz::file_pos& pos) const = 0;



  /// Function-pointer type for Canvas state-change operations.
  typedef void (Canvas::* Manip)(const char*);

  /// Function-pointer type for Canvas state-change operations.
  typedef void (Canvas::* Unmanip)();

#ifndef MEMBER_TEMPLATES_BROKEN
  /** \c MatrixSaver handles saving and restoring of some part of the
      matrix state within a lexical scope, in an exception-safe manner. */
  template <Manip doit, Unmanip undoit>
  class Saver
  {
  public:
    /** Save the state of \a canvas. Its state will be restored to the
        saved state when the \c MatrixSaver is destroyed. */
    Saver(Canvas& canvas, const char* comment="") :
      itsCanvas(canvas)
    { (itsCanvas.*doit)(comment); }

    /** Save the state of \a canvas. Its state will be restored to the
        saved state when the \c MatrixSaver is destroyed. */
    template <class Arg>
    Saver(Canvas& canvas, Arg a, const char* comment="") :
      itsCanvas(canvas)
    { (itsCanvas.*doit)(a, comment); }

    /// Destroy the \c MatrixSaver and restore the state of the \c Canvas.
    ~Saver()
    { (itsCanvas.*undoit)(); }

  private:
    Saver(const Saver&);
    Saver& operator=(const Saver&);

    Canvas& itsCanvas;
  };

#else
  // A macro-emulation for the Saver template class that can be used when
  // the compiler would otherwise crash on the templates (e.g. gcc-ssa).

  #define SAVER_CLASS(name, doit, undoit)                       \
  class name                                                    \
  {                                                             \
  public:                                                       \
    name(Canvas& canvas, const char* comment="") :              \
      itsCanvas(canvas)                                         \
    { itsCanvas.doit(comment); }                                \
                                                                \
    template <class Arg>                                        \
    name(Canvas& canvas, Arg a, const char* comment="") :       \
      itsCanvas(canvas)                                         \
    { itsCanvas.doit(a, comment); }                             \
                                                                \
    ~name()                                                     \
    { itsCanvas.undoit(); }                                     \
                                                                \
  private:                                                      \
    name(const name&);                                          \
    name& operator=(const name&);                               \
                                                                \
    Canvas& itsCanvas;                                          \
  }
#endif

  ///////////////////////////////////////////////////////////
  //
  // Attribs (saved with an AttribSaver)
  //
  ///////////////////////////////////////////////////////////

  /// Save the entire current attrib set.
  virtual void pushAttribs(const char* comment="") = 0;

  /// Restore the previously saved entire attrib set.
  virtual void popAttribs() = 0;

  /// Select the front buffer for future drawing operations.
  virtual void drawOnFrontBuffer() = 0;

  /// Select the back buffer for future drawing operations.
  virtual void drawOnBackBuffer() = 0;

  /// Set the current foreground/drawing color.
  virtual void setColor(const Gfx::RgbaColor& rgba) = 0;
  /// Set the current background/clear color.
  virtual void setClearColor(const Gfx::RgbaColor& rgba) = 0;

  /// Set the current foreground/drawing color index.
  virtual void setColorIndex(unsigned int index) = 0;
  /// Set the current background/clear color index.
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

  /// Turn on antialiasing if available.
  virtual void enableAntialiasing() = 0;

  ///////////////////////////////////////////////////////////
  //
  // Viewport and projection
  //
  ///////////////////////////////////////////////////////////

  /// Specify a viewport for the canvas.
  virtual void viewport(int x, int y, int w, int h) = 0;

  /// Specify an orthographic projection for the canvas.
  virtual void orthographic(const geom::rect<double>& bounds,
                            double zNear, double zFar) = 0;

  /// Specify an perspective projection for the canvas.
  virtual void perspective(double fovy, double aspect,
                           double zNear, double zFar) = 0;

  ///////////////////////////////////////////////////////////
  //
  // Transformation matrix (saved with a MatrixSaver)
  //
  ///////////////////////////////////////////////////////////

  /// Save the current transformation matrix.
  virtual void pushMatrix(const char* comment="") = 0;

  /// Restore the previously saved transformation matrix.
  virtual void popMatrix() = 0;

  /// Translate the coordinate system by the given vector.
  virtual void translate(const geom::vec3<double>& v) = 0;
  /// Scale/reflect the coordinate system by the given vector.
  virtual void scale(const geom::vec3<double>& v) = 0;
  /// Rotate the coordinate system around the given vector.
  virtual void rotate(const geom::vec3<double>& v, double degrees) = 0;

  /// Apply a generic transformation to the coordinate system.
  virtual void transform(const geom::txform& tx) = 0;

  ///////////////////////////////////////////////////////////
  //
  // Drawing
  //
  ///////////////////////////////////////////////////////////

  /// Draw pixmap data at the specified position.
  virtual void drawPixels(const media::bmap_data& data,
                          const geom::vec2<double>& world_pos,
                          const geom::vec2<double>& zoom) = 0;

  /// Draw 1-bit bitmap data at the specified position.
  virtual void drawBitmap(const media::bmap_data& data,
                          const geom::vec2<double>& world_pos) = 0;

  /// Read pixel data from the screen rect \a bounds into \a data_out.
  virtual void grabPixels(const geom::rect<int>& bounds,
                          media::bmap_data& data_out) = 0;

  /// Clear the color buffer to the clear color.
  virtual void clearColorBuffer() = 0;

  /// Clear a region of the color buffer to the clear color.
  virtual void clearColorBuffer(const geom::rect<int>& screen_rect) = 0;

  /// Draw a rectangle.
  virtual void drawRect(const geom::rect<double>& rect) = 0;

  /// Draw a rectangle, forcing whether it is filled or not.
  void drawRect(const geom::rect<double>& rect, bool filled);

  /// Draw a cube.
  virtual void drawBox(const geom::box<double>& box);

  /// Draw a circle.
  virtual void drawCircle(double inner_radius, double outer_radius, bool fill,
                          unsigned int slices, unsigned int loops) = 0;

  /// Draw a cylinder.
  virtual void drawCylinder(double base_radius, double top_radius,
                            double height, int slices, int stacks,
                            bool fill) = 0;

  /// Draw a sphere.
  virtual void drawSphere(double radius, int slices, int stacks,
                          bool fill) = 0;

  /// Draw a Bezier curve with 4 control points.
  virtual void drawBezier4(const geom::vec3<double>& p1,
                           const geom::vec3<double>& p2,
                           const geom::vec3<double>& p3,
                           const geom::vec3<double>& p4,
                           unsigned int subdivisions);

  /// Draw a filled Bezier curve with 4 control points.
  virtual void drawBezierFill4(const geom::vec3<double>& center,
                               const geom::vec3<double>& p1,
                               const geom::vec3<double>& p2,
                               const geom::vec3<double>& p3,
                               const geom::vec3<double>& p4,
                               unsigned int subdivisions);

  /// Draw a NURBS curve.
  /** The default implementation splits the NURBS curve into 4-pt
      Bezier curve components, and then draws those with
      drawBezier4(). */
  virtual void drawNurbsCurve
    (const rutz::dynamic_block<float>& knots,
     const rutz::dynamic_block<geom::vec3<float> >& pts);

  virtual void beginPoints(const char* comment="") = 0;         ///< Start a series of points vertices.
  virtual void beginLines(const char* comment="") = 0;          ///< Start a set of lines.
  virtual void beginLineStrip(const char* comment="") = 0;      ///< Start a continuous strip of lines.
  virtual void beginLineLoop(const char* comment="") = 0;       ///< Start a closed loop of lines.
  virtual void beginTriangles(const char* comment="") = 0;      ///< Start a set of triangles.
  virtual void beginTriangleStrip(const char* comment="") = 0;  ///< Start a continuous strip of triangles.
  virtual void beginTriangleFan(const char* comment="") = 0;    ///< Start a fan of triangles with a common anchor point.
  virtual void beginQuads(const char* comment="") = 0;          ///< Start a set of quadrilaterals.
  virtual void beginQuadStrip(const char* comment="") = 0;      ///< Start a continous strip of quadrilaterals.
  virtual void beginPolygon(const char* comment="") = 0;        ///< Start a convex polygon.

  /// Symbolic tags for the vertex-series specified by the begin*() functions.
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

  /// Start a series of vertices according to the given Vertex Style.
  void begin(VertexStyle s, const char* comment="");

  /// Put a 2-D vertex (with z = 0) into the current vertex-series.
  virtual void vertex2(const geom::vec2<double>& v) = 0;
  /// Put a 3-D vertex into the current vertex-series.
  virtual void vertex3(const geom::vec3<double>& v) = 0;

  /// End the current vertex-series.
  virtual void end() = 0;

  /// Render text with the given raster font.
  virtual void drawRasterText(const rutz::fstring& text,
                              const GxRasterFont& font) = 0;

  /// Render text with the given vector font.
  virtual void drawVectorText(const rutz::fstring& text,
                              const GxVectorFont& font) = 0;

  /// Flush all pending drawing requests; swap buffers if double-buffering.
  virtual void flushOutput() = 0;

  /// Wait (i.e., don't return) until all pending drawing requests are completed.
  /** Default implementation is a no-op. */
  virtual void finishDrawing();
};

#ifndef MEMBER_TEMPLATES_BROKEN
#define BLOCK_TYPEDEF(name) \
  typedef Canvas::Saver<&Canvas::begin##name, &Canvas::end> name##Block
#else
#define BLOCK_TYPEDEF(name) \
  SAVER_CLASS(name##Block, begin##name, end)
#endif

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

#ifndef MEMBER_TEMPLATES_BROKEN

  /** \c MatrixSaver handles saving and restoring of the matrix within
      a lexical scope. */
  typedef Canvas::Saver<&Canvas::pushMatrix, &Canvas::popMatrix>
  MatrixSaver;

  /** \c MatrixSaver handles saving and restoring of all of the canvas
      attribs within a lexical scope. */
  typedef Canvas::Saver<&Canvas::pushAttribs, &Canvas::popAttribs>
  AttribSaver;

#else
  SAVER_CLASS(MatrixSaver, pushMatrix, popMatrix);
  SAVER_CLASS(AttribSaver, pushAttribs, popAttribs);
#endif

} // end namespace Gfx

static const char vcid_canvas_h[] = "$Header$";
#endif // !CANVAS_H_DEFINED
