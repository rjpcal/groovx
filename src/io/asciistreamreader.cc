///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:54:55 1999
// written: Sat May 19 11:44:06 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMREADER_CC_DEFINED
#define ASCIISTREAMREADER_CC_DEFINED

#include "io/asciistreamreader.h"

#include "io/io.h"
#include "io/iditem.h"
#include "io/iomgr.h"

#include "util/arrays.h"
#include "util/lists.h"
#include "util/pointers.h"
#include "util/strings.h"
#include "util/value.h"

#include <iostream.h>
#include <strstream.h>
#include <map>
#include <list>

#define NO_TRACE
#include "util/trace.h"
#define LOCAL_ASSERT
#include "util/debug.h"

#if defined(IRIX6) || defined(HP9000S700)
#define AsciiStreamReader ASR
#endif

#ifdef PRESTANDARD_IOSTREAMS
#define NO_IOS_EXCEPTIONS
#endif

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

class AttributeReadError : public IO::ReadError {
public:
  AttributeReadError(const fixed_string& attrib_name) :
	 IO::ReadError("input failed while reading attribute: ")
	 {
		appendMsg(attrib_name.c_str());
	 }
};

class AsciiStreamReader::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

  // Creators
public:
  Impl(AsciiStreamReader* /*owner*/, STD_IO::istream& is) :
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
	 typedef std::map<unsigned long, IdItem<IO::IoObject> > MapType;
	 MapType itsMap;

  public:
	 ObjectMap() : itsMap() {}

	 // This returns the object for the given id; the object must
	 // already have been created, otherwise an exception will be thrown.
	 IdItem<IO::IoObject> getObject(unsigned long id)
		{
		  MapType::const_iterator itr = itsMap.find(id);
		  if ( itr == itsMap.end() ) {
			 IO::ReadError err("no object was found for the given id\n");
			 err.appendMsg("\tid: ").appendNumber(int(id));
			 throw err;
		  }

		  return (*itr).second;
		}

	 // This will create an object for the id if one has not yet been
	 // created, then return the object for that id.
	 IdItem<IO::IoObject> fetchObject(const fixed_string& type, unsigned long id)
	   {
		  MapType::const_iterator itr = itsMap.find(id);

		  if ( itr == itsMap.end() )
			 {
				IdItem<IO::IoObject> obj(IO::IoMgr::newIO(type));

				itsMap.insert(MapType::value_type(id, obj));

				return obj;
			 }

		  return (*itr).second;
		}

	 void assignObjectForId(unsigned long id, IO::IoObject* object)
		{
		  MapType::const_iterator itr = itsMap.find(id);

		  // See if an object has already been created for this id
		  if ( itr != itsMap.end() ) {
			 IO::ReadError err("object has already been created\n");
			 err.appendMsg("\ttype: ", object->ioTypename().c_str(), "\n");
			 err.appendMsg("\tid: ").appendNumber(int(id));
			 throw err;
		  }

		  itsMap.insert(MapType::value_type(id, IdItem<IO::IoObject>(object)));
		}

	 void clear() { itsMap.clear(); }
  };

  struct Attrib {
  private:
	 Attrib(); // not implemented

  public:
	 Attrib(const fixed_string& t, const fixed_string& v) :
		type(t), value(v) {}

	 fixed_string type;
	 fixed_string value;
  };

  class AttribMap {
  private:
	 fixed_string itsObjTag;

	 typedef std::pair<fixed_string, Attrib> ValueType;
	 typedef std::list<ValueType> ListType;

	 ListType itsMap;

	 IO::VersionId itsSerialVersionId;

  public:
	 AttribMap(const fixed_string& obj_tag) :
		itsObjTag(obj_tag), itsMap(), itsSerialVersionId(0) {}

	 void readAttributes(STD_IO::istream& buf);

	 IO::VersionId getSerialVersionId() const { return itsSerialVersionId; }

	 Attrib get(const fixed_string& attrib_name)
		{
		  ListType::iterator itr = itsMap.begin(), end = itsMap.end();
		  while (itr != end)
			 {
				if ((*itr).first == attrib_name)
				  {
					 Attrib result = (*itr).second;
					 itsMap.erase(itr);
					 return result;
				  }
				++itr;
			 }

		  IO::ReadError err("no attribute named '");
		  err.appendMsg(attrib_name.c_str());
		  err.appendMsg("' for ", itsObjTag.c_str());
		  throw err;
		}

  private:
	 fixed_string readAndUnEscape(STD_IO::istream& is);

	 void addNewAttrib(const fixed_string& attrib_name,
							 const fixed_string& type, const fixed_string& value)
		{
		  itsMap.push_back(ValueType(attrib_name, Attrib(type,value)));
		}
  };

  // Data members
