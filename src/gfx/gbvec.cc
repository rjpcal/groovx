///////////////////////////////////////////////////////////////////////
//
// gbvec.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:11:19 2000
// written: Wed Aug 22 17:20:17 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBVEC_CC_DEFINED
#define GBVEC_CC_DEFINED

#include "gfx/gbvec.h"

#include "util/strings.h"

template <class T>
GbVec3<T>::GbVec3(T x_, T y_, T z_) :
  Gfx::Vec3<T>(x_, y_, z_)
{}

template <class T>
GbVec3<T>::~GbVec3() {}

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
unsigned int GbVec3<T>::numValues() const { return 3; }

template <class T>
const T* GbVec3<T>::constBegin() const { return Gfx::Vec3<T>::data(); }

template class GbVec3<int>;
template class GbVec3<double>;

static const char vcid_gbvec_cc[] = "$Header$";
#endif // !GBVEC_CC_DEFINED
