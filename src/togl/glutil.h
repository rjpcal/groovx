///////////////////////////////////////////////////////////////////////
//
// glutil.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Aug  3 17:25:24 2002
// written: Wed Nov 13 14:39:33 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GLUTIL_H_DEFINED
#define GLUTIL_H_DEFINED

namespace GLUtil
{
  // Returns the starting index of a series of GL display lists
  unsigned int loadBitmapFont(const char* fontname);

  enum NamedFont
  {
    BITMAP_FIXED,
    BITMAP_8_BY_13,
    BITMAP_9_BY_15,
    BITMAP_TIMES_ROMAN_10,
    BITMAP_TIMES_ROMAN_24,
    BITMAP_HELVETICA_10,
    BITMAP_HELVETICA_12,
    BITMAP_HELVETICA_18
  };

  unsigned int loadBitmapFont(NamedFont font);

  void unloadBitmapFont(unsigned int fontbase);
}

static const char vcid_glutil_h[] = "$Header$";
#endif // !GLUTIL_H_DEFINED
