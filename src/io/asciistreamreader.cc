///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun  7 12:54:55 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//
// GroovX is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with GroovX; if not, write to the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef ASCIISTREAMREADER_CC_DEFINED
#define ASCIISTREAMREADER_CC_DEFINED

#include "io/asciistreamreader.h"

#include "io/io.h"
#include "io/reader.h"

#include "util/arrays.h"
#include "util/cstrstream.h"
#include "util/error.h"
#include "util/objmgr.h"
#include "util/pointers.h"
#include "util/ref.h"
#include "util/slink_list.h"
#include "util/strings.h"
#include "util/value.h"

#include <iostream>
#include <map>
#include <list>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER;

using Util::Ref;
using Util::SoftRef;

namespace IO
{
  class ObjectMap
  {
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
            throw Util::Error(fstring("no object was found "
                                      "for the given id:", id));
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
            fstring msg;
            msg.append("object has already been created\n");
            msg.append("\ttype: ", object->objTypename().c_str(), "\n");
            msg.append("\tid: ", id);
            throw Util::Error(msg);
          }

        itsMap.insert(MapType::value_type(id, object));
      }

    void clear() { itsMap.clear(); }
  };


  class AttribMap
  {
  public:

    struct Attrib
    {
    private:
      Attrib(); // not implemented

    public:
      Attrib(const fstring& t, const fstring& v) :
        type(t), value(v) {}

      fstring type;
      fstring value;
    };

  private:
    fstring itsObjTag;

    typedef std::pair<fstring, Attrib> ValueType;
    typedef std::list<ValueType> ListType;

    ListType itsMap;

    IO::VersionId itsSerialVersionId;

  public:
    AttribMap(const fstring& obj_tag) :
      itsObjTag(obj_tag), itsMap(), itsSerialVersionId(0) {}

    IO::VersionId getSerialVersionId() const { return itsSerialVersionId; }

    void setSerialVersionId(IO::VersionId id) { itsSerialVersionId = id; }

    Attrib get(const fstring& attrib_name)
      {
        ListType::iterator itr = itsMap.begin(), end = itsMap.end();
        while (itr != end)
          {
            dbgEval(3, attrib_name); dbgEvalNL(3, (*itr).first);
            if ((*itr).first == attrib_name)
              {
                Attrib result = (*itr).second;
                itsMap.erase(itr);
                return result;
              }
            ++itr;
          }

        throw Util::Error(fstring("no attribute named '",
                                  attrib_name.c_str(), "' for ",
                                  itsObjTag.c_str()));
      }

    void addNewAttrib(const fstring& attrib_name,
                      const fstring& type, const fstring& value)
      {
        itsMap.push_back(ValueType(attrib_name, Attrib(type,value)));
      }
  };
}

using IO::AttribMap;

namespace
{
  class AttributeReadError : public Util::Error
  {
  public:
    AttributeReadError(const fstring& attrib_name,
                       const fstring& attrib_value) :
      Util::Error(fstring("error reading attribute '", attrib_name,
                          "' with value '", attrib_value, "'"))
    {}
  };


  fstring readAndUnEscape(STD_IO::istream& is)
  {
  DOTRACE("<asciistreamreader.cc>::readAndUnEscape");

    static const char STRING_ENDER = '^';

    static fixed_block<char> READ_BUFFER(4096);

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
            throw Util::Error(fstring("AsciiStreamReader exceeded "
                                      "read buffer capacity\n"
                                      "buffer contents: \n",
                                      &(READ_BUFFER[0])));
          }

        // We only substitute in the escape sequence if we are reading at the
        // zero-th brace level; otherwise, we leave the escape sequence in
        // since it will eventually be parsed when the brace-nested object
        // itself is parsed.
        if (ch != '\\' || brace_level > 0)
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
              throw Util::Error("missing character after trailing backslash");
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
                *itr = '\0';
                throw Util::Error
                  (fstring("invalid escape character '", char(ch2),
                           "' with buffer contents: ", &READ_BUFFER[0]));
                break;
              }
          }
      }

    *itr = '\0';

    return fstring(Util::CharData(READ_BUFFER.begin(),
                                  itr - READ_BUFFER.begin()));
  }
}

// This is a hack to help shorten up names for assemblers on systems
// that need short identifier names.
#if defined(SHORTEN_SYMBOL_NAMES)
#define AsciiStreamReader ASR
#endif

class AsciiStreamReader : public IO::Reader
{
public:
  AsciiStreamReader(STD_IO::istream& is);

  virtual ~AsciiStreamReader() throw();

  virtual IO::VersionId readSerialVersionId();

  virtual char readChar(const fstring& name);
  virtual int readInt(const fstring& name);
  virtual bool readBool(const fstring& name);
  virtual double readDouble(const fstring& name);
  virtual void readValueObj(const fstring& name, Value& value);

