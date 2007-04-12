/** @file io/ioproxy.h make proxy serializable's that can be used for
    reading/writing base class sub-objects */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Mar 22 21:41:38 2000
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
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

#ifndef GROOVX_IO_IOPROXY_H_UTC20050626084021_DEFINED
#define GROOVX_IO_IOPROXY_H_UTC20050626084021_DEFINED

#include "io/io.h"

#include "nub/ref.h"

#include "rutz/fstring.h"
#include "rutz/demangle.h"

#include <typeinfo>

namespace io
{

//  #######################################################
//  =======================================================

/// proxy makes a proxy serializable that behaves as if it were another type.
/** This is typically used to get an object to behave as if it were
    actually an object of its base class: i.e., all virtual function
    calls get dispatched to the base class rather than to the most
    derived class. This is what is needed when trying to do
    read_from() or write_to() for the base class portion of an
    object. */

template <class C>
class proxy : public serializable
{
protected:
  proxy(C* ref) : serializable(), m_referand(ref) {}
  virtual ~proxy() throw() {}

public:
  static nub::ref<serializable> make(C* ref)
    { return nub::ref<serializable>( new proxy(ref), nub::PRIVATE ); }

  virtual void read_from(io::reader& reader)
    { m_referand->C::read_from(reader); }

  virtual void write_to(io::writer& writer) const
    { m_referand->C::write_to(writer); }

  virtual io::version_id class_version_id() const
    { return m_referand->C::class_version_id(); }

  virtual rutz::fstring obj_typename() const
    { return rutz::demangled_name(typeid(C)); }

private:
  proxy(const proxy&);
  proxy& operator=(const proxy&);

  nub::ref<C> m_referand;
};

template <class C>
inline nub::ref<serializable> make_proxy(C* ref)
  { return proxy<C>::make(ref); }


//  #######################################################
//  =======================================================

/// const_proxy is just like proxy except for const objects.

template <class C>
class const_proxy : public serializable
{
protected:
  const_proxy(const C* ref) : serializable(), m_referand(const_cast<C*>(ref)) {}
  virtual ~const_proxy() throw() {}

public:
  static nub::ref<const serializable> make(const C* ref)
    { return nub::ref<serializable>( new const_proxy(ref), nub::PRIVATE ); }

  virtual void read_from(io::reader& reader)
    { m_referand->C::read_from(reader); }

  virtual void write_to(io::writer& writer) const
    { m_referand->C::write_to(writer); }

  virtual io::version_id class_version_id() const
    { return m_referand->C::class_version_id(); }

  virtual rutz::fstring obj_typename() const
    { return rutz::demangled_name(typeid(C)); }

private:
  const_proxy(const const_proxy&);
  const_proxy& operator=(const const_proxy&);

  nub::ref<C> m_referand;
};

template <class C>
inline nub::ref<const serializable> make_const_proxy(const C* ref)
  { return const_proxy<C>::make(ref); }


} // end namespace io

static const char __attribute__((used)) vcid_groovx_io_ioproxy_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_IOPROXY_H_UTC20050626084021_DEFINED
