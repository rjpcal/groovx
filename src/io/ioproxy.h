///////////////////////////////////////////////////////////////////////
//
// ioproxy.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Mar 22 21:41:38 2000
// written: Wed Sep 25 18:54:53 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPROXY_H_DEFINED
#define IOPROXY_H_DEFINED

#include "util/ref.h"
#include "util/strings.h"

#include "io/io.h"

#include "system/demangle.h"

#include <typeinfo>

namespace IO
{

//  ###################################################################
//  ===================================================================

/// IoProxy makes a proxy IoObject that behaves as if it were another type.
/** This is typically used to get an object to behave as if it were
    actually an object of its base class: i.e., all virtual function calls
    get dispatched to the base class rather than to the most derived
    class. This is what is needed when trying to do readFrom() or writeTo()
    for the base class portion of an object. */

template <class C>
class IoProxy : public IoObject
{
protected:
  IoProxy(C* ref) : IoObject(), itsReferand(ref) {}
  virtual ~IoProxy() {}

public:
  static Util::Ref<IoObject> make(C* ref)
    { return Util::Ref<IoObject>( new IoProxy(ref), Util::PRIVATE ); }

  virtual void readFrom(Reader* reader)
    { itsReferand->C::readFrom(reader); }

  virtual void writeTo(Writer* writer) const
    { itsReferand->C::writeTo(writer); }

  virtual VersionId serialVersionId() const
    { return itsReferand->C::serialVersionId(); }

  virtual fstring ioTypename() const
    { return demangle_cstr(typeid(C).name()); }

private:
  IoProxy(const IoProxy&);
  IoProxy& operator=(const IoProxy&);

  Util::Ref<C> itsReferand;
};

template <class C>
inline Util::Ref<IoObject> makeProxy(C* ref)
  { return IoProxy<C>::make(ref); }


//  ###################################################################
//  ===================================================================

/// ConstIoProxy is just like IoProxy except for const objects.

template <class C>
class ConstIoProxy : public IoObject
{
protected:
  ConstIoProxy(const C* ref) : IoObject(), itsReferand(const_cast<C*>(ref)) {}
  virtual ~ConstIoProxy() {}

public:
  static Util::Ref<const IoObject> make(const C* ref)
    { return Util::Ref<IoObject>( new ConstIoProxy(ref), Util::PRIVATE ); }

  virtual void readFrom(Reader* reader)
    { itsReferand->C::readFrom(reader); }

  virtual void writeTo(Writer* writer) const
    { itsReferand->C::writeTo(writer); }

  virtual VersionId serialVersionId() const
    { return itsReferand->C::serialVersionId(); }

  virtual fstring ioTypename() const
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
