///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:54:55 1999
// written: Tue Mar 14 14:12:45 2000
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

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

class AttributeReadError : public ReadError {
public:
  AttributeReadError(const string& attrib_name) :
	 ReadError("input failed while reading attribute: ")
	 {
		appendMsg(attrib_name.c_str());
	 }
};

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

  // Creators
public:
  Impl(AsciiStreamReader* owner, istream& is) :
	 itsBuf(is), itsObjects(), itsAttribs()
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

  // Nested types
public:

  class ObjectMap {
  private:
	 map<unsigned long, IO*> itsMap;

  public:
	 ObjectMap() : itsMap() {}

	 bool hasBeenCreated(unsigned long id) {
		return (itsMap[id] != 0);
	 }

	 // This returns the object for the given id; the object must
	 // already have been created, otherwise an exception will be thrown.
	 IO* getObject(unsigned long id)
		{
		  if ( !hasBeenCreated(id) )
			 throw ReadError("no object was found for the given id");

		  return itsMap[id];
		}

	 // This will create an object for the id if one has not yet been
	 // created, then return the object for that id.
	 IO* fetchObject(const char* type, unsigned long id) {
		if ( !hasBeenCreated(id) )
		  {
			 IO* obj = IoMgr::newIO(type);

			 // If there was a problem within IoMgr::newIO(), it will throw
			 // an exception, so we should never pass this point with a
			 // NULL obj.
			 DebugEvalNL((void*) obj);
			 Assert(obj != 0);

			 itsMap[id] = obj;
		  }

		Assert(itsMap[id] != 0);
		return itsMap[id];
	 }

	 void assignObjectForId(unsigned long id, IO* object)
		{
		  if ( hasBeenCreated(id) )
			 throw ReadError("object has already been created");

		  itsMap[id] = object;
		}

	 void clear() { itsMap.clear(); }
  };

  struct Attrib {
	 Attrib(const char* t=0, const char* v=0) :
		type(t ? t : ""), value(v ? v : "") {}

	 Attrib(const string& t, const string& v) : type(t), value(v) {}

	 string type;
	 string value;
  };

  class AttribMap {
  private:
	 map<string, Attrib> itsMap;

  public:
	 AttribMap() : itsMap() {}

	 const Attrib& operator[](const string& attrib_name)
		{
		  if ( itsMap[attrib_name].type.empty() )
			 {
				ReadError err("invalid attribute name: ");
				err.appendMsg(attrib_name.c_str());
				throw err;
			 }

		  return itsMap[attrib_name];
		}

	 void clear() { itsMap.clear(); }

	 void assign(const string& attrib_name,
					 const char* type, const char* value)
		{
		DOTRACE("AsciiStreamReader::Impl::AttribMap::assign");
		  Attrib& attrib = itsMap[attrib_name];
		  attrib.type = type;
		  attrib.value = value;    DebugEvalNL(attrib.value);
		  unEscape(attrib.value);  DebugEvalNL(attrib.value);
		}
  };

  // Data members
private:
  istream& itsBuf;
  ObjectMap itsObjects;
  AttribMap itsAttribs;

#ifndef NO_IOS_EXCEPTIONS
  ios::iostate itsOriginalExceptionState;
#endif

  // Delegands -- this is the public interface that AsciiStreamReader
  // forwards to in implementing its own member functions.
public:
  template <class T>
  T readBasicType(const string& name, T* /* dummy */) {
	 istrstream ist(itsAttribs[name].value.c_str());

	 T return_val;
	 ist >> return_val;
	 DebugEval(itsAttribs.[name].value); DebugEvalNL(return_val);

	 if (ist.fail())
		throw AttributeReadError(name);

	 return return_val;
  }

  // Returns a new dynamically allocated char array
  char* readStringType(const string& name);

  IO* readObject(const string& attrib_name);

  void readValueObj(const string& name, Value& value);

  void readOwnedObject(const string& name, IO* obj);

  void initAttributes();

  IO* readRoot(Reader* reader, IO* given_root);
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

  if (len < 0)
	 throw ReadError("found a negative length for a string attribute");

  char* new_cstring = new char[len+1];
  if (len > 0)
	 ist.read(new_cstring, len);
  new_cstring[len] = '\0';

  if (ist.fail())
	 {
		delete [] new_cstring;
		throw AttributeReadError(name);
	 }  

  DebugEval(itsAttribs.[name].value); DebugEvalNL(new_cstring);
  return new_cstring;
}

