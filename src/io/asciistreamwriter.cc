///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 13:05:57 1999
// written: Mon Mar 20 20:25:37 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMWRITER_CC_DEFINED
#define ASCIISTREAMWRITER_CC_DEFINED

#include "asciistreamwriter.h"

#include "demangle.h"
#include "io.h"
#include "value.h"
#include "util/arrays.h"

#include <iostream.h>
#include <string>
#include <typeinfo>
#include <set>

#define NO_TRACE
#include "util/trace.h"
#include "util/debug.h"

#if defined(IRIX6) || defined(HP9000S700)
#define AsciiStreamWriter ASW
#endif

#ifdef ACC_COMPILER
#define NO_IOS_EXCEPTIONS
#endif

namespace {
  const char* ATTRIB_ENDER = "^\n";

  void addEscapes(string& text) {
  DOTRACE("AsciiStreamWriter::Impl::addEscapes");
	 // Escape any special characters
	 for (size_t pos = 0; pos < text.length(); /* ++ done in loop body */ ) {

		switch (text[pos]) {
		case '\\': // i.e., a single backslash
		  text.replace(pos, 1, "\\\\"); // i.e., two backslashes
		  pos += 2;
		  break;
		case '^':
		  text.replace(pos, 1, "\\c");
		  pos += 2;
		  break;
		default:
		  ++pos;
		  break;
		}

	 }
  }

}

class AsciiStreamWriter::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(AsciiStreamWriter* owner, ostream& os) :
	 itsOwner(owner), itsBuf(os), itsToHandle(), itsWrittenObjects()
#ifndef NO_IOS_EXCEPTIONS
	 , itsOriginalExceptionState(itsBuf.exceptions())
#endif
	 {
#ifndef NO_IOS_EXCEPTIONS
	 itsOriginalExceptionState = itsBuf.exceptions();
	 itsBuf.exceptions( ios::eofbit | ios::badbit | ios::failbit );
#endif
	 }

  ~Impl()
	 {
#ifndef NO_IOS_EXCEPTIONS
		itsBuf.exceptions(itsOriginalExceptionState);
#endif
	 }

  AsciiStreamWriter* itsOwner;
  ostream& itsBuf;
  set<const IO *> itsToHandle;
  set<const IO *> itsWrittenObjects;

#ifndef NO_IOS_EXCEPTIONS
  ios::iostate itsOriginalExceptionState;
#endif

  class DummyCountingWriter : public Writer {
  public:
	 DummyCountingWriter() : itsCount(0) {}

	 virtual void writeChar(const char*, char)             { ++itsCount; }
	 virtual void writeInt(const char*, int)               { ++itsCount; }
	 virtual void writeBool(const char*, bool)             { ++itsCount; }
	 virtual void writeDouble(const char*, double)         { ++itsCount; }
	 virtual void writeCstring(const char*, const char*)   { ++itsCount; }
	 virtual void writeValueObj(const char*, const Value&) { ++itsCount; }
	 virtual void writeObject(const char*, const IO*)      { ++itsCount; }
	 virtual void writeOwnedObject(const char*, const IO*) { ++itsCount; }
	 virtual void writeRoot(const IO*) {}

	 void reset() { itsCount = 0; }
	 unsigned int attribCount() const { return itsCount; }

  private:
	 unsigned int itsCount;
  };

  // Helper functions
private:
  bool haveMoreObjectsToHandle() const {
	 return !itsToHandle.empty();
  }

  bool alreadyWritten(const IO* obj) const {
	 return ( itsWrittenObjects.find(obj) != 
				 itsWrittenObjects.end() );
  }

  const IO* getNextObjectToHandle() const {
	 return *(itsToHandle.begin());
  }

  void markObjectAsWritten(const IO* obj) {
	 itsToHandle.erase(obj);
	 itsWrittenObjects.insert(obj);
  }

  void addObjectToBeHandled(const IO* obj) {
	 if ( !alreadyWritten(obj) ) 
		{ itsToHandle.insert(obj); }
  }

  // Delegands
public:
  void writeValueObj(const char* name, const Value& value);

  void writeObject(const char* name, const IO* obj);

  void writeOwnedObject(const char* name, const IO* obj);

  template <class T>
  void writeBasicType(const char* name, T val,
							 const char* string_typename) {
	 itsBuf << string_typename << " " 
			  << name << " := "
			  << val << ATTRIB_ENDER;
  }

  void writeStringType(const char* name, const char* val,
							  const char* string_typename) {
	 string escaped_val(val);
	 int val_length = escaped_val.length();
	 addEscapes(escaped_val);

	 itsBuf << string_typename << " "
			  << name << " := " 
			  << val_length << " " << escaped_val << ATTRIB_ENDER;
  }

  void writeRoot(const IO* root);
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamWriter::Impl delegand definitions
//
///////////////////////////////////////////////////////////////////////

