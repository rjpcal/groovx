///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Mon Jun  7 12:54:55 1999
// written: Wed Aug  8 20:16:40 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMREADER_CC_DEFINED
#define ASCIISTREAMREADER_CC_DEFINED

#include "io/asciistreamreader.h"

#include "io/io.h"

#include "util/arrays.h"
#include "util/objmgr.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/slink_list.h"
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

///////////////////////////////////////////////////////////////////////
//
// File scope stuff
//
///////////////////////////////////////////////////////////////////////

class AttributeReadError : public IO::ReadError {
public:
  AttributeReadError(const fstring& attrib_name) :
    IO::ReadError()
  {
    append("input failed while reading attribute: ", attrib_name.c_str());
  }

  FIX_COPY_CTOR(AttributeReadError, IO::ReadError);
};

class AsciiStreamReader::Impl {
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

  // Creators
public:
  Impl(AsciiStreamReader* /*owner*/, STD_IO::istream& is) :
    itsBuf(is), itsObjects(), itsAttribs()
  {}

  ~Impl() {}

  // Nested types
public:

  class ObjectMap {
  private:
    typedef std::map<Util::UID, Ref<IO::IoObject> > MapType;
    MapType itsMap;

  public:
    ObjectMap() : itsMap() {}

    // This returns the object for the given id; the object must
    // already have been created, otherwise an exception will be thrown.
    Ref<IO::IoObject> getObject(Util::UID id)
      {
        MapType::const_iterator itr = itsMap.find(id);
        if ( itr == itsMap.end() )
          {
            throw IO::ReadError("no object was found for the given id:",
                                      id);
          }

        return (*itr).second;
      }

    // This will create an object for the id if one has not yet been
    // created, then return the object for that id.
    Ref<IO::IoObject> fetchObject(const fstring& type, Util::UID id)
      {
        MapType::const_iterator itr = itsMap.find(id);

        if ( itr == itsMap.end() )
          {
            Ref<IO::IoObject> obj(
                     Util::ObjMgr::newTypedObj<IO::IoObject>(type));

            itsMap.insert(MapType::value_type(id, obj));

            return obj;
          }

        return (*itr).second;
      }

    void assignObjectForId(Util::UID id, Ref<IO::IoObject> object)
      {
        MapType::const_iterator itr = itsMap.find(id);

        // See if an object has already been created for this id
        if ( itr != itsMap.end() )
          {
            IO::ReadError err;
            err.append("object has already been created\n");
            err.append("\ttype: ", object->ioTypename().c_str(), "\n");
            err.append("\tid: ", id);
            throw err;
          }

        itsMap.insert(MapType::value_type(id, object));
      }

    void clear() { itsMap.clear(); }
  };

  struct Attrib {
  private:
    Attrib(); // not implemented

  public:
    Attrib(const fstring& t, const fstring& v) :
      type(t), value(v) {}

    fstring type;
    fstring value;
  };

  class AttribMap {
  private:
    fstring itsObjTag;

    typedef std::pair<fstring, Attrib> ValueType;
    typedef std::list<ValueType> ListType;

    ListType itsMap;

    IO::VersionId itsSerialVersionId;

  public:
    AttribMap(const fstring& obj_tag) :
      itsObjTag(obj_tag), itsMap(), itsSerialVersionId(0) {}

    void readAttributes(STD_IO::istream& buf);

    IO::VersionId getSerialVersionId() const { return itsSerialVersionId; }

    Attrib get(const fstring& attrib_name)
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

        throw IO::ReadError("no attribute named '",
                            attrib_name.c_str(), "' for ",
                            itsObjTag.c_str());
      }

  private:
    fstring readAndUnEscape(STD_IO::istream& is);

    void addNewAttrib(const fstring& attrib_name,
                      const fstring& type, const fstring& value)
      {
        itsMap.push_back(ValueType(attrib_name, Attrib(type,value)));
      }
  };

  // Data members
private:
  STD_IO::istream& itsBuf;
  ObjectMap itsObjects;
  slink_list<shared_ptr<AttribMap> > itsAttribs;

  // Helper functions
  AttribMap& currentAttribs()
    {
      if ( itsAttribs.empty() )
        throw IO::ReadError("attempted to read attribute "
                            "when no attribute map was active");
      return *(itsAttribs.front());
    }

  void inflateObject(IO::Reader* reader, STD_IO::istream& buf,
                     const fstring& obj_tag, Ref<IO::IoObject> obj);

  // Delegands -- this is the public interface that AsciiStreamReader
  // forwards to in implementing its own member functions.
