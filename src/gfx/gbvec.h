///////////////////////////////////////////////////////////////////////
//
// gxvec.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:10:45 2000
// written: Wed Nov 29 13:08:12 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXVEC_H_DEFINED
#define GXVEC_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(VEC3_H_DEFINED)
#include "gx/vec3.h"
#endif

template <class T>
class GxVec3 : public Field, public Value {
private:
  Vec3<T> itsData;

public:
  GxVec3(FieldContainer* owner, double x_=0.0, double y_=0.0, double z_=0.0);
  virtual ~GxVec3();

        T& x()       { return itsData.x(); }
  const T& x() const { return itsData.x(); }

        T& y()       { return itsData.y(); }
  const T& y() const { return itsData.y(); }

        T& z()       { return itsData.z(); }
  const T& z() const { return itsData.z(); }

        Vec3<T>& vec()       { return itsData; }
  const Vec3<T>& vec() const { return itsData; }

  //
  // Value interface
  //

public:
  virtual Value* clone() const;
  virtual Type getNativeType() const;
  virtual const char* getNativeTypeName() const;
  virtual void printTo(STD_IO::ostream& os) const;
  virtual void scanFrom(STD_IO::istream& is);

  virtual const char* getCstring() const;

  //
  // Field interface
  //

protected:
  virtual void doSetValue(const Value& new_val);
public:
  virtual void readValueFrom(IO::Reader* reader, const fixed_string& name);
  virtual void writeValueTo(IO::Writer* writer, const fixed_string& name) const;
  virtual const Value& value() const;
};

static const char vcid_gxvec_h[] = "$Header$";
#endif // !GXVEC_H_DEFINED
