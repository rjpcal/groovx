///////////////////////////////////////////////////////////////////////
//
// writeutils.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:18:36 1999
// written: Wed Nov 17 12:35:12 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_H_DEFINED
#define WRITEUTILS_H_DEFINED

#ifndef STRING_DEFINED
#include <string>
#define STRING_DEFINED
#endif

#ifndef WRITER_H_DEFINED
#include "writer.h"
#endif

///
class WriteUtils {
public:
  /** A generic interface for handling containers, sequences, etc. of
      value types. */
  template <class Itr>
  static void writeValueSeq(Writer* writer,
									 const string& name, Itr begin, Itr end) {
	 int count = 0;

	 while (begin != end) {
		writer->writeValue(name+makeNumberString(count), *begin);
		++count;
		++begin;
	 }

	 writer->writeInt(name+"Count", count);
  }

  /** A generic interface for handling containers, sequences, etc. of
		objects of Value subtypes */
  template <class Itr>
  static void writeValueObjSeq(Writer* writer,
										 const string& name, Itr begin, Itr end) {
	 int count = 0;

	 while (begin != end) {
		writer->writeValueObj(name+makeNumberString(count), *begin);
		++count;
		++begin;
	 }

	 writer->writeInt(name+"Count", count);
  }


  /// A generic interface for handling containers, sequences, etc. of objects
  template <class Itr>
  static void writeObjectSeq(Writer* writer,
									  const string& name, Itr begin, Itr end) {
	 int count = 0;
	 
	 while (begin != end) {
		writer->writeObject(name+makeNumberString(count), *begin);
		++count;
		++begin;
	 }

	 writer->writeInt(name+"Count", count);
  }

  static string makeNumberString(int number);
};

static const char vcid_writeutils_h[] = "$Header$";
#endif // !WRITEUTILS_H_DEFINED
