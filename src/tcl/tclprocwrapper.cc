///////////////////////////////////////////////////////////////////////
//
// tclprocwrapper.cc
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jan 30 11:47:10 2002
// written: Wed Jan 30 11:49:33 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPROCWRAPPER_CC_DEFINED
#define TCLPROCWRAPPER_CC_DEFINED

#include "tcl/tclprocwrapper.h"

#include "io/reader.h"
#include "io/writer.h"
#include "tcl/tclcode.h"

Tcl::ProcWrapper::ProcWrapper(const Tcl::Interp& intp, const fstring& name) :
  itsInterp(intp.intp()),
  itsName(name),
  itsArgs(),
  itsBody()
{}

Tcl::ProcWrapper::~ProcWrapper()
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

void Tcl::ProcWrapper::readFrom(IO::Reader* reader)
{
  fstring args, body;
  reader->readValue("args", args);
  reader->readValue("body", body);
  define(args, body);
}

void Tcl::ProcWrapper::writeTo(IO::Writer* writer) const
{
  writer->writeValue("args", itsArgs);
  writer->writeValue("body", itsBody);
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

void Tcl::ProcWrapper::invoke(const fstring& args) const
{
  fstring cmd = itsName; cmd.append(" ").append(args);

  Tcl::Code code(cmd, Tcl::Code::THROW_EXCEPTION);

  // might throw if Tcl code raises an error:
  code.invoke(itsInterp);
}

static const char vcid_tclprocwrapper_cc[] = "$Header$";
#endif // !TCLPROCWRAPPER_CC_DEFINED
