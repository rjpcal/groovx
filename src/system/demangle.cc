///////////////////////////////////////////////////////////////////////
//
// demangle.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Oct 13 10:41:19 1999
// written: Fri Nov 10 17:03:56 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef DEMANGLE_CC_DEFINED
#define DEMANGLE_CC_DEFINED

#include "system/demangle.h"

#if defined(ACC_COMPILER) || defined(MIPSPRO_COMPILER)

const char* demangle_cstr(const char* in) { return in; }

#elif defined(GCC_COMPILER)

#include <cctype>
#include <cstdlib>
#include <string>

#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

string demangle(const string& in);

const char* demangle_cstr(const char* in) {
  static string result;
  result = demangle(in);
  return result.c_str();
}

namespace {
  int munchInt(string& str, string::size_type pos) {
	 int val = 0;
	 while ( isdigit(str[pos]) ) {
		val *= 10;
		val += (str[pos] - '0');
		str.erase(pos, 1);
	 }

	 return val;
  }

  void putInt(int val, string& str, string::size_type pos) {
	 while (val != 0) {
		str.insert(pos, 1, '0' + (val%10));
		val /= 10;
	 }
  }

  string readModifiers(string& str, string::size_type& pos) {
	 string modifiers("");
	 string array_dims("");

	 while ( isupper(str[pos]) ) {
		switch (str[pos]) {
		case 'A':
		  str.erase(pos, 1); // we need to also erase the '_'
		  array_dims.append("[");
		  if ( isdigit(str[pos]) )
			 {
				int dim_size = munchInt(str, pos);
				putInt(dim_size+1,array_dims,array_dims.length());
			 }
		  array_dims.append("]");
		  break;
		case 'C':
		  modifiers.insert(0, " const");
		  break;
		case 'P':
		  modifiers.insert(0, "*");
		  break;
		case 'R':
		  modifiers.insert(0, "&");
		  break;
		case 'U':
		  modifiers.insert(0, " unsigned");
		  break;
		default:
		  Assert(0);
		}
		str.erase(pos, 1);
	 }
	 DebugEvalNL(str);

	 modifiers.append(array_dims);

	 return modifiers;
  }

  void insertBuiltinTypename(string& str, string::size_type& pos) {

	 char typecode = str[pos];
	 str.erase(pos, 1);
	 switch (typecode) {
	 case 'b':
		str.insert(pos, "bool"); pos += 4;
		break;
	 case 'c':
		str.insert(pos, "char"); pos += 4;
		break;
	 case 'd':
		str.insert(pos, "double"); pos += 6;
		break;
	 case 'f':
		str.insert(pos, "float"); pos += 5;
		break;
	 case 'i':
		str.insert(pos, "int"); pos += 3;
		break;
	 case 'l':
		str.insert(pos, "long"); pos += 4;
		break;
	 case 's':
		str.insert(pos, "short"); pos += 5;
		break;
	 case 'v':
		str.insert(pos, "void"); pos += 4;
		break;
	 default:
		DebugEvalNL(typecode);
		Assert(0);
		break;
	 }
  }
}

string demangle(const string& in) {
DOTRACE("demangle");
  string out=in;

  DebugEvalNL(out);

  bool isTemplate = false;

  if (out.length() == 0) return out;

  string::size_type pos = 0;

  // Check if we have namespace qualifiers...
  if (out[0] == 'Q') {
	 out.erase(0, 1);
	 int num_levels = 0;
	 // get the number of nesting levels...
	 if (out[0] == '_') {
		out.erase(0, 1);
		num_levels = munchInt(out, 0);
		DebugEvalNL(num_levels);
		Assert(out[0] == '_');
		out.erase(0, 1);
	 }
	 else {
		num_levels = out[0] - '0';
		out.erase(0, 1);
	 }
	 while (num_levels > 1) {
		// Get the length of the current qualifier
		int length = munchInt(out, pos);
		// Skip over the qualifier itself
		pos += length;
		// Insert a scope resolution operator after the qualifier "::"
		out.insert(pos, "::");
		// Skip over the just-inserted "::"
		pos += 2;
		--num_levels;
	 }
  }

  DebugEvalNL(out);

  if (out[pos] == 't') {
	 isTemplate = true;
	 out.erase(pos, 1);
  }

  if ( !isTemplate ) {
	 while ( isdigit(out[pos]) ) {
		out.erase(pos, 1);
	 }

	 DebugEvalNL(out);
  }
  else {
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

		// Get the parameter name:
		string modifiers = readModifiers(out, pos);

		if ( !isdigit(out[pos]) ) {
		  insertBuiltinTypename(out, pos);
		}
		else {
		  // Read length of template parameter name and skip over it
		  int param_length = munchInt(out, pos);
		  pos += param_length;
		}

		out.insert(pos, modifiers);
		pos += modifiers.length();

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

#else
#error no compiler macro is defined
#endif // switch on compiler macro

static const char vcid_demangle_cc[] = "$Header$";
#endif // !DEMANGLE_CC_DEFINED
