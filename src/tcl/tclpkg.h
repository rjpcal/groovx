///////////////////////////////////////////////////////////////////////
//
// tclpkg.h
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Jun 15 12:33:59 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPKG_H_DEFINED
#define TCLPKG_H_DEFINED

#include "tcl/tclfunctor.h"
#include "tcl/tclpkgbase.h"

namespace Tcl
{
  class Pkg;
}

///////////////////////////////////////////////////////////////////////
/**
 *
 * Tcl::Pkg extends Tcl::PkgBase by providing a set of functions to
 * define Tcl commands from C++ functors.
 *
 **/
///////////////////////////////////////////////////////////////////////

class Tcl::Pkg : public PkgBase
{
public:
  Pkg(Tcl_Interp* interp, const char* pkg_name, const char* version);

  virtual ~Pkg();

  template <class Func>
  inline void def(const char* cmd_name, const char* usage, Func f)
  {
    addCommand( makeCmd(interp(), f, makePkgCmdName(cmd_name), usage) );
  }

  template <class Func>
  inline void defVec(const char* cmd_name, const char* usage, Func f,
                     unsigned int keyarg=1)
  {
    addCommand( makeVecCmd(interp(), f, makePkgCmdName(cmd_name),
                           usage, keyarg) );
  }

  template <class Func>
  inline void defRaw(const char* cmd_name, unsigned int nargs,
                     const char* usage, Func f)
  {
    addCommand( makeGenericCmd(interp(), f, makePkgCmdName(cmd_name),
                               usage, nargs) );
  }

  template <class Func>
  inline void defVecRaw(const char* cmd_name, unsigned int nargs,
                        const char* usage, Func f,
                        unsigned int keyarg=1)
  {
    addCommand( makeGenericVecCmd(interp(), f, makePkgCmdName(cmd_name),
                                  usage, nargs, keyarg) );
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) ())
  {
    defVec( cmd_name, actionUsage(""), actionFunc );
  }

  template <class C>
  void defAction(const char* cmd_name, void (C::* actionFunc) () const)
  {
    defVec( cmd_name, actionUsage(""), actionFunc );
  }

  template <class C, class T>
  void defGetter(const char* cmd_name, const char* usage,
                 T (C::* getterFunc) () const)
  {
    defVec( cmd_name, getterUsage(usage), getterFunc );
  }

  template <class C, class T>
  void defGetter(const char* cmd_name, T (C::* getterFunc) () const)
  {
    defVec( cmd_name, getterUsage(""), getterFunc );
  }

  template <class C, class T>
  void defSetter(const char* cmd_name, const char* usage,
                 void (C::* setterFunc) (T))
  {
    defVec( cmd_name, setterUsage(usage), setterFunc );
  }

  template <class C, class T>
  void defSetter(const char* cmd_name, void (C::* setterFunc) (T))
  {
    defVec( cmd_name, setterUsage(""), setterFunc );
  }

  template <class C, class T>
  void defAttrib(const char* cmd_name,
                 T (C::* getterFunc) () const,
                 void (C::* setterFunc) (T))
  {
    defGetter( cmd_name, getterFunc );
    defSetter( cmd_name, setterFunc );
  }

protected:
  static const char* actionUsage(const char* usage);
  static const char* getterUsage(const char* usage);
  static const char* setterUsage(const char* usage);
};

static const char vcid_tclpkg_h[] = "$Header$";
#endif // !TCLPKG_H_DEFINED
