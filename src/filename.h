///////////////////////////////////////////////////////////////////////
//
// filename.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Thu Mar 23 11:36:28 2000
// written: Fri Nov 10 17:03:48 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef FILENAME_H_DEFINED
#define FILENAME_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(STRINGS_H)
#include "util/strings.h"
#endif

class Filename : public dynamic_string {
public:
  Filename(const char* text = "") : dynamic_string(text) {}
  Filename(const fixed_string& other) : dynamic_string(other) {}
  Filename(const dynamic_string& other) : dynamic_string(other) {}
  Filename(const Filename& other) : dynamic_string(other) {}

  const char* all_extensions() {
	 const char* itr = c_str();
	 while ( *itr != '.' && *itr != '\0' ) ++itr;
	 return itr;
  }

  const char* last_extension() {
	 const char* begin = c_str();
	 const char* itr = begin + length();

	 while ( itr > begin && *itr != '.' ) --itr;

	 if (itr == begin)
		return begin + length();

	 return itr;
  }
};

static const char vcid_filename_h[] = "$Header$";
#endif // !FILENAME_H_DEFINED
