///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:54:55 1999
// written: Fri Nov  5 10:10:53 1999
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
  Impl(istream& is) :
	 itsBuf(is), itsCreatedObjects(), itsAttribs() 
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


  istream& itsBuf;

  map<unsigned long, IO*> itsCreatedObjects;
  
  struct Type_Value {
	 Type_Value(const char* t=0, const char* v=0) :
		type(t ? t : ""), value(v ? v : "") {}
	 Type_Value(const string& t, const string& v) : type(t), value(v) {}
	 string type;
	 string value;
  };

  map<string, Type_Value> itsAttribs;

#ifndef NO_IOS_EXCEPTIONS
  ios::iostate itsOriginalExceptionState;
#endif
};

AsciiStreamReader::AsciiStreamReader (istream& is) :
  itsImpl( *(new Impl(is)) )
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
  istrstream ist(itsImpl.itsAttribs[name].value.c_str());
  char val;
  ist >> val;
  DebugEval(itsImpl.itsAttribs[name].value); DebugEvalNL(val);
  return val;
}

int AsciiStreamReader::readInt(const string& name) {
DOTRACE("AsciiStreamReader::readInt");
  istrstream ist(itsImpl.itsAttribs[name].value.c_str());
  int val;
  ist >> val;
  DebugEval(itsImpl.itsAttribs[name].value); DebugEvalNL(val);
  return val;
}

bool AsciiStreamReader::readBool(const string& name) {
DOTRACE("AsciiStreamReader::readBool");
  istrstream ist(itsImpl.itsAttribs[name].value.c_str());
  int val;
  ist >> val;
  DebugEval(itsImpl.itsAttribs[name].value); DebugEvalNL(val);
  return bool(val);
}

double AsciiStreamReader::readDouble(const string& name) {
DOTRACE("AsciiStreamReader::readDouble");
  istrstream ist(itsImpl.itsAttribs[name].value.c_str());
  double val;
  ist >> val;
  DebugEval(itsImpl.itsAttribs[name].value); DebugEvalNL(val);
  return val;
}

string AsciiStreamReader::readString(const string& name) {
DOTRACE("AsciiStreamReader::readString");
  istrstream ist(itsImpl.itsAttribs[name].value.c_str());

  int len;
  ist >> len;                     DebugEvalNL(len);
  ist.get(); // ignore one char of whitespace after the length

  vector<char> cstr(len+1);
  ist.read(&cstr[0], len);
  cstr.back() = '\0';

  DebugEval(itsImpl.itsAttribs[name].value); DebugEvalNL(&cstr[0]);
  return string(&cstr[0]);
}

char* AsciiStreamReader::readCstring(const string& name) {
DOTRACE("AsciiStreamReader::readCstring");
  istrstream ist(itsImpl.itsAttribs[name].value.c_str());

  int len;
  ist >> len;
  ist.get(); // ignore one char of whitespace after the length

  char* val = new char[len+1];
  ist.read(val, len);
  val[len] = '\0';

  DebugEval(itsImpl.itsAttribs[name].value); DebugEvalNL(val);
  return val;
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

IO* AsciiStreamReader::readObject(const string& name) {
DOTRACE("AsciiStreamReader::readObject");
  IO* obj = 0;

  istrstream ist(itsImpl.itsAttribs[name].value.c_str());
  unsigned long id;
  ist >> id;

  if (id == 0) { return 0; }

  // If we need to create a new object for this id:
  if ( itsImpl.itsCreatedObjects[id] == 0 ) {

	 // If 'name' is not in the itsAttribs map, then type will be
	 // returned as an empty string. This, in turn, will cause
	 // IoMgr::newIO() to throw an exception when it attempts to create
	 // an object of type "".
	 string type = itsImpl.itsAttribs[name].type;
	 obj = IoMgr::newIO(type.c_str());

	 // If there was a problem within IoMgr::newIO(), it will throw an
	 // exception, so we should never pass this point with a NULL obj.
	 Assert(obj != 0);

	 itsImpl.itsCreatedObjects[id] = obj;
  }

  // ...otherwise we use the previously created object for this id:
  else obj = itsImpl.itsCreatedObjects[id];

  return obj;
}

IO* AsciiStreamReader::readRoot(IO* root) {
DOTRACE("AsciitStreamReader::readRoot");
  itsImpl.itsCreatedObjects.clear();

  if (root != 0) {
	 itsImpl.itsCreatedObjects[root->id()] = root;
  }

  while ( itsImpl.itsBuf.peek() != EOF ) {
	 IO* obj = NULL;
  	 string type, equal, bracket;	 
	 unsigned long id;

	 itsImpl.itsBuf >> type >> id >> equal >> bracket;
	 DebugEval(type); DebugEvalNL(id);

	 if ( itsImpl.itsCreatedObjects[id] != 0 ) {
		obj = itsImpl.itsCreatedObjects[id];
	 }
	 else {
		obj = IoMgr::newIO(type.c_str());
		Assert(obj != 0);
		itsImpl.itsCreatedObjects[id] = obj;
		if (!root) root = obj;
	 }
	 
	 DebugEvalNL((void*) obj);
	 Assert(obj != 0);
	 
	 initAttributes();
	 obj->readFrom(this);

	 itsImpl.itsBuf >> bracket;
	 eatWhitespace();
  }

  return root;
}

void AsciiStreamReader::initAttributes() {
DOTRACE("AsciiStreamReader::initAttributes");
  itsImpl.itsAttribs.clear();

  int attrib_count;
  itsImpl.itsBuf >> attrib_count;

  DebugEvalNL(attrib_count);

  for (int i = 0; i < attrib_count; ++i) {
	 string type, name, equal, value;

	 itsImpl.itsBuf >> type >> name >> equal;
	 getline(itsImpl.itsBuf, value, STRING_ENDER);

	 DebugEval(type); DebugEval(name); DebugEvalNL(value);

	 unEscape(value);

	 DebugEvalNL(value);

	 itsImpl.itsAttribs[name] = Impl::Type_Value(type, value);
  }
}

int AsciiStreamReader::eatWhitespace() {
DOTRACE("AsciiStreamReader::eatWhitespace");
  int c=0;
  while ( isspace(itsImpl.itsBuf.peek()) )
	 { itsImpl.itsBuf.get(); ++c; }
  return c;
}

#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamReader
#endif

static const char vcid_asciistreamreader_cc[] = "$Header$";
#endif // !ASCIISTREAMREADER_CC_DEFINED
