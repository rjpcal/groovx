///////////////////////////////////////////////////////////////////////
//
// gbcolor.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:09:31 2000
// written: Wed Aug 22 18:36:01 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBCOLOR_H_DEFINED
#define GBCOLOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(MULTIVALUE_H_DEFINED)
#include "util/multivalue.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RGBACOLOR_H_DEFINED)
#include "gfx/rgbacolor.h"
#endif

class GbColor : public Gfx::RgbaColor, public TMultiValue<double>
{
public:
  GbColor(double v = 1.0);
  GbColor(double r, double g, double b, double a);
  virtual ~GbColor();

  // These help to disambiguate function calls to set()
        Gfx::RgbaColor& color()       { return *this; }
  const Gfx::RgbaColor& color() const { return *this; }

  virtual Value* clone() const;
  virtual fstring getNativeTypeName() const;

  virtual unsigned int numValues() const;
  virtual const double* constBegin() const;
};

static const char vcid_gbcolor_h[] = "$Header$";
#endif // !GBCOLOR_H_DEFINED
