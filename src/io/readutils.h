///////////////////////////////////////////////////////////////////////
//
// readutils.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:25:40 1999
// written: Fri Mar  3 23:53:14 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef READUTILS_H_DEFINED
#define READUTILS_H_DEFINED

#ifndef READER_H_DEFINED
#include "reader.h"
#endif

#ifdef ACC_COMPILER
// For some reason aCC wants to see the full definition of class IO
// before parsing the dynamic_cast<C*>(IO*) in readObjectSeq
#  ifndef IO_H_DEFINED
#  include "io.h"
#  endif
#endif

class ReadUtils {
public:

  /** Provides a generic interface for handling containers, sequences,
		etc. of value types. */
  template <class Inserter, class T>
  static void readValueSeq(Reader* reader,
									const char* name, Inserter inserter, T* = 0) {
	 int count = reader->readInt(makeSeqCountString(name));

	 for (int i = 0; i < count; ++i) {
		T temp;
		reader->readValue(makeElementNameString(name, i), temp);
		*inserter = temp;
		++inserter;
	 }
  }

  /** Provides a generic interface for handling containers, sequences,
		etc. of Value subtypes. */
  template <class Inserter, class T>
  static void readValueObjSeq(Reader* reader,
										const char* name, Inserter inserter, T* = 0) {
	 int count = reader->readInt(makeSeqCountString(name));

	 for (int i = 0; i < count; ++i) {
		T temp;
		reader->readValueObj(makeElementNameString(name, i), temp);
		*inserter = temp;
		++inserter;
	 }
  }

  /** Provides a generic interface for handling containers, sequences,
      etc. of IO objects */
  template <class Inserter, class C>
  static void readObjectSeq(Reader* reader,
									 const char* name, Inserter inserter, C* /*dummy*/) {
	 int count = reader->readInt(makeSeqCountString(name));

	 for (int i = 0; i < count; ++i) {
		IO* io = reader->readObject(makeElementNameString(name, i));

		if (io == 0) {
		  *inserter = static_cast<C*>(0);
		}

		else {
		  C* obj = dynamic_cast<C*>(io);
		  if (obj == 0) throw ReadError("failed cast in readObjectSeq");
		  *inserter = obj;
		}

	 }
  }
  
  static string makeNumberString(int number);

private:
  // Warning: the return value of these functions is only valid up
  // until the next call to the function.
  static const char* makeElementNameString(const char* seq_name,
														 int element_num);
  static const char* makeSeqCountString(const char* seq_name);
};

static const char vcid_readutils_h[] = "$Header$";
#endif // !READUTILS_H_DEFINED
