///////////////////////////////////////////////////////////////////////
//
// stringifycmd.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 21:43:28 1999
// written: Fri Sep 29 13:45:53 2000
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

#include "util/arrays.h"

#include <cstring>
#include <fstream.h>
#include <strstream.h>
#include <typeinfo>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

Tcl::StringifyCmd::StringifyCmd(Tcl_Interp* interp, const char* cmd_name, 
										  const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::StringifyCmd::~StringifyCmd() {}

void Tcl::StringifyCmd::invoke() {
DOTRACE("Tcl::StringifyCmd::invoke");
  IO::IoObject& io = getIO();

  DebugEval(typeid(io).name());

  ostrstream ost;

  try {
	 IO::LegacyWriter writer(ost, IO::BASES|IO::TYPENAME);
	 writer.writeRoot(&(getIO()));
	 ost << '\0';
  }
  catch (IO::IoError& err) {
	 err.appendMsg(" with buffer contents ==\n");

	 ost << '\0';
	 err.appendMsg(ost.str());

	 ost.rdbuf()->freeze(0); // avoids leaking the buffer memory

	 throw err;
  }

  returnCstring(ost.str());

  ost.rdbuf()->freeze(0); // avoids leaking the buffer memory
}

Tcl::DestringifyCmd::DestringifyCmd(Tcl_Interp* interp, const char* cmd_name, 
												const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::DestringifyCmd::~DestringifyCmd() {}

void Tcl::DestringifyCmd::invoke() {
DOTRACE("Tcl::DestringifyCmd::invoke");

  // We assume that the string is contained in the last argument in the command
  const char* buf = getCstringFromArg(objc() - 1);
  Assert(buf);

  istrstream ist(buf);

  IO::LegacyReader reader(ist, IO::BASES|IO::TYPENAME);
  reader.readRoot(&(getIO()));

  returnVoid();
}

Tcl::WriteCmd::WriteCmd(Tcl_Interp* interp, const char* cmd_name,
								const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::WriteCmd::~WriteCmd() {}

void Tcl::WriteCmd::invoke() {
DOTRACE("Tcl::WriteCmd::invoke");
  IO::IoObject& io = getIO();   

  const int BUF_SIZE = 4096;
  static_block<char, BUF_SIZE> buf;

  ostrstream ost(&(buf[0]), buf.size()-1);

  DebugEval(typeid(io).name());
  DebugEval(buf.size());

  AsciiStreamWriter writer(ost);
  writer.writeRoot(&io);
  ost << '\0';

  returnCstring(&(buf[0]));
}

Tcl::ReadCmd::ReadCmd(Tcl_Interp* interp, const char* cmd_name,
							 const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::ReadCmd::~ReadCmd() {}

void Tcl::ReadCmd::invoke() {
DOTRACE("Tcl::ReadCmd::invoke");
  IO::IoObject& io = getIO();

  const char* str = arg(objc() - 1).getCstring();

  istrstream ist(str);

  AsciiStreamReader reader(ist);
  reader.readRoot(&io);
}

Tcl::ASWSaveCmd::ASWSaveCmd(Tcl_Interp* interp, const char* cmd_name,
									 const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::ASWSaveCmd::~ASWSaveCmd() {}

void Tcl::ASWSaveCmd::invoke() {
DOTRACE("Tcl::ASWSaveCmd::invoke");
  IO::IoObject& io = getIO();
  const char* filename = getFilename();

  STD_IO::ofstream ofs(filename);
  if ( ofs.fail() ) {
	 Tcl::TclError err("couldn't open file ");
	 err.appendMsg("'", filename, "'");
	 err.appendMsg("for writing");
	 throw err;
  }

  AsciiStreamWriter writer(ofs);
  writer.writeRoot(&io);
}

Tcl::ASRLoadCmd::ASRLoadCmd(Tcl_Interp* interp, const char* cmd_name,
									 const char* usage, int objc) :
  TclCmd(interp, cmd_name, usage, objc, objc, true) {}

Tcl::ASRLoadCmd::~ASRLoadCmd() {}

void Tcl::ASRLoadCmd::beforeLoadHook() {}
void Tcl::ASRLoadCmd::afterLoadHook() {}

void Tcl::ASRLoadCmd::invoke() {
DOTRACE("Tcl::ASRLoadCmd::invoke");

  beforeLoadHook(); 

  IO::IoObject& io = getIO();
  const char* filename = getFilename();

  STD_IO::ifstream ifs(filename);
  if ( ifs.fail() ) {
	 Tcl::TclError err("couldn't open file ");
	 err.appendMsg("'", filename, "'");
	 err.appendMsg("for reading");
	 throw err;
  }

  AsciiStreamReader reader(ifs);
  reader.readRoot(&io);

  afterLoadHook();
}

static const char vcid_stringifycmd_cc[] = "$Header$";
#endif // !STRINGIFYCMD_CC_DEFINED
