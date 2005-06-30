///////////////////////////////////////////////////////////////////////
//
// asciistreamreader.cc
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 12:54:55 1999
// commit: $Id$
// $HeadURL$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_IO_ASCIISTREAMREADER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_ASCIISTREAMREADER_CC_UTC20050626084021_DEFINED

#include "io/asciistreamreader.h"

#include "io/io.h"
#include "io/reader.h"
#include "io/readattribmap.h"
#include "io/readobjectmap.h"

#include "nub/ref.h"

#include "rutz/cstrstream.h"
#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/sharedptr.h"
#include "rutz/value.h"

#include <istream>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;
using rutz::shared_ptr;

using nub::ref;
using nub::soft_ref;

using IO::AttribMap;

namespace
{
  void throwAttrError(const fstring& attrib_name,
                      const fstring& attrib_value,
                      const rutz::file_pos& pos)
  {
    throw rutz::error(fstring("error reading attribute '", attrib_name,
                              "' with value '", attrib_value, "'"),
                      pos);
  }

  fstring readAndUnEscape(std::istream& is)
  {
  GVX_TRACE("<asciistreamreader.cc>::readAndUnEscape");

    static const char STRING_ENDER = '^';

    static std::vector<char> buffer(4096);

    buffer.resize(0);

    int brace_level = 0;

    int ch = 0;

    while ( (ch = is.get()) != EOF &&
            !(brace_level == 0 && ch == STRING_ENDER) )
      {
        // We only substitute in the escape sequence if we are reading at the
        // zero-th brace level; otherwise, we leave the escape sequence in
        // since it will eventually be parsed when the brace-nested object
        // itself is parsed.
        if (ch != '\\' || brace_level > 0)
          {
            if (ch == '{') ++brace_level;
            if (ch == '}') --brace_level;
            buffer.push_back(char(ch));
            continue;
          }
        else
          {
            const int ch2 = is.get();

            if (ch2 == EOF || ch2 == STRING_ENDER)
              throw rutz::error("missing character "
                                "after trailing backslash", SRC_POS);

            switch (ch2)
              {
              case '\\': buffer.push_back('\\'); break;
              case 'c':  buffer.push_back('^');  break;
              case '{':  buffer.push_back('{');  break;
              case '}':  buffer.push_back('}');  break;

              default:
                buffer.push_back('\0');
                throw rutz::error
                  (fstring("invalid escape character '", char(ch2),
                           "' with buffer contents: ", &buffer[0]),
                   SRC_POS);
                break;
              }
          }
      } // while loop

    return fstring(rutz::char_range(&buffer[0], buffer.size()));
  }
}

// This is a hack to help shorten up names for assemblers on systems
// that need short identifier names.
#if defined(GVX_SHORTEN_SYMBOL_NAMES)
#define AsciiStreamReader ASR
#endif

class AsciiStreamReader : public IO::Reader
{
public:
  AsciiStreamReader(std::istream& is);
  AsciiStreamReader(const char* filename);

  virtual ~AsciiStreamReader() throw();

  virtual IO::VersionId readSerialVersionId();

  virtual char readChar(const fstring& name);
  virtual int readInt(const fstring& name);
  virtual bool readBool(const fstring& name);
  virtual double readDouble(const fstring& name);
  virtual void readValueObj(const fstring& name, rutz::value& v);

  virtual void readRawData(const fstring& name, rutz::byte_array& data)
  { defaultReadRawData(name, data); }

  virtual nub::ref<IO::IoObject> readObject(const fstring& name);
  virtual nub::soft_ref<IO::IoObject>
    readMaybeObject(const fstring& name);

  virtual void readOwnedObject(const fstring& name,
                               nub::ref<IO::IoObject> obj);
  virtual void readBaseClass(const fstring& baseClassName,
                             nub::ref<IO::IoObject> basePart);

  virtual nub::ref<IO::IoObject> readRoot(IO::IoObject* root=0);

protected:
  virtual fstring readStringImpl(const fstring& name);

private:
  shared_ptr<std::istream> itsOwnedStream;
  std::istream& itsBuf;
  IO::ObjectMap itsObjects;
  std::vector<shared_ptr<AttribMap> > itsAttribs;

  AttribMap& currentAttribs()
  {
    if ( itsAttribs.empty() )
      throw rutz::error("attempted to read attribute "
                        "when no attribute map was active", SRC_POS);
    return *(itsAttribs.back());
  }

  void inflateObject(std::istream& buf,
                     const fstring& obj_tag, ref<IO::IoObject> obj);

