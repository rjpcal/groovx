///////////////////////////////////////////////////////////////////////
//
// ioproxy.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Mar 22 21:41:38 2000
// written: Thu Mar 23 00:10:16 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPROXY_H_DEFINED
#define IOPROXY_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H)
#include "io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H)
#include "util/strings.h"
#endif

template <class C>
class IOProxy : public IO {
public:
  IOProxy(C* ref) : itsReferand(ref) {}

  virtual void serialize(ostream& os, IOFlag flag) const
	 { itsReferand->C::serialize(os, flag); }

  virtual void deserialize(istream& is, IOFlag flag)
	 { itsReferand->C::deserialize(is, flag); }

  virtual int charCount() const
	 { return itsReferand->C::charCount(); }

  virtual void readFrom(Reader* reader)
	 { itsReferand->C::readFrom(reader); }

  virtual void writeTo(Writer* writer) const
	 { itsReferand->C::writeTo(writer); }

  virtual unsigned long id() const
	 { return itsReferand->C::id(); }

  virtual unsigned long serialVersionId() const
	 { return itsReferand->C::serialVersionId(); }

  virtual fixed_string ioTypename() const
	 { return itsReferand->C::ioTypename(); }

private:
  C* itsReferand;
};

static const char vcid_ioproxy_h[] = "$Header$";
#endif // !IOPROXY_H_DEFINED