private:
  STD_IO::istream& itsBuf;
  ObjectMap itsObjects;
  slink_list<shared_ptr<AttribMap> > itsAttribs;

#ifndef NO_IOS_EXCEPTIONS
  ios::iostate itsOriginalExceptionState;
#endif

  // Helper functions
  AttribMap& currentAttribs()
	 {
		if ( itsAttribs.empty() )
		  throw IO::ReadError("attempted to read attribute "
								"when no attribute map was active");
		return *(itsAttribs.front());
	 }

  void inflateObject(IO::Reader* reader, STD_IO::istream& buf,
							const fixed_string& obj_tag, IO::IoObject* obj);

  // Delegands -- this is the public interface that AsciiStreamReader
  // forwards to in implementing its own member functions.
public:
  template <class T>
  T readBasicType(const fixed_string& name) {
	 Attrib a = currentAttribs().get(name);
	 istrstream ist(a.value.c_str());

	 T return_val;
	 ist >> return_val;
	 DebugEval(a.value); DebugEvalNL(return_val);

	 if (ist.fail())
		throw AttributeReadError(name);

	 return return_val;
  }

  IO::VersionId readSerialVersionId()
	 { return currentAttribs().getSerialVersionId(); }

  // Returns a new dynamically allocated char array
  fixed_string readStringType(const fixed_string& name);

  MaybeIdItem<IO::IoObject> readMaybeObject(const fixed_string& attrib_name);

  void readValueObj(const fixed_string& name, Value& value);

  void readOwnedObject(const fixed_string& name, IdItem<IO::IoObject> obj);

  void readBaseClass(IO::Reader* reader, const fixed_string& baseClassName,
							IdItem<IO::IoObject> basePart);

  IdItem<IO::IoObject> readRoot(IO::Reader* reader, IO::IoObject* given_root);
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader::Impl::AttribMap member definitions
//
///////////////////////////////////////////////////////////////////////

namespace {
  const char STRING_ENDER = '^';

  fixed_block<char> READ_BUFFER(4096);
}

fixed_string AsciiStreamReader::Impl::AttribMap::readAndUnEscape(
  STD_IO::istream& is
) {
DOTRACE("AsciiStreamReader::Impl::AttribMap::readAndUnEscape");

  int brace_level = 0;

  fixed_block<char>::iterator
	 itr = READ_BUFFER.begin(),
	 stop = READ_BUFFER.end();

  int ch = 0;

  while ( (ch = is.get()) != EOF &&
			 !(brace_level == 0 && ch == STRING_ENDER) )
	 {
		if (itr >= stop)
		  {
			 IO::ReadError err("AsciiStreamReader exceeded "
									 "read buffer capacity\n"
									 "buffer contents: \n");
			 READ_BUFFER[ READ_BUFFER.size() - 1 ] = '\0';
			 err.appendMsg(&(READ_BUFFER[0]));
			 throw err;
		  }

		if (ch != '\\')
		  {
			 if (ch == '{') ++brace_level;
			 if (ch == '}') --brace_level;
			 *itr++ = char(ch);
			 continue;
		  }
		else
		  {
			 int ch2 = is.get();
			 if (ch2 == EOF || ch2 == STRING_ENDER)
				throw IO::ReadError("missing character after trailing backslash");
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
				throw IO::ReadError("invalid escape character");
				break;
			 }
		  }
	 }

  *itr = '\0';

  return fixed_string(READ_BUFFER.begin());
}

void AsciiStreamReader::Impl::AttribMap::readAttributes(STD_IO::istream& buf)
{
DOTRACE("AsciiStreamReader::Impl::AttribMap::readAttributes");

  // Skip all whitespace
  buf >> ws;

  // Check if there is a version id in the stream
  if (buf.peek() == 'v')
	 {
		int ch = buf.get();  Assert(ch == 'v');
		buf >> itsSerialVersionId;
		if ( buf.fail() )
		  throw IO::ReadError("input failed while reading serialization version id");
	 }
  else
	 {
		itsSerialVersionId = 0;
	 }

  // Get the attribute count
  int attrib_count;
  buf >> attrib_count;    DebugEvalNL(attrib_count);

  if (attrib_count < 0) {
	 IO::ReadError err("found a negative attribute count: ");
	 err.appendNumber(attrib_count);
	 throw err;
  }

  if ( buf.fail() ) {
	 IO::ReadError err("input failed while reading attribute count: ");
	 err.appendNumber(attrib_count);
	 throw err;
  }

  // Loop and load all the attributes
  char type[64], name[64], equal[16];

  for (int i = 0; i < attrib_count; ++i) {

 	 buf >> type >> name >> equal;   DebugEval(type); DebugEvalNL(name); 

	 if ( buf.fail() ) {
		IO::ReadError err("input failed while reading attribute type and name\n");
		type[63] = '\0'; err.appendMsg("\ttype: ", type, "\n");
		name[63] = '\0'; err.appendMsg("\tname: ", name, "\n");
		equal[15] = '\0'; err.appendMsg("\tequal: ", equal);
		throw err;
	 }

	 addNewAttrib(name, type, readAndUnEscape(buf));
  }
}

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader::Impl member definitions
//
///////////////////////////////////////////////////////////////////////

