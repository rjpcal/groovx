///////////////////////////////////////////////////////////////////////
//
// gxrasterfont.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 16:45:32 2002
// written: Thu Nov 14 15:25:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXRASTERFONT_CC_DEFINED
#define GXRASTERFONT_CC_DEFINED

#include "gxrasterfont.h"

#include "gfx/canvas.h"

#include "gx/rect.h"

#include "system/system.h"

#include "util/error.h"
#include "util/strings.h"

#include <cctype>
#include <cstdio>
#include <strstream.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#include "util/debug.h"
#include "util/trace.h"

struct GxRasterFont::Impl
{
  Impl(const char* n) : fontInfo(0), fontName(n), listBase(0), listCount(0) {}

  XFontStruct* fontInfo;
  fstring fontName;
  unsigned int listBase;
  unsigned int listCount;
};

GxRasterFont::GxRasterFont(const char* fontname) :
  rep(new Impl(fontname))
{
DOTRACE("GxRasterFont::GxRasterFont");

  if (fontname == 0)
    fontname = "fixed";

  Assert( fontname != 0 );

  Display* dpy = XOpenDisplay(System::theSystem().getenv("DISPLAY"));

  const char* xname = "";
  char buf[256];

  if (isdigit(fontname[0]))
    {
      // e.g. 8x13, 9x15
      xname = fontname;
    }
  else if (fontname[0] == '-')
    {
      // e.g. a full X font name "-adobe-..."
      xname = fontname;
    }
  else
    {
      istrstream ist(fontname);
      fstring family;
      ist >> family;
      int fontsize = 12;
      ist >> fontsize;

      const char* weight = "medium";
      const char* slant = "r";

      snprintf(buf, 256, "-*-%s-%s-%s-*-*-%d-*-*-*-*-*-*-*",
               family.c_str(), weight, slant, fontsize);
      xname = buf;
    }

  rep->fontInfo = XLoadQueryFont( dpy, xname );

  dbgEval(2, xname);
  dbgEval(2, rep->fontInfo);
  dbgEvalNL(2, rep->fontInfo->fid);

  if (rep->fontInfo == 0)
    {
      throw Util::Error(fstring("couldn't load X font '", xname, "'"));
    }

  const int first = rep->fontInfo->min_char_or_byte2;
  dbgEval(2, first);
  const int last = rep->fontInfo->max_char_or_byte2;
  dbgEval(2, last);

  rep->listCount = last-first+1;
  dbgEvalNL(2, rep->listCount);

  rep->listBase = glGenLists( last+1 );
  dbgEvalNL(2, rep->listBase);

  if (rep->listBase==0)
    {
      XFreeFontInfo(NULL, rep->fontInfo, 1);
      throw Util::Error(fstring("couldn't allocate GL display lists"));
    }

  glXUseXFont(rep->fontInfo->fid,
              first,
              rep->listCount,
              (int) rep->listBase+first);

#if 0
  // for debugging
  for (int l = first; l < first+rep->listCount; ++l)
    {
      double p = 4*double((l-first-1)-48)/42.0 - 2.0;
      glRasterPos2d(-1.0, p);
      glCallList(l);
    }
  glFlush();
#endif
}

GxRasterFont::~GxRasterFont()
{
DOTRACE("GxRasterFont::~GxRasterFont");

  glDeleteLists(rep->listBase, rep->listCount);
  XFreeFontInfo(NULL, rep->fontInfo, 1);

  delete rep;
  rep = 0;
}

const char* GxRasterFont::fontName() const
{
DOTRACE("GxRasterFont::fontName");
  return rep->fontName.c_str();
}

unsigned int GxRasterFont::listBase() const
{
DOTRACE("GxRasterFont::listBase");
  return rep->listBase;
}

Gfx::Rect<double> GxRasterFont::sizeOf(const char* text,
                                       Gfx::Canvas& canvas) const
{
  int asc = 0;
  int desc = 0;
  int wid = 0;

  while (*text != '\0')
    {
      XCharStruct& ch = rep->fontInfo->per_char[*text];

      if (ch.ascent > asc)
        asc = ch.ascent;

      if (ch.descent > desc)
        desc = ch.descent;

      wid += ch.width;

      ++text;
    }

  Gfx::Rect<int> screen = canvas.screenFromWorld(Gfx::Rect<double>());

  screen.right() += wid;
  screen.bottom() -= desc;
  screen.top() += asc;

  return canvas.worldFromScreen(screen);
}

static const char vcid_gxrasterfont_cc[] = "$Header$";
#endif // !GXRASTERFONT_CC_DEFINED
