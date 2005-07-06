/** @file io/asciistreamwriter.cc write IO::IoObject objects in the ASW
    format */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Mon Jun  7 13:05:57 1999
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

#ifndef GROOVX_IO_ASCIISTREAMWRITER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_ASCIISTREAMWRITER_CC_UTC20050626084021_DEFINED

#include "io/asciistreamwriter.h"

#include "io/io.h"
#include "io/writeidmap.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/arrays.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/value.h"

#include <ostream>
#include <string>
#include <set>
#include <vector>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::fstring;
using rutz::shared_ptr;

using nub::ref;
using nub::soft_ref;

namespace
{
  const char* ATTRIB_ENDER = "^\n";

  void addEscapes(std::string& text)
  {
  GVX_TRACE("AsciiStreamWriter::addEscapes");
    // Escape any special characters
    for (size_t pos = 0; pos < text.length(); /* ++ done in loop body */ )
      {
        switch (text[pos])
          {
          case '\\': // i.e., a single backslash
            text.replace(pos, 1, "\\\\"); // i.e., two backslashes
            pos += 2;
            break;
          case '^':
            text.replace(pos, 1, "\\c");
            pos += 2;
            break;
          case '{':
            text.replace(pos, 1, "\\{");
            pos += 2;
            break;
          case '}':
            text.replace(pos, 1, "\\}");
            pos += 2;
            break;
          default:
            ++pos;
            break;
          }
      }
  }
}

// This is a hack to help shorten up names for assemblers on systems
// that need short identifier names. A typedef at the end of this file
// re-introduces AsciiStreamWriter as a typedef for ASW, so that
// client code should be able to use the typename AsciiStreamWriter in
// all cases.
#if defined(GVX_SHORTEN_SYMBOL_NAMES)
#define AsciiStreamWriter ASW
#endif

class AsciiStreamWriter : public IO::Writer
{
public:
  AsciiStreamWriter(std::ostream& os);

  AsciiStreamWriter(const char* filename);

  virtual ~AsciiStreamWriter() throw();

  virtual void writeChar(const char* name, char val);
  virtual void writeInt(const char* name, int val);
  virtual void writeBool(const char* name, bool val);
  virtual void writeDouble(const char* name, double val);
  virtual void writeValueObj(const char* name, const rutz::value& v);

  virtual void writeRawData(const char* name,
                            const unsigned char* data,
                            unsigned int length)
  { defaultWriteRawData(name, data, length); }

  virtual void writeObject(const char* name,
                           nub::soft_ref<const IO::IoObject> obj);

  virtual void writeOwnedObject(const char* name,
                                nub::ref<const IO::IoObject> obj);

  virtual void writeBaseClass(const char* baseClassName,
                              nub::ref<const IO::IoObject> basePart);

  virtual void writeRoot(const IO::IoObject* root);

protected:
  virtual void writeCstring(const char* name, const char* val);

private:
  shared_ptr<std::ostream> itsOwnedStream;
  std::ostream& itsBuf;
  mutable std::vector<soft_ref<const IO::IoObject> > itsToHandleV;
  std::set<soft_ref<const IO::IoObject> > itsWrittenObjects;
  IO::WriteIdMap itsIdMap;

  void addObjectToBeHandled(soft_ref<const IO::IoObject> obj)
  {
    if ( !alreadyWritten(obj) )
      {
        itsToHandleV.push_back(obj);
      }
  }

  bool alreadyWritten(soft_ref<const IO::IoObject> obj) const
  {
    return ( itsWrittenObjects.find(obj) !=
             itsWrittenObjects.end() );
  }

  void markObjectAsWritten(soft_ref<const IO::IoObject> obj)
  {
    itsWrittenObjects.insert(obj);
  }

  void flattenObject(soft_ref<const IO::IoObject> obj);

  template <class T>
  void writeBasicType(const char* name, T val,
                      const char* string_typename)
  {
    itsBuf << string_typename << " "
           << name << " := "
           << val << ATTRIB_ENDER;
  }

