///////////////////////////////////////////////////////////////////////
//
// glutil.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 17:25:48 2002
// written: Tue Nov 12 18:38:29 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLUTIL_CC_DEFINED
#define GLUTIL_CC_DEFINED

#include "togl/glutil.h"

#include "gfx/gxfont.h"

#include "util/error.h"
#include "util/minivec.h"
#include "util/pointers.h"

#include "util/trace.h"

namespace
{
  minivec<shared_ptr<GxFont> > fontInfos;
}

unsigned int GLUtil::loadBitmapFont(Display* dpy, const char* fontname)
{
DOTRACE("GLUtil::loadBitmapFont");

  // Record the list base and number of display lists for unloadBitmapFont().
  fontInfos.push_back(shared_ptr<GxFont>(new GxFont(dpy, fontname)));

  return fontInfos.back()->listBase();
}

unsigned int GLUtil::loadBitmapFont(Display* dpy, NamedFont font)
{
DOTRACE("GLUtil::loadBitmapFont");

  const char* name = 0;

  switch (font)
    {
    case BITMAP_FIXED:
      name = "fixed";
      break;
    case BITMAP_8_BY_13:
      name = "8x13";
      break;
    case BITMAP_9_BY_15:
      name = "9x15";
      break;
    case BITMAP_TIMES_ROMAN_10:
      name = "-adobe-times-medium-r-normal--10-100-75-75-p-54-iso8859-1";
      break;
    case BITMAP_TIMES_ROMAN_24:
      name = "-adobe-times-medium-r-normal--24-240-75-75-p-124-iso8859-1";
      break;
    case BITMAP_HELVETICA_10:
      name = "-adobe-helvetica-medium-r-normal--10-100-75-75-p-57-iso8859-1";
      break;
    case BITMAP_HELVETICA_12:
      name = "-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1";
      break;
    case BITMAP_HELVETICA_18:
      name = "-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1";
      break;
    }

  return GLUtil::loadBitmapFont(dpy, name);
}

// Release the display lists which were generated by loadBitmapFont().
void GLUtil::unloadBitmapFont(unsigned int fontbase)
{
DOTRACE("GLUtil::unloadBitmapFont");

  for (minivec<shared_ptr<GxFont> >::iterator
         itr = fontInfos.begin(), end = fontInfos.end();
       itr != end;
       ++itr)
    {
      if ((*itr)->listBase() == fontbase)
        {
          fontInfos.erase(itr);
        }
    }
}

static const char vcid_glutil_cc[] = "$Header$";
#endif // !GLUTIL_CC_DEFINED
