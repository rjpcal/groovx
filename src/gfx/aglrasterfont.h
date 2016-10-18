/** @file gfx/aglrasterfont.h Apple-AGL raster fonts */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue Jul 20 20:21:35 2004
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

#ifndef GROOVX_GFX_AGLRASTERFONT_H_UTC20050626084024_DEFINED
#define GROOVX_GFX_AGLRASTERFONT_H_UTC20050626084024_DEFINED

#include "geom/rect.h"
#include "geom/vec3.h"

#include "gfx/bbox.h"
#include "gfx/fontspec.h"
#include "gfx/glcanvas.h"
#include "gfx/gxrasterfont.h"

#include "rutz/cstrstream.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <cctype>
#include <cstdio>
#include <cstdlib> // for atoi()
#include <cstring>
#include <AGL/agl.h>
#include <OpenGL/gl.h>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

class AglRasterFont : public GxRasterFont
{
public:
  AglRasterFont(const char* n);
  virtual ~AglRasterFont() noexcept;

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
  rutz::fstring itsFontName;
  FontInfo itsFontInfo;
  AppleFontSpec itsFontSpec;
  unsigned int itsListBase;
  unsigned int itsListCount;
};

AglRasterFont::AglRasterFont(const char* fontname) :
  itsFontName(fontname ? fontname : "fixed"),
  itsListBase(0),
  itsListCount(0)
{
GVX_TRACE("AglRasterFont::AglRasterFont");

  // This seems to do nothing (but it should make the Font Manager pick
  // nice outline fonts instead of uglier scaled-bitmap fonts):

  // SetOutlinePreferred(TRUE);

  itsFontSpec = pickAppleFont(fontname);

  itsListCount = 256;
  itsListBase = GLCanvas::genLists( 256 );

  if (itsListBase==0)
    {
      throw rutz::error(rutz::fstring("couldn't allocate GL display lists"),
                        SRC_POS);
    }

  GLboolean status = aglUseFont(aglGetCurrentContext(),
                                itsFontSpec.fontID,
                                itsFontSpec.face,
                                itsFontSpec.size,
                                0,
                                256,
                                itsListBase);

  if (status == GL_FALSE)
    {
      throw rutz::error("aglUseFont failed", SRC_POS);
    }

  /*
    struct FontInfo {
      short ascent;
      short descent;
      short widMax;
      short leading;
    };
  */

  OSErr err = FetchFontInfo(itsFontSpec.fontID, itsFontSpec.size,
                            itsFontSpec.face, &itsFontInfo);

  if (err != noErr)
    {
      throw rutz::error("FetchFontInfo failed", SRC_POS);
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

AglRasterFont::~AglRasterFont() noexcept
{
GVX_TRACE("AglRasterFont::~AglRasterFont");

  if (aglGetCurrentContext() != 0)
    {
      GLCanvas::deleteLists(itsListBase, itsListCount);
    }
}

GLint AglRasterFont::getFontId(const char* name)
{
GVX_TRACE("AglRasterFont::getFontId");

  unsigned char pstring[256];

  c2pstrcpy(pstring, name);

  short fnum = 0;
  GetFNum(pstring, &fnum);

  if (fnum == 0)
    throw rutz::error(rutz::sfmt("couldn't get Apple Font ID "
                                 "for font '%s'", name),
                      SRC_POS);

  return GLint(fnum);
}

AglRasterFont::AppleFontSpec AglRasterFont::pickAppleFont(const char* spec)
{
GVX_TRACE("AglRasterFont::pickAppleFont");

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
        throw rutz::error(rutz::sfmt("couldn't parse fontspec '%s' as "
                                     "'[width]x[height]", spec),
                          SRC_POS);

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
        throw rutz::error(rutz::sfmt("couldn't parse fontspec '%s' "
                                     "(expected 4 conversions, got %d)",
                                     spec, n),
                          SRC_POS);

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
            throw rutz::error(rutz::sfmt("couldn't parse pixel size "
                                         "from '%s'", pxlsize),
                              SRC_POS);
        }

      result.size = ipxlsize;
    }
  else
    {
      rutz::fstring family, pxlsize, mods;

      parseFontSpec(spec, &family, &pxlsize, &mods);

      if (!family.is_empty())
        result.fontID = getFontId(family.c_str());

      if (!pxlsize.is_empty())
        result.size = atoi(pxlsize.c_str());

      result.face = 0;

      const char* mod = mods.c_str();

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

  return result;
}

const char* AglRasterFont::fontName() const
{
GVX_TRACE("AglRasterFont::fontName");
  return itsFontName.c_str();
}

unsigned int AglRasterFont::listBase() const
{
GVX_TRACE("AglRasterFont::listBase");
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
GVX_TRACE("AglRasterFont::bboxOf");

  const int asc = itsFontInfo.ascent;
  const int desc = itsFontInfo.descent;
  int maxwid = 0;

  int lines = 0;

  // make a copy so that we can pass a non-const pointer to StdTxMeas
  FontInfo finfo = itsFontInfo;

  CGrafPtr saveWorld;
  GDHandle saveDevice;

  static GWorldPtr gWorld = NULL;

  if (gWorld == 0)
    {
      Rect rect = {0, 0, 1, 1};
      SetFractEnable(0);

      if (NewGWorld(&gWorld, 0, &rect, NULL, NULL, 0) != noErr)
        {
          throw rutz::error("NewGWorld failed", SRC_POS);
        }
    }

  GetGWorld(&saveWorld, &saveDevice);
  SetGWorld(gWorld, NULL);

  TextFont(itsFontSpec.fontID);
  TextFace(itsFontSpec.face);
  TextSize(itsFontSpec.size);

  while (1)
    {
      int len = linelength(text);
#if 0
      Point numer, denom;
      numer.h = numer.v = denom.h = denom.v = 1;
      int wid = StdTxMeas(len, text, &numer, &denom, &finfo);

      dbg_eval(2, numer.h); dbg_eval_nl(2, numer.v);
      dbg_eval(2, denom.h); dbg_eval_nl(2, denom.v);
#else
      int wid = TextWidth(text, 0, len);
#endif

      int wid2 = 0;
      for (int i = 0; i < len; ++i)
        {
          int widthOfChar = CharWidth(text[i]);
          wid2 += widthOfChar;
          dbg_eval(4, text[i]); dbg_eval_nl(4, widthOfChar);
        }

      dbg_eval(2, wid); dbg_eval_nl(2, wid2);

      text += len;
      if (wid > maxwid)
        maxwid = wid;

      ++lines;

      if (*text == '\0')
        break;

      GVX_ASSERT(*text == '\n');
      ++text;
    }

  dbg_eval(2, lines);
  dbg_eval(2, itsFontInfo.widMax);
  dbg_eval(2, asc);
  dbg_eval(2, desc);
  dbg_eval_nl(2, maxwid);

  const int l = 0;
  const int r = maxwid;
  const int b = -(itsFontInfo.descent + (lines - 1) * (rasterHeight()));
  const int t = itsFontInfo.ascent;

  dbg_eval(2, l);
  dbg_eval(2, r);
  dbg_eval(2, b);
  dbg_eval_nl(2, t);

  GVX_ASSERT(r >= 0);
  GVX_ASSERT(b <= 0);
  GVX_ASSERT(t >= 0);

  bbox.drawScreenRect(geom::vec3d::zeros(),
                      geom::rect<int>::ltrb(l,t,r,b));

  SetGWorld(saveWorld, saveDevice);
}

void AglRasterFont::drawText(const char* text, Gfx::Canvas& canvas) const
{
GVX_TRACE("AglRasterFont::drawText");

  canvas.drawRasterText(text, *this);
}

int AglRasterFont::rasterHeight() const
{
GVX_TRACE("AglRasterFont::rasterHeight");

  return itsFontInfo.ascent + itsFontInfo.descent + itsFontInfo.leading;
}

#endif // !GROOVX_GFX_AGLRASTERFONT_H_UTC20050626084024_DEFINED
