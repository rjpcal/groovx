///////////////////////////////////////////////////////////////////////
//
// glcanvas.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Dec  6 20:27:48 1999
// written: Sun Aug 26 08:35:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLCANVAS_H_DEFINED
#define GLCANVAS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CANVAS_H_DEFINED)
#include "gfx/canvas.h"
#endif

class GLCanvas : public Gfx::Canvas
{
public:
  GLCanvas(bool is_rgba, bool is_doublebuffered);

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

  virtual void drawPixels(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos,
                          const Gfx::Vec2<double>& zoom) const;

  virtual void drawBitmap(const Gfx::BmapData& data,
                          const Gfx::Vec2<double>& world_pos) const;

  virtual void grabPixels(const Gfx::Rect<int>& bounds,
                          Gfx::BmapData& data_out) const;

  /** Swaps the foreground and background colors, in a way that is
      appropriate to color-index/RGBA modes. In order to be able to
      unswap the colors, this function should normally be preceded by
      glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT), so that a
      call to glPopAttrib() will then unswap the colors. */
  virtual void swapForeBack() const;

  virtual void flushOutput() const;

  virtual void clearColorBuffer() const;
  virtual void clearColorBuffer(const Gfx::Rect<int>& screen_rect) const;

  virtual void drawOnFrontBuffer() const;
  virtual void drawOnBackBuffer() const;

  virtual void pushMatrix() const;
  virtual void popMatrix() const;

  virtual void pushAttribs() const;
  virtual void popAttribs() const;

  virtual void setColor(const Gfx::RgbaColor& rgba) const;
  virtual void setClearColor(const Gfx::RgbaColor& rgba) const;

  virtual void setColorIndex(unsigned int index) const;
  virtual void setClearColorIndex(unsigned int index) const;

  virtual void setLineWidth(double width) const;

  virtual void translate(const Gfx::Vec3<double>& v) const;
  virtual void scale(const Gfx::Vec3<double>& v) const;
  virtual void rotate(const Gfx::Vec3<double>& v, double degrees) const;

  virtual void throwIfError(const char* where) const;

  virtual void drawRect(const Gfx::Rect<double>& rect) const;

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

  virtual void vertex2(const Gfx::Vec2<double>& v) const;
  virtual void vertex3(const Gfx::Vec3<double>& v) const;

private:
  GLCanvas(const GLCanvas&);
  GLCanvas& operator=(const GLCanvas&);

  class Impl;
  Impl* itsImpl;
};

static const char vcid_glcanvas_h[] = "$Header$";
#endif // !GLCANVAS_H_DEFINED
