///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:54:55 1999
// written: Thu Nov 11 17:45:42 1999
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMREADER_CC_DEFINED
#define ASCIISTREAMREADER_CC_DEFINED

#include "asciistreamreader.h"

#include <cctype>
#include <iostream.h>
#include <strstream.h>
#include <map>
#include <vector>

#include "io.h"
#include "iomgr.h"
#include "value.h"

#define NO_TRACE
#include "trace.h"
#define LOCAL_ASSERT
#include "debug.h"

#if defined(IRIX6) || defined(HP9000S700)
#define AsciiStreamReader ASR
#endif

#ifdef ACC_COMPILER
#define NO_IOS_EXCEPTIONS
#endif

namespace {
  const char STRING_ENDER = '^';

  void unEscape(string& text) {
  DOTRACE("<asciistreamreader.cc>::unEscape");
	 // un-escape any special characters
	 for (int pos = 0; pos < text.length(); ++pos) {

		if (text[pos] == '\\') {
		  if ( (pos+1) >= text.length() ) {
			 throw ReadError("missing character after trailing backslash");
		  }
		  switch (text[pos+1]) {
		  case '\\':
			 text.erase(pos+1, 1);
			 break;
		  case 'c':
			 text.replace(pos, 2, "^");
			 break;
		  default:
			 throw ReadError("invalid escape character");
			 break;
		  }
		}
	 }
  }

}

class AsciiStreamReader::Impl {
public:
  Impl(AsciiStreamReader* owner, istream& is) :
	 itsOwner(owner), itsBuf(is), itsCreatedObjects(), itsAttribs() 
  {
#ifndef NO_IOS_EXCEPTIONS
	 itsOriginalExceptionState = itsBuf.exceptions();
	 itsBuf.exceptions( ios::badbit | ios::failbit );
#endif
  }

  ~Impl()
	 {
#ifndef NO_IOS_EXCEPTIONS
		itsBuf.exceptions(itsOriginalExceptionState);
#endif
	 }


  AsciiStreamReader* itsOwner;

  istream& itsBuf;

  map<unsigned long, IO*> itsCreatedObjects;
  
  struct Attrib {
	 Attrib(const char* t=0, const char* v=0) :
		type(t ? t : ""), value(v ? v : "") {}
	 Attrib(const string& t, const string& v) : type(t), value(v) {}
	 string type;
	 string value;
  };

  map<string, Attrib> itsAttribs;

#ifndef NO_IOS_EXCEPTIONS
  ios::iostate itsOriginalExceptionState;
#endif

  // Helpers
  bool hasBeenCreated(unsigned long id) {
	 return (itsCreatedObjects[id] != 0);
  }

  void assertAttribExists(const string& attrib_name) {
	 if ( itsAttribs[attrib_name].type == "" ) {
		throw ReadError("invalid attribute name: " + attrib_name);
	 }
  }

  const string& getTypeOfAttrib(const string& attrib_name) {
	 assertAttribExists(attrib_name);
	 return itsAttribs[attrib_name].type;
  }

  void makeObjectFromTypeForId(const string& type, unsigned long id) {
	 IO* obj = IoMgr::newIO(type.c_str());

	 // If there was a problem within IoMgr::newIO(), it will throw an
	 // exception, so we should never pass this point with a NULL obj.
	 DebugEvalNL((void*) obj);
	 Assert(obj != 0);

	 itsCreatedObjects[id] = obj;
  }

  int eatWhitespace() {
	 int c=0;
	 while ( isspace(itsBuf.peek()) ) { itsBuf.get(); ++c; }
	 return c;
  }

  // Delegands
public:
  template <class T>
  T readBasicType(const string& name, T* /* dummy */) {
	 T return_val;
	 istrstream ist(itsAttribs[name].value.c_str());
	 ist >> return_val;
	 DebugEval(itsAttribs[name].value); DebugEvalNL(return_val);
	 return return_val;
  }

  // Returns a new dynamically allocated char array
  char* readStringType(const string& name);

  IO* readObject(const string& attrib_name);

  void readOwnedObject(const string& name, IO* obj);

  void initAttributes();

