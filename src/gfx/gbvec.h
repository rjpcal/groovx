///////////////////////////////////////////////////////////////////////
//
// gbvec.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:10:45 2000
// written: Wed Aug 15 17:59:13 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBVEC_H_DEFINED
#define GBVEC_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VALUE_H_DEFINED)
#include "util/value.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VEC3_H_DEFINED)
#include "gfx/vec3.h"
#endif

template <class T>
class GbVec3 : public Value {
private:
  Gfx::Vec3<T> itsData;

public:
  GbVec3(T x_=T(), T y_=T(), T z_=T());
  virtual ~GbVec3();

        T& x()       { return itsData.x(); }
  const T& x() const { return itsData.x(); }

        T& y()       { return itsData.y(); }
  const T& y() const { return itsData.y(); }

        T& z()       { return itsData.z(); }
  const T& z() const { return itsData.z(); }

        Gfx::Vec3<T>& vec()       { return itsData; }
  const Gfx::Vec3<T>& vec() const { return itsData; }

  //
  // Value interface
  //

public:
  virtual Value* clone() const;
  virtual fstring getNativeTypeName() const;
  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual const char* get(Util::TypeCue<const char*>) const;

  virtual void assignTo(Value& other) const;
  virtual void assignFrom(const Value& other);
};

static const char vcid_gbvec_h[] = "$Header$";
#endif // !GBVEC_H_DEFINED
