///////////////////////////////////////////////////////////////////////
//
// readutils.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:25:40 1999
// written: Thu Oct 26 08:58:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READUTILS_H_DEFINED
#define READUTILS_H_DEFINED

#if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(READER_H_DEFINED)
#include "io/reader.h"
#endif

#ifdef ACC_COMPILER
// For some reason aCC wants to see the full definition of class IO
// before parsing the dynamic_cast<C*>(IO*) in readObjectSeq
#  if defined(NO_EXTERNAL_INCLUDE_GUARDS) || !defined(IO_H_DEFINED)
#  include "io/io.h"
#  endif
#endif

namespace IO {

/// Utilities for reading sequences from a \c IO::Reader.
namespace ReadUtils {

  // Warning: the return value of these functions is only valid up
  // until the next call to the function.
  const char* makeElementNameString(const char* seq_name,
												int element_num);
  const char* makeSeqCountString(const char* seq_name);

  /// Get the number of elements in the stored sequence \a seq_name.
  inline int readSequenceCount(IO::Reader* reader, const char* seq_name)
	 {
		int count = reader->readInt(makeSeqCountString(seq_name));
 		if (0 > count)
 		  throw IO::ReadError("read negative value for sequence count");
		return count;
	 }

  /** Provides a generic interface for handling containers, sequences,
		etc. of value types. If the count has already been read from the
		reader, this value can be passed as \c known_count, so that we
		avoid reading the value twice (this may be important if the
		reader does not support random access to the attributes). */
  template <class T, class Inserter>
  inline void readValueSeq(IO::Reader* reader, const char* seq_name,
									Inserter inserter, int known_count = -1)
	 {
		int count = (known_count == -1) ?
		  readSequenceCount(reader, seq_name) : known_count;

		for (int i = 0; i < count; ++i) {
		  T temp;
		  reader->readValue(makeElementNameString(seq_name, i), temp);
		  *inserter = temp;
		  ++inserter;
		}
	 }

  /** Provides a generic interface for handling containers, sequences,
		etc. of Value subtypes. If the count has already been read from
		the reader, this value can be passed as \c known_count, so that
		we avoid reading the value twice (this may be important if the
		reader does not support random access to the attributes). */
  template <class T, class Inserter>
  inline void readValueObjSeq(IO::Reader* reader, const char* seq_name,
										Inserter inserter, int known_count = -1)
	 {
		int count = (known_count == -1) ?
		  readSequenceCount(reader, seq_name) : known_count;

		for (int i = 0; i < count; ++i) {
		  T temp;
		  reader->readValueObj(makeElementNameString(seq_name, i), temp);
		  *inserter = temp;
		  ++inserter;
		}
	 }

  /** Provides a generic interface for handling containers, sequences,
      etc. of IO objects. If the count has already been read from
		the reader, this value can be passed as \c known_count, so that
		we avoid reading the value twice (this may be important if the
		reader does not support random access to the attributes). */
  template <class C, class Inserter>
  inline void readObjectSeq(IO::Reader* reader, const char* seq_name,
									 Inserter inserter, int known_count = -1)
	 {
		int count = (known_count == -1) ?
		  readSequenceCount(reader, seq_name) : known_count;

		for (int i = 0; i < count; ++i) {
		  IO::IoObject* io = reader->readObject(makeElementNameString(seq_name, i));

		  if (io == 0) {
			 *inserter = static_cast<C*>(0);
		  }

		  else {
			 C* obj = dynamic_cast<C*>(io);
			 if (obj == 0)
				{
				  IO::ReadError err("failed cast in readObjectSeq for sequence '");
				  err.appendMsg(seq_name, "' on item number ");
				  err.appendNumber(i);
				  throw err;
				}
			 *inserter = obj;
		  }

		  ++inserter;
		}
	 }
};

} // end namespace IO

static const char vcid_readutils_h[] = "$Header$";
#endif // !READUTILS_H_DEFINED
