///////////////////////////////////////////////////////////////////////
//
// stringifycmd.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 21:43:28 1999
// written: Mon Mar  6 16:53:17 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_CC_DEFINED
#define STRINGIFYCMD_CC_DEFINED

#include "stringifycmd.h"

#include "io.h"
#include "util/arrays.h"

#include <cstring>
#include <strstream.h>
#include <typeinfo>

#include "asciistreamreader.h"
#include "asciistreamwriter.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

void Tcl::StringifyCmd::invoke() {
DOTRACE("Tcl::StringifyCmd::invoke");
  IO& io = getIO();

  int buf_size = io.charCount();

  // Give ourselves a little extra space above what is returned by
  // io.charCount, so we have a chance to detect underestimates by
  // charCount.
  fixed_block<char> buf(buf_size+32);

  ostrstream ost(&(buf[0]), buf_size+20);

  DebugEval(typeid(io).name());
  DebugEval(buf_size);

  try {
	 getIO().serialize(ost, IO::BASES|IO::TYPENAME);
	 ost << '\0';
	 int chars_used = strlen(&(buf[0]));
	 DebugEvalNL(chars_used);
	 if (chars_used > buf_size) {
		throw TclError("buffer overflow during stringify");
	 }
  }
  catch (IoError& err) {
	 err.appendMsg(" with buffer contents ==\n");

	 buf[buf.size()-1] = '\0';
	 err.appendMsg(&buf[0]);

	 throw err;
  }
  returnCstring(&(buf[0]));
}

void Tcl::DestringifyCmd::invoke() {
DOTRACE("Tcl::DestringifyCmd::invoke");

  // We assume that the string is contained in the last argument in the command
  const char* buf = getCstringFromArg(objc() - 1);
  Assert(buf);

  istrstream ist(buf);

  try {
	 getIO().deserialize(ist, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 throw TclError(err.msg_cstr());
  }
  returnVoid();
}

void Tcl::WriteCmd::invoke() {
DOTRACE("Tcl::WriteCmd::invoke");
  IO& io = getIO();   

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

void Tcl::ReadCmd::invoke() {
DOTRACE("Tcl::ReadCmd::invoke");
  IO& io = getIO();

  const char* str = arg(objc() - 1).getCstring();

  istrstream ist(str);

  AsciiStreamReader reader(ist);
  reader.readRoot(&io);
}

static const char vcid_stringifycmd_cc[] = "$Header$";
#endif // !STRINGIFYCMD_CC_DEFINED
