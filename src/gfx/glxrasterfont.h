///////////////////////////////////////////////////////////////////////
//
// glxrasterfont.h
//
// Copyright (c) 2004-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Jul 20 20:09:41 2004
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

#ifndef GLXRASTERFONT_H_DEFINED
#define GLXRASTERFONT_H_DEFINED

#include "geom/rect.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/glcanvas.h"
#include "gfx/gxrasterfont.h"

#include "tcl/tcllistobj.h"

#include "util/error.h"
#include "util/fstring.h"

#include <cctype>
#include <cstdio>
#include <cstdlib> // for getenv()
#include <cstring>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

class GlxRasterFont : public GxRasterFont
{
public:
  GlxRasterFont(const char* n);
  virtual ~GlxRasterFont() throw();

  static rutz::fstring pickXFont(const char* spec);

  virtual const char* fontName() const;

  virtual unsigned int listBase() const;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const;

  virtual void drawText(const char* text, Gfx::Canvas& canvas) const;

  /// Return the line height of the font in screen coords (i.e., pixels)
  virtual int rasterHeight() const;

private:
  XFontStruct* itsFontInfo;
  rutz::fstring itsFontName;
  unsigned int itsListBase;
  unsigned int itsListCount;
};

GlxRasterFont::GlxRasterFont(const char* fontname) :
  itsFontInfo(0),
  itsFontName(fontname ? fontname : "fixed"),
  itsListBase(0),
  itsListCount(0)
{
DOTRACE("GlxRasterFont::GlxRasterFont");

  static Display* dpy = 0;

  if (dpy == 0)
    {
      dpy = XOpenDisplay(getenv("DISPLAY"));
      if (dpy == 0)
        throw rutz::error("couldn't open X server connection", SRC_POS);
    }

  rutz::fstring xname = pickXFont(fontname);

  dbg_eval_nl(2, xname.c_str());
  dbg_eval_nl(2, xname);

  itsFontInfo = XLoadQueryFont( dpy, xname.c_str() );

  dbg_eval_nl(2, itsFontInfo);

  if (itsFontInfo == 0)
    {
      throw rutz::error(rutz::fstring("couldn't load X font '",
                                      xname, "'"), SRC_POS);
    }

  dbg_eval_nl(2, itsFontInfo->fid);

  const int first = itsFontInfo->min_char_or_byte2; dbg_eval(2, first);
  const int last = itsFontInfo->max_char_or_byte2;  dbg_eval(2, last);

  itsListCount = last-first+1;
  itsListBase = GLCanvas::genLists( last+1 );

  dbg_eval_nl(2, itsListCount);
  dbg_eval_nl(2, itsListBase);

  if (itsListBase==0)
    {
      XFreeFontInfo(NULL, itsFontInfo, 1);
      throw rutz::error("couldn't allocate GL display lists", SRC_POS);
    }

  glXUseXFont(itsFontInfo->fid,
              first,
              itsListCount,
              static_cast<int>(itsListBase+first));

#if 0
  // for debugging
  for (int l = first; l < first+itsListCount; ++l)
    {
      double p = 4*double((l-first-1)-48)/42.0 - 2.0;
      glRasterPos2d(-1.0, p);
      glCallList(l);
    }
  glFlush();
#endif
}

GlxRasterFont::~GlxRasterFont() throw()
{
DOTRACE("GlxRasterFont::~GlxRasterFont");

  GLCanvas::deleteLists(itsListBase, itsListCount);
  XFreeFontInfo(NULL, itsFontInfo, 1);
}

