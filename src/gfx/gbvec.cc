///////////////////////////////////////////////////////////////////////
//
// gbvec.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:11:19 2000
// written: Fri Jan 18 16:07:07 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBVEC_CC_DEFINED
#define GBVEC_CC_DEFINED

#include "gfx/gbvec.h"

#include "util/strings.h"

#define INSTANTIATE(T) \
 \
template <> \
GbVec3<T>::GbVec3(T x_, T y_, T z_) : \
  Gfx::Vec3<T>(x_, y_, z_) \
{} \
 \
template <> \
GbVec3<T>::~GbVec3() {} \
 \
template <> \
fstring GbVec3<T>::getNativeTypeName() const \
{ \
  return fstring("GbVec3"); \
} \
 \
template <> \
void GbVec3<T>::printTo(STD_IO::ostream& os) const \
{ \
  TMultiValue<T>::doPrintTo(os); \
} \
 \
template <> \
void GbVec3<T>::scanFrom(STD_IO::istream& is) \
{ \
  TMultiValue<T>::doScanFrom(is); \
} \
 \
template <> \
unsigned int GbVec3<T>::numValues() const { return 3; } \
 \
template <> \
const T* GbVec3<T>::constBegin() const { return Gfx::Vec3<T>::data(); }

INSTANTIATE(int);
INSTANTIATE(double);

template class GbVec3<int>;
template class GbVec3<double>;

static const char vcid_gbvec_cc[] = "$Header$";
#endif // !GBVEC_CC_DEFINED
