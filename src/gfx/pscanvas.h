///////////////////////////////////////////////////////////////////////
//
// pscanvas.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Aug 27 17:18:49 2001
// written: Mon Jun 24 12:28:28 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PSCANVAS_H_DEFINED
#define PSCANVAS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(CANVAS_H_DEFINED)
#include "gfx/canvas.h"
#endif

namespace Gfx
{
  class PSCanvas;
}

class Gfx::PSCanvas : public Gfx::Canvas
{
public:
  PSCanvas(const char* filename);

  virtual ~PSCanvas();

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

  class Impl;

private:
  PSCanvas(const PSCanvas&);
  PSCanvas& operator=(const PSCanvas&);

  Impl* itsImpl;
};

static const char vcid_pscanvas_h[] = "$Header$";
#endif // !PSCANVAS_H_DEFINED
