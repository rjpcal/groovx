///////////////////////////////////////////////////////////////////////
//
// gxvectorfont.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Nov 13 20:05:23 2002
// written: Wed Nov 13 20:08:46 2002
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

  virtual unsigned int listBase() const;

  virtual Gfx::Rect<double> sizeOf(const char* text,
                                   Gfx::Canvas& canvas) const;

private:
  GxVectorFont(const GxVectorFont&);
  GxVectorFont& operator=(const GxVectorFont&);
};

static const char vcid_gxvectorfont_h[] = "$Header$";
#endif // !GXVECTORFONT_H_DEFINED