public:
  template <class T>
  T readBasicType(const fstring& name)
  {
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
  fstring readStringType(const fstring& name);

  WeakRef<IO::IoObject> readMaybeObject(const fstring& attrib_name);

  void readValueObj(const fstring& name, Value& value);

  void readOwnedObject(const fstring& name, Ref<IO::IoObject> obj);

  void readBaseClass(IO::Reader* reader, const fstring& baseClassName,
                     Ref<IO::IoObject> basePart);

  Ref<IO::IoObject> readRoot(IO::Reader* reader, IO::IoObject* given_root);
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader::Impl::AttribMap member definitions
//
///////////////////////////////////////////////////////////////////////

namespace
{
  const char STRING_ENDER = '^';

  fixed_block<char> READ_BUFFER(4096);
}

fstring AsciiStreamReader::Impl::AttribMap::readAndUnEscape(
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
          READ_BUFFER[ READ_BUFFER.size() - 1 ] = '\0';
          throw IO::ReadError("AsciiStreamReader exceeded "
                              "read buffer capacity\n"
                              "buffer contents: \n",
                              &(READ_BUFFER[0]));
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
            throw IO::ReadError("missing character "
                                "after trailing backslash");
          switch (ch2)
            {
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

  return fstring(READ_BUFFER.begin());
}

void AsciiStreamReader::Impl::AttribMap::readAttributes(STD_IO::istream& buf)
{
DOTRACE("AsciiStreamReader::Impl::AttribMap::readAttributes");

  // Skip all whitespace
  buf >> STD_IO::ws;

  // Check if there is a version id in the stream
  if (buf.peek() == 'v')
    {
      int ch = buf.get();  Assert(ch == 'v');
      buf >> itsSerialVersionId;
      if ( buf.fail() )
        throw IO::ReadError("input failed while reading "
                            "serialization version id");
    }
  else
    {
      itsSerialVersionId = 0;
    }

  // Get the attribute count
  int attrib_count;
  buf >> attrib_count;    DebugEvalNL(attrib_count);

  if (attrib_count < 0)
    {
      throw IO::ReadError("found a negative attribute count: ", attrib_count);
    }

  if ( buf.fail() )
    {
      throw IO::ReadError("input failed while reading attribute count: ",
                          attrib_count);
    }

  // Loop and load all the attributes
  char type[64], name[64], equal[16];

  for (int i = 0; i < attrib_count; ++i)
    {
      buf >> type >> name >> equal;   DebugEval(type); DebugEvalNL(name);

      if ( buf.fail() )
        {
          IO::ReadError err;
          err.append("input failed while reading attribute type and name\n");
          type[63] = '\0'; err.append("\ttype: ", type, "\n");
          name[63] = '\0'; err.append("\tname: ", name, "\n");
          equal[15] = '\0'; err.append("\tequal: ", equal);
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
  const fstring& obj_tag, Ref<IO::IoObject> obj
)
{
DOTRACE("AsciiStreamReader::Impl::inflateObject");

  itsAttribs.push_front(shared_ptr<AttribMap>(new AttribMap(obj_tag)));

  //   ...read the object's attributes from the stream...
  itsAttribs.front()->readAttributes(buf);

  //   ...now the object can query us for its attributes...
  obj->readFrom(reader);

  itsAttribs.pop_front();
}

fstring AsciiStreamReader::Impl::readStringType(const fstring& name)
{
DOTRACE("AsciiStreamReader::Impl::readStringType");

  Attrib a = currentAttribs().get(name);
  istrstream ist(a.value.c_str());

  int len;
  ist >> len;                     DebugEvalNL(len);
  ist.get(); // ignore one char of whitespace after the length

  if (len < 0)
    {
      throw IO::ReadError("found a negative length "
                          "for a string attribute: ", len);
    }

  fstring new_string(len+1);
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

WeakRef<IO::IoObject>
AsciiStreamReader::Impl::readMaybeObject(const fstring& attrib_name)
{
DOTRACE("AsciiStreamReader::Impl::readMaybeObject");

  Attrib attrib = currentAttribs().get(attrib_name);

  istrstream ist(attrib.value.c_str());
  Util::UID id;
  ist >> id;

  if (ist.fail())
    throw AttributeReadError(attrib_name);

  if (id == 0) { return WeakRef<IO::IoObject>(); }

  // Return the object for this id, creating a new object if necessary:
#ifndef ACC_COMPILER
  return itsObjects.fetchObject(attrib.type, id);
#else
  return WeakRef<IO::IoObject>(itsObjects.fetchObject(attrib.type, id));
#endif
}

void AsciiStreamReader::Impl::readValueObj(
  const fstring& attrib_name,
  Value& value
  )
{
DOTRACE("AsciiStreamReader::Impl::readValueObj");

  Attrib a = currentAttribs().get(attrib_name);
  istrstream ist(a.value.c_str());

  ist >> value;

  if (ist.fail())
    throw AttributeReadError(attrib_name);
}

void AsciiStreamReader::Impl::readOwnedObject(
  const fstring& attrib_name, Ref<IO::IoObject> obj
  )
{
DOTRACE("AsciiStreamReader::Impl::readOwnedObject");

  Attrib a = currentAttribs().get(attrib_name);
  istrstream ist(a.value.c_str());
  Util::UID id;
  ist >> id;

  if (ist.fail())
    throw AttributeReadError(attrib_name);

  if ( id == 0 )
    throw IO::ReadError("owned object had object id of 0");

  itsObjects.assignObjectForId(id, obj);
}

void AsciiStreamReader::Impl::readBaseClass(
  IO::Reader* reader, const fstring& baseClassName,
  Ref<IO::IoObject> basePart
  )
{
DOTRACE("AsciiStreamReader::Impl::readBaseClass");

  Attrib a = currentAttribs().get(baseClassName);
  istrstream ist(a.value.c_str());
  char bracket[16];

  ist >> bracket;

  inflateObject(reader, ist, baseClassName, basePart);

  ist >> bracket >> STD_IO::ws;
}

Ref<IO::IoObject> AsciiStreamReader::Impl::readRoot(
  IO::Reader* reader, IO::IoObject* given_root
  )
{
DOTRACE("AsciiStreamReader::Impl::readRoot");

  itsObjects.clear();

  bool haveReadRoot = false;
  Util::UID rootid = 0;

  while ( itsBuf.peek() != EOF )
    {
      char type[64], equal[16], bracket[16];
      Util::UID id;

      itsBuf >> type >> id >> equal >> bracket;
      DebugEval(type); DebugEvalNL(id);

      if ( itsBuf.fail() )
        {
          type[63] = '\0'; equal[15] = '\0'; bracket[15] = '\0';
          IO::ReadError err;
          err.append("input failed while reading typename and object id\n");
          err.append("\ttype: ", type, "\n");
          err.append("id: ", id, "\n");
          err.append("\tequal: ", equal, "\n");
          err.append("\tbracket: ", bracket);
          throw err;
        }

      if ( !haveReadRoot )
        {
          rootid = id;

          if (given_root != 0)
            itsObjects.assignObjectForId(rootid,
                                         Ref<IO::IoObject>(given_root));

          haveReadRoot = true;
        }

      Ref<IO::IoObject> obj = itsObjects.fetchObject(type, id);

      inflateObject(reader, itsBuf, type, obj);

      itsBuf >> bracket >> STD_IO::ws;

      if ( itsBuf.fail() )
        {
          bracket[15] = '\0';
          throw IO::ReadError("input failed "
                              "while parsing ending bracket\n",
                              "\tbracket: ", bracket);
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

AsciiStreamReader::~AsciiStreamReader ()
{
DOTRACE("AsciiStreamReader::~AsciiStreamReader");
  delete &itsImpl;
}

IO::VersionId AsciiStreamReader::readSerialVersionId()
{
DOTRACE("AsciiStreamReader::readSerialVersionId");
  return itsImpl.readSerialVersionId();
}

char AsciiStreamReader::readChar(const fstring& name)
{
DOTRACE("AsciiStreamReader::readChar");
  DebugEvalNL(name);
  return itsImpl.template readBasicType<char>(name);
}

int AsciiStreamReader::readInt(const fstring& name)
{
DOTRACE("AsciiStreamReader::readInt");
  DebugEvalNL(name);
  return itsImpl.template readBasicType<int>(name);
}

bool AsciiStreamReader::readBool(const fstring& name)
{
DOTRACE("AsciiStreamReader::readBool");
  DebugEvalNL(name);
  return bool(itsImpl.template readBasicType<int>(name));
}

double AsciiStreamReader::readDouble(const fstring& name)
{
DOTRACE("AsciiStreamReader::readDouble");
  DebugEvalNL(name);
  return itsImpl.template readBasicType<double>(name);
}

fstring AsciiStreamReader::readStringImpl(const fstring& name)
{
  DebugEvalNL(name);
  return itsImpl.readStringType(name);
}

void AsciiStreamReader::readValueObj(const fstring& name, Value& value)
{
  DebugEvalNL(name);
  itsImpl.readValueObj(name, value);
}

Ref<IO::IoObject>
AsciiStreamReader::readObject(const fstring& name)
{
  DebugEvalNL(attrib_name);
  return Ref<IO::IoObject>(itsImpl.readMaybeObject(name));
}

WeakRef<IO::IoObject>
AsciiStreamReader::readMaybeObject(const fstring& name)
{
  DebugEvalNL(attrib_name);
  return itsImpl.readMaybeObject(name);
}

void AsciiStreamReader::readOwnedObject(const fstring& name,
                                        Ref<IO::IoObject> obj)
{
  DebugEvalNL(name);
  itsImpl.readOwnedObject(name, obj);
}

void AsciiStreamReader::readBaseClass(
  const fstring& baseClassName, Ref<IO::IoObject> basePart
)
{
  DebugEvalNL(baseClassName);
  itsImpl.readBaseClass(this, baseClassName, basePart);
}

Ref<IO::IoObject> AsciiStreamReader::readRoot(IO::IoObject* given_root)
{
  return itsImpl.readRoot(this, given_root);
}

#if defined(IRIX6) || defined(HP9000S700)
#undef AsciiStreamReader
#endif

static const char vcid_asciistreamreader_cc[] = "$Header$";
#endif // !ASCIISTREAMREADER_CC_DEFINED
