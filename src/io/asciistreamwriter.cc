///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 13:05:57 1999
// written: Fri Mar  3 23:48:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMWRITER_CC_DEFINED
#define ASCIISTREAMWRITER_CC_DEFINED

#include "asciistreamwriter.h"

#include <cstring>
#include <iostream.h>
#include <string>
#include <strstream.h>
#include <typeinfo>
#include <vector>
#include <set>

#include "demangle.h"
#include "io.h"
#include "value.h"

#define NO_TRACE
#include "trace.h"
#include "debug.h"

#if defined(IRIX6) || defined(HP9000S700)
#define AsciiStreamWriter ASW
#endif

#ifdef ACC_COMPILER
#define NO_IOS_EXCEPTIONS
#endif

namespace {
  const char STRING_ENDER = '^';

  void addEscapes(string& text) {
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
public:
  Impl(AsciiStreamWriter* owner, ostream& os) :
	 itsOwner(owner), itsBuf(os), itsToHandle(), itsWrittenObjects() 
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
  vector<string> itsAttribs;

#ifndef NO_IOS_EXCEPTIONS
  ios::iostate itsOriginalExceptionState;
#endif

  // Helper functions
private:
  void flushAttributes();

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
  void writeValueObj(const string& name, const Value& value);

  void writeObject(const string& name, const IO* obj);

  void writeOwnedObject(const string& name, const IO* obj);

  template <class T>
  void writeBasicType(const string& name, T val) {
	 vector<char> buf(32 + name.length());
	 ostrstream ost(&buf[0], 32 + name.length());
	 ost << demangle(typeid(T).name()) << " " 
		  << name << " := "
		  << val << '\0';
	 itsAttribs.push_back(&buf[0]);
  }

  void writeStringType(const string& name, const string& val,
							  const char* string_typename) {
	 string escaped_val(val);
	 addEscapes(escaped_val);

	 vector<char> buf(32 + name.length() + escaped_val.length());
	 ostrstream ost(&buf[0], 32 + name.length() + escaped_val.length());
	 ost << string_typename << " "
		  << name << " := " 
		  << val.length() << " " << escaped_val << '\0';
	 itsAttribs.push_back(&buf[0]);
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

  while ( haveMoreObjectsToHandle() ) {
	 const IO* obj = getNextObjectToHandle();

	 if ( alreadyWritten(obj) ) { continue; }

	 itsBuf << demangle(typeid(*obj).name())
			  << " " << obj->id() << " := { ";

	 obj->writeTo(itsOwner);
	 flushAttributes();

	 markObjectAsWritten(obj);

	 itsBuf << '}' << endl;
  }
}

void AsciiStreamWriter::Impl::flushAttributes() {
DOTRACE("AsciiStreamWriter::Impl::flushAttributes");

  itsBuf << itsAttribs.size() << endl;

  for (size_t i = 0; i < itsAttribs.size(); ++i) {
	 itsBuf << itsAttribs[i] << STRING_ENDER << endl;
  }

  itsAttribs.clear();
}

void AsciiStreamWriter::Impl::writeValueObj(
  const string& attrib_name,
  const Value& value
  ) {
DOTRACE("AsciiStreamWriter::Impl::writeValueObj");

  vector<char> buf(256 + attrib_name.length()); 
  ostrstream ost(&buf[0], 256 + attrib_name.length());

  ost << value.getNativeTypeName() << " "
		<< attrib_name << " := "
		<< value << '\0';

  itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::Impl::writeObject(const string& name, const IO* obj) {
DOTRACE("AsciiStreamWriter::Impl::writeObject");
  vector<char> buf(32 + name.length());
  ostrstream ost(&buf[0], 32 + name.length());

  if (obj == 0) {
	 ost << "NULL " << name << " := 0" << '\0';
  }
  else {
	 ost << demangle(typeid(*obj).name())
		  << " " << name << " := " << obj->id() << '\0';
	 
	 addObjectToBeHandled(obj);
  }

  itsAttribs.push_back(&buf[0]);
}

void AsciiStreamWriter::Impl::writeOwnedObject(
  const string& name, const IO* obj
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
  itsImpl.writeBasicType(name, val); 
}

void AsciiStreamWriter::writeInt(const char* name, int val) {
DOTRACE("AsciiStreamWriter::writeInt");
  itsImpl.writeBasicType(name, val); 
}

void AsciiStreamWriter::writeBool(const char* name, bool val) {
DOTRACE("AsciiStreamWriter::writeBool");
  itsImpl.writeBasicType(name, val); 
}

void AsciiStreamWriter::writeDouble(const char* name, double val) {
DOTRACE("AsciiStreamWriter::writeDouble");
  itsImpl.writeBasicType(name, val); 
}

void AsciiStreamWriter::writeString(const char* name, const string& val) {
DOTRACE("AsciiStreamWriter::writeString");
  itsImpl.writeStringType(name, val, "string");
}

void AsciiStreamWriter::writeCstring(const char* name, const char* val) {
DOTRACE("AsciiStreamWriter::writeCstring");
  itsImpl.writeStringType(name, val, "cstring");
}

void AsciiStreamWriter::writeValueObj(const char* name, const Value& value) {
DOTRACE("AsciiStreamWriter::writeValueObj");
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
