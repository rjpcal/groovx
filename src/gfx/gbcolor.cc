///////////////////////////////////////////////////////////////////////
//
// gbcolor.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:10:25 2000
// written: Thu Aug  9 07:28:16 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBCOLOR_CC_DEFINED
#define GBCOLOR_CC_DEFINED

#include "gx/gbcolor.h"

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

GbColor::GbColor(FieldContainer* owner, double v) :
  RgbaColor(v),
  Field()
{}

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

//
// Field interface
//

void GbColor::doSetValue(const Value& new_val)
{
  istrstream ist(new_val.get(Util::TypeCue<const char*>()));
  DebugEvalNL(new_val.get(Util::TypeCue<const char*>()));
  scanFrom(ist);
}

void GbColor::readValueFrom(IO::Reader* reader, const fstring& name)
{ reader->readValueObj(name, *this); }

void GbColor::writeValueTo(IO::Writer* writer, const fstring& name) const
{ writer->writeValueObj(name.c_str(), *this); }

shared_ptr<Value> GbColor::value() const
{ return shared_ptr<Value>(this->clone()); }

static const char vcid_gbcolor_cc[] = "$Header$";
#endif // !GBCOLOR_CC_DEFINED
