///////////////////////////////////////////////////////////////////////
//
// gbcolor.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Sat Dec  2 13:09:31 2000
// written: Thu Aug  9 07:32:49 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GBCOLOR_H_DEFINED
#define GBCOLOR_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(RGBACOLOR_H_DEFINED)
#include "gx/rgbacolor.h"
#endif

class GbColor : public RgbaColor, public Field, public Value {
public:
  GbColor(FieldContainer* owner, double v = 1.0);
  virtual ~GbColor();

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

  //
  // Field interface
  //

protected:
  virtual void doSetValue(const Value& new_val);
public:
  virtual void readValueFrom(IO::Reader* reader, const fstring& name);
  virtual void writeValueTo(IO::Writer* writer, const fstring& name) const;
  virtual shared_ptr<Value> value() const;
};

static const char vcid_gbcolor_h[] = "$Header$";
#endif // !GBCOLOR_H_DEFINED
