///////////////////////////////////////////////////////////////////////
//
// stringifycmd.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 21:43:28 1999
// written: Fri Jun 25 11:42:30 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STRINGIFYCMD_CC_DEFINED
#define STRINGIFYCMD_CC_DEFINED

#include "stringifycmd.h"

#include <strstream.h>
#include <vector>
#include <cstring>
#include <typeinfo>

#include "io.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

void StringifyCmd::invoke() {
DOTRACE("StringifyCmd::invoke");
  IO& io = getIO();

  int buf_size = io.charCount();

  // Give ourselves a little extra space above what is returned by
  // io.charCount, so we have a chance to detect underestimates by
  // charCount.
  vector<char> buf(buf_size+32);

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
	 throw TclError(err.msg());
  }
  returnCstring(&(buf[0]));
}

void DestringifyCmd::invoke() {
DOTRACE("DestringifyCmd::invoke");

  // We assume that the string is contained in the last argument in the command
  const char* buf = getCstringFromArg(objc() - 1);
  Assert(buf);

  istrstream ist(buf);

  try {
	 getIO().deserialize(ist, IO::BASES|IO::TYPENAME);
  }
  catch (IoError& err) {
	 throw TclError(err.msg());
  }
  returnVoid();
}

static const char vcid_stringifycmd_cc[] = "$Header$";
#endif // !STRINGIFYCMD_CC_DEFINED
