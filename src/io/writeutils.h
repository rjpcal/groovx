///////////////////////////////////////////////////////////////////////
//
// writeutils.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Nov 16 14:18:36 1999
// written: Mon Jun 11 15:08:15 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef WRITEUTILS_H_DEFINED
#define WRITEUTILS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IDITEM_H_DEFINED)
#include "util/iditem.h"
#endif

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(WRITER_H_DEFINED)
#include "io/writer.h"
#endif

namespace IO {

/// Utilities for writing sequences to a \c IO::Writer.
class WriteUtils {
public:
  /** A generic interface for handling containers, sequences, etc. of
      value types. */
  template <class Itr>
  static void writeValueSeq(IO::Writer* writer, const char* name,
									 Itr begin, Itr end, bool skip_count=false)
	 {
		if (!skip_count) {
		  writer->writeValue(makeSeqCountString(name),
									computeCount(begin, end));
		}

		int count = 0;

		while (begin != end) {
		  writer->writeValue(makeElementNameString(name, count), *begin);
		  ++begin;
		  ++count;
		}
	 }

  /** A generic interface for handling containers, sequences, etc. of
		objects of Value subtypes */
  template <class Itr>
  static void writeValueObjSeq(IO::Writer* writer, const char* name,
										 Itr begin, Itr end, bool skip_count=false)
	 {
		if (!skip_count) {
		  writer->writeValue(makeSeqCountString(name),
									computeCount(begin, end));
		}

		int count = 0;

		while (begin != end) {
		  writer->writeValueObj(makeElementNameString(name, count), *begin);
		  ++begin;
		  ++count;
		}
	 }


  /// A generic interface for handling containers, sequences, etc. of objects
  template <class Itr>
  static void writeObjectSeq(IO::Writer* writer, const char* name,
									  Itr begin, Itr end, bool skip_count=false)
	 {
		if (!skip_count) {
		  writer->writeValue(makeSeqCountString(name),
									computeCount(begin, end));
		}

		int count = 0;

		while (begin != end) {
		  writer->writeObject(makeElementNameString(name, count),
									 MaybeRef<const IO::IoObject>(*begin));
		  ++begin;
		  ++count;
		}
	 }

private:
  template <class Itr>
  static int computeCount(Itr begin, Itr end)
	 {
		int count = 0;
		while (begin != end) { ++count; ++begin; }
		return count;
	 }

  // Warning: the return value of these functions is only valid up
  // until the next call to the function.
  static const char* makeElementNameString(const char* seq_name,
														 int element_num);
  static const char* makeSeqCountString(const char* seq_name);
};

} // end namespace IO

static const char vcid_writeutils_h[] = "$Header$";
#endif // !WRITEUTILS_H_DEFINED