  template <class T>
  T readBasicType(const fstring& name)
  {
    dbg_eval_nl(3, name);

    AttribMap::Attrib a = currentAttribs().get(name);
    rutz::icstrstream ist(a.value.c_str());

    T return_val;
    ist >> return_val;
    dbg_eval(3, a.value); dbg_eval_nl(3, return_val);

    if (ist.fail())
      throwAttrError(name, a.value, SRC_POS);

    return return_val;
  }
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamReader member definitions
//
///////////////////////////////////////////////////////////////////////

AsciiStreamReader::AsciiStreamReader(std::istream& is) :
  itsOwnedStream(),
  itsBuf(is),
  itsObjects(),
  itsAttribs()
{
GVX_TRACE("AsciiStreamReader::AsciiStreamReader");
}

AsciiStreamReader::AsciiStreamReader(const char* filename) :
  itsOwnedStream(rutz::igzopen(filename)),
  itsBuf(*itsOwnedStream),
  itsObjects(),
  itsAttribs()
{
GVX_TRACE("AsciiStreamReader::AsciiStreamReader");
}

AsciiStreamReader::~AsciiStreamReader () throw()
{
GVX_TRACE("AsciiStreamReader::~AsciiStreamReader");
}

IO::VersionId AsciiStreamReader::readSerialVersionId()
{
GVX_TRACE("AsciiStreamReader::readSerialVersionId");
  return currentAttribs().getSerialVersionId();
}

char AsciiStreamReader::readChar(const fstring& name)
{
GVX_TRACE("AsciiStreamReader::readChar");
  return readBasicType<char>(name);
}

int AsciiStreamReader::readInt(const fstring& name)
{
GVX_TRACE("AsciiStreamReader::readInt");
  return readBasicType<int>(name);
}

bool AsciiStreamReader::readBool(const fstring& name)
{
GVX_TRACE("AsciiStreamReader::readBool");
  return bool(readBasicType<int>(name));
}

double AsciiStreamReader::readDouble(const fstring& name)
{
GVX_TRACE("AsciiStreamReader::readDouble");
  return readBasicType<double>(name);
}

fstring AsciiStreamReader::readStringImpl(const fstring& name)
{
GVX_TRACE("AsciiStreamReader::readStringImpl");
  dbg_eval_nl(3, name);

  AttribMap::Attrib a = currentAttribs().get(name);
  rutz::icstrstream ist(a.value.c_str());

  int len;
  ist >> len;                     dbg_eval_nl(3, len);
  ist.get(); // ignore one char of whitespace after the length

  if (len < 0)
    {
      throw rutz::error(fstring("found a negative length "
                                "for a string attribute: ", len),
                        SRC_POS);
    }

  fstring new_string;
  new_string.readsome(ist, static_cast<unsigned int>(len));

  if (ist.fail())
    {
      throwAttrError(name, a.value, SRC_POS);
    }

  dbg_eval(3, a.value); dbg_eval_nl(3, new_string);
  return new_string;
}

void AsciiStreamReader::readValueObj(const fstring& name,
                                     rutz::value& v)
{
GVX_TRACE("AsciiStreamReader::readValueObj");
  dbg_eval_nl(3, name);

  AttribMap::Attrib a = currentAttribs().get(name);

  v.set_string(a.value);
}

ref<IO::IoObject>
AsciiStreamReader::readObject(const fstring& name)
{
GVX_TRACE("AsciiStreamReader::readObject");
  dbg_eval_nl(3, name);
  return ref<IO::IoObject>(readMaybeObject(name));
}

soft_ref<IO::IoObject>
AsciiStreamReader::readMaybeObject(const fstring& name)
{
GVX_TRACE("AsciiStreamReader::readMaybeObject");
  dbg_eval_nl(3, name);

  AttribMap::Attrib attrib = currentAttribs().get(name);

  rutz::icstrstream ist(attrib.value.c_str());
  nub::uid id;
  ist >> id;

  if (ist.fail())
    throwAttrError(name, attrib.value, SRC_POS);

  if (id == 0) { return soft_ref<IO::IoObject>(); }

  // Return the object for this id, creating a new object if necessary:
  return itsObjects.fetchObject(attrib.type, id);
}

void AsciiStreamReader::readOwnedObject(const fstring& name,
                                        ref<IO::IoObject> obj)
{
GVX_TRACE("AsciiStreamReader::readOwnedObject");
  dbg_eval_nl(3, name);

  AttribMap::Attrib a = currentAttribs().get(name);
  rutz::icstrstream ist(a.value.c_str());
  char bracket[16];

  ist >> bracket;

  inflateObject(ist, name, obj);

  ist >> bracket >> std::ws;
}

void AsciiStreamReader::readBaseClass(const fstring& baseClassName,
                                      ref<IO::IoObject> basePart)
{
GVX_TRACE("AsciiStreamReader::readBaseClass");
  dbg_eval_nl(3, baseClassName);
  readOwnedObject(baseClassName, basePart);
}

ref<IO::IoObject> AsciiStreamReader::readRoot(IO::IoObject* given_root)
{
GVX_TRACE("AsciiStreamReader::readRoot");

  itsObjects.clear();

  bool haveReadRoot = false;
  nub::uid rootid = 0;

  fstring type;
  fstring equal;
  fstring bracket;
  nub::uid id;

  while ( itsBuf.peek() != EOF )
    {
      itsBuf >> type >> id >> equal >> bracket;
      dbg_eval(3, type); dbg_eval_nl(3, id);

      if ( itsBuf.fail() )
        {
          fstring msg;
          msg.append("input failed while reading typename and object id\n");
          msg.append("\ttype: ", type, "\n");
          msg.append("id: ", id, "\n");
          msg.append("\tequal: ", equal, "\n");
          msg.append("\tbracket: ", bracket);
          throw rutz::error(msg, SRC_POS);
        }

      if ( !haveReadRoot )
        {
          rootid = id;

          if (given_root != 0)
            itsObjects.assignObjectForId(rootid,
                                         ref<IO::IoObject>(given_root));

          haveReadRoot = true;
        }

      ref<IO::IoObject> obj = itsObjects.fetchObject(type, id);

      inflateObject(itsBuf, type, obj);

      itsBuf >> bracket >> std::ws;

      if ( itsBuf.fail() )
        {
          throw rutz::error(fstring("input failed "
                                    "while parsing ending bracket\n",
                                    "\tbracket: ", bracket), SRC_POS);
        }
    }

  return itsObjects.getObject(rootid);
}

void AsciiStreamReader::inflateObject(std::istream& buf,
                                      const fstring& obj_tag,
                                      ref<IO::IoObject> obj)
{
GVX_TRACE("AsciiStreamReader::inflateObject");

  //
  // (1) read the object's attributes from the stream...
  //
  shared_ptr<AttribMap> attribMap( new AttribMap(obj_tag) );

  // Skip all whitespace
  buf >> std::ws;

  IO::VersionId svid = 0;

  // Check if there is a version id in the stream
  if (buf.peek() == 'v')
    {
      int ch = buf.get();  GVX_ASSERT(ch == 'v');
      buf >> svid;
      if ( buf.fail() )
        throw rutz::error("input failed while reading "
                          "serialization version id", SRC_POS);
    }

  attribMap->setSerialVersionId(svid);

  // Get the attribute count
  int attrib_count;
  buf >> attrib_count;    dbg_eval_nl(3, attrib_count);

  if (attrib_count < 0)
    {
      throw rutz::error(fstring("found a negative attribute count: ",
                                attrib_count), SRC_POS);
    }

  if ( buf.fail() )
    {
      throw rutz::error(fstring("input failed while reading "
                                "attribute count: ", attrib_count),
                        SRC_POS);
    }

  // Loop and load all the attributes
  fstring type;
  fstring name;
  fstring equal;

  for (int i = 0; i < attrib_count; ++i)
    {
      buf >> type >> name >> equal;   dbg_eval(3, type); dbg_eval_nl(3, name);

      if ( buf.fail() )
        {
          fstring msg;
          msg.append("input failed while reading attribute type and name\n");
          msg.append("\ttype: ", type, "\n");
          msg.append("\tname: ", name, "\n");
          msg.append("\tequal: ", equal);
          throw rutz::error(msg, SRC_POS);
        }

      attribMap->addNewAttrib(name, type, readAndUnEscape(buf));
    }

  itsAttribs.push_back(attribMap);

  //
  // (2) now the object can query us for its attributes...
  //
  obj->readFrom(*this);

  itsAttribs.pop_back();
}

shared_ptr<IO::Reader> IO::makeAsciiStreamReader(std::istream& os)
{
  return rutz::make_shared( new AsciiStreamReader(os) );
}

shared_ptr<IO::Reader> IO::makeAsciiStreamReader(const char* filename)
{
  return rutz::make_shared( new AsciiStreamReader(filename) );
}

static const char vcid_groovx_io_asciistreamreader_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_ASCIISTREAMREADER_CC_UTC20050626084021_DEFINED
