///////////////////////////////////////////////////////////////////////
//
// ioproxy.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 22 21:41:38 2000
// written: Tue Jun 12 11:17:10 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPROXY_H_DEFINED
#define IOPROXY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(REF_H_DEFINED)
#include "util/ref.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
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
class IoProxy : public IoObject {
protected:
  IoProxy(C* ref) : IoObject(), itsReferand(ref) {}
  virtual ~IoProxy() {}

public:
  static Util::Ref<IoObject> make(C* ref)
    { return Util::Ref<IoObject>( new IoProxy(ref), true ); }

  virtual void readFrom(Reader* reader)
    { itsReferand->C::readFrom(reader); }

  virtual void writeTo(Writer* writer) const
    { itsReferand->C::writeTo(writer); }

  virtual VersionId serialVersionId() const
    { return itsReferand->C::serialVersionId(); }

  virtual fixed_string ioTypename() const
    { return demangle_cstr(typeid(C).name()); }

private:
  IoProxy(const IoProxy&);
  IoProxy& operator=(const IoProxy&);

  Util::Ref<C> itsReferand;
};

template <class C>
inline Util::Ref<IoObject> makeProxy(C* ref)
  { return IoProxy<C>::make(ref); }


template <class C>
class ConstIoProxy : public IoObject {
protected:
  ConstIoProxy(const C* ref) : IoObject(), itsReferand(const_cast<C*>(ref)) {}
  virtual ~ConstIoProxy() {}

public:
  static Util::Ref<const IoObject> make(const C* ref)
    { return Util::Ref<const IoObject>( new ConstIoProxy(ref), true ); }

  virtual void readFrom(Reader* reader)
    { itsReferand->C::readFrom(reader); }

  virtual void writeTo(Writer* writer) const
    { itsReferand->C::writeTo(writer); }

  virtual VersionId serialVersionId() const
    { return itsReferand->C::serialVersionId(); }

  virtual fixed_string ioTypename() const
    { return demangle_cstr(typeid(C).name()); }

private:
  ConstIoProxy(const ConstIoProxy&);
  ConstIoProxy& operator=(const ConstIoProxy&);

  Util::Ref<C> itsReferand;
};

template <class C>
inline Util::Ref<const IoObject> makeConstProxy(const C* ref)
  { return ConstIoProxy<C>::make(ref); }


} // end namespace IO

static const char vcid_ioproxy_h[] = "$Header$";
#endif // !IOPROXY_H_DEFINED
