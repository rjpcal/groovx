///////////////////////////////////////////////////////////////////////
//
// gxvec.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Nov 16 00:10:45 2000
// written: Thu Nov 16 01:19:10 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GXVEC_H_DEFINED
#define GXVEC_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

template <class T>
class GxVec3 : public Field, public Value {
private:
  T itsData[3];

public:
  GxVec3(FieldContainer* owner);
  virtual ~GxVec3();

        T& x()       { return itsData[0]; }
  const T& x() const { return itsData[0]; }

        T& y()       { return itsData[1]; }
  const T& y() const { return itsData[1]; }

        T& z()       { return itsData[2]; }
  const T& z() const { return itsData[2]; }

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
