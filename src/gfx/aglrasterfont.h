///////////////////////////////////////////////////////////////////////
//
// aglrasterfont.h
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jul 20 20:21:35 2004
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

#ifndef AGLRASTERFONT_H_DEFINED
#define AGLRASTERFONT_H_DEFINED

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
#include <cstring>
#include <AGL/agl.h>
#include <AGL/gl.h>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

class AglRasterFont : public GxRasterFont
{
public:
  AglRasterFont(const char* n);
  virtual ~AglRasterFont() throw();

  struct AppleFontSpec
  {
    GLint fontID;
    Style face;
    GLint size;
  };

  static GLint getFontId(const char* name);

  static GLint getFontId(ConstStr255Param name);

  static AppleFontSpec pickAppleFont(const char* spec);

  virtual const char* fontName() const;

  virtual unsigned int listBase() const;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const;

  virtual void drawText(const char* text, Gfx::Canvas& canvas) const;

  /// Return the line height of the font, in screen coords (i.e., pixels).
  virtual int rasterHeight() const;

private:
  fstring itsFontName;
  unsigned int itsListBase;
  unsigned int itsListCount;
};

AglRasterFont::AglRasterFont(const char* fontname) :
  itsFontName(fontname ? fontname : "fixed"),
  itsListBase(0),
  itsListCount(0)
{
DOTRACE("AglRasterFont::AglRasterFont");

  AppleFontSpec spec = pickAppleFont(fontname);

  itsListCount = 256;
  itsListBase = GLCanvas::genLists( 256 );

  if (itsListBase==0)
    {
      throw Util::Error(fstring("couldn't allocate GL display lists"));
    }

  GLboolean status = aglUseFont(aglGetCurrentContext(),
                                spec.fontID,
                                spec.face,
                                spec.size,
                                0,
                                256,
                                itsListBase);

  if (status == GL_FALSE)
    {
      throw Util::Error("aglUseFont failed");
    }

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

AglRasterFont::~AglRasterFont() throw()
{
DOTRACE("AglRasterFont::~AglRasterFont");

  if (aglGetCurrentContext() != 0)
    {
      GLCanvas::deleteLists(itsListBase, itsListCount);
    }
}

GLint AglRasterFont::getFontId(const char* name)
{
DOTRACE("AglRasterFont::getFontId");

  unsigned char pstring[256];

  c2pstrcpy(pstring, name);

  short fnum = 0;
  GetFNum(pstring, &fnum);

  if (fnum == 0)
    throw Util::Error(fstring("couldn't get Apple Font ID for font '", name, "'"));

  return GLint(fnum);
}

AglRasterFont::AppleFontSpec AglRasterFont::pickAppleFont(const char* spec)
{
DOTRACE("AglRasterFont::pickAppleFont");

  AppleFontSpec result;

  result.fontID = getFontId("Geneva");
  result.face = bold;
  result.size = 18;

  if (isdigit(spec[0]))
    {
      int w, h;
      // e.g. 8x13, 9x15
      int n = sscanf(spec, "%dx%d", &w, &h);

      if (n != 2)
        throw Util::Error(fstring("couldn't parse fontspec '",
                                  spec, "' as '[width]x[height]"));

      result.fontID = getFontId("Courier");
      result.face = bold;
      result.size = h;
    }
  else if (spec[0] == '-')
    {
      char family[64] = { '\0' };
      char weight[64] = { '\0' };
      char slant[64] = { '\0' };
      char pxlsize[64] = { '\0' };

      int n = sscanf(spec, "-%*[^-]-%40[^-]-%40[^-]-%40[^-]-%*[^-]-%*[^-]-%40[^-]-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%*[^-]-%*[^-]",
                     family, weight, slant, pxlsize);

      if (n != 4)
        throw Util::Error(fstring("couldn't parse fontspec '", spec,
                                  "' (expected 4 conversions, got ", n, ")"));

      result.fontID = getFontId(family);

      result.face = 0;

      if (strcmp(weight, "bold") == 0)
        result.face |= bold;

      if (*slant == 'i' || *slant == 'o')
        result.face |= italic;

      int ipxlsize = result.size;

      if (*pxlsize != '*')
        {
          int n2 = sscanf(pxlsize, "%d", &ipxlsize);

          if (n2 != 1)
            throw Util::Error(fstring("couldn't parse pixel size from '", pxlsize, "'"));
        }

      result.size = ipxlsize;
    }
  else
    {
      // Parse the spec as a Tcl list. This is useful for font names with
      // embedded spaces, such that the spec needs quotes or
      // braces... e.g., {"luxi mono" 34 ib}. In cases like that, we can't
      // just split the spec on whitespace.
      Tcl::List specitems = Tcl::toTcl(spec);

      if (specitems.length() >= 1)
        result.fontID = getFontId(specitems.get<const char*>(0));

      if (specitems.length() >= 2)
        result.size = specitems.get<GLint>(1);

      result.face = 0;

      if (specitems.length() >= 3)
        {
          const char* mod = specitems.get<const char*>(2);

          while (*mod != '\0')
            {
              switch (*mod)
                {
                case 'b': result.face |= bold; break;
                case 'i': result.face |= italic; break;
                case 'o': result.face |= italic; break;
                case 'u': result.face |= underline; break;
                case 't': result.face |= outline; break;
                case 's': result.face |= shadow; break;
                case 'c': result.face |= condense; break;
                case 'e': result.face |= extend; break;
                }
              ++mod;
            }
        }
    }

  return result;
}

const char* AglRasterFont::fontName() const
{
DOTRACE("AglRasterFont::fontName");
  return itsFontName.c_str();
}

unsigned int AglRasterFont::listBase() const
{
DOTRACE("AglRasterFont::listBase");
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

void AglRasterFont::bboxOf(const char* text, Gfx::Bbox& bbox) const
{
DOTRACE("AglRasterFont::bboxOf");
#if 0
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

      Assert(*text == '\n');
      ++text;
    }

  dbgEval(2, lines); dbgEval(2, asc); dbgEval(2, desc); dbgEvalNL(2, maxwid);
#endif
  Gfx::Rect<int> screen = bbox.screenFromWorld(Gfx::Rect<double>());

//   screen.right() += maxwid;
//   screen.bottom() -= desc + (lines - 1) * (asc+desc);
//   screen.top() += asc;
  screen.right() += 50;
  screen.bottom() -= 10;
  screen.top() += 20;
  (void)text;

  bbox.drawRect(bbox.worldFromScreen(screen));
}

void AglRasterFont::drawText(const char* text, Gfx::Canvas& canvas) const
{
DOTRACE("AglRasterFont::drawText");

  canvas.drawRasterText(text, *this);
}

int AglRasterFont::rasterHeight() const
{
DOTRACE("AglRasterFont::rasterHeight");
//   const int asc = itsFontInfo->max_bounds.ascent;
//   const int desc = itsFontInfo->max_bounds.descent;

//   return asc + desc;
  return 15;
}

static const char vcid_aglrasterfont_h[] = "$Header$";
#endif // !AGLRASTERFONT_H_DEFINED
