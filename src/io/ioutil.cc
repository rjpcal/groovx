///////////////////////////////////////////////////////////////////////
//
// stringifycmd.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Jun 11 21:43:28 1999
// written: Tue Dec  7 18:19:56 1999
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

#include "asciistreamreader.h"
#include "asciistreamwriter.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace Tcl {

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
	 buf[buf.size()-1] = '\0';
	 string buf_contents(buf.begin(), buf.end());
	 throw TclError(err.msg() + " with buffer contents ==\n" + buf_contents);
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

void WriteCmd::invoke() {
DOTRACE("WriteCmd::invoke");
  IO& io = getIO();   

  int buf_size = 4096;
  vector<char> buf(buf_size);

  ostrstream ost(&(buf[0]), buf_size-1);

  DebugEval(typeid(io).name());
  DebugEval(buf_size);

  AsciiStreamWriter writer(ost);
  writer.writeRoot(&io);
  ost << '\0';

  returnCstring(&(buf[0]));
}

void ReadCmd::invoke() {
DOTRACE("ReadCmd::invoke");
  IO& io = getIO();

  const char* str = arg(objc() - 1).getCstring();

  istrstream ist(str);

  AsciiStreamReader reader(ist);
  reader.readRoot(&io);
}

} // end namespace Tcl

static const char vcid_stringifycmd_cc[] = "$Header$";
#endif // !STRINGIFYCMD_CC_DEFINED
