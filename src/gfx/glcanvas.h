///////////////////////////////////////////////////////////////////////
//
// glcanvas.h
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Dec  6 20:27:48 1999
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

#ifndef GLCANVAS_H_DEFINED
#define GLCANVAS_H_DEFINED

#include "gfx/canvas.h"

#include "nub/volatileobject.h"

class GlxOpts;
class GlWindowInterface;

namespace rutz
{
  template <class T> class shared_ptr;
}

/// GLCanvas implements Gfx::Canvas using OpenGL.
class GLCanvas : public Gfx::Canvas, public virtual Nub::VolatileObject
{
protected:
  /// Construct from windowsystem options and a windowsystem wrapper.
  GLCanvas(rutz::shared_ptr<GlxOpts> opts,
           rutz::shared_ptr<GlWindowInterface> glx);

public:
  /// Factory function.
  static GLCanvas* make(rutz::shared_ptr<GlxOpts> opts,
                        rutz::shared_ptr<GlWindowInterface> glx);

  virtual ~GLCanvas() throw();

  void drawBufferFront() throw();
  void drawBufferBack() throw();

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

  virtual void throwIfError(const char* where,
                            const rutz::file_pos& pos) const;


  virtual void pushAttribs(const char* comment="");
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


  virtual void pushMatrix(const char* comment="");
  virtual void popMatrix();

  virtual void translate(const Gfx::Vec3<double>& v);
  virtual void scale(const Gfx::Vec3<double>& v);
  virtual void rotate(const Gfx::Vec3<double>& v, double degrees);

  virtual void transform(const Gfx::Txform& tx);

  /// Do a "safe" glRasterPos().
  /** If the given position does not fall in the screen viewport, then we
      use the "fake glBitmap()" hack to get a valid raster position outside
      the viewport. */
  void rasterPos(const Gfx::Vec2<double>& world_pos);

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

  virtual void beginPoints(const char* comment="");
  virtual void beginLines(const char* comment="");
  virtual void beginLineStrip(const char* comment="");
  virtual void beginLineLoop(const char* comment="");
  virtual void beginTriangles(const char* comment="");
  virtual void beginTriangleStrip(const char* comment="");
  virtual void beginTriangleFan(const char* comment="");
  virtual void beginQuads(const char* comment="");
  virtual void beginQuadStrip(const char* comment="");
  virtual void beginPolygon(const char* comment="");

  virtual void vertex2(const Gfx::Vec2<double>& v);
  virtual void vertex3(const Gfx::Vec3<double>& v);

  virtual void end();

  virtual void drawRasterText(const rutz::fstring& text,
                              const GxRasterFont& font);
  virtual void drawVectorText(const rutz::fstring& text,
                              const GxVectorFont& font);

  virtual void flushOutput();

  virtual void finishDrawing();

  // OpenGL-specific stuff:

  /// Create space for a consecutive sequence of \a num display lists.
  /** The index of the first display list is returned. */
  static int genLists(int num);
  /// Deallocate \a num display lists starting with the one given by \a start.
  static void deleteLists(int start, int num);
  /// Start recording the display list given by \a i.
  /** Optionally execute the commands at the same time if \a do_execute is true. */
  void newList(int i, bool do_execute);
  /// Stop recording display list commands.
  void endList();
  /// Check if the given index refers to a valid display list.
  bool isList(int i);
  /// Trigger the commands recorded in the given display list.
  void callList(int i);

  /// Specify colors and positions associated with a light source.
  void light(int lightnum,
             const Gfx::RgbaColor* specular,
             const Gfx::RgbaColor* diffuse,
             const Gfx::RgbaColor* ambient,
             const Gfx::Vec3<double>* position,
             const Gfx::Vec3<double>* spotDirection,
             double attenuation,
             double spotExponent,
             double spotCutoff);

  /// Specify colors and attributes for a material.
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
