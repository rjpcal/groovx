///////////////////////////////////////////////////////////////////////
//
// stringifycmd.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 21:43:28 1999
// written: Thu Jul 12 13:23:43 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_CC_DEFINED
#define STRINGIFYCMD_CC_DEFINED

#include "tcl/stringifycmd.h"

#include "io/asciistreamreader.h"
#include "io/asciistreamwriter.h"
#include "io/io.h"
#include "io/iolegacy.h"

#include "tcl/tclerror.h"
#include "tcl/tclvalue.h"

#include "util/arrays.h"
#include "util/gzstreambuf.h"
#include "util/ref.h"
#include "util/strings.h"

#include <cstring>
#include <fstream.h>
#include <strstream.h>
#include <typeinfo>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"


///////////////////////////////////////////////////////////////////////
//
// StringifyCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::StringifyCmd::StringifyCmd(Tcl_Interp* interp, const char* cmd_name,
                                const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::StringifyCmd::~StringifyCmd() {}

void Tcl::StringifyCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::StringifyCmd::invoke");
  IO::IoObject& io = getIO(ctx);

  DebugEval(typeid(io).name());

  ostrstream ost;

  try {
    IO::LegacyWriter writer(ost);
    writer.writeRoot(&io);
    ost << '\0';
  }
  catch (IO::IoError& err) {
    err.appendMsg(" with buffer contents ==\n");

    ost << '\0';
    err.appendMsg(ost.str());

    ost.rdbuf()->freeze(0); // avoids leaking the buffer memory

    throw err;
  }
  catch (...) {
    ost.rdbuf()->freeze(0); // avoids leaking the buffer memory
    throw;
  }

  ctx.setResult(ost.str());

  ost.rdbuf()->freeze(0); // avoids leaking the buffer memory
}


///////////////////////////////////////////////////////////////////////
//
// DestringifyCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::DestringifyCmd::DestringifyCmd(Tcl_Interp* interp, const char* cmd_name,
                                    const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::DestringifyCmd::~DestringifyCmd() {}

void Tcl::DestringifyCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::DestringifyCmd::invoke");

  // We assume that the string is contained in the last argument in the command
  const char* buf = ctx.getCstringFromArg(ctx.objc() - 1);
  Assert(buf);

  istrstream ist(buf);

  IO::LegacyReader reader(ist);
  reader.readRoot(&(getIO(ctx)));
}


///////////////////////////////////////////////////////////////////////
//
// WriteCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::WriteCmd::WriteCmd(Tcl_Interp* interp, const char* cmd_name,
                        const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::WriteCmd::~WriteCmd() {}

void Tcl::WriteCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::WriteCmd::invoke");
  IO::IoObject& io = getIO(ctx);

  ostrstream ost;

  DebugEval(typeid(io).name());

  try {
    AsciiStreamWriter writer(ost);
    writer.writeRoot(&io);
    ost << '\0';
  }
  catch (IO::IoError& err) {
    err.appendMsg(" with buffer contents ==\n");

    ost << '\0';
    err.appendMsg(ost.str());

    ost.rdbuf()->freeze(0); // avoids leaking the buffer memory

    throw err;
  }
  catch (...) {
    ost.rdbuf()->freeze(0); // avoids leaking the buffer memory
    throw;
  }

  ctx.setResult(ost.str());

  ost.rdbuf()->freeze(0); // avoids leaking the buffer memory
}


///////////////////////////////////////////////////////////////////////
//
// ReadCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::ReadCmd::ReadCmd(Tcl_Interp* interp, const char* cmd_name,
                      const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::ReadCmd::~ReadCmd() {}

void Tcl::ReadCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::ReadCmd::invoke");
  IO::IoObject& io = getIO(ctx);

  const char* str = ctx.getCstringFromArg(ctx.objc() - 1);

  istrstream ist(str);

  AsciiStreamReader reader(ist);
  reader.readRoot(&io);
}


///////////////////////////////////////////////////////////////////////
//
// ASWSaveCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::ASWSaveCmd::ASWSaveCmd(Tcl_Interp* interp, const char* cmd_name,
                            const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::ASWSaveCmd::~ASWSaveCmd() {}

void Tcl::ASWSaveCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::ASWSaveCmd::invoke");
  IO::IoObject& io = getIO(ctx);
  fixed_string filename = getFilename(ctx);

  STD_IO::ofstream ofs(filename.c_str());
  if ( ofs.fail() ) {
    Tcl::TclError err("couldn't open file ");
    err.appendMsg("'", filename.c_str(), "'");
    err.appendMsg("for writing");
    throw err;
  }

  string_literal gz_ext(".gz");

  const char* filename_ext = filename.c_str() + filename.length() - 3;

  if ( gz_ext == filename_ext ) {
    ofs.close();

    Util::gzstreambuf buf(filename.c_str(), STD_IO::ios::out);
    STD_IO::ostream os(&buf);

    AsciiStreamWriter writer(os);
    writer.writeRoot(&io);
  }
  else {
    AsciiStreamWriter writer(ofs);
    writer.writeRoot(&io);
  }
}


///////////////////////////////////////////////////////////////////////
//
// ASRLoadCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::ASRLoadCmd::ASRLoadCmd(Tcl_Interp* interp, const char* cmd_name,
                            const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::ASRLoadCmd::~ASRLoadCmd() {}

void Tcl::ASRLoadCmd::beforeLoadHook() {}
void Tcl::ASRLoadCmd::afterLoadHook() {}

void Tcl::ASRLoadCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::ASRLoadCmd::invoke");

  beforeLoadHook();

  IO::IoObject& io = getIO(ctx);
  fixed_string filename = getFilename(ctx);

  STD_IO::ifstream ifs(filename.c_str());
  if ( ifs.fail() ) {
    Tcl::TclError err("couldn't open file ");
    err.appendMsg("'", filename.c_str(), "'");
    err.appendMsg("for reading");
    throw err;
  }

  string_literal gz_ext(".gz");

  const char* filename_ext = filename.c_str() + filename.length() - 3;

  if ( gz_ext == filename_ext ) {
    ifs.close();

    Util::gzstreambuf buf(filename.c_str(), STD_IO::ios::in);
    STD_IO::istream is(&buf);

    AsciiStreamReader reader(is);
    reader.readRoot(&io);
  }
  else {
    AsciiStreamReader reader(ifs);
    reader.readRoot(&io);
  }

  afterLoadHook();
}

static const char vcid_stringifycmd_cc[] = "$Header$";
#endif // !STRINGIFYCMD_CC_DEFINED
