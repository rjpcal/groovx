///////////////////////////////////////////////////////////////////////
//
// pscanvas.h
//
// Copyright (c) 2001-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Aug 27 17:18:49 2001
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

#ifndef PSCANVAS_H_DEFINED
#define PSCANVAS_H_DEFINED

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

  virtual ~PSCanvas() throw();

  virtual geom::vec3<int> screenFromWorld(const geom::vec3<double>& world_pos) const;
  virtual geom::vec3<double> worldFromScreen(const geom::vec3<int>& screen_pos) const;

  virtual geom::rect<int> screenFromWorld(const geom::rect<double>& world_pos) const;
  virtual geom::rect<double> worldFromScreen(const geom::rect<int>& screen_pos) const;

  virtual geom::rect<int> getScreenViewport() const;
  virtual geom::rect<double> getWorldViewport() const;


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



  virtual void drawPixels(const media::bmap_data& data,
                          const geom::vec2<double>& world_pos,
                          const geom::vec2<double>& zoom);

  virtual void drawBitmap(const media::bmap_data& data,
                          const geom::vec2<double>& world_pos);

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

  class Impl;

private:
  PSCanvas(const PSCanvas&);
  PSCanvas& operator=(const PSCanvas&);

  Impl* rep;
};

static const char vcid_pscanvas_h[] = "$Header$";
#endif // !PSCANVAS_H_DEFINED