IO* AsciiStreamReader::Impl::readObject(const string& attrib_name) {
DOTRACE("AsciiStreamReader::Impl::readObject");

  const Attrib& attrib = itsAttribs[attrib_name];

  istrstream ist(attrib.value.c_str());
  unsigned long id;
  ist >> id;

  if (ist.fail())
	 throw AttributeReadError(attrib_name);

  if (id == 0) { return 0; }

  // Return the object for this id, creating a new object if necessary:
  return itsObjects.fetchObject(attrib.type.c_str(), id);
}

void AsciiStreamReader::Impl::readValueObj(
  const string& attrib_name,
  Value& value
  ) {
DOTRACE("AsciiStreamReader::Impl::readValueObj");

  istrstream ist(itsAttribs[attrib_name].value.c_str());

  ist >> value;

  if (ist.fail())
	 throw AttributeReadError(attrib_name);
}

void AsciiStreamReader::Impl::readOwnedObject(
  const string& attrib_name, IO* obj
  ) {
DOTRACE("AsciiStreamReader::Impl::readOwnedObject");

  istrstream ist(itsAttribs[attrib_name].value.c_str());
  unsigned long id;
  ist >> id;

  if (ist.fail())
	 throw AttributeReadError(attrib_name);

  if ( id == 0 )
	 { throw ReadError("owned object had object id of 0"); }

  itsObjects.assignObjectForId(id, obj);
}

void AsciiStreamReader::Impl::initAttributes() {
DOTRACE("AsciiStreamReader::Impl::initAttributes");
  itsAttribs.clear();

  int attrib_count;
  itsBuf >> attrib_count;    DebugEvalNL(attrib_count);

  if (attrib_count < 0)
	 throw ReadError("found a negative attribute count");

  if ( itsBuf.fail() )
	 throw ReadError("input failed while reading attribute count");

  char type[64], name[64], equal[16];

  static fixed_block<char> value_buffer(4096);;

  for (int i = 0; i < attrib_count; ++i) {

	 itsBuf >> type >> name >> equal;   DebugEval(type); DebugEval(name); 

	 if ( itsBuf.fail() )
		throw ReadError("input failed while reading attribute type and name");

  	 itsBuf.getline(&value_buffer[0], value_buffer.size(), STRING_ENDER);
	 if ( itsBuf.gcount() >= (value_buffer.size() - 1) ) {
		throw ReadError("value buffer overflow in "
							 "AsciiStreamReader::initAttributes");
	 }

	 itsAttribs.assign(name, type, &value_buffer[0]);
  }
}

IO* AsciiStreamReader::Impl::readRoot(Reader* reader, IO* given_root) {
DOTRACE("AsciiStreamReader::Impl::readRoot");

  itsObjects.clear();

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
		  itsObjects.assignObjectForId(rootid, given_root);

		haveReadRoot = true;
	 }

	 obj = itsObjects.fetchObject(type, id);
	 
	 initAttributes();
	 obj->readFrom(reader);

	 itsBuf >> bracket >> ws;

	 if ( itsBuf.fail() )
		throw ReadError("input failed while parsing ending bracket");
  }

  return itsObjects.getObject(rootid);
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
  return itsImpl.readRoot(this, given_root);
}

#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamReader
#endif

static const char vcid_asciistreamreader_cc[] = "$Header$";
#endif // !ASCIISTREAMREADER_CC_DEFINED
