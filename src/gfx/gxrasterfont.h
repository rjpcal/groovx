///////////////////////////////////////////////////////////////////////
//
// gxrasterfont.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 16:44:36 2002
// written: Mon Jan 13 11:01:38 2003
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

  virtual const char* fontName() const;

  virtual unsigned int listBase() const;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const;

private:
  GxRasterFont(const GxRasterFont&);
  GxRasterFont& operator=(const GxRasterFont&);

  struct Impl;
  Impl* rep;
};

static const char vcid_gxrasterfont_h[] = "$Header$";
#endif // !GXRASTERFONT_H_DEFINED
