///////////////////////////////////////////////////////////////////////
//
// objpkg.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Nov 22 17:05:11 2002
// commit: $Id$
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

#ifndef OBJPKG_H_DEFINED
#define OBJPKG_H_DEFINED

#include "nub/objfactory.h"

#include "util/sharedptr.h"

namespace rutz
{
  struct file_pos;
}

namespace Tcl
{
  class ObjCaster;
  class Pkg;
}

namespace Nub
{
  class Object;
}

// ########################################################
/// ObjCaster class encapsulates casts to see if objects match a given type.

class Tcl::ObjCaster
{
protected:
  ObjCaster();

public:
  virtual ~ObjCaster();

  virtual bool isMyType(const Nub::Object* obj) const = 0;

  virtual unsigned int getSizeof() const = 0;

  bool isNotMyType(const Nub::Object* obj) const { return !isMyType(obj); }

  bool isIdMyType(Nub::UID uid) const;

  bool isIdNotMyType(Nub::UID uid) const { return !isIdMyType(uid); }
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

    virtual bool isMyType(const Nub::Object* obj) const
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
      Nub::ObjFactory::theOne().register_creator(&C::make);

    if (aliasName != 0)
      Nub::ObjFactory::theOne().register_alias(origName, aliasName);
  }
}

static const char vcid_objpkg_h[] = "$Id$ $URL$";
#endif // !OBJPKG_H_DEFINED