rutz::fstring GlxRasterFont::pickXFont(const char* spec)
{
DOTRACE("GlxRasterFont::pickXFont");

  if (isdigit(spec[0]))
    {
      // e.g. 8x13, 9x15
      return rutz::fstring(spec);
    }
  else if (spec[0] == '-')
    {
      // e.g. a full X font name "-adobe-..."
      return rutz::fstring(spec);
    }
  else
    {
      const char* foundry = "*";
      rutz::fstring family= "helvetica";
      const char* weight  = "medium"; // black, bold, demibold, light, medium, regular
      const char* slant   = "r"; // 'i', 'o', 'r'
      const char* width   = "normal"; // condensed, normal
      const char* style   = "*"; // could by '', 'ja', 'ko', 'medium', 'sans'
      rutz::fstring pxlsize= "12";
      const char* ptsize  = "*";
      const char* resx    = "*"; // could be 72, 75, 100
      const char* resy    = "*"; // could be 72, 75, 100
      const char* spc     = "*"; // could be 'c', 'm', 'p'
      const char* avgwid  = "*";
      const char* rgstry  = "*"; // e.g. iso8859
      const char* encdng  = "*";

      rutz::fstring mods  = "";

      // Parse the spec as a Tcl list. This is useful for font names
      // with embedded spaces, such that the spec needs quotes or
      // braces... e.g., {"luxi mono" 34 ib}. In cases like that, we
      // can't just split the spec on whitespace.
      Tcl::List specitems = Tcl::toTcl(spec);

      if (specitems.length() >= 1)
        family = specitems.get<const char*>(0);

      if (specitems.length() >= 2)
        pxlsize = specitems.get<const char*>(1);

      if (specitems.length() >= 3)
        mods = specitems.get<const char*>(2);

      const char* mod = mods.c_str();
      while (*mod != '\0')
        {
          switch (*mod)
            {
            case 'b': weight = "bold"; break;
            case 'i': slant = "i"; break;
            case 'o': slant = "o"; break;
            }
          ++mod;
        }

      char buf[256];

      snprintf(buf, 256, "-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s",
               foundry,
               family.c_str(),
               weight,
               slant,
               width,
               style,
               pxlsize.c_str(),
               ptsize,
               resx,
               resy,
               spc,
               avgwid,
               rgstry,
               encdng);
      dbg_eval_nl(2, buf);
      return rutz::fstring(buf);
    }
}

const char* GlxRasterFont::fontName() const
{
DOTRACE("GlxRasterFont::fontName");
  return itsFontName.c_str();
}

unsigned int GlxRasterFont::listBase() const
{
DOTRACE("GlxRasterFont::listBase");
  return itsListBase;
}

namespace
{
  int linelength(const char* p)
  {
    int n = 0;
    while (p[n] != '\n' && p[n] != '\0')
      {
        ++n;
      }
    return n;
  }
}

void GlxRasterFont::bboxOf(const char* text, Gfx::Bbox& bbox) const
{
DOTRACE("GlxRasterFont::bboxOf");

  const int asc = itsFontInfo->max_bounds.ascent;
  const int desc = itsFontInfo->max_bounds.descent;

  int maxwid = 0;

  int lines = 0;

  while (1)
    {
      int len = linelength(text);
      int wid = XTextWidth(itsFontInfo, text, len);
      text += len;
      if (wid > maxwid)
        maxwid = wid;

      ++lines;

      if (*text == '\0')
        break;

      ASSERT(*text == '\n');
      ++text;
    }

  dbg_eval(2, lines);
  dbg_eval(2, asc);
  dbg_eval(2, desc);
  dbg_eval_nl(2, maxwid);

  const int l = 0;
  const int r = maxwid;
  const int b = -desc + (lines - 1) * (asc+desc);
  const int t = asc;

  bbox.drawScreenRect(geom::vec3d::zeros(),
                      geom::rect<int>::ltrb(l,t,r,b));
}

void GlxRasterFont::drawText(const char* text,
                             Gfx::Canvas& canvas) const
{
DOTRACE("GlxRasterFont::drawText");

  canvas.drawRasterText(text, *this);
}

int GlxRasterFont::rasterHeight() const
{
DOTRACE("GlxRasterFont::rasterHeight");
  const int asc = itsFontInfo->max_bounds.ascent;
  const int desc = itsFontInfo->max_bounds.descent;

  return asc + desc;
}

static const char vcid_glxrasterfont_h[] = "$Header$";
#endif // !GLXRASTERFONT_H_DEFINED
