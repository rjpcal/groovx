///////////////////////////////////////////////////////////////////////
//
// tclprocwrapper.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jan 30 11:41:47 2002
// written: Wed Jan 30 11:44:26 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TCLPROCWRAPPER_H_DEFINED
#define TCLPROCWRAPPER_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#include "io/io.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(TCLSAFEINTERP_H_DEFINED)
#include "tcl/tclsafeinterp.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H_DEFINED)
#include "util/strings.h"
#endif

#include "io/reader.h"
#include "io/writer.h"
#include "tcl/tclcode.h"

namespace Tcl
{
  class ProcWrapper;
}

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
  ProcWrapper(const Tcl::Interp& intp, const fstring& name) :
    itsInterp(intp.intp()),
    itsName(name),
    itsArgs(),
    itsBody()
  {}

  ~ProcWrapper()
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

  virtual void readFrom(IO::Reader* reader)
  {
    fstring args, body;
    reader->readValue("args", args);
    reader->readValue("body", body);
    define(args, body);
  }

  virtual void writeTo(IO::Writer* writer) const
  {
    writer->writeValue("args", itsArgs);
    writer->writeValue("body", itsBody);
  }

  void define(const fstring& args, const fstring& body)
  {
    itsArgs = args;
    itsBody = body;
    itsInterp.createProc("", itsName.c_str(),
                         itsArgs.c_str(), itsBody.c_str());
  }

  bool isNoop() const { return (itsArgs.is_empty() && itsBody.is_empty()); }

  template <class T>
  T call(const fstring& args) const
  {
    fstring cmd = itsName; cmd.append(" ").append(args);

    Tcl::Code code(cmd, Tcl::Code::THROW_EXCEPTION);

    // might throw if Tcl code raises an error:
    code.invoke(itsInterp);

    // might throw if conversion to T fails:
    return itsInterp.template getResult<T>();
  }

  const fstring& name() const { return itsName; }
  const fstring& args() const { return itsArgs; }
  const fstring& body() const { return itsBody; }
};

static const char vcid_tclprocwrapper_h[] = "$Header$";
#endif // !TCLPROCWRAPPER_H_DEFINED