void AsciiStreamWriter::Impl::writeRoot(const IO* root) {
DOTRACE("AsciiStreamWriter::Impl::writeRoot");
  itsToHandle.clear();
  itsWrittenObjects.clear();

  itsToHandle.insert(root);

  DummyCountingWriter counter;

  while ( haveMoreObjectsToHandle() ) {
	 const IO* obj = getNextObjectToHandle();

	 if ( alreadyWritten(obj) ) { continue; }

	 itsBuf << demangle_cstr(typeid(*obj).name())
			  << " " << obj->id() << " := { ";

	 counter.reset();
	 obj->writeTo(&counter);

	 itsBuf << counter.attribCount() << '\n';

	 obj->writeTo(itsOwner);

	 markObjectAsWritten(obj);

	 itsBuf << '}' << '\n';
  }

  itsBuf.flush();
}

void AsciiStreamWriter::Impl::writeValueObj(
  const char* attrib_name,
  const Value& value
  ) {
DOTRACE("AsciiStreamWriter::Impl::writeValueObj");

  itsBuf << value.getNativeTypeName() << " "
			<< attrib_name << " := "
			<< value << ATTRIB_ENDER;
}

void AsciiStreamWriter::Impl::writeObject(const char* name,
														const IO* obj) {
DOTRACE("AsciiStreamWriter::Impl::writeObject");

  if (obj == 0) {
	 itsBuf << "NULL " << name << " := 0" << ATTRIB_ENDER;
  }
  else {
	 itsBuf << demangle_cstr(typeid(*obj).name()) << " "
			  << name << " := "
			  << obj->id() << ATTRIB_ENDER;
	 
	 addObjectToBeHandled(obj);
  }
}

void AsciiStreamWriter::Impl::writeOwnedObject(
  const char* name, const IO* obj
  ) {
DOTRACE("AsciiStreamWriter::Impl::writeOwnedObject");
  writeObject(name, obj); 
}

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamWriter member definitions
//
///////////////////////////////////////////////////////////////////////

AsciiStreamWriter::AsciiStreamWriter (ostream& os) : 
  itsImpl( *(new Impl(this, os)) )
{
DOTRACE("AsciiStreamWriter::AsciiStreamWriter");
  // nothing
}

AsciiStreamWriter::~AsciiStreamWriter () {
DOTRACE("AsciiStreamWriter::~AsciiStreamWriter");
  delete &itsImpl; 
}

void AsciiStreamWriter::writeChar(const char* name, char val) {
DOTRACE("AsciiStreamWriter::writeChar");
  itsImpl.writeBasicType(name, val, "char"); 
}

void AsciiStreamWriter::writeInt(const char* name, int val) {
DOTRACE("AsciiStreamWriter::writeInt");
  itsImpl.writeBasicType(name, val, "int"); 
}

void AsciiStreamWriter::writeBool(const char* name, bool val) {
DOTRACE("AsciiStreamWriter::writeBool");
  itsImpl.writeBasicType(name, val, "bool"); 
}

void AsciiStreamWriter::writeDouble(const char* name, double val) {
DOTRACE("AsciiStreamWriter::writeDouble");
  itsImpl.writeBasicType(name, val, "double"); 
}

void AsciiStreamWriter::writeCstring(const char* name, const char* val) {
DOTRACE("AsciiStreamWriter::writeCstring");
  itsImpl.writeStringType(name, val, "cstring");
}

void AsciiStreamWriter::writeValueObj(const char* name, const Value& value) {
  itsImpl.writeValueObj(name, value);
}

void AsciiStreamWriter::writeObject(const char* name, const IO* obj) {
  itsImpl.writeObject(name, obj);
}

void AsciiStreamWriter::writeOwnedObject(const char* name, const IO* obj) {
  itsImpl.writeOwnedObject(name, obj);
}

void AsciiStreamWriter::writeRoot(const IO* root) {
  itsImpl.writeRoot(root);
}

#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamWriter
#endif

static const char vcid_asciistreamwriter_cc[] = "$Header$";
#endif // !ASCIISTREAMWRITER_CC_DEFINED
