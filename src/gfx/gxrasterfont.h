///////////////////////////////////////////////////////////////////////
//
// gxrasterfont.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 16:44:36 2002
// written: Wed Nov 13 19:59:47 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXRASTERFONT_H_DEFINED
#define GXRASTERFONT_H_DEFINED

#include "gfx/gxfont.h"

class GxRasterFont : public GxFont
{
public:
  GxRasterFont(const char* fontname);

  virtual ~GxRasterFont();

  virtual unsigned int listBase() const;

  virtual Gfx::Rect<double> sizeOf(const char* text,
                                   Gfx::Canvas& canvas) const;

private:
  GxRasterFont(const GxRasterFont&);
  GxRasterFont& operator=(const GxRasterFont&);

  struct Impl;
  Impl* rep;
};

static const char vcid_gxrasterfont_h[] = "$Header$";
#endif // !GXRASTERFONT_H_DEFINED
