///////////////////////////////////////////////////////////////////////
//
// gbcolor.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:09:31 2000
// written: Wed Jun 26 12:31:58 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBCOLOR_H_DEFINED
#define GBCOLOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(MULTIVALUE_H_DEFINED)
#include "util/multivalue.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RGBACOLOR_H_DEFINED)
#include "gx/rgbacolor.h"
#endif

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

  virtual void printTo(STD_IO::ostream& os) const
  { TMultiValue<double>::doPrintTo(os); }
  virtual void scanFrom(STD_IO::istream& is)
  { TMultiValue<double>::doScanFrom(is); }

  virtual unsigned int numValues() const;
  virtual const double* constBegin() const;
};

static const char vcid_gbcolor_h[] = "$Header$";
#endif // !GBCOLOR_H_DEFINED
