///////////////////////////////////////////////////////////////////////
//
// glxrasterfont.h
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jul 20 20:09:41 2004
// commit: $Id$
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

#ifndef GLXRASTERFONT_H_DEFINED
#define GLXRASTERFONT_H_DEFINED

#include "gfx/glcanvas.h"
#include "gfx/gxrasterfont.h"

#include "gx/bbox.h"
#include "gx/rect.h"

#include "tcl/tcllistobj.h"

#include "util/cstrstream.h"
#include "util/error.h"
#include "util/strings.h"

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
  ~GlxRasterFont();

  virtual ~GxRasterFont() throw();

  static fstring pickXFont(const char* spec);

  virtual const char* fontName() const;

  virtual unsigned int listBase() const;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const;

  virtual void drawText(const char* text, Gfx::Canvas& canvas) const;

  /// Return the line height of the font, in screen coords (i.e., pixels).
  virtual int rasterHeight() const;

private:
  XFontStruct* fontInfo;
  fstring fontName;
  unsigned int listBase;
  unsigned int listCount;
};

GlxRasterFont::GlxRasterFont(const char* n) :
  fontInfo(0), fontName(n), listBase(0), listCount(0)
{
DOTRACE("GlxRasterFont::GlxRasterFont");

  if (fontname == 0)
    fontname = "fixed";

  Assert( fontname != 0 );

  static Display* dpy = 0;

  if (dpy == 0)
    {
      dpy = XOpenDisplay(getenv("DISPLAY"));
      if (dpy == 0)
        throw Util::Error("couldn't open connection to X server");
    }

  fstring xname = Impl::pickXFont(fontname);

  dbgEvalNL(2, xname.c_str());
  dbgEvalNL(2, xname);

  fontInfo = XLoadQueryFont( dpy, xname.c_str() );

  dbgEvalNL(2, fontInfo);

  if (fontInfo == 0)
    {
      throw Util::Error(fstring("couldn't load X font '", xname, "'"));
    }

  dbgEvalNL(2, fontInfo->fid);

  const int first = fontInfo->min_char_or_byte2;    dbgEval(2, first);
  const int last = fontInfo->max_char_or_byte2;     dbgEval(2, last);

  listCount = last-first+1;                dbgEvalNL(2, listCount);
  listBase = GLCanvas::genLists( last+1 ); dbgEvalNL(2, listBase);

  if (listBase==0)
    {
      XFreeFontInfo(NULL, fontInfo, 1);
      throw Util::Error(fstring("couldn't allocate GL display lists"));
    }

  glXUseXFont(fontInfo->fid,
              first,
              listCount,
              (int) listBase+first);

#if 0
  // for debugging
  for (int l = first; l < first+listCount; ++l)
    {
      double p = 4*double((l-first-1)-48)/42.0 - 2.0;
      glRasterPos2d(-1.0, p);
      glCallList(l);
    }
  glFlush();
#endif
}

GxRasterFont::~GlxRasterFont()
{
DOTRACE("GxRasterFont::~GlxRasterFont");

  GLCanvas::deleteLists(listBase, listCount);
  XFreeFontInfo(NULL, fontInfo, 1);
}

fstring GlxRasterFont::pickXFont(const char* spec)
{
DOTRACE("GlxRasterFont::pickXFont");

  if (isdigit(spec[0]))
    {
      // e.g. 8x13, 9x15
      return fstring(spec);
    }
  else if (spec[0] == '-')
    {
      // e.g. a full X font name "-adobe-..."
      return fstring(spec);
    }
  else
    {
      const char* foundry = "*";
      fstring     family  = "helvetica";
      const char* weight  = "medium"; // black, bold, demibold, light, medium, regular
      const char* slant   = "r"; // 'i', 'o', 'r'
      const char* width   = "normal"; // condensed, normal
      const char* style   = "*"; // could by '', 'ja', 'ko', 'medium', 'sans'
      fstring     pxlsize = "12";
      const char* ptsize  = "*";
      const char* resx    = "*"; // could be 72, 75, 100
      const char* resy    = "*"; // could be 72, 75, 100
      const char* spc     = "*"; // could be 'c', 'm', 'p'
      const char* avgwid  = "*";
      const char* rgstry  = "*"; // e.g. iso8859
      const char* encdng  = "*";

      fstring     mods    = "";

      // Parse the spec as a Tcl list. This is useful for font names with
      // embedded spaces, such that the spec needs quotes or
      // braces... e.g., {"luxi mono" 34 ib}. In cases like that, we can't
      // just split the spec on whitespace.
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
      dbgEvalNL(2, buf);
      return fstring(buf);
    }
}

const char* GlxRasterFont::fontName() const
{
DOTRACE("GlxRasterFont::fontName");
  return fontName.c_str();
}

unsigned int GlxRasterFont::listBase() const
{
DOTRACE("GlxRasterFont::listBase");
  return listBase;
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

  const int asc = fontInfo->max_bounds.ascent;
  const int desc = fontInfo->max_bounds.descent;

  int maxwid = 0;

  int lines = 0;

  while (1)
    {
      int len = linelength(text);
      int wid = XTextWidth(rep->fontInfo, text, len);
      text += len;
      if (wid > maxwid)
        maxwid = wid;

      ++lines;

      if (*text == '\0')
        break;

      Assert(*text == '\n');
      ++text;
    }

  dbgEval(2, lines); dbgEval(2, asc); dbgEval(2, desc); dbgEvalNL(2, maxwid);

  Gfx::Rect<int> screen = bbox.screenFromWorld(Gfx::Rect<double>());

  screen.right() += maxwid;
  screen.bottom() -= desc + (lines - 1) * (asc+desc);
  screen.top() += asc;

  bbox.drawRect(bbox.worldFromScreen(screen));
}

void GlxRasterFont::drawText(const char* text, Gfx::Canvas& canvas) const
{
DOTRACE("GlxRasterFont::drawText");

  canvas.drawRasterText(text, *this);
}

int GlxRasterFont::rasterHeight() const
{
DOTRACE("GlxRasterFont::rasterHeight");
  const int asc = fontInfo->max_bounds.ascent;
  const int desc = fontInfo->max_bounds.descent;

  return asc + desc;
}

static const char vcid_glxrasterfont_h[] = "$Header$";
#endif // !GLXRASTERFONT_H_DEFINED
