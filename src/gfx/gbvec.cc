///////////////////////////////////////////////////////////////////////
//
// gbvec.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:11:19 2000
// written: Wed Aug 15 18:00:21 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBVEC_CC_DEFINED
#define GBVEC_CC_DEFINED

#include "gfx/gbvec.h"

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
GbVec3<T>::GbVec3(T x_, T y_, T z_) :
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

template <class T>
void GbVec3<T>::assignFrom(const Value& other)
{
  istrstream ist(other.get(Util::TypeCue<const char*>()));
  DebugEvalNL(other.get(Util::TypeCue<const char*>()));
  scanFrom(ist);
}

template class GbVec3<int>;
template class GbVec3<double>;

static const char vcid_gbvec_cc[] = "$Header$";
#endif // !GBVEC_CC_DEFINED
