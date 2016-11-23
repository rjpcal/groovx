/** @file gfx/glcanvas.h Gfx::Canvas subclass that renders to an
    OpenGL context */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Dec  6 20:27:48 1999
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_GFX_GLCANVAS_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_GLCANVAS_H_UTC20050626084024_DEFINED

#include "gfx/canvas.h"

#include "nub/volatileobject.h"

#include <memory>

struct GlxOpts;
class GlWindowInterface;

/// GLCanvas implements Gfx::Canvas using OpenGL.
class GLCanvas : public Gfx::Canvas, public virtual nub::volatile_object
{
protected:
  /// Construct from windowsystem options and a windowsystem wrapper.
  GLCanvas(std::shared_ptr<GlxOpts> opts,
           std::shared_ptr<GlWindowInterface> glx);

public:
  /// Factory function.
  static GLCanvas* make(std::shared_ptr<GlxOpts> opts,
                        std::shared_ptr<GlWindowInterface> glx);

  virtual ~GLCanvas() noexcept;

  void makeCurrent();

  static nub::soft_ref<GLCanvas> getCurrent();

  void drawBufferFront() noexcept;
  void drawBufferBack() noexcept;

  virtual geom::vec3<double> screenFromWorld3(const geom::vec3<double>& world_pos) const;
  virtual geom::vec3<double> worldFromScreen3(const geom::vec3<double>& screen_pos) const;

  virtual geom::rect<int> getScreenViewport() const;


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

  virtual void orthographic(const geom::rect<double>& bounds,
                            double zNear, double zFar);

  virtual void perspective(double fovy, double aspect,
                           double zNear, double zFar);


  virtual void pushMatrix(const char* comment="");
  virtual void popMatrix();

  virtual void translate(const geom::vec3<double>& v);
  virtual void scale(const geom::vec3<double>& v);
  virtual void rotate(const geom::vec3<double>& v, double degrees);

  virtual void transform(const geom::txform& tx);
  virtual void loadMatrix(const geom::txform& tx);

  /// Do a "safe" glRasterPos().
  /** If the given position does not fall in the screen viewport, then we
      use the "fake glBitmap()" hack to get a valid raster position outside
      the viewport. */
  void rasterPos(const geom::vec3<double>& world_pos);

  virtual void drawPixels(const media::bmap_data& data,
                          const geom::vec3<double>& world_pos,
                          const geom::vec2<double>& zoom);

  virtual void drawBitmap(const media::bmap_data& data,
                          const geom::vec3<double>& world_pos);

  virtual void grabPixels(const geom::rect<int>& bounds,
                          media::bmap_data& data_out);

  virtual void clearColorBuffer();
  virtual void clearColorBuffer(const geom::rect<int>& screen_rect);

  virtual void drawRect(const geom::rect<double>& rect);

  virtual void drawCircle(double inner_radius, double outer_radius, bool fill,
                          unsigned int slices, unsigned int loops);

  virtual void drawCylinder(double base_radius, double top_radius,
                            double height, int slices, int stacks,
                            bool fill);

  virtual void drawSphere(double radius, int slices, int stacks,
                          bool fill);

  virtual void drawBezier4(const geom::vec3<double>& p1,
                           const geom::vec3<double>& p2,
                           const geom::vec3<double>& p3,
                           const geom::vec3<double>& p4,
                           unsigned int subdivisions);

  virtual void drawBezierFill4(const geom::vec3<double>& center,
                               const geom::vec3<double>& p1,
                               const geom::vec3<double>& p2,
                               const geom::vec3<double>& p3,
                               const geom::vec3<double>& p4,
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

  virtual void vertex2(const geom::vec2<double>& v);
  virtual void vertex3(const geom::vec3<double>& v);

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
  static unsigned int genLists(unsigned int num);
  /// Deallocate \a num display lists starting with the one given by \a start.
  static void deleteLists(unsigned int start, unsigned int num);
  /// Start recording the display list given by \a i.
  /** Optionally execute the commands at the same time if \a do_execute is true. */
  void newList(unsigned int i, bool do_execute);
  /// Stop recording display list commands.
  void endList();
  /// Check if the given index refers to a valid display list.
  bool isList(unsigned int i);
  /// Trigger the commands recorded in the given display list.
  void callList(unsigned int i);

  /// Specify colors and positions associated with a light source.
  void light(unsigned int lightnum,
             const Gfx::RgbaColor* specular,
             const Gfx::RgbaColor* diffuse,
             const Gfx::RgbaColor* ambient,
             const geom::vec3<double>* position,
             const geom::vec3<double>* spotDirection,
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

#endif // !GROOVX_GFX_GLCANVAS_H_UTC20050626084024_DEFINED
