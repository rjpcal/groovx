/** @file gfx/pscanvas.h Gfx::Canvas subclass that writes to a
    PostScript file */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2001-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Mon Aug 27 17:18:49 2001
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#ifndef GROOVX_GFX_PSCANVAS_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_PSCANVAS_H_UTC20050626084024_DEFINED

#include "gfx/canvas.h"

namespace Gfx
{
  class PSCanvas;
}

/// Gfx::PSCanvas implements Gfx::Canvas using PostScript commands.
class Gfx::PSCanvas : public Gfx::Canvas
{
public:
  PSCanvas(const char* filename);

  virtual ~PSCanvas() noexcept;

  virtual geom::vec3<double> screenFromWorld3(const geom::vec3<double>& world_pos) const override;
  virtual geom::vec3<double> worldFromScreen3(const geom::vec3<double>& screen_pos) const override;

  virtual geom::rect<int> getScreenViewport() const override;


  virtual bool isRgba() const override;
  virtual bool isColorIndex() const override;
  virtual bool isDoubleBuffered() const override;

  virtual unsigned int bitsPerPixel() const override;

  virtual void throwIfError(const char* where,
                            const rutz::file_pos& pos) const override;


  virtual void pushAttribs(const char* comment="") override;
  virtual void popAttribs() override;

  virtual void drawOnFrontBuffer() override;
  virtual void drawOnBackBuffer() override;

  virtual void setColor(const Gfx::RgbaColor& rgba) override;
  virtual void setClearColor(const Gfx::RgbaColor& rgba) override;

  virtual void setColorIndex(unsigned int index) override;
  virtual void setClearColorIndex(unsigned int index) override;

  /** Swaps the foreground and background colors, in a way that is
      appropriate to color-index/RGBA modes. */
  virtual void swapForeBack() override;

  virtual void setPolygonFill(bool on) override;
  virtual void setPointSize(double size) override;
  virtual void setLineWidth(double width) override;
  virtual void setLineStipple(unsigned short bit_pattern) override;

  virtual void enableAntialiasing() override;



  virtual void viewport(int x, int y, int w, int h) override;

  virtual void orthographic(const geom::rect<double>& bounds,
                            double zNear, double zFar) override;

  virtual void perspective(double fovy, double aspect,
                           double zNear, double zFar) override;


  virtual void pushMatrix(const char* comment="") override;
  virtual void popMatrix() override;

  virtual void translate(const geom::vec3<double>& v) override;
  virtual void scale(const geom::vec3<double>& v) override;
  virtual void rotate(const geom::vec3<double>& v, double degrees) override;

  virtual void transform(const geom::txform& tx) override;
  virtual void loadMatrix(const geom::txform& tx) override;



  virtual void drawPixels(const media::bmap_data& data,
                          const geom::vec3<double>& world_pos,
                          const geom::vec2<double>& zoom) override;

  virtual void drawBitmap(const media::bmap_data& data,
                          const geom::vec3<double>& world_pos) override;

  virtual media::bmap_data grabPixels(const geom::rect<int>& bounds) override;

  virtual void clearColorBuffer() override;
  virtual void clearColorBuffer(const geom::rect<int>& screen_rect) override;

  virtual void drawRect(const geom::rect<double>& rect) override;

  virtual void drawCircle(double inner_radius, double outer_radius, bool fill,
                          unsigned int slices, unsigned int loops) override;

  virtual void drawCylinder(double base_radius, double top_radius,
                            double height, int slices, int stacks,
                            bool fill) override;

  virtual void drawSphere(double radius, int slices, int stacks,
                          bool fill) override;

  virtual void drawBezier4(const geom::vec3<double>& p1,
                           const geom::vec3<double>& p2,
                           const geom::vec3<double>& p3,
                           const geom::vec3<double>& p4,
                           unsigned int subdivisions) override;

  virtual void drawBezierFill4(const geom::vec3<double>& center,
                               const geom::vec3<double>& p1,
                               const geom::vec3<double>& p2,
                               const geom::vec3<double>& p3,
                               const geom::vec3<double>& p4,
                               unsigned int subdivisions) override;

  virtual void beginPoints(const char* comment="") override;
  virtual void beginLines(const char* comment="") override;
  virtual void beginLineStrip(const char* comment="") override;
  virtual void beginLineLoop(const char* comment="") override;
  virtual void beginTriangles(const char* comment="") override;
  virtual void beginTriangleStrip(const char* comment="") override;
  virtual void beginTriangleFan(const char* comment="") override;
  virtual void beginQuads(const char* comment="") override;
  virtual void beginQuadStrip(const char* comment="") override;
  virtual void beginPolygon(const char* comment="") override;

  virtual void vertex2(const geom::vec2<double>& v) override;
  virtual void vertex3(const geom::vec3<double>& v) override;

  virtual void end() override;

  virtual void drawRasterText(const rutz::fstring& text,
                              const GxRasterFont& font) override;
  virtual void drawVectorText(const rutz::fstring& text,
                              const GxVectorFont& font) override;

  virtual void flushOutput() override;

  class Impl;

private:
  PSCanvas(const PSCanvas&);
  PSCanvas& operator=(const PSCanvas&);

  Impl* rep;
};

#endif // !GROOVX_GFX_PSCANVAS_H_UTC20050626084024_DEFINED
