///////////////////////////////////////////////////////////////////////
//
// gxvectorfont.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 20:05:23 2002
// written: Tue Nov 19 13:59:32 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXVECTORFONT_H_DEFINED
#define GXVECTORFONT_H_DEFINED

#include "gfx/gxfont.h"

class GxVectorFont : public GxFont
{
public:
  GxVectorFont();

  virtual ~GxVectorFont();

  virtual const char* fontName() const;

  virtual unsigned int listBase() const;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const;

private:
  GxVectorFont(const GxVectorFont&);
  GxVectorFont& operator=(const GxVectorFont&);
};

static const char vcid_gxvectorfont_h[] = "$Header$";
#endif // !GXVECTORFONT_H_DEFINED
