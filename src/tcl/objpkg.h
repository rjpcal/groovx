///////////////////////////////////////////////////////////////////////
//
// objpkg.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Nov 22 17:05:11 2002
// written: Fri Nov 22 17:18:36 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef OBJPKG_H_DEFINED
#define OBJPKG_H_DEFINED

#include "util/objfactory.h"
#include "util/pointers.h"

namespace Tcl
{
  class ObjCaster;
  class Pkg;
}

namespace Util
{
  class Object;
}

// ####################################################################
/// ObjCaster class encapsulates casts to see if objects match a given type.

class Tcl::ObjCaster
{
protected:
  ObjCaster();

public:
  virtual ~ObjCaster();

  virtual bool isMyType(const Util::Object* obj) = 0;

  bool isNotMyType(const Util::Object* obj) { return !isMyType(obj); }

  bool isIdMyType(Util::UID uid);

  bool isIdNotMyType(Util::UID uid) { return !isIdMyType(uid); }
};

namespace Tcl
{
  /// CObjCaster implements ObjCaster with dynamic_cast.
  template <class C>
  class CObjCaster : public ObjCaster
  {
  public:
    virtual bool isMyType(const Util::Object* obj)
    {
      return (obj != 0 && dynamic_cast<const C*>(obj) != 0);
    }
  };

  void defGenericObjCmds(Pkg* pkg, shared_ptr<ObjCaster> caster);

  template <class C>
  void defGenericObjCmds(Pkg* pkg)
  {
    shared_ptr<ObjCaster> caster(new CObjCaster<C>);
    defGenericObjCmds(pkg, caster);
  }

  template <class C>
  void defCreator(Pkg*, const char* aliasName = 0)
  {
    const char* origName =
      Util::ObjFactory::theOne().registerCreatorFunc(&C::make);

    if (aliasName != 0)
      Util::ObjFactory::theOne().registerAlias(origName, aliasName);
  }
}

static const char vcid_objpkg_h[] = "$Header$";
#endif // !OBJPKG_H_DEFINED
