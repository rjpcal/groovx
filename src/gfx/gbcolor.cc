///////////////////////////////////////////////////////////////////////
//
// gbcolor.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:10:25 2000
// written: Mon Nov  4 11:30:58 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBCOLOR_CC_DEFINED
#define GBCOLOR_CC_DEFINED

#include "gfx/gbcolor.h"

#include "util/strings.h"

GbColor::GbColor(double v) :
  Gfx::RgbaColor(v),
  TMultiValue<double>(4)
{}

GbColor::GbColor(double r, double g, double b, double a) :
  Gfx::RgbaColor(r,g,b,a),
  TMultiValue<double>(4)
{}

GbColor::~GbColor() {}

//
// Value interface
//

fstring GbColor::getNativeTypeName() const
{
  static fstring name("GbColor"); return name;
}

const double* GbColor::constBegin() const { return Gfx::RgbaColor::data(); }

static const char vcid_gbcolor_cc[] = "$Header$";
#endif // !GBCOLOR_CC_DEFINED
