///////////////////////////////////////////////////////////////////////
//
// writeutils.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:18:36 1999
// written: Mon Mar  6 12:35:01 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_H_DEFINED
#define WRITEUTILS_H_DEFINED

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
									 const char* name, Itr begin, Itr end) {
	 int count = 0;

	 while (begin != end) {
		writer->writeValue(makeElementNameString(name, count), *begin);
		++count;
		++begin;
	 }

	 writer->writeInt(makeSeqCountString(name), count);
  }

  /** A generic interface for handling containers, sequences, etc. of
		objects of Value subtypes */
  template <class Itr>
  static void writeValueObjSeq(Writer* writer,
										 const char* name, Itr begin, Itr end) {
	 int count = 0;

	 while (begin != end) {
		writer->writeValueObj(makeElementNameString(name, count), *begin);
		++count;
		++begin;
	 }

	 writer->writeInt(makeSeqCountString(name), count);
  }


  /// A generic interface for handling containers, sequences, etc. of objects
  template <class Itr>
  static void writeObjectSeq(Writer* writer,
									  const char* name, Itr begin, Itr end) {
	 int count = 0;
	 
	 while (begin != end) {
		writer->writeObject(makeElementNameString(name, count), *begin);
		++count;
		++begin;
	 }

	 writer->writeValue(makeSeqCountString(name), count);
  }

private:
  // Warning: the return value of these functions is only valid up
  // until the next call to the function.
  static const char* makeElementNameString(const char* seq_name,
														 int element_num);
  static const char* makeSeqCountString(const char* seq_name);
};

static const char vcid_writeutils_h[] = "$Header$";
#endif // !WRITEUTILS_H_DEFINED
