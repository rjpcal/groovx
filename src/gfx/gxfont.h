///////////////////////////////////////////////////////////////////////
//
// gxfont.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 12 18:34:57 2002
// written: Tue Nov 19 13:59:13 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXFONT_H_DEFINED
#define GXFONT_H_DEFINED

namespace Gfx
{
  class Bbox;
  class Canvas;
  template <class V> class Rect;
}

class GxFont
{
public:
  static GxFont* make(const char* name);

  virtual ~GxFont();

  virtual const char* fontName() const = 0;

  virtual unsigned int listBase() const = 0;

  virtual void bboxOf(const char* text, Gfx::Bbox& bbox) const = 0;
};

static const char vcid_gxfont_h[] = "$Header$";
#endif // !GXFONT_H_DEFINED
