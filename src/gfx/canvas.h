///////////////////////////////////////////////////////////////////////
//
// canvas.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Nov 15 18:00:27 1999
// written: Wed Aug 22 18:34:33 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef CANVAS_H_DEFINED
#define CANVAS_H_DEFINED

namespace Gfx
{
  class BmapData;
  class Canvas;
  class RgbaColor;
  template <class V> class Rect;
  template <class V> class Vec2;
  template <class V> class Vec3;
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

class Gfx::Canvas {
public:
  /// Virtual destructor ensures proper destruction of subclasses.
  virtual ~Canvas();

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

  /// Draw pixmap data at the specified position.
  virtual void drawPixels(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos,
                          const Gfx::Vec2<double>& zoom) const = 0;

  /// Draw bitmap data at the specified position.
  virtual void drawBitmap(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos) const = 0;

  /// Read pixel data from the screen rect \a bounds into \a data_out.
  virtual void grabPixels(const Gfx::Rect<int>& bounds,
                          Gfx::BmapData& data_out) const = 0;

  /// Swap the foreground and background colors.
  virtual void swapForeBack() const = 0;

  /// Flush all pending drawing requests.
  virtual void flushOutput() const = 0;

  /// Clear the color buffer to the clear color.
  virtual void clearColorBuffer() const = 0;

  /// Clear a region of the color buffer to the clear color.
  virtual void clearColorBuffer(const Gfx::Rect<int>& screen_rect) const = 0;

  /// Select the front buffer for future drawing operations.
  virtual void drawOnFrontBuffer() const = 0;

  /// Select the back buffer for future drawing operations.
  virtual void drawOnBackBuffer() const = 0;

  /// Save the current matrix.
  virtual void pushMatrix() const = 0;

  /// Restore the previously saved matrix.
  virtual void popMatrix() const = 0;

  /// Save the entire current attrib set.
  virtual void pushAttribs() const = 0;

  /// Restore the previously saved entire attrib set.
  virtual void popAttribs() const = 0;

  typedef void (Gfx::Canvas::* Manip)() const;

  /** \c MatrixSaver handles saving and restoring of some part of the
      matrix state within a lexical scope, in an exception-safe manner. */
  template <Manip doit, Manip undoit>
  class Saver {
  public:
    /** Save the state of \a canvas. Its state will be restored to the
        saved state when the \c MatrixSaver is destroyed. */
    Saver(const Canvas& canvas) :
      itsCanvas(canvas)
    { (itsCanvas.*doit)(); }

    /// Destroy the \c MatrixSaver and restore the state of the \c Canvas.
    ~Saver()
    { (itsCanvas.*undoit)(); }

  private:
    Saver(const Saver&);
    Saver& operator=(const Saver&);

    const Canvas& itsCanvas;
  };

  /** \c MatrixSaver handles saving and restoring of the matrix within
      a lexical scope. */
  typedef Saver<&Gfx::Canvas::pushMatrix, &Gfx::Canvas::popMatrix>
  MatrixSaver;

  /** \c MatrixSaver handles saving and restoring of all of the canvas
      attribs within a lexical scope. */
  typedef Saver<&Gfx::Canvas::pushAttribs, &Gfx::Canvas::popAttribs>
  AttribSaver;

  virtual void setColor(const Gfx::RgbaColor& rgba) const = 0;
  virtual void setClearColor(const Gfx::RgbaColor& rgba) const = 0;

  virtual void setColorIndex(unsigned int index) const = 0;
  virtual void setClearColorIndex(unsigned int index) const = 0;

  virtual void setLineWidth(double width) const = 0;

  virtual void translate(const Gfx::Vec3<double>& v) const = 0;
  virtual void scale(const Gfx::Vec3<double>& v) const = 0;
  virtual void rotate(const Gfx::Vec3<double>& v, double degrees) const = 0;

  virtual void throwIfError(const char* where) const = 0;

  virtual void drawRect(const Gfx::Rect<double>& rect) const = 0;

  virtual void beginPoints() const = 0;
  virtual void beginLines() const = 0;
  virtual void beginLineStrip() const = 0;
  virtual void beginLineLoop() const = 0;
  virtual void beginTriangles() const = 0;
  virtual void beginTriangleStrip() const = 0;
  virtual void beginTriangleFan() const = 0;
  virtual void beginQuads() const = 0;
  virtual void beginQuadStrip() const = 0;
  virtual void beginPolygon() const = 0;

  virtual void end() const = 0;

#define BLOCK_TYPEDEF(name) \
  typedef Saver<&Gfx::Canvas::begin##name, &Gfx::Canvas::end> name##Block;

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

  virtual void vertex2(const Gfx::Vec2<double>& v) const = 0;
  virtual void vertex3(const Gfx::Vec3<double>& v) const = 0;
};

static const char vcid_canvas_h[] = "$Header$";
#endif // !CANVAS_H_DEFINED
