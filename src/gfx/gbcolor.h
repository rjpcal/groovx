///////////////////////////////////////////////////////////////////////
//
// gbcolor.h
//
// Copyright (c) 2000-2003 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:09:31 2000
// written: Mon Jan 13 11:04:47 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBCOLOR_H_DEFINED
#define GBCOLOR_H_DEFINED

#include "util/multivalue.h"

#include "gx/rgbacolor.h"

/// Field class for RGBA colors.
class GbColor : public Gfx::RgbaColor, public TMultiValue<double>
{
public:
  GbColor(double v = 1.0);
  GbColor(double r, double g, double b, double a);
  virtual ~GbColor();

  // These help to disambiguate function calls to set()
        Gfx::RgbaColor& color()       { return *this; }
  const Gfx::RgbaColor& color() const { return *this; }

  virtual fstring getNativeTypeName() const;

  virtual const double* constBegin() const;
};

static const char vcid_gbcolor_h[] = "$Header$";
#endif // !GBCOLOR_H_DEFINED
