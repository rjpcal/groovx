///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Mon Jun  7 12:54:55 1999
// written: Wed Mar 29 23:42:38 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMREADER_CC_DEFINED
#define ASCIISTREAMREADER_CC_DEFINED

#include "asciistreamreader.h"

#include "io.h"
#include "iomgr.h"

#include "util/arrays.h"
#include "util/lists.h"
#include "util/value.h"

#include <iostream.h>
#include <strstream.h>
#include <map>
#include <string>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

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

  fixed_block<char> READ_BUFFER(4096);

  void readAndUnEscape(istream& is, string& text_out)
	 {
// 		DOTRACE("AsciiStreamReader::Impl::readAndUnEscape");

		int brace_level = 0;

	   fixed_block<char>::iterator
		  itr = READ_BUFFER.begin(),
		  stop = READ_BUFFER.end();

		int ch = 0;

		while ( (ch = is.get()) != EOF &&
				  !(brace_level == 0 && ch == STRING_ENDER) )
		  {
			 if (itr >= stop)
				throw ("AsciiStreamReader exceeded read buffer capacity");
			 if (ch != '\\')
				{
				  if (ch == '{') ++brace_level;
				  if (ch == '}') --brace_level;
				  *itr++ = ch;
				  continue;
				}
			 else
				{
				  int ch2 = is.get();
				  if (ch2 == EOF || ch2 == STRING_ENDER)
					 throw ReadError("missing character after trailing backslash");
				  switch (ch2) {
				  case '\\':
					 *itr++ = '\\';
					 break;
				  case 'c':
					 *itr++ = '^';
					 break;
				  case '{':
					 *itr++ = '{';
					 break;
				  case '}':
					 *itr++ = '}';
					 break;
				  default:
					 throw ReadError("invalid escape character");
					 break;
				  }
				}
		  }
		text_out.assign(READ_BUFFER.begin(), itr);
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

	 // This returns the object for the given id; the object must
	 // already have been created, otherwise an exception will be thrown.
	 IO* getObject(unsigned long id)
		{
		  IO* obj = itsMap[id];
		  if ( obj == 0 )
			 throw ReadError("no object was found for the given id");

		  return obj;
		}

	 // This will create an object for the id if one has not yet been
	 // created, then return the object for that id.
	 IO* fetchObject(const char* type, unsigned long id) {
		IO*& itsMap_at_id = itsMap[id];

		if ( itsMap_at_id == 0 )
		  {
			 IO* obj = IoMgr::newIO(type);

			 // If there was a problem within IoMgr::newIO(), it will throw
			 // an exception, so we should never pass this point with a
			 // NULL obj.
			 DebugEvalNL((void*) obj);
			 Assert(obj != 0);

			 itsMap_at_id = obj;
		  }

		Assert(itsMap_at_id != 0);
		return itsMap_at_id;
	 }

	 void assignObjectForId(unsigned long id, IO* object)
		{
		  IO*& itsMap_at_id = itsMap[id];

		  // See if an object has already been created for this id
		  if ( itsMap_at_id != 0 )
			 throw ReadError("object has already been created");

		  itsMap_at_id = object;
		}

	 void clear() { itsMap.clear(); }
  };

  struct Attrib {
	 string type;
	 string value;
  };

  class AttribMap {
  private:
	 map<string, Attrib> itsMap;
	 unsigned long itsSerialVersionId;

  public:
	 AttribMap() : itsMap(), itsSerialVersionId(0) {}

	 void readAttributes(istream& buf);

	 unsigned long getSerialVersionId() const { return itsSerialVersionId; }

	 const Attrib& operator[](const string& attrib_name)
		{
		  // DOTRACE("AsciiStreamReader::Impl::AttribMap::operator[]");
		  const Attrib& attrib = itsMap[attrib_name];
		  if ( attrib.type.empty() )
			 {
				ReadError err("invalid attribute name: ");
				err.appendMsg(attrib_name.c_str());
				throw err;
			 }

		  return attrib;
		}

  private:
	 Attrib& getNewAttrib(const string& attrib_name)
		{
		  // DOTRACE("AsciiStreamReader::Impl::AttribMap::getNewAttrib");
		  Attrib& attrib = itsMap[attrib_name];
		  if ( !attrib.type.empty() )
			 {
				ReadError err("object input stream contains"
								  "multiple attributes with name: ");
				err.appendMsg(attrib_name.c_str());
				throw err;
			 }
		  return attrib;
		}
  };

  // Data members
private:
  istream& itsBuf;
  ObjectMap itsObjects;
  slink_list<AttribMap> itsAttribs;

#ifndef NO_IOS_EXCEPTIONS
  ios::iostate itsOriginalExceptionState;
#endif

  // Helper functions
  AttribMap& currentAttribs()
	 {
		if ( itsAttribs.empty() )
		  throw ReadError("attempted to read attribute "
								"when no attribute map was active");
		return itsAttribs.front();
	 }

  void inflateObject(Reader* reader, istream& buf, IO* obj);

  // Delegands -- this is the public interface that AsciiStreamReader
  // forwards to in implementing its own member functions.
