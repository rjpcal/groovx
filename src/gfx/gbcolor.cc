///////////////////////////////////////////////////////////////////////
//
// gbcolor.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:10:25 2000
// written: Wed Aug 15 17:47:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBCOLOR_CC_DEFINED
#define GBCOLOR_CC_DEFINED

#include "gfx/gbcolor.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/strings.h"

#include <strstream.h>

#include "util/debug.h"

namespace GbColorLocal
{
  void raiseScanError();
}

void GbColorLocal::raiseScanError()
{
  throw ValueError("couldn't scan data for GbColor");
}

GbColor::GbColor(double v) : RgbaColor(v) {}

GbColor::~GbColor() {}

//
// Value interface
//

Value* GbColor::clone() const
{
  return new GbColor(*this);
}

fstring GbColor::getNativeTypeName() const
{
  static fstring name("GbColor"); return name;
}

void GbColor::printTo(STD_IO::ostream& os) const
{
  os << r() << " " << g()  << " " << b() << " " << a();
}

void GbColor::scanFrom(STD_IO::istream& is)
{
  is >> r();
  if (is.fail() || is.eof()) GbColorLocal::raiseScanError();

  is >> g();
  if (is.fail() || is.eof()) GbColorLocal::raiseScanError();

  is >> b();
  if (is.fail() || is.eof()) GbColorLocal::raiseScanError();

  is >> a();
  if (is.fail() && !is.eof()) GbColorLocal::raiseScanError();
}

const char* GbColor::get(Util::TypeCue<const char*>) const
{
  static char buf[256];
  ostrstream ost(buf, 256);
  printTo(ost);
  return buf;
}

void GbColor::assignTo(Value& other) const
{
  other.set(this->get(Util::TypeCue<const char*>()));
}

static const char vcid_gbcolor_cc[] = "$Header$";
#endif // !GBCOLOR_CC_DEFINED
