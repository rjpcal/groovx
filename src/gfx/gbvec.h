///////////////////////////////////////////////////////////////////////
//
// gbvec.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:10:45 2000
// written: Mon Sep  3 14:47:55 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBVEC_H_DEFINED
#define GBVEC_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(MULTIVALUE_H_DEFINED)
#include "util/multivalue.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VEC3_H_DEFINED)
#include "gfx/vec3.h"
#endif

template <class T>
class GbVec3 : public Gfx::Vec3<T>,
               public TMultiValue<T>
{
public:
  GbVec3(T x_=T(), T y_=T(), T z_=T());
  virtual ~GbVec3();

  // These help to disambiguate function calls to set()
        Gfx::Vec3<T>& vec()       { return *this; }
  const Gfx::Vec3<T>& vec() const { return *this; }

  virtual fstring getNativeTypeName() const;

  virtual unsigned int numValues() const;
  virtual const T* constBegin() const;
};

static const char vcid_gbvec_h[] = "$Header$";
#endif // !GBVEC_H_DEFINED
