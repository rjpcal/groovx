///////////////////////////////////////////////////////////////////////
//
// tclprocwrapper.h
//
// Copyright (c) 2002-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jan 30 11:41:47 2002
// written: Sat Mar 29 12:54:17 2003
// $Id$
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

#ifndef TCLPROCWRAPPER_H_DEFINED
#define TCLPROCWRAPPER_H_DEFINED

#include "io/io.h"

#include "tcl/tclsafeinterp.h"

#include "util/strings.h"

namespace Tcl
{
  class ProcWrapper;
}

/// A serializable wrapper for Tcl script procedures.
class Tcl::ProcWrapper : public IO::IoObject
{
private:
  ProcWrapper(const ProcWrapper&);
  ProcWrapper& operator=(const ProcWrapper&);

  Tcl::Interp itsInterp;
  const fstring itsName;
  fstring itsArgs;
  fstring itsBody;

public:
  /// Define with a unique generic name.
  ProcWrapper(const Tcl::Interp& intp);

  /// Define with a given name.
  ProcWrapper(const Tcl::Interp& intp, const fstring& name);

  ~ProcWrapper();

  virtual void readFrom(IO::Reader* reader);
  virtual void writeTo(IO::Writer* writer) const;

  /** Redefine the code chunk with a new args and body, but keeping the same
      name as always. */
  void define(const fstring& args, const fstring& body);

  /// Query whether the code chunk is a no-op (i.e. both args+body are empty)
  bool isNoop() const;

  // Just run the code, but don't yet extract a result from the interpreter
  void invoke(const fstring& args);

  template <class T>
  T call(const fstring& args)
  {
    // might throw if Tcl code raises an error:
    invoke(args);

    // might throw if conversion to T fails:
    return itsInterp.template getResult<T>();
  }

  const fstring& name() const { return itsName; }
  const fstring& args() const { return itsArgs; }
  const fstring& body() const { return itsBody; }

  fstring fullSpec() const;
};

static const char vcid_tclprocwrapper_h[] = "$Header$";
#endif // !TCLPROCWRAPPER_H_DEFINED
