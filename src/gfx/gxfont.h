///////////////////////////////////////////////////////////////////////
//
// gxfont.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 12 18:34:57 2002
// written: Wed Nov 13 14:36:52 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXFONT_H_DEFINED
#define GXFONT_H_DEFINED

class GxFont
{
public:
  GxFont(const char* fontname);

  ~GxFont();

  unsigned int listBase() const;

  int widthOf(const char* text) const;
  int heightOf(const char* text) const;

  int ascentOf(const char* text) const;
  int descentOf(const char* text) const;

private:
  GxFont(const GxFont&);
  GxFont& operator=(const GxFont&);

  struct Impl;
  Impl* rep;
};

static const char vcid_gxfont_h[] = "$Header$";
#endif // !GXFONT_H_DEFINED
