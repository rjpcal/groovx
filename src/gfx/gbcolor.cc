///////////////////////////////////////////////////////////////////////
//
// gbcolor.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:10:25 2000
// written: Mon Sep  3 14:49:24 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBCOLOR_CC_DEFINED
#define GBCOLOR_CC_DEFINED

#include "gfx/gbcolor.h"

#include "util/strings.h"

GbColor::GbColor(double v) : Gfx::RgbaColor(v) {}

GbColor::GbColor(double r, double g, double b, double a) :
  Gfx::RgbaColor(r,g,b,a) {}

GbColor::~GbColor() {}

//
// Value interface
//

fstring GbColor::getNativeTypeName() const
{
  static fstring name("GbColor"); return name;
}

unsigned int GbColor::numValues() const { return 4; }

const double* GbColor::constBegin() const { return Gfx::RgbaColor::data(); }

static const char vcid_gbcolor_cc[] = "$Header$";
#endif // !GBCOLOR_CC_DEFINED
