///////////////////////////////////////////////////////////////////////
//
// gccdemangle.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Oct 13 10:10:30 1999
// written: Wed Oct 13 11:30:03 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GCCDEMANGLE_CC_DEFINED
#define GCCDEMANGLE_CC_DEFINED

#include <cctype>
#include <cstdlib>

#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

namespace {
  int munchInt(string& str, int pos) {
	 int val = 0;
	 while ( isdigit(str[pos]) ) {
		val *= 10;
		val += (str[pos] - '0');
		str.erase(pos, 1);
	 }

	 return val;
  }
}

string demangle(const string& in) {
DOTRACE("demangle");
  string out=in;

  DebugEvalNL(out);

  bool isTemplate = false;

  if (out.length() == 0) return out;

  if (out[0] == 't') {
	 isTemplate = true;
	 out.erase(0, 1);
  }

  DebugEvalNL(out);

  if ( !isTemplate ) {
	 while ( isdigit(out[0]) ) {
		out.erase(0, 1);
	 }

	 DebugEvalNL(out);
  }
  else {
	 int pos = 0;

	 // Read length of template base name and skip over it
	 int base_length = munchInt(out, pos);
	 pos += base_length;

	 // Insert left bracket
	 out.insert(pos++, 1, '<');

	 DebugEvalNL(out);

	 int num_parameters = munchInt(out, pos);;

	 for (int n = 0; n < num_parameters; ++n) {

		// Template parameters must start with 'Z'
		Assert( out[pos] == 'Z' );
		out.erase(pos, 1);

		DebugEvalNL(out);

		// Read length of template parameter name and skip over it
		int param_length = munchInt(out, pos);
		pos += param_length;

		// Insert a comma if this is not the last parameter
		if (n < (num_parameters-1)) {
		  out.insert(pos++, 1, ',');
		  out.insert(pos++, 1, ' ');
		}

		DebugEvalNL(out);
	 }

	 // Insert right bracket
	 out.insert(pos++, 1, '>');
  }

  DebugEvalNL(out);

  return out;
}

static const char vcid_gccdemangle_cc[] = "$Header$";
#endif // !GCCDEMANGLE_CC_DEFINED
