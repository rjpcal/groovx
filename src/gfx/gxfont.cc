///////////////////////////////////////////////////////////////////////
//
// gxfont.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 12 18:35:02 2002
// written: Wed Nov 13 14:37:53 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXFONT_CC_DEFINED
#define GXFONT_CC_DEFINED

#include "gxfont.h"

#include "system/system.h"

#include "util/error.h"

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#include "util/debug.h"
#include "util/trace.h"

struct GxFont::Impl
{
  Impl() : fontInfo(0), listBase(0), listCount(0) {}

  XFontStruct* fontInfo;
  unsigned int listBase;
  unsigned int listCount;
};

GxFont::GxFont(const char* fontname) :
  rep(new Impl)
{
DOTRACE("GxFont::GxFont");

  if (fontname == 0)
    fontname = "fixed";

  Assert( fontname != 0 );

  Display* dpy = XOpenDisplay(System::theSystem().getenv("DISPLAY"));

  rep->fontInfo = XLoadQueryFont( dpy, fontname );
  dbgEval(2, rep->fontInfo); dbgEvalNL(2, rep->fontInfo->fid);

  if (rep->fontInfo == 0)
    {
      throw Util::Error(fstring("couldn't load X font '", fontname, "'"));
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

GxFont::~GxFont()
{
DOTRACE("GxFont::~GxFont");

  glDeleteLists(rep->listBase, rep->listCount);
  XFreeFontInfo(NULL, rep->fontInfo, 1);

  delete rep;
  rep = 0;
}

unsigned int GxFont::listBase() const
{
DOTRACE("GxFont::listBase");
  return rep->listBase;
}

int GxFont::widthOf(const char* text) const
{
  int w = 0;
  while (*text != '\0')
    {
      w += rep->fontInfo->per_char[*text].width;
      ++text;
    }

  return w;
}

int GxFont::heightOf(const char* text) const
{
  int desc = 0;
  int asc = 0;

  while (*text != '\0')
    {
      XCharStruct& ch = rep->fontInfo->per_char[*text];

      if (ch.ascent > asc)
        asc = ch.ascent;

      if (ch.descent > desc)
        desc = ch.descent;

      ++text;
    }

  return desc+asc;
}

int GxFont::ascentOf(const char* text) const
{
  int asc = 0;

  while (*text != '\0')
    {
      XCharStruct& ch = rep->fontInfo->per_char[*text];

      if (ch.ascent > asc)
        asc = ch.ascent;

      ++text;
    }

  return asc;
}

int GxFont::descentOf(const char* text) const
{
  int desc = 0;

  while (*text != '\0')
    {
      XCharStruct& ch = rep->fontInfo->per_char[*text];

      if (ch.descent > desc)
        desc = ch.descent;

      ++text;
    }

  return desc;
}

static const char vcid_gxfont_cc[] = "$Header$";
#endif // !GXFONT_CC_DEFINED
