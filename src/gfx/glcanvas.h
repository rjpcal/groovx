///////////////////////////////////////////////////////////////////////
//
// glcanvas.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Dec  6 20:27:48 1999
// written: Wed Jun 26 12:54:52 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCANVAS_H_DEFINED
#define GLCANVAS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CANVAS_H_DEFINED)
#include "gfx/canvas.h"
#endif

/// GLCanvas implements Gfx::Canvas using OpenGL.
class GLCanvas : public Gfx::Canvas
{
protected:
  GLCanvas(unsigned int bits_per_pixel,
           bool is_rgba,
           bool is_doublebuffered);

public:
  static GLCanvas* make(unsigned int bits_per_pixel,
                        bool is_rgba,
                        bool is_doublebuffered);

  virtual ~GLCanvas();

  virtual Gfx::Vec2<int> screenFromWorld(const Gfx::Vec2<double>& world_pos) const;
  virtual Gfx::Vec2<double> worldFromScreen(const Gfx::Vec2<int>& screen_pos) const;

  virtual Gfx::Rect<int> screenFromWorld(const Gfx::Rect<double>& world_pos) const;
  virtual Gfx::Rect<double> worldFromScreen(const Gfx::Rect<int>& screen_pos) const;

  virtual Gfx::Rect<int> getScreenViewport() const;
  virtual Gfx::Rect<double> getWorldViewport() const;


  virtual bool isRgba() const;
  virtual bool isColorIndex() const;
  virtual bool isDoubleBuffered() const;

  virtual unsigned int bitsPerPixel() const;

  virtual void throwIfError(const char* where) const;


  virtual void pushAttribs();
  virtual void popAttribs();

  virtual void drawOnFrontBuffer();
  virtual void drawOnBackBuffer();

  virtual void setColor(const Gfx::RgbaColor& rgba);
  virtual void setClearColor(const Gfx::RgbaColor& rgba);

  virtual void setColorIndex(unsigned int index);
  virtual void setClearColorIndex(unsigned int index);

  /** Swaps the foreground and background colors, in a way that is
      appropriate to color-index/RGBA modes. */
  virtual void swapForeBack();

  virtual void setPolygonFill(bool on);
  virtual void setPointSize(double size);
  virtual void setLineWidth(double width);
  virtual void setLineStipple(unsigned short bit_pattern);

  virtual void enableAntialiasing();



  virtual void pushMatrix();
  virtual void popMatrix();

  virtual void translate(const Gfx::Vec3<double>& v);
  virtual void scale(const Gfx::Vec3<double>& v);
  virtual void rotate(const Gfx::Vec3<double>& v, double degrees);

  virtual void transform(const Gfx::Txform& tx);


  virtual void drawPixels(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos,
                          const Gfx::Vec2<double>& zoom);

  virtual void drawBitmap(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos);

  virtual void grabPixels(const Gfx::Rect<int>& bounds,
                          Gfx::BmapData& data_out);

  virtual void clearColorBuffer();
  virtual void clearColorBuffer(const Gfx::Rect<int>& screen_rect);

  virtual void drawRect(const Gfx::Rect<double>& rect);

  virtual void drawCircle(double inner_radius, double outer_radius, bool fill,
                          unsigned int slices, unsigned int loops);

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

  virtual void beginPoints();
  virtual void beginLines();
  virtual void beginLineStrip();
  virtual void beginLineLoop();
  virtual void beginTriangles();
  virtual void beginTriangleStrip();
  virtual void beginTriangleFan();
  virtual void beginQuads();
  virtual void beginQuadStrip();
  virtual void beginPolygon();

  virtual void vertex2(const Gfx::Vec2<double>& v);
  virtual void vertex3(const Gfx::Vec3<double>& v);

  virtual void end();

  virtual void flushOutput();

private:
  GLCanvas(const GLCanvas&);
  GLCanvas& operator=(const GLCanvas&);

  class Impl;
  Impl* itsImpl;
};

static const char vcid_glcanvas_h[] = "$Header$";
#endif // !GLCANVAS_H_DEFINED
