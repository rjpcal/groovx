/** @file io/tclprocwrapper.cc serializable wrapper for Tcl script
    'proc' definitions */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jan 30 11:47:10 2002
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

#ifndef GROOVX_TCL_IO_TCLPROCWRAPPER_CC_UTC20050712162004_DEFINED
#define GROOVX_TCL_IO_TCLPROCWRAPPER_CC_UTC20050712162004_DEFINED

#include "tcl-io/tclprocwrapper.h"

#include "io/reader.h"
#include "io/writer.h"

#include "rutz/sfmt.h"

using rutz::fstring;

namespace
{
  fstring uniqName()
  {
    static int c = 0;
    return rutz::sfmt("::ProcWrapper_UniqCmd%d", ++c);
  }
}

tcl::ProcWrapper::ProcWrapper(const tcl::interpreter& interp) :
  itsInterp(interp),
  itsName(uniqName()),
  itsArgs(),
  itsBody()
{}

tcl::ProcWrapper::ProcWrapper(const tcl::interpreter& interp,
                              const fstring& name) :
  itsInterp(interp),
  itsName(name),
  itsArgs(),
  itsBody()
{}

tcl::ProcWrapper::~ProcWrapper() throw()
{
  // We must check is_valid() in case we are here because the application
  // is being exited, causing the interpreter to be deleted, in which case
  // we can't use the intrepeter (and don't need to, since it will destroy
  // the proc on its own)
  if (itsInterp.is_valid())
    {
      if (itsInterp.has_command(itsName.c_str()))
        itsInterp.delete_proc("", itsName.c_str());
    }
}

void tcl::ProcWrapper::read_from(io::reader& reader)
{
  fstring args, body;
  reader.read_value("args", args);
  reader.read_value("body", body);
  define(args, body);
}

void tcl::ProcWrapper::write_to(io::writer& writer) const
{
  writer.write_value("args", itsArgs);
  writer.write_value("body", itsBody);
}

void tcl::ProcWrapper::define(const fstring& args, const fstring& body)
{
  itsArgs = args;
  itsBody = body;
  itsInterp.create_proc("", itsName.c_str(),
                       itsArgs.c_str(), itsBody.c_str());
}

bool tcl::ProcWrapper::isNoop() const
{
  return (itsArgs.is_empty() && itsBody.is_empty());
}

void tcl::ProcWrapper::invoke(const fstring& args)
{
  if (isNoop()) return;

  itsInterp.eval(rutz::sfmt("%s %s", itsName.c_str(), args.c_str()));
}

fstring tcl::ProcWrapper::fullSpec() const
{
  return rutz::sfmt("{%s} {%s}", itsArgs.c_str(), itsBody.c_str());
}

#endif // !GROOVX_TCL_IO_TCLPROCWRAPPER_CC_UTC20050712162004_DEFINED