  void writeStringType(const char* name, const char* val)
  {
    std::string escaped_val(val);
    int val_length = escaped_val.length();
    addEscapes(escaped_val);

    itsBuf << "cstring "
           << name << " := "
           << val_length << " " << escaped_val.c_str() << ATTRIB_ENDER;
  }
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamWriter member definitions
//
///////////////////////////////////////////////////////////////////////

AsciiStreamWriter::AsciiStreamWriter(std::ostream& os) :
  itsOwnedStream(),
  itsBuf(os),
  itsToHandleV(),
  itsWrittenObjects()
{
GVX_TRACE("AsciiStreamWriter::AsciiStreamWriter");
}

AsciiStreamWriter::AsciiStreamWriter(const char* filename) :
  itsOwnedStream(rutz::ogzopen(filename)),
  itsBuf(*itsOwnedStream),
  itsToHandleV(),
  itsWrittenObjects()
{
GVX_TRACE("AsciiStreamWriter::AsciiStreamWriter(const char*)");
}

AsciiStreamWriter::~AsciiStreamWriter () throw()
{
GVX_TRACE("AsciiStreamWriter::~AsciiStreamWriter");
}

void AsciiStreamWriter::writeChar(const char* name, char val)
{
GVX_TRACE("AsciiStreamWriter::writeChar");
  writeBasicType(name, val, "char");
}

void AsciiStreamWriter::writeInt(const char* name, int val)
{
GVX_TRACE("AsciiStreamWriter::writeInt");
  writeBasicType(name, val, "int");
}

void AsciiStreamWriter::writeBool(const char* name, bool val)
{
GVX_TRACE("AsciiStreamWriter::writeBool");
  writeBasicType(name, val, "bool");
}

void AsciiStreamWriter::writeDouble(const char* name, double val)
{
GVX_TRACE("AsciiStreamWriter::writeDouble");
  writeBasicType(name, val, "double");
}

void AsciiStreamWriter::writeCstring(const char* name, const char* val)
{
GVX_TRACE("AsciiStreamWriter::writeCstring");
  writeStringType(name, val);
}

void AsciiStreamWriter::writeValueObj(const char* name,
                                      const rutz::value& v)
{
GVX_TRACE("AsciiStreamWriter::writeValueObj");

  writeBasicType<const rutz::value&>(name, v, v.value_typename().c_str());
}

void AsciiStreamWriter::writeObject(const char* name,
                                    soft_ref<const IO::IoObject> obj)
{
GVX_TRACE("AsciiStreamWriter::writeObject");

  fstring type = "NULL";
  nub::uid id = 0;

  if (obj.is_valid())
    {
      GVX_ASSERT(dynamic_cast<const IO::IoObject*>(obj.get()) != 0);

      type = obj->obj_typename();
      id = itsIdMap.get(obj->id());

      addObjectToBeHandled(obj);
    }

  itsBuf << type << " "
         << name << " := "
         << id << ATTRIB_ENDER;
}

void AsciiStreamWriter::writeOwnedObject(const char* name,
                                         ref<const IO::IoObject> obj)
{
GVX_TRACE("AsciiStreamWriter::writeOwnedObject");

  fstring type = obj->obj_typename().c_str();

  itsBuf << type.c_str() << ' ' << name << " := ";

  flattenObject(obj);

  itsBuf << ATTRIB_ENDER;
}

void AsciiStreamWriter::writeBaseClass(const char* baseClassName,
                                       ref<const IO::IoObject> basePart)
{
GVX_TRACE("AsciiStreamWriter::writeBaseClass");
  writeOwnedObject(baseClassName, basePart);
}

void AsciiStreamWriter::writeRoot(const IO::IoObject* root)
{
GVX_TRACE("AsciiStreamWriter::writeRoot");
  itsToHandleV.clear();
  itsWrittenObjects.clear();

  // need the const_cast here because:
  // (1) soft_ref constructor will optionally call Detail::insert_item()
  // (2) insert_item() will put the object in the nub::objectdb
  // (3) objects in the nub::objectdb are non-const since they can be
  //     retrieved and modified
  itsToHandleV.push_back
    (soft_ref<IO::IoObject>(const_cast<IO::IoObject*>(root)));

  while ( !itsToHandleV.empty() )
    {
      soft_ref<const IO::IoObject> obj = itsToHandleV.back();
      itsToHandleV.pop_back();

      if ( !alreadyWritten(obj) )
        {
          itsBuf << obj->obj_typename().c_str() << ' '
                 << itsIdMap.get(obj->id()) << " := ";
          flattenObject(obj);
        }
    }

  itsBuf.flush();
}

void AsciiStreamWriter::flattenObject(soft_ref<const IO::IoObject> obj)
{
GVX_TRACE("AsciiStreamWriter::flattenObject");

  // Objects are written in the following format:
  //
  // { ?<version id>? <attribute count>
  // ...attributes...
  // }
  //

  // Open the object's braces...
  itsBuf << "{ ";

  //   ...write <version id> if it is nonzero...
  IO::VersionId serial_ver_id = obj->serialVersionId();;
  if ( serial_ver_id > 0 )
    itsBuf << 'v' << serial_ver_id << ' ';

  //   ...write the <attribute count>...
  itsBuf << obj->ioAttribCount() << '\n';

  //   ...write the object's <attributes>...
  obj->writeTo(*this);

  markObjectAsWritten(obj);

  //   ...and finally, close the object's braces.
  itsBuf << '}' << '\n';
}

shared_ptr<IO::Writer> IO::makeAsciiStreamWriter(std::ostream& os)
{
  return rutz::make_shared(new AsciiStreamWriter(os));
}

shared_ptr<IO::Writer> IO::makeAsciiStreamWriter(const char* filename)
{
  return rutz::make_shared(new AsciiStreamWriter(filename));
}

static const char vcid_groovx_io_asciistreamwriter_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_ASCIISTREAMWRITER_CC_UTC20050626084021_DEFINED
