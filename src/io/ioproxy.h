///////////////////////////////////////////////////////////////////////
//
// ioproxy.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 22 21:41:38 2000
// written: Sat May 19 10:54:58 2001
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
class IoProxy : public IoObject {
protected:
  IoProxy(C* ref) : IoObject(false), itsReferand(ref) {}
  virtual ~IoProxy() {}

public:
  static IdItem<IoObject> make(C* ref)
    { return IdItem<IoProxy>( new IoProxy(ref) ); }

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

  IdItem<C> itsReferand;
};

template <class C>
inline IdItem<IoObject> makeProxy(C* ref)
  { return IoProxy<C>::make(ref); }


template <class C>
class ConstIoProxy : public IoObject {
protected:
  ConstIoProxy(const C* ref) :
	 IoObject(false),
	 itsReferand(const_cast<C*>(ref))
  {}
  virtual ~ConstIoProxy() {}

public:
  static IdItem<IoObject> make(const C* ref)
    { return IdItem<ConstIoProxy>( new ConstIoProxy(ref) ); }

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

  IdItem<C> itsReferand;
};

template <class C>
inline IdItem<IoObject> makeConstProxy(const C* ref)
  { return ConstIoProxy<C>::make(ref); }


} // end namespace IO

static const char vcid_ioproxy_h[] = "$Header$";
#endif // !IOPROXY_H_DEFINED
