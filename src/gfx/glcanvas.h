///////////////////////////////////////////////////////////////////////
//
// glcanvas.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Dec  6 20:27:48 1999
// written: Fri Mar 28 17:56:31 2003
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

#ifndef GLCANVAS_H_DEFINED
#define GLCANVAS_H_DEFINED

#include "gfx/canvas.h"

#include "util/volatileobject.h"

#include <X11/Xlib.h>

/// GLCanvas implements Gfx::Canvas using OpenGL.
class GLCanvas : public Gfx::Canvas, public virtual Util::VolatileObject
{
protected:
  GLCanvas(Display* dpy);

public:
  static GLCanvas* make(Display* dpy);

  virtual ~GLCanvas();

  Visual* visual() const;

  int screen() const;

  void makeCurrent(Window win);

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



  virtual void viewport(int x, int y, int w, int h);

  virtual void orthographic(const Gfx::Rect<double>& bounds,
                            double zNear, double zFar);

  virtual void perspective(double fovy, double aspect,
                           double zNear, double zFar);


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

  virtual void drawCylinder(double base_radius, double top_radius,
                            double height, int slices, int stacks,
                            bool fill);

  virtual void drawSphere(double radius, int slices, int stacks,
                          bool fill);

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

  virtual void drawText(const fstring& text, const GxFont& font);

  virtual void flushOutput();

  // OpenGL-specific stuff:
  static int genLists(int num);
  static void deleteLists(int start, int num);
  void newList(int i, bool do_execute);
  void endList();
  bool isList(int i);
  void callList(int i);

  void light(int lightnum,
             const Gfx::RgbaColor* specular,
             const Gfx::RgbaColor* diffuse,
             const Gfx::RgbaColor* ambient,
             const Gfx::Vec3<double>* position,
             const Gfx::Vec3<double>* spotDirection,
             double attenuation,
             double spotExponent,
             double spotCutoff);

  void material(const Gfx::RgbaColor* specular,
                const Gfx::RgbaColor* diffuse,
                const Gfx::RgbaColor* ambient,
                const double* shininess);

private:
  GLCanvas(const GLCanvas&);
  GLCanvas& operator=(const GLCanvas&);

  class Impl;
  Impl* rep;
};

static const char vcid_glcanvas_h[] = "$Header$";
#endif // !GLCANVAS_H_DEFINED
