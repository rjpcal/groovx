///////////////////////////////////////////////////////////////////////
//
// gbvec.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:11:19 2000
// written: Thu Aug  9 11:53:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBVEC_CC_DEFINED
#define GBVEC_CC_DEFINED

#include "gx/gbvec.h"

#include "io/reader.h"
#include "io/writer.h"

#include "util/strings.h"

#include <strstream.h>

#include "util/debug.h"

namespace GbVecLocal {
  void raiseScanError();
}

void GbVecLocal::raiseScanError() {
  throw ValueError("couldn't scan data for GbVec3");
}

template <class T>
GbVec3<T>::GbVec3(double x_, double y_, double z_) :
  Field(),
  itsData(x_, y_, z_)
{}

template <class T>
GbVec3<T>::~GbVec3() {}

//
// Value interface
//

template <class T>
Value* GbVec3<T>::clone() const
{
  return new GbVec3(*this);
}

template <class T>
fstring GbVec3<T>::getNativeTypeName() const
{
  static fstring name("GbVec3"); return name;
}

template <class T>
void GbVec3<T>::printTo(STD_IO::ostream& os) const
{
  os << x() << " " << y()  << " " << z();
}

template <class T>
void GbVec3<T>::scanFrom(STD_IO::istream& is)
{
  is >> x();
  if (is.fail() || is.eof()) GbVecLocal::raiseScanError();

  is >> y();
  if (is.fail() || is.eof()) GbVecLocal::raiseScanError();

  is >> z();
  if (is.fail() && !is.eof()) GbVecLocal::raiseScanError();
}

template <class T>
const char* GbVec3<T>::get(Util::TypeCue<const char*>) const
{
  static char buf[256];
  ostrstream ost(buf, 256);
  printTo(ost);
  return buf;
}

template <class T>
void GbVec3<T>::assignTo(Value& other) const
{ other.set(this->get(Util::TypeCue<const char*>())); }

//
// Field interface
//

template <class T>
void GbVec3<T>::doSetValue(const Value& new_val)
{
  istrstream ist(new_val.get(Util::TypeCue<const char*>()));
  DebugEvalNL(new_val.get(Util::TypeCue<const char*>()));
  scanFrom(ist);
}

template <class T>
void GbVec3<T>::readValueFrom(IO::Reader* reader, const fstring& name)
{ reader->readValueObj(name, *this); }

template <class T>
void GbVec3<T>::writeValueTo(IO::Writer* writer,
                            const fstring& name) const
{ writer->writeValueObj(name.c_str(), *this); }

template <class T>
shared_ptr<Value> GbVec3<T>::value() const
{ return shared_ptr<Value>(this->clone()); }

template class GbVec3<int>;
template class GbVec3<double>;

static const char vcid_gbvec_cc[] = "$Header$";
#endif // !GBVEC_CC_DEFINED
