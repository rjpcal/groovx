///////////////////////////////////////////////////////////////////////
//
// stringifycmd.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jun 11 21:43:28 1999
// written: Mon Jul 16 07:32:18 2001
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

Tcl::StringifyCmd::StringifyCmd(Tcl_Interp* interp, const char* cmd_name)
  :
  TclCmd(interp, cmd_name, "item_id", 2) {}

Tcl::StringifyCmd::~StringifyCmd() {}

void Tcl::StringifyCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::StringifyCmd::invoke");
  Util::Ref<IO::IoObject> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));

  DebugEval(typeid(*item).name());

  ostrstream ost;

  try {
    IO::LegacyWriter writer(ost);
    writer.writeRoot(item.get());
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

Tcl::DestringifyCmd::DestringifyCmd(Tcl_Interp* interp, const char* cmd_name)
  :
  TclCmd(interp, cmd_name, "item_id string", 3) {}

Tcl::DestringifyCmd::~DestringifyCmd() {}

void Tcl::DestringifyCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::DestringifyCmd::invoke");

  // We assume that the string is contained in the last argument in the command
  const char* buf = ctx.getCstringFromArg(ctx.objc() - 1);
  Assert(buf);

  istrstream ist(buf);

  IO::LegacyReader reader(ist);

  Util::Ref<IO::IoObject> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));

  reader.readRoot(item.get());
}


///////////////////////////////////////////////////////////////////////
//
// WriteCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::WriteCmd::WriteCmd(Tcl_Interp* interp, const char* cmd_name)
  :
  TclCmd(interp, cmd_name, "item_id", 2) {}

Tcl::WriteCmd::~WriteCmd() {}

void Tcl::WriteCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::WriteCmd::invoke");
  Util::Ref<IO::IoObject> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));

  ostrstream ost;

  DebugEval(typeid(*item).name());

  try {
    AsciiStreamWriter writer(ost);
    writer.writeRoot(item.get());
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

Tcl::ReadCmd::ReadCmd(Tcl_Interp* interp, const char* cmd_name)
  :
  TclCmd(interp, cmd_name, "item_id string", 3) {}

Tcl::ReadCmd::~ReadCmd() {}

void Tcl::ReadCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::ReadCmd::invoke");
  Util::Ref<IO::IoObject> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));

  const char* str = ctx.getCstringFromArg(ctx.objc() - 1);

  istrstream ist(str);

  AsciiStreamReader reader(ist);
  reader.readRoot(item.get());
}


///////////////////////////////////////////////////////////////////////
//
// ASWSaveCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::ASWSaveCmd::ASWSaveCmd(Tcl_Interp* interp, const char* cmd_name)
  :
  TclCmd(interp, cmd_name, "item_id filename", 3) {}

Tcl::ASWSaveCmd::~ASWSaveCmd() {}

void Tcl::ASWSaveCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::ASWSaveCmd::invoke");
  Util::Ref<IO::IoObject> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));
  fixed_string filename = ctx.getCstringFromArg(2);

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
    writer.writeRoot(item.get());
  }
  else {
    AsciiStreamWriter writer(ofs);
    writer.writeRoot(item.get());
  }
}


///////////////////////////////////////////////////////////////////////
//
// ASRLoadCmd
//
///////////////////////////////////////////////////////////////////////

Tcl::ASRLoadCmd::ASRLoadCmd(Tcl_Interp* interp, const char* cmd_name)
  :
  TclCmd(interp, cmd_name, "item_id filename", 3) {}

Tcl::ASRLoadCmd::~ASRLoadCmd() {}

void Tcl::ASRLoadCmd::invoke(Tcl::Context& ctx)
{
DOTRACE("Tcl::ASRLoadCmd::invoke");

  Util::Ref<IO::IoObject> item(ctx.getValFromArg(1, TypeCue<Util::UID>()));
  fixed_string filename = ctx.getCstringFromArg(2);

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
    reader.readRoot(item.get());
  }
  else {
    AsciiStreamReader reader(ifs);
    reader.readRoot(item.get());
  }
}

static const char vcid_stringifycmd_cc[] = "$Header$";
#endif // !STRINGIFYCMD_CC_DEFINED