  virtual Util::Ref<IO::IoObject> readObject(const fstring& name);
  virtual Util::SoftRef<IO::IoObject>
    readMaybeObject(const fstring& name);

  virtual void readOwnedObject(const fstring& name,
                               Util::Ref<IO::IoObject> obj);
  virtual void readBaseClass(const fstring& baseClassName,
                             Util::Ref<IO::IoObject> basePart);

  virtual Util::Ref<IO::IoObject> readRoot(IO::IoObject* root=0);

protected:
  virtual fstring readStringImpl(const fstring& name);

private:
  STD_IO::istream& itsBuf;
  IO::ObjectMap itsObjects;
  slink_list<shared_ptr<AttribMap> > itsAttribs;

  AttribMap& currentAttribs()
  {
    if ( itsAttribs.is_empty() )
      throw Util::Error("attempted to read attribute "
                        "when no attribute map was active");
    return *(itsAttribs.front());
  }

  void inflateObject(STD_IO::istream& buf,
                     const fstring& obj_tag, Ref<IO::IoObject> obj);

  template <class T>
  T readBasicType(const fstring& name)
  {
    dbgEvalNL(3, name);

    AttribMap::Attrib a = currentAttribs().get(name);
    Util::icstrstream ist(a.value.c_str());

    T return_val;
    ist >> return_val;
    dbgEval(3, a.value); dbgEvalNL(3, return_val);

    if (ist.fail())
      throw AttributeReadError(name, a.value);

    return return_val;
  }
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader member definitions
//
///////////////////////////////////////////////////////////////////////

AsciiStreamReader::AsciiStreamReader (STD_IO::istream& is) :
  itsBuf(is), itsObjects(), itsAttribs()
{
DOTRACE("AsciiStreamReader::AsciiStreamReader");
  // nothing
}

AsciiStreamReader::~AsciiStreamReader () throw()
{
DOTRACE("AsciiStreamReader::~AsciiStreamReader");
}

IO::VersionId AsciiStreamReader::readSerialVersionId()
{
DOTRACE("AsciiStreamReader::readSerialVersionId");
  return currentAttribs().getSerialVersionId();
}

char AsciiStreamReader::readChar(const fstring& name)
{
DOTRACE("AsciiStreamReader::readChar");
  return readBasicType<char>(name);
}

int AsciiStreamReader::readInt(const fstring& name)
{
DOTRACE("AsciiStreamReader::readInt");
  return readBasicType<int>(name);
}

bool AsciiStreamReader::readBool(const fstring& name)
{
DOTRACE("AsciiStreamReader::readBool");
  return bool(readBasicType<int>(name));
}

double AsciiStreamReader::readDouble(const fstring& name)
{
DOTRACE("AsciiStreamReader::readDouble");
  return readBasicType<double>(name);
}

fstring AsciiStreamReader::readStringImpl(const fstring& name)
{
DOTRACE("AsciiStreamReader::readStringImpl");
  dbgEvalNL(3, name);

  AttribMap::Attrib a = currentAttribs().get(name);
  Util::icstrstream ist(a.value.c_str());

  int len;
  ist >> len;                     dbgEvalNL(3, len);
  ist.get(); // ignore one char of whitespace after the length

  if (len < 0)
    {
      throw Util::Error(fstring("found a negative length "
                                "for a string attribute: ", len));
    }

  fstring new_string;
  new_string.readsome(ist, (unsigned int) len);

  if (ist.fail())
    {
      throw AttributeReadError(name, a.value);
    }

  dbgEval(3, a.value); dbgEvalNL(3, new_string);
  return new_string;
}

void AsciiStreamReader::readValueObj(const fstring& name, Value& value)
{
DOTRACE("AsciiStreamReader::readValueObj");
  dbgEvalNL(3, name);

  AttribMap::Attrib a = currentAttribs().get(name);
  Util::icstrstream ist(a.value.c_str());

  ist >> value;

  if (ist.fail())
    throw AttributeReadError(name, a.value);
}

Ref<IO::IoObject>
AsciiStreamReader::readObject(const fstring& name)
{
DOTRACE("AsciiStreamReader::readObject");
  dbgEvalNL(3, name);
  return Ref<IO::IoObject>(readMaybeObject(name));
}

SoftRef<IO::IoObject>
AsciiStreamReader::readMaybeObject(const fstring& name)
{
DOTRACE("AsciiStreamReader::readMaybeObject");
  dbgEvalNL(3, name);

  AttribMap::Attrib attrib = currentAttribs().get(name);

  Util::icstrstream ist(attrib.value.c_str());
  Util::UID id;
  ist >> id;

  if (ist.fail())
    throw AttributeReadError(name, attrib.value);

  if (id == 0) { return SoftRef<IO::IoObject>(); }

  // Return the object for this id, creating a new object if necessary:
  return itsObjects.fetchObject(attrib.type, id);
}

void AsciiStreamReader::readOwnedObject(const fstring& name,
                                        Ref<IO::IoObject> obj)
{
DOTRACE("AsciiStreamReader::readOwnedObject");
  dbgEvalNL(3, name);

  AttribMap::Attrib a = currentAttribs().get(name);
  Util::icstrstream ist(a.value.c_str());
  char bracket[16];

  ist >> bracket;

  inflateObject(ist, name, obj);

  ist >> bracket >> STD_IO::ws;
}

void AsciiStreamReader::readBaseClass(const fstring& baseClassName,
                                      Ref<IO::IoObject> basePart)
{
DOTRACE("AsciiStreamReader::readBaseClass");
  dbgEvalNL(3, baseClassName);
  readOwnedObject(baseClassName, basePart);
}

Ref<IO::IoObject> AsciiStreamReader::readRoot(IO::IoObject* given_root)
{
DOTRACE("AsciiStreamReader::readRoot");

  itsObjects.clear();

  bool haveReadRoot = false;
  Util::UID rootid = 0;

  fstring type;
  fstring equal;
  fstring bracket;
  Util::UID id;

  while ( itsBuf.peek() != EOF )
    {
      itsBuf >> type >> id >> equal >> bracket;
      dbgEval(3, type); dbgEvalNL(3, id);

      if ( itsBuf.fail() )
        {
          fstring msg;
          msg.append("input failed while reading typename and object id\n");
          msg.append("\ttype: ", type, "\n");
          msg.append("id: ", id, "\n");
          msg.append("\tequal: ", equal, "\n");
          msg.append("\tbracket: ", bracket);
          throw Util::Error(msg);
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

      inflateObject(itsBuf, type, obj);

      itsBuf >> bracket >> STD_IO::ws;

      if ( itsBuf.fail() )
        {
          throw Util::Error(fstring("input failed "
                                    "while parsing ending bracket\n",
                                    "\tbracket: ", bracket));
        }
    }

  return itsObjects.getObject(rootid);
}

void AsciiStreamReader::inflateObject(STD_IO::istream& buf,
                                      const fstring& obj_tag,
                                      Ref<IO::IoObject> obj)
{
DOTRACE("AsciiStreamReader::inflateObject");

  //
  // (1) read the object's attributes from the stream...
  //
  shared_ptr<AttribMap> attribMap( new AttribMap(obj_tag) );

  // Skip all whitespace
  buf >> STD_IO::ws;

  IO::VersionId svid = 0;

  // Check if there is a version id in the stream
  if (buf.peek() == 'v')
    {
      int ch = buf.get();  Assert(ch == 'v');
      buf >> svid;
      if ( buf.fail() )
        throw Util::Error("input failed while reading "
                          "serialization version id");
    }

  attribMap->setSerialVersionId(svid);

  // Get the attribute count
  int attrib_count;
  buf >> attrib_count;    dbgEvalNL(3, attrib_count);

  if (attrib_count < 0)
    {
      throw Util::Error(fstring("found a negative attribute count: ",
                                attrib_count));
    }

  if ( buf.fail() )
    {
      throw Util::Error(fstring("input failed while reading "
                                "attribute count: ", attrib_count));
    }

  // Loop and load all the attributes
  fstring type;
  fstring name;
  fstring equal;

  for (int i = 0; i < attrib_count; ++i)
    {
      buf >> type >> name >> equal;   dbgEval(3, type); dbgEvalNL(3, name);

      if ( buf.fail() )
        {
          fstring msg;
          msg.append("input failed while reading attribute type and name\n");
          msg.append("\ttype: ", type, "\n");
          msg.append("\tname: ", name, "\n");
          msg.append("\tequal: ", equal);
          throw Util::Error(msg);
        }

      attribMap->addNewAttrib(name, type, readAndUnEscape(buf));
    }

  itsAttribs.push_front(attribMap);

  //
  // (2) now the object can query us for its attributes...
  //
  obj->readFrom(*this);

  itsAttribs.pop_front();
}

shared_ptr<IO::Reader> IO::makeAsciiStreamReader(STD_IO::istream& os)
{
  return make_shared( new AsciiStreamReader(os) );
}

#if 0
shared_ptr<IO::Reader> IO::makeAsciiStreamReader(const char* filename)
{
  return make_shared( new AsciiStreamReader(filename) );
}
#endif

static const char vcid_asciistreamreader_cc[] = "$Header$";
#endif // !ASCIISTREAMREADER_CC_DEFINED
