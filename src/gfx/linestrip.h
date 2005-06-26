///////////////////////////////////////////////////////////////////////
//
// linestrip.h
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jun  3 17:01:33 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_GFX_LINESTRIP_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_LINESTRIP_H_UTC20050626084024_DEFINED

#include <vector>

namespace geom
{
  template <class T> class vec2;
  template <class T> class vec3;
}

namespace Gfx
{
  class Canvas;

  /// Helps to render line strips, with optional corner joining.
  class LineStrip
  {
  private:
    Gfx::Canvas* canvas;
    std::vector<geom::vec2<double> > pts;
    double width;
    bool join;
    bool loop;

  public:
    LineStrip();

    // default copy c-tor and destructor OK

    void lineJoin(bool doJoin);

    void closeLoop(bool doClose);

    void begin(Gfx::Canvas& c, double w);

    void vertex(const geom::vec2<double>& pt);

    /// Generates a series of vertex() calls to render the bezier curve.
    void drawBezier4(const geom::vec3<double>& p1,
                     const geom::vec3<double>& p2,
                     const geom::vec3<double>& p3,
                     const geom::vec3<double>& p4,
                     unsigned int subdivisions,
                     unsigned int start = 0);

    void end();

  private:
    void drawSimpleLineStrip();
    void drawJoinedLineStrip();
  };

} // end namespace Gfx

static const char vcid_groovx_gfx_linestrip_h_utc20050626084024[] = "$Id$ $HeadURL$";
#endif // !GROOVX_GFX_LINESTRIP_H_UTC20050626084024_DEFINED
