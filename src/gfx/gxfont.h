///////////////////////////////////////////////////////////////////////
//
// gxfont.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 12 18:34:57 2002
// written: Tue Nov 12 18:40:54 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXFONT_H_DEFINED
#define GXFONT_H_DEFINED

#include <X11/Xlib.h>

class GxFont
{
public:
  GxFont(Display* dpy, const char* fontname);

  ~GxFont();

  unsigned int listBase() const;

private:
  GxFont(const GxFont&);
  GxFont& operator=(const GxFont&);

  struct Impl;
  Impl* rep;
};

static const char vcid_gxfont_h[] = "$Header$";
#endif // !GXFONT_H_DEFINED
