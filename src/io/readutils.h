///////////////////////////////////////////////////////////////////////
//
// readutils.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Nov 16 14:25:40 1999
// written: Wed Dec  1 11:43:56 1999
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
									const string& name, Inserter inserter, T* = 0) {
	 int count = reader->readInt(name+"Count");

	 for (int i = 0; i < count; ++i) {
		T temp;
		reader->readValue(name+makeNumberString(i), temp);
		*inserter = temp;
		++inserter;
	 }
  }

  /** Provides a generic interface for handling containers, sequences,
		etc. of Value subtypes. */
  template <class Inserter, class T>
  static void readValueObjSeq(Reader* reader,
										const string& name, Inserter inserter, T* = 0) {
	 int count = reader->readInt(name+"Count");

	 for (int i = 0; i < count; ++i) {
		T temp;
		reader->readValueObj(name+makeNumberString(i), temp);
		*inserter = temp;
		++inserter;
	 }
  }

  /** Provides a generic interface for handling containers, sequences,
      etc. of IO objects */
  template <class Inserter, class C>
  static void readObjectSeq(Reader* reader,
									 const string& name, Inserter inserter, C* /*dummy*/) {
	 int count = reader->readInt(name+"Count");

	 for (int i = 0; i < count; ++i) {
		IO* io = reader->readObject(name+makeNumberString(i));

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
};

static const char vcid_readutils_h[] = "$Header$";
#endif // !READUTILS_H_DEFINED
