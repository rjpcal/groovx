///////////////////////////////////////////////////////////////////////
//
// tclprocwrapper.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jan 30 11:41:47 2002
// written: Wed Jan 30 15:07:54 2002
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
  void invoke(const fstring& args) const;

  template <class T>
  T call(const fstring& args) const
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
