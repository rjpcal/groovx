/** @file io/tclprocwrapper.h serializable wrapper for Tcl script
    'proc' definitions */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jan 30 11:41:47 2002
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

#ifndef GROOVX_TCL_IO_TCLPROCWRAPPER_H_UTC20050712162004_DEFINED
#define GROOVX_TCL_IO_TCLPROCWRAPPER_H_UTC20050712162004_DEFINED

#include "io/io.h"

#include "tcl/interp.h"

#include "rutz/fstring.h"

namespace tcl
{
  class ProcWrapper;
}

/// A serializable wrapper for Tcl script procedures.
class tcl::ProcWrapper : public io::serializable
{
private:
  ProcWrapper(const ProcWrapper&);
  ProcWrapper& operator=(const ProcWrapper&);

  tcl::interpreter itsInterp;
  const rutz::fstring itsName;
  rutz::fstring itsArgs;
  rutz::fstring itsBody;

public:
  /// Define with a unique generic name.
  ProcWrapper(const tcl::interpreter& intp);

  /// Define with a given name.
  ProcWrapper(const tcl::interpreter& intp, const rutz::fstring& name);

  virtual ~ProcWrapper() throw();

  virtual void read_from(io::reader& reader);
  virtual void write_to(io::writer& writer) const;

  /** Redefine the code chunk with a new args and body, but keeping the same
      name as always. */
  void define(const rutz::fstring& args, const rutz::fstring& body);

  /// Query whether the code chunk is a no-op (i.e. both args+body are empty)
  bool isNoop() const;

  // Just run the code, but don't yet extract a result from the interpreter
  void invoke(const rutz::fstring& args);

  template <class T>
  T call(const rutz::fstring& args)
  {
    // might throw if Tcl code raises an error:
    invoke(args);

    // might throw if conversion to T fails:
    return itsInterp.template get_result<T>();
  }

  const rutz::fstring& name() const { return itsName; }
  const rutz::fstring& args() const { return itsArgs; }
  const rutz::fstring& body() const { return itsBody; }

  rutz::fstring fullSpec() const;
};

static const char __attribute__((used)) vcid_groovx_tcl_io_tclprocwrapper_h_utc20050712162004[] = "$Id$ $HeadURL$";
#endif // !GROOVX_TCL_IO_TCLPROCWRAPPER_H_UTC20050712162004_DEFINED
