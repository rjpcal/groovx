///////////////////////////////////////////////////////////////////////
//
// gxfont.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 12 18:34:57 2002
// written: Wed Nov 13 19:52:08 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXFONT_H_DEFINED
#define GXFONT_H_DEFINED

namespace Gfx
{
  class Canvas;
  template <class V> class Rect;
}

class GxFont
{
public:
  virtual ~GxFont();

  virtual unsigned int listBase() const = 0;

  virtual Gfx::Rect<double> sizeOf(const char* text,
                                   Gfx::Canvas& canvas) const = 0;
};

static const char vcid_gxfont_h[] = "$Header$";
#endif // !GXFONT_H_DEFINED
