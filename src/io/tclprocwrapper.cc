///////////////////////////////////////////////////////////////////////
//
// tclprocwrapper.cc
//
// Copyright (c) 2002-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Wed Jan 30 11:47:10 2002
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

#ifndef TCLPROCWRAPPER_CC_DEFINED
#define TCLPROCWRAPPER_CC_DEFINED

#include "io/tclprocwrapper.h"

#include "io/reader.h"
#include "io/writer.h"

namespace
{
  fstring uniqName()
  {
    static int c = 0;
    return fstring("::ProcWrapper_UniqCmd", ++c);
  }
}

Tcl::ProcWrapper::ProcWrapper(const Tcl::Interp& interp) :
  itsInterp(interp),
  itsName(uniqName()),
  itsArgs(),
  itsBody()
{}

Tcl::ProcWrapper::ProcWrapper(const Tcl::Interp& interp, const fstring& name) :
  itsInterp(interp),
  itsName(name),
  itsArgs(),
  itsBody()
{}

Tcl::ProcWrapper::~ProcWrapper() throw()
{
  // We must check hasInterp() in case we are here because the application
  // is being exited, causing the interpreter to be deleted, in which case
  // we can't use the intrepeter (and don't need to, since it will destroy
  // the proc on its own)
  if (itsInterp.hasInterp())
    {
      if (itsInterp.hasCommand(itsName.c_str()))
        itsInterp.deleteProc("", itsName.c_str());
    }
}

void Tcl::ProcWrapper::readFrom(IO::Reader& reader)
{
  fstring args, body;
  reader.readValue("args", args);
  reader.readValue("body", body);
  define(args, body);
}

void Tcl::ProcWrapper::writeTo(IO::Writer& writer) const
{
  writer.writeValue("args", itsArgs);
  writer.writeValue("body", itsBody);
}

void Tcl::ProcWrapper::define(const fstring& args, const fstring& body)
{
  itsArgs = args;
  itsBody = body;
  itsInterp.createProc("", itsName.c_str(),
                       itsArgs.c_str(), itsBody.c_str());
}

bool Tcl::ProcWrapper::isNoop() const
{
  return (itsArgs.is_empty() && itsBody.is_empty());
}

void Tcl::ProcWrapper::invoke(const fstring& args)
{
  if (isNoop()) return;

  fstring cmd(itsName, " ", args);

  itsInterp.eval(cmd);
}

fstring Tcl::ProcWrapper::fullSpec() const
{
  fstring result;
  result.append('{', itsArgs, "} ");
  result.append('{', itsBody, '}');
  return result;
}

static const char vcid_tclprocwrapper_cc[] = "$Header$";
#endif // !TCLPROCWRAPPER_CC_DEFINED