void AsciiStreamReader::Impl::inflateObject(
  IO::Reader* reader, STD_IO::istream& buf,
  const fixed_string& obj_tag, IO::IoObject* obj
) {
DOTRACE("AsciiStreamReader::Impl::inflateObject");

  itsAttribs.push_front(shared_ptr<AttribMap>(new AttribMap(obj_tag)));

  //   ...read the object's attributes from the stream...
  itsAttribs.front()->readAttributes(buf);

  //   ...now the object can query us for its attributes...
  obj->readFrom(reader);

  itsAttribs.pop_front();
}

fixed_string AsciiStreamReader::Impl::readStringType(const fixed_string& name) {
DOTRACE("AsciiStreamReader::Impl::readStringType");

  Attrib a = currentAttribs().get(name);
  istrstream ist(a.value.c_str());

  int len;
  ist >> len;                     DebugEvalNL(len);
  ist.get(); // ignore one char of whitespace after the length

  if (len < 0) {
	 IO::ReadError err("found a negative length for a string attribute: ");
	 err.appendNumber(len);
	 throw err;
  }

  fixed_string new_string(len+1);
  if (len > 0)
	 ist.read(new_string.data(), len);
  new_string.data()[len] = '\0';

  if (ist.fail())
	 {
		throw AttributeReadError(name);
	 }  

  DebugEval(a.value); DebugEvalNL(new_string);
  return new_string;
}

MaybeIdItem<IO::IoObject>
AsciiStreamReader::Impl::readMaybeObject(const fixed_string& attrib_name) {
DOTRACE("AsciiStreamReader::Impl::readMaybeObject");

  Attrib attrib = currentAttribs().get(attrib_name);

  istrstream ist(attrib.value.c_str());
  unsigned long id;
  ist >> id;

  if (ist.fail())
	 throw AttributeReadError(attrib_name);

  if (id == 0) { return MaybeIdItem<IO::IoObject>(); }

  // Return the object for this id, creating a new object if necessary:
  return itsObjects.fetchObject(attrib.type, id);
}

void AsciiStreamReader::Impl::readValueObj(
  const fixed_string& attrib_name,
  Value& value
  ) {
DOTRACE("AsciiStreamReader::Impl::readValueObj");

  Attrib a = currentAttribs().get(attrib_name);
  istrstream ist(a.value.c_str());

  ist >> value;

  if (ist.fail())
	 throw AttributeReadError(attrib_name);
}

void AsciiStreamReader::Impl::readOwnedObject(
  const fixed_string& attrib_name, IdItem<IO::IoObject> obj
  ) {
DOTRACE("AsciiStreamReader::Impl::readOwnedObject");

  Attrib a = currentAttribs().get(attrib_name);
  istrstream ist(a.value.c_str());
  unsigned long id;
  ist >> id;

  if (ist.fail())
	 throw AttributeReadError(attrib_name);

  if ( id == 0 )
	 throw IO::ReadError("owned object had object id of 0");

  itsObjects.assignObjectForId(id, obj.get());
}

void AsciiStreamReader::Impl::readBaseClass(
  IO::Reader* reader, const fixed_string& baseClassName,
  IdItem<IO::IoObject> basePart
  ) {
DOTRACE("AsciiStreamReader::Impl::readBaseClass");

  Attrib a = currentAttribs().get(baseClassName);
  istrstream ist(a.value.c_str());
  char bracket[16];

  ist >> bracket;

  inflateObject(reader, ist, baseClassName, basePart.get());

  ist >> bracket >> ws;
}

