///////////////////////////////////////////////////////////////////////
//
// attribs.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 11 13:21:57 2001
// written: Wed Aug 15 10:09:46 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ATTRIBS_H_DEFINED
#define ATTRIBS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(FIELDS_H_DEFINED)
#include "io/fields.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(READER_H_DEFINED)
#include "io/reader.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(WRITER_H_DEFINED)
#include "io/writer.h"
#endif

template <class C, class T>
class ReadWriteAttrib : public FieldMemberPtr {
  typedef T (C::* Getter)() const;
  typedef void (C::* Setter)(T);

  Getter itsGetter;
  Setter itsSetter;

  ReadWriteAttrib& operator=(const ReadWriteAttrib&);
  ReadWriteAttrib(const ReadWriteAttrib&);

public:
  ReadWriteAttrib(Getter g, Setter s) : itsGetter(g), itsSetter(s) {}

  virtual void set(FieldContainer* obj, const Value& new_val) const
  {
    C& cobj = dynamic_cast<C&>(*obj);

    (cobj.*itsSetter)(new_val.get(Util::TypeCue<T>()));
  }

  virtual shared_ptr<Value> get(const FieldContainer* obj) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    return shared_ptr<Value>(new TValue<T>((cobj.*itsGetter)()));
  }

  virtual void readValueFrom(FieldContainer* obj,
                             IO::Reader* reader, const fstring& name)
  {
    C& cobj = dynamic_cast<C&>(*obj);

    T temp;
    reader->readValue(name, temp);
    (cobj.*itsSetter)(temp);
  }

  virtual void writeValueTo(const FieldContainer* obj,
                            IO::Writer* writer, const fstring& name) const
  {
    const C& cobj = dynamic_cast<const C&>(*obj);

    writer->writeValue(name.c_str(), (cobj.*itsGetter)());
  }
};

static const char vcid_attribs_h[] = "$Header$";
#endif // !ATTRIBS_H_DEFINED
