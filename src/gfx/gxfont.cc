///////////////////////////////////////////////////////////////////////
//
// gxfont.cc
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Nov 12 18:35:02 2002
// written: Wed Mar 19 12:46:01 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXFONT_CC_DEFINED
#define GXFONT_CC_DEFINED

#include "gxfont.h"

#include "gfx/gxrasterfont.h"
#include "gfx/gxvectorfont.h"

#include "util/strings.h"

GxFont* GxFont::make(const char* name_cstr)
{
  fstring name(name_cstr);

  if (name == "vector")
    return new GxVectorFont;

  // else...
  return new GxRasterFont(name_cstr);
}

GxFont::~GxFont() {}

static const char vcid_gxfont_cc[] = "$Header$";
#endif // !GXFONT_CC_DEFINED