  IO* readRoot(IO* given_root);
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

char* AsciiStreamReader::Impl::readStringType(const string& name) {
DOTRACE("AsciiStreamReader::Impl::readStringType");
  istrstream ist(itsAttribs[name].value.c_str());

  int len;
  ist >> len;                     DebugEvalNL(len);
  ist.get(); // ignore one char of whitespace after the length

  char* new_cstring = new char[len+1];
  ist.read(new_cstring, len);
  new_cstring[len] = '\0';

  DebugEval(itsAttribs[name].value); DebugEvalNL(new_cstring);
  return new_cstring;
}

IO* AsciiStreamReader::Impl::readObject(const string& attrib_name) {
DOTRACE("AsciiStreamReader::Impl::readObject");

  assertAttribExists(attrib_name);

  istrstream ist(itsAttribs[attrib_name].value.c_str());
  unsigned long id;
  ist >> id;

  if (id == 0) { return 0; }

  // Create a new object for this id, if necessary:
  if ( !hasBeenCreated(id) ) {
	 makeObjectFromTypeForId(getTypeOfAttrib(attrib_name), id);
  }

  return itsCreatedObjects[id];
}

void AsciiStreamReader::Impl::readOwnedObject(
  const string& attrib_name, IO* obj
  ) {
DOTRACE("AsciiStreamReader::Impl::readOwnedObject");

  assertAttribExists(attrib_name);

  istrstream ist(itsAttribs[attrib_name].value.c_str());
  unsigned long id;
  ist >> id;

  if ( id == 0 )
	 { throw ReadError("owned object had object id of 0"); }

  if ( hasBeenCreated(id) )
	 { throw ReadError("'owned object' was already created"); }

  itsCreatedObjects[id] = obj;
}

void AsciiStreamReader::Impl::initAttributes() {
DOTRACE("AsciiStreamReader::Impl::initAttributes");
  itsAttribs.clear();

  int attrib_count;
  itsBuf >> attrib_count;

  DebugEvalNL(attrib_count);

  for (int i = 0; i < attrib_count; ++i) {
	 char type[64], name[64], equal[16];

	 itsBuf >> type >> name >> equal;

	 Attrib& attrib = itsAttribs[name];
	 attrib.type = type;

	 getline(itsBuf, attrib.value, STRING_ENDER);

	 DebugEval(type); DebugEval(name); DebugEvalNL(attrib.value);

	 unEscape(attrib.value);
	 DebugEvalNL(attrib.value);
  }
}

IO* AsciiStreamReader::Impl::readRoot(IO* given_root) {
DOTRACE("AsciiStreamReader::Impl::readRoot");

  itsCreatedObjects.clear();

  bool haveReadRoot = false;
  unsigned long rootid;

  while ( itsBuf.peek() != EOF ) {
	 IO* obj = NULL;
  	 string type, equal, bracket;	 
	 unsigned long id;

	 itsBuf >> type >> id >> equal >> bracket;
	 DebugEval(type); DebugEvalNL(id);

	 if ( !haveReadRoot ) {
		rootid = id;

		if (given_root != 0) 
		  { itsCreatedObjects[rootid] = given_root; }

		haveReadRoot = true;
	 }

	 if ( !hasBeenCreated(id) ) {
		makeObjectFromTypeForId(type, id);
	 }

	 obj = itsCreatedObjects[id];
	 
	 initAttributes();
	 obj->readFrom(itsOwner);

	 itsBuf >> bracket;
	 eatWhitespace();
  }

  return itsCreatedObjects[rootid];
}

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader member definitions
//
///////////////////////////////////////////////////////////////////////

AsciiStreamReader::AsciiStreamReader (istream& is) :
  itsImpl( *(new Impl(this, is)) )
{
DOTRACE("AsciiStreamReader::AsciiStreamReader");
  // nothing
}

AsciiStreamReader::~AsciiStreamReader () {
DOTRACE("AsciiStreamReader::~AsciiStreamReader");
  delete &itsImpl; 
}

char AsciiStreamReader::readChar(const string& name) {
DOTRACE("AsciiStreamReader::readChar");
  return itsImpl.readBasicType(name, (char*) 0);
}

int AsciiStreamReader::readInt(const string& name) {
DOTRACE("AsciiStreamReader::readInt");
  return itsImpl.readBasicType(name, (int*) 0);
}

bool AsciiStreamReader::readBool(const string& name) {
DOTRACE("AsciiStreamReader::readBool");
  return bool(itsImpl.readBasicType(name, (int*) 0));
}

double AsciiStreamReader::readDouble(const string& name) {
DOTRACE("AsciiStreamReader::readDouble");
  return itsImpl.readBasicType(name, (double*) 0);
}

string AsciiStreamReader::readString(const string& name) {
DOTRACE("AsciiStreamReader::readString");
  char* cstring = itsImpl.readStringType(name);
  string return_val(cstring);
  delete [] cstring;
  return return_val;
}

char* AsciiStreamReader::readCstring(const string& name) {
  return itsImpl.readStringType(name);
}

void AsciiStreamReader::readValueObj(const string& name, Value& value) {
DOTRACE("AsciiStreamReader::readValueObj");
  switch (value.getNativeType()) {
  case Value::INT:
	 value.setInt(readInt(name));
	 break;
  case Value::LONG:
	 value.setLong(readInt(name));
	 break;
  case Value::BOOL:
	 value.setBool(readBool(name));
	 break;
  case Value::DOUBLE:
	 value.setDouble(readDouble(name));
	 break;
  case Value::CSTRING:
	 value.setCstring(readCstring(name));
	 break;
  case Value::STRING:
	 value.setString(readString(name));
	 break;
  default:
	 throw ReadError("couldn't handle Value type");
	 break;
  }
}

IO* AsciiStreamReader::readObject(const string& attrib_name) {
  return itsImpl.readObject(attrib_name);
}

void AsciiStreamReader::readOwnedObject(const string& name, IO* obj) {
  itsImpl.readOwnedObject(name, obj);
}

IO* AsciiStreamReader::readRoot(IO* given_root) {
  return itsImpl.readRoot(given_root);
}

int AsciiStreamReader::eatWhitespace() {
  return itsImpl.eatWhitespace();
}

#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamReader
#endif

static const char vcid_asciistreamreader_cc[] = "$Header$";
#endif // !ASCIISTREAMREADER_CC_DEFINED
