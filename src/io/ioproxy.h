///////////////////////////////////////////////////////////////////////
//
// ioproxy.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 22 21:41:38 2000
// written: Thu May 10 12:04:37 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPROXY_H_DEFINED
#define IOPROXY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
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
public:
  IoProxy(C* ref) : itsReferand(ref) {}
  virtual ~IoProxy() {}

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
class ConstIoProxy : public IO::IoObject {
public:
  ConstIoProxy(const C* ref) : itsReferand(ref) {}
  virtual ~ConstIoProxy() {}

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

} // end namespace IO

static const char vcid_ioproxy_h[] = "$Header$";
#endif // !IOPROXY_H_DEFINED
