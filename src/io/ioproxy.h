///////////////////////////////////////////////////////////////////////
//
// ioproxy.h
//
// Copyright (c) 2000-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Mar 22 21:41:38 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef IOPROXY_H_DEFINED
#define IOPROXY_H_DEFINED

#include "io/io.h"

#include "nub/ref.h"

#include "rutz/fstring.h"
#include "rutz/demangle.h"

#include <typeinfo>

namespace IO
{

//  #######################################################
//  =======================================================

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
  virtual ~IoProxy() throw() {}

public:
  static Nub::Ref<IoObject> make(C* ref)
    { return Nub::Ref<IoObject>( new IoProxy(ref), Nub::PRIVATE ); }

  virtual void readFrom(Reader& reader)
    { itsReferand->C::readFrom(reader); }

  virtual void writeTo(Writer& writer) const
    { itsReferand->C::writeTo(writer); }

  virtual VersionId serialVersionId() const
    { return itsReferand->C::serialVersionId(); }

  virtual rutz::fstring objTypename() const
    { return rutz::demangled_name(typeid(C)); }

private:
  IoProxy(const IoProxy&);
  IoProxy& operator=(const IoProxy&);

  Nub::Ref<C> itsReferand;
};

template <class C>
inline Nub::Ref<IoObject> makeProxy(C* ref)
  { return IoProxy<C>::make(ref); }


//  #######################################################
//  =======================================================

/// ConstIoProxy is just like IoProxy except for const objects.

template <class C>
class ConstIoProxy : public IoObject
{
protected:
  ConstIoProxy(const C* ref) : IoObject(), itsReferand(const_cast<C*>(ref)) {}
  virtual ~ConstIoProxy() throw() {}

public:
  static Nub::Ref<const IoObject> make(const C* ref)
    { return Nub::Ref<IoObject>( new ConstIoProxy(ref), Nub::PRIVATE ); }

  virtual void readFrom(Reader& reader)
    { itsReferand->C::readFrom(reader); }

  virtual void writeTo(Writer& writer) const
    { itsReferand->C::writeTo(writer); }

  virtual VersionId serialVersionId() const
    { return itsReferand->C::serialVersionId(); }

  virtual rutz::fstring objTypename() const
    { return rutz::demangled_name(typeid(C)); }

private:
  ConstIoProxy(const ConstIoProxy&);
  ConstIoProxy& operator=(const ConstIoProxy&);

  Nub::Ref<C> itsReferand;
};

template <class C>
inline Nub::Ref<const IoObject> makeConstProxy(const C* ref)
  { return ConstIoProxy<C>::make(ref); }


} // end namespace IO

static const char vcid_ioproxy_h[] = "$Id$ $URL$";
#endif // !IOPROXY_H_DEFINED