IdItem<IO::IoObject> AsciiStreamReader::Impl::readRoot(
  IO::Reader* reader, IO::IoObject* given_root
) {
DOTRACE("AsciiStreamReader::Impl::readRoot");

  itsObjects.clear();

  bool haveReadRoot = false;
  unsigned long rootid = 0;

  while ( itsBuf.peek() != EOF ) {
	 char type[64], equal[16], bracket[16];
	 unsigned long id;

	 itsBuf >> type >> id >> equal >> bracket;
	 DebugEval(type); DebugEvalNL(id);

	 if ( itsBuf.fail() ) {
		IO::ReadError err("input failed while reading typename and object id\n");
		type[63] = '\0'; err.appendMsg("\ttype: ", type, "\n");
		err.appendMsg("id: ").appendNumber(int(id)).appendMsg("\n");
		equal[15] = '\0'; err.appendMsg("\tequal: ", equal, "\n");
		bracket[15] = '\0'; err.appendMsg("\tbracket: ", bracket);
		throw err;
	 }

	 if ( !haveReadRoot ) {
		rootid = id;
		
		if (given_root != 0) 
		  itsObjects.assignObjectForId(rootid, given_root);

		haveReadRoot = true;
	 }

	 IdItem<IO::IoObject> obj = itsObjects.fetchObject(type, id);

	 inflateObject(reader, itsBuf, type, obj.get());

	 itsBuf >> bracket >> ws;

	 if ( itsBuf.fail() ) {
		IO::ReadError err("input failed while parsing ending bracket\n");
		bracket[15] = '\0'; err.appendMsg("\tbracket: ", bracket);
		throw err;
	 }
  }

  return itsObjects.getObject(rootid);
}

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader member definitions
//
///////////////////////////////////////////////////////////////////////

AsciiStreamReader::AsciiStreamReader (STD_IO::istream& is) :
  itsImpl( *(new Impl(this, is)) )
{
DOTRACE("AsciiStreamReader::AsciiStreamReader");
  // nothing
}

AsciiStreamReader::~AsciiStreamReader () {
DOTRACE("AsciiStreamReader::~AsciiStreamReader");
  delete &itsImpl; 
}

IO::VersionId AsciiStreamReader::readSerialVersionId() {
DOTRACE("AsciiStreamReader::readSerialVersionId");
  return itsImpl.readSerialVersionId();
}

char AsciiStreamReader::readChar(const fixed_string& name) {
DOTRACE("AsciiStreamReader::readChar");
  DebugEvalNL(name); 
  return itsImpl.template readBasicType<char>(name);
}

int AsciiStreamReader::readInt(const fixed_string& name) {
DOTRACE("AsciiStreamReader::readInt");
  DebugEvalNL(name); 
  return itsImpl.template readBasicType<int>(name);
}

bool AsciiStreamReader::readBool(const fixed_string& name) {
DOTRACE("AsciiStreamReader::readBool");
  DebugEvalNL(name);
  return bool(itsImpl.template readBasicType<int>(name));
}

double AsciiStreamReader::readDouble(const fixed_string& name) {
DOTRACE("AsciiStreamReader::readDouble");
  DebugEvalNL(name); 
  return itsImpl.template readBasicType<double>(name);
}

fixed_string AsciiStreamReader::readStringImpl(const fixed_string& name) {
  DebugEvalNL(name); 
  return itsImpl.readStringType(name);
}

void AsciiStreamReader::readValueObj(const fixed_string& name, Value& value) {
  DebugEvalNL(name); 
  itsImpl.readValueObj(name, value); 
}

IdItem<IO::IoObject>
AsciiStreamReader::readObject(const fixed_string& name)
{
  DebugEvalNL(attrib_name);
  return IdItem<IO::IoObject>(itsImpl.readMaybeObject(name));
}

MaybeIdItem<IO::IoObject>
AsciiStreamReader::readMaybeObject(const fixed_string& name)
{
  DebugEvalNL(attrib_name);
  return itsImpl.readMaybeObject(name);
}

void AsciiStreamReader::readOwnedObject(const fixed_string& name,
													 IdItem<IO::IoObject> obj) {
  DebugEvalNL(name); 
  itsImpl.readOwnedObject(name, obj);
}

void AsciiStreamReader::readBaseClass(
  const fixed_string& baseClassName, IdItem<IO::IoObject> basePart
) {
  DebugEvalNL(baseClassName); 
  itsImpl.readBaseClass(this, baseClassName, basePart);
}

IdItem<IO::IoObject> AsciiStreamReader::readRoot(IO::IoObject* given_root) {
  return itsImpl.readRoot(this, given_root);
}

#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamReader
#endif

static const char vcid_asciistreamreader_cc[] = "$Header$";
#endif // !ASCIISTREAMREADER_CC_DEFINED
