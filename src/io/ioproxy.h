///////////////////////////////////////////////////////////////////////
//
// ioproxy.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 22 21:41:38 2000
// written: Sat May 19 10:44:19 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPROXY_H_DEFINED
#define IOPROXY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "io/iditem.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(DEMANGLE_H_DEFINED)
#include "system/demangle.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TYPEINFO_DEFINED)
#include <typeinfo>
#define TYPEINFO_DEFINED
#endif

namespace IO {

template <class C>
class IoProxy : public IO::IoObject {
protected:
  IoProxy(C* ref) : IO::IoObject(false), itsReferand(ref) {}
  virtual ~IoProxy() {}

public:
  static IdItem<IO::IoObject> make(C* ref)
    { return IdItem<IoProxy>( new IoProxy(ref) ); }

  virtual void readFrom(IO::Reader* reader)
	 { itsReferand->C::readFrom(reader); }

  virtual void writeTo(IO::Writer* writer) const
	 { itsReferand->C::writeTo(writer); }

  virtual IO::VersionId serialVersionId() const
	 { return itsReferand->C::serialVersionId(); }

  virtual fixed_string ioTypename() const
	 { return demangle_cstr(typeid(C).name()); }

private:
  IoProxy(const IoProxy&);
  IoProxy& operator=(const IoProxy&);

  C* itsReferand;
};

template <class C>
inline IdItem<IO::IoObject> makeProxy(C* ref)
  { return IoProxy<C>::make(ref); }


template <class C>
class ConstIoProxy : public IO::IoObject {
protected:
  ConstIoProxy(const C* ref) : IO::IoObject(false), itsReferand(ref) {}
  virtual ~ConstIoProxy() {}

public:
  static IdItem<IO::IoObject> make(const C* ref)
    { return IdItem<ConstIoProxy>( new ConstIoProxy(ref) ); }

  virtual void readFrom(IO::Reader* reader)
	 { const_cast<C*>(itsReferand)->C::readFrom(reader); }

  virtual void writeTo(IO::Writer* writer) const
	 { itsReferand->C::writeTo(writer); }

  virtual IO::VersionId serialVersionId() const
	 { return itsReferand->C::serialVersionId(); }

  virtual fixed_string ioTypename() const
	 { return demangle_cstr(typeid(C).name()); }

private:
  ConstIoProxy(const ConstIoProxy&);
  ConstIoProxy& operator=(const ConstIoProxy&);

  const C* itsReferand;
};

template <class C>
inline IdItem<IO::IoObject> makeConstProxy(const C* ref)
  { return ConstIoProxy<C>::make(ref); }


} // end namespace IO

static const char vcid_ioproxy_h[] = "$Header$";
#endif // !IOPROXY_H_DEFINED
