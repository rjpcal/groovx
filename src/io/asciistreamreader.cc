///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:54:55 1999
// written: Fri Mar 10 19:45:44 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMREADER_CC_DEFINED
#define ASCIISTREAMREADER_CC_DEFINED

#include "asciistreamreader.h"

#include "io.h"
#include "iomgr.h"
#include "value.h"
#include "util/arrays.h"

#include <cctype>
#include <iostream.h>
#include <strstream.h>
#include <map>
#include <string>

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
	 for (size_t pos = 0; pos < text.length(); ++pos) {

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
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(AsciiStreamReader* owner, istream& is) :
	 itsOwner(owner), itsBuf(is), itsCreatedObjects(), itsAttribs(),
	 itsValueBuffer(4096)
#ifndef NO_IOS_EXCEPTIONS
	 , itsOriginalExceptionState(itsBuf.exceptions())
#endif
  {
#ifndef NO_IOS_EXCEPTIONS
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

  fixed_block<char> itsValueBuffer;

#ifndef NO_IOS_EXCEPTIONS
  ios::iostate itsOriginalExceptionState;
#endif

  // Helpers
  bool hasBeenCreated(unsigned long id) {
	 return (itsCreatedObjects[id] != 0);
  }

  void assertAttribExists(const string& attrib_name) {
	 if ( itsAttribs[attrib_name].type == "" ) {
		string msg = "invalid attribute name: ";
		msg += attrib_name;
		throw ReadError(msg.c_str());
	 }
  }

  const string& getTypeOfAttrib(const string& attrib_name) {
	 assertAttribExists(attrib_name);
	 return itsAttribs[attrib_name].type;
  }

  void makeObjectFromTypeForId(const char* type, unsigned long id) {
	 IO* obj = IoMgr::newIO(type);

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

  void readValueObj(const string& name, Value& value);

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

  assertAttribExists(name); 

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
	 makeObjectFromTypeForId(getTypeOfAttrib(attrib_name).c_str(), id);
  }

  return itsCreatedObjects[id];
}

void AsciiStreamReader::Impl::readValueObj(
  const string& attrib_name,
  Value& value
  ) {
DOTRACE("AsciiStreamReader::Impl::readValueObj");

  assertAttribExists(attrib_name);

  istrstream ist(itsAttribs[attrib_name].value.c_str());

  ist >> value;
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

  if ( itsBuf.fail() )
	 throw ReadError("input failed while reading attribute count");

  char type[64], name[64], equal[16];

  for (int i = 0; i < attrib_count; ++i) {

	 itsBuf >> type >> name >> equal;
	 DebugEval(type); DebugEval(name); 

	 if ( itsBuf.fail() )
		throw ReadError("input failed while reading attribute type and name");

	 Attrib& attrib = itsAttribs[name];
	 attrib.type = type;

  	 itsBuf.getline(&itsValueBuffer[0], itsValueBuffer.size(), STRING_ENDER);
	 if ( itsBuf.gcount() >= (itsValueBuffer.size() - 1) ) {
		throw ReadError("value buffer overflow in "
							 "AsciiStreamReader::initAttributes");
	 }

	 attrib.value = &itsValueBuffer[0];
	 DebugEvalNL(attrib.value);

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
	 char type[64], equal[16], bracket[16];
	 unsigned long id;

	 itsBuf >> type >> id >> equal >> bracket;
	 DebugEval(type); DebugEvalNL(id);

	 if ( itsBuf.fail() )
		throw ReadError("input failed while reading typename and object id");

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

	 if ( itsBuf.fail() )
		throw ReadError("input failed while parsing ending bracket");
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

char AsciiStreamReader::readChar(const char* name) {
DOTRACE("AsciiStreamReader::readChar");
  return itsImpl.readBasicType(name, (char*) 0);
}

int AsciiStreamReader::readInt(const char* name) {
DOTRACE("AsciiStreamReader::readInt");
  return itsImpl.readBasicType(name, (int*) 0);
}

bool AsciiStreamReader::readBool(const char* name) {
DOTRACE("AsciiStreamReader::readBool");
  return bool(itsImpl.readBasicType(name, (int*) 0));
}

double AsciiStreamReader::readDouble(const char* name) {
DOTRACE("AsciiStreamReader::readDouble");
  return itsImpl.readBasicType(name, (double*) 0);
}

// XXX this may leak!
char* AsciiStreamReader::readCstring(const char* name) {
  return itsImpl.readStringType(name);
}

void AsciiStreamReader::readValueObj(const char* name, Value& value) {
DOTRACE("AsciiStreamReader::readValueObj");
  itsImpl.readValueObj(name, value); 
}

IO* AsciiStreamReader::readObject(const char* attrib_name) {
  return itsImpl.readObject(attrib_name);
}

void AsciiStreamReader::readOwnedObject(const char* name, IO* obj) {
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