public:
  template <class T>
  T readBasicType(const string& name, T* /* dummy */) {
	 istrstream ist(currentAttribs()[name].value.c_str());

	 T return_val;
	 ist >> return_val;
	 DebugEval(currentAttribs()[name].value); DebugEvalNL(return_val);

	 if (ist.fail())
		throw AttributeReadError(name);

	 return return_val;
  }

  unsigned long readSerialVersionId()
	 { return currentAttribs().getSerialVersionId(); }

  // Returns a new dynamically allocated char array
  char* readStringType(const string& name);

  IO* readObject(const string& attrib_name);

  void readValueObj(const string& name, Value& value);

  void readOwnedObject(const string& name, IO* obj);

  void readBaseClass(Reader* reader, const char* baseClassName, IO* basePart);

  IO* readRoot(Reader* reader, IO* given_root);
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader::Impl::AttribMap member definitions
//
///////////////////////////////////////////////////////////////////////

void AsciiStreamReader::Impl::AttribMap::readAttributes(istream& buf) {
DOTRACE("AsciiStreamReader::Impl::AttribMap::readAttributes");

  itsMap.clear();

  // Skip all whitespace
  buf >> ws;

  // Check if there is a version id in the stream
  if (buf.peek() == 'v')
	 {
		int ch = buf.get();  Assert(ch == 'v');
		buf >> itsSerialVersionId;
		if ( buf.fail() )
		  throw ReadError("input failed while reading serialization version id");
	 }
  else
	 {
		itsSerialVersionId = 0;
	 }

  // Get the attribute count
  int attrib_count;
  buf >> attrib_count;    DebugEvalNL(attrib_count);

  if (attrib_count < 0)
	 throw ReadError("found a negative attribute count");

  if ( buf.fail() )
	 throw ReadError("input failed while reading attribute count");

  // Loop and load all the attributes
  char type[64], name[64], equal[16];

  for (int i = 0; i < attrib_count; ++i) {

 	 buf >> type >> name >> equal;   DebugEval(type); DebugEvalNL(name); 

	 if ( buf.fail() )
		throw ReadError("input failed while reading attribute type and name");

	 Attrib& attrib = getNewAttrib(name);

	 attrib.type = type;

	 readAndUnEscape(buf, attrib.value);
  }
}

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void AsciiStreamReader::Impl::inflateObject(Reader* reader,
														  istream& buf, IO* obj) {
DOTRACE("AsciiStreamReader::Impl::inflateObject");

  itsAttribs.push_front(AttribMap()); 

  //   ...read the object's attributes from the stream...
  itsAttribs.front().readAttributes(buf);

  //   ...now the object can query us for its attributes...
  obj->readFrom(reader);

  itsAttribs.pop_front();
}

char* AsciiStreamReader::Impl::readStringType(const string& name) {
DOTRACE("AsciiStreamReader::Impl::readStringType");

  istrstream ist(currentAttribs()[name].value.c_str());

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

  DebugEval(currentAttribs()[name].value); DebugEvalNL(new_cstring);
  return new_cstring;
}

IO* AsciiStreamReader::Impl::readObject(const string& attrib_name) {
DOTRACE("AsciiStreamReader::Impl::readObject");

  const Attrib& attrib = currentAttribs()[attrib_name];

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

  istrstream ist(currentAttribs()[attrib_name].value.c_str());

  ist >> value;

  if (ist.fail())
	 throw AttributeReadError(attrib_name);
}

void AsciiStreamReader::Impl::readOwnedObject(
  const string& attrib_name, IO* obj
  ) {
DOTRACE("AsciiStreamReader::Impl::readOwnedObject");

  istrstream ist(currentAttribs()[attrib_name].value.c_str());
  unsigned long id;
  ist >> id;

  if (ist.fail())
	 throw AttributeReadError(attrib_name);

  if ( id == 0 )
	 { throw ReadError("owned object had object id of 0"); }

  itsObjects.assignObjectForId(id, obj);
}

void AsciiStreamReader::Impl::readBaseClass(
  Reader* reader, const char* baseClassName, IO* basePart
  ) {
DOTRACE("AsciiStreamReader::Impl::readBaseClass");

  istrstream ist(currentAttribs()[baseClassName].value.c_str());
  char bracket[16];

  ist >> bracket;

  inflateObject(reader, ist, basePart);

  ist >> bracket >> ws;
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

	 inflateObject(reader, itsBuf, obj);

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

unsigned long AsciiStreamReader::readSerialVersionId() {
DOTRACE("AsciiStreamReader::readSerialVersionId");
  return itsImpl.readSerialVersionId();
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
  itsImpl.readValueObj(name, value); 
}

IO* AsciiStreamReader::readObject(const char* attrib_name) {
  return itsImpl.readObject(attrib_name);
}

void AsciiStreamReader::readOwnedObject(const char* name, IO* obj) {
  itsImpl.readOwnedObject(name, obj);
}

void AsciiStreamReader::readBaseClass(
  const char* baseClassName, IO* basePart
  ) {
  itsImpl.readBaseClass(this, baseClassName, basePart);
}

IO* AsciiStreamReader::readRoot(IO* given_root) {
  return itsImpl.readRoot(this, given_root);
}

#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamReader
#endif

static const char vcid_asciistreamreader_cc[] = "$Header$";
#endif // !ASCIISTREAMREADER_CC_DEFINED
