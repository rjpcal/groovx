///////////////////////////////////////////////////////////////////////
//
// attribs.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun 11 13:21:57 2001
// written: Wed Aug  8 20:16:38 2001
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
class ReadWriteAttrib : public Field {
  C* itsC;

  typedef T (C::* Getter)() const;
  typedef void (C::* Setter)(T);

  Getter itsGetter;
  Setter itsSetter;

  ReadWriteAttrib(const ReadWriteAttrib&);
  ReadWriteAttrib& operator=(const ReadWriteAttrib&);

protected:
  virtual void doSetValue(const Value& new_val)
  {
    (itsC->*itsSetter)(new_val.get(Util::TypeCue<T>()));
  }

public:
  ReadWriteAttrib(C* t, Getter g, Setter s) :
    Field(), itsC(t), itsGetter(g), itsSetter(s) {}

  virtual void readValueFrom(IO::Reader* reader, const fstring& name)
  {
    T temp;
    reader->readValue(name, temp);
    (itsC->*itsSetter)(temp);
  }

  virtual void writeValueTo(IO::Writer* writer,
                            const fstring& name) const
  {
    writer->writeValue(name.c_str(), (itsC->*itsGetter)());
  }

  virtual shared_ptr<Value> value() const
  {
    return shared_ptr<Value>(new TValue<T>((itsC->*itsGetter)()));
  }
};

static const char vcid_attribs_h[] = "$Header$";
#endif // !ATTRIBS_H_DEFINED
