///////////////////////////////////////////////////////////////////////
//
// ioproxy.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Mar 22 21:41:38 2000
// written: Wed Sep 27 14:42:45 2000
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

namespace IO {

template <class C>
class IoProxy : public IO::IoObject {
public:
  IoProxy(C* ref) : itsReferand(ref) {}

  virtual void legacySrlz(IO::Writer* writer) const
	 { itsReferand->C::legacySrlz(writer); }

  virtual void legacyDesrlz(IO::Reader* reader)
	 { itsReferand->C::legacyDesrlz(reader); }

  virtual void readFrom(IO::Reader* reader)
	 { itsReferand->C::readFrom(reader); }

  virtual void writeTo(IO::Writer* writer) const
	 { itsReferand->C::writeTo(writer); }

  virtual unsigned long id() const
	 { return itsReferand->C::id(); }

  virtual IO::VersionId serialVersionId() const
	 { return itsReferand->C::serialVersionId(); }

  virtual fixed_string ioTypename() const
	 { return itsReferand->C::ioTypename(); }

private:
  IoProxy(const IoProxy&);
  IoProxy& operator=(const IoProxy&);

  C* itsReferand;
};

template <class C>
class ConstIoProxy : public IO::IoObject {
public:
  ConstIoProxy(const C* ref) : itsReferand(ref) {}

  virtual void legacySrlz(IO::Writer* writer) const
	 { itsReferand->C::legacySrlz(writer); }

  virtual void legacyDesrlz(IO::Reader* reader)
	 { const_cast<C*>(itsReferand)->C::legacyDesrlz(reader); }

  virtual void readFrom(IO::Reader* reader)
	 { const_cast<C*>(itsReferand)->C::readFrom(reader); }

  virtual void writeTo(IO::Writer* writer) const
	 { itsReferand->C::writeTo(writer); }

  virtual unsigned long id() const
	 { return itsReferand->C::id(); }

  virtual IO::VersionId serialVersionId() const
	 { return itsReferand->C::serialVersionId(); }

  virtual fixed_string ioTypename() const
	 { return itsReferand->C::ioTypename(); }

private:
  ConstIoProxy(const ConstIoProxy&);
  ConstIoProxy& operator=(const ConstIoProxy&);

  const C* itsReferand;
};

} // end namespace IO

static const char vcid_ioproxy_h[] = "$Header$";
#endif // !IOPROXY_H_DEFINED
