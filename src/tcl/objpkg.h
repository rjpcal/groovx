///////////////////////////////////////////////////////////////////////
//
// objpkg.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Fri Nov 22 17:05:11 2002
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

#ifndef GROOVX_TCL_OBJPKG_H_UTC20050626084018_DEFINED
#define GROOVX_TCL_OBJPKG_H_UTC20050626084018_DEFINED

#include "nub/objfactory.h"

#include "rutz/sharedptr.h"

namespace rutz
{
  struct file_pos;
}

namespace Tcl
{
  class ObjCaster;
  class Pkg;
}

namespace nub
{
  class object;
}

// ########################################################
/// ObjCaster class encapsulates casts to see if objects match a given type.

class Tcl::ObjCaster
{
protected:
  ObjCaster();

public:
  virtual ~ObjCaster();

  virtual bool isMyType(const nub::object* obj) const = 0;

  virtual unsigned int getSizeof() const = 0;

  bool isNotMyType(const nub::object* obj) const { return !isMyType(obj); }

  bool isIdMyType(nub::uid uid) const;

  bool isIdNotMyType(nub::uid uid) const { return !isIdMyType(uid); }
};

namespace Tcl
{
  /// CObjCaster implements ObjCaster with dynamic_cast.
  template <class C>
  class CObjCaster : public ObjCaster
  {
  public:
    virtual unsigned int getSizeof() const
    {
      return sizeof(C);
    }

    virtual bool isMyType(const nub::object* obj) const
    {
      return (obj != 0 && dynamic_cast<const C*>(obj) != 0);
    }
  };

  void defGenericObjCmds(Pkg* pkg, rutz::shared_ptr<ObjCaster> caster,
                         const rutz::file_pos& src_pos);

  template <class C>
  void defGenericObjCmds(Pkg* pkg, const rutz::file_pos& src_pos)
  {
    rutz::shared_ptr<ObjCaster> caster(new CObjCaster<C>);
    defGenericObjCmds(pkg, caster, src_pos);
  }

  template <class C>
  void defCreator(Pkg*, const char* aliasName = 0)
  {
    const char* origName =
      nub::obj_factory::instance().register_creator(&C::make);

    if (aliasName != 0)
      nub::obj_factory::instance().register_alias(origName, aliasName);
  }
}

static const char vcid_groovx_tcl_objpkg_h_utc20050626084018[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_OBJPKG_H_UTC20050626084018_DEFINED
