///////////////////////////////////////////////////////////////////////
//
// gxvec.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:11:19 2000
// written: Wed Nov 29 13:15:01 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXVEC_CC_DEFINED
#define GXVEC_CC_DEFINED

#include "gx/gxvec.h"

#include "io/reader.h"
#include "io/writer.h"

#include <strstream.h>

#include "util/debug.h"

namespace GxVecLocal {
  void raiseScanError();
}

void GxVecLocal::raiseScanError() {
  throw ValueError("couldn't scan data for GxVec3");
}

template <class T>
GxVec3<T>::GxVec3(FieldContainer* owner, double x_, double y_, double z_) :
  Field(owner),
  itsData(x_, y_, z_)
{}

template <class T>
GxVec3<T>::~GxVec3() {}

//
// Value interface
//

template <class T>
Value* GxVec3<T>::clone() const
{ return new GxVec3(*this);}

template <class T>
Value::Type GxVec3<T>::getNativeType() const
{ return Value::UNKNOWN; }

template <class T>
const char* GxVec3<T>::getNativeTypeName() const
{ return "GxVec3"; }

template <class T>
void GxVec3<T>::printTo(STD_IO::ostream& os) const
{ os << x() << " " << y()  << " " << z(); }

template <class T>
void GxVec3<T>::scanFrom(STD_IO::istream& is)
{
  is >> x();
  if (is.fail() || is.eof()) GxVecLocal::raiseScanError();

  is >> y();
  if (is.fail() || is.eof()) GxVecLocal::raiseScanError();

  is >> z();
  if (is.fail() && !is.eof()) GxVecLocal::raiseScanError();
}

template <class T>
const char* GxVec3<T>::getCstring() const
{
  static char buf[256];
  ostrstream ost(buf, 256);
  printTo(ost);
  return buf;
}

//
// Field interface
//

template <class T>
void GxVec3<T>::doSetValue(const Value& new_val)
{
  istrstream ist(new_val.getCstring());
  DebugEvalNL(new_val.getCstring());
  scanFrom(ist);
}

template <class T>
void GxVec3<T>::readValueFrom(IO::Reader* reader, const fixed_string& name)
{ reader->readValueObj(name, *this); }

template <class T>
void GxVec3<T>::writeValueTo(IO::Writer* writer,
									 const fixed_string& name) const
{ writer->writeValueObj(name.c_str(), *this); }

template <class T>
const Value& GxVec3<T>::value() const
{ return *this; }

template class GxVec3<int>;
template class GxVec3<double>;

static const char vcid_gxvec_cc[] = "$Header$";
#endif // !GXVEC_CC_DEFINED
