///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.cc
//
// Copyright (c) 1999-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Mon Jun  7 13:05:57 1999
// commit: $Id$
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://www.klab.caltech.edu/rjpeters/groovx/]
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

#ifndef ASCIISTREAMWRITER_CC_DEFINED
#define ASCIISTREAMWRITER_CC_DEFINED

#include "io/asciistreamwriter.h"

#include "io/io.h"
#include "io/writeidmap.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "util/arrays.h"
#include "util/fstring.h"
#include "util/gzstreambuf.h"
#include "util/value.h"

#include <ostream>
#include <string>
#include <set>
#include <vector>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using rutz::fstring;
using rutz::shared_ptr;

using Nub::Ref;
using Nub::SoftRef;

namespace
{
  const char* ATTRIB_ENDER = "^\n";

  void addEscapes(std::string& text)
  {
  DOTRACE("AsciiStreamWriter::addEscapes");
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
#if defined(SHORTEN_SYMBOL_NAMES)
#define AsciiStreamWriter ASW
#endif

class AsciiStreamWriter : public IO::Writer
{
public:
  AsciiStreamWriter(STD_IO::ostream& os);

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
                           Nub::SoftRef<const IO::IoObject> obj);

  virtual void writeOwnedObject(const char* name,
                                Nub::Ref<const IO::IoObject> obj);

  virtual void writeBaseClass(const char* baseClassName,
                              Nub::Ref<const IO::IoObject> basePart);

  virtual void writeRoot(const IO::IoObject* root);

protected:
  virtual void writeCstring(const char* name, const char* val);

private:
  shared_ptr<STD_IO::ostream> itsOwnedStream;
  STD_IO::ostream& itsBuf;
  mutable std::vector<SoftRef<const IO::IoObject> > itsToHandleV;
  std::set<SoftRef<const IO::IoObject> > itsWrittenObjects;
  IO::WriteIdMap itsIdMap;

  void addObjectToBeHandled(SoftRef<const IO::IoObject> obj)
  {
    if ( !alreadyWritten(obj) )
      {
        itsToHandleV.push_back(obj);
      }
  }

  bool alreadyWritten(SoftRef<const IO::IoObject> obj) const
  {
    return ( itsWrittenObjects.find(obj) !=
             itsWrittenObjects.end() );
  }

  void markObjectAsWritten(SoftRef<const IO::IoObject> obj)
  {
    itsWrittenObjects.insert(obj);
  }

  void flattenObject(SoftRef<const IO::IoObject> obj);

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

AsciiStreamWriter::AsciiStreamWriter(STD_IO::ostream& os) :
  itsOwnedStream(),
  itsBuf(os),
  itsToHandleV(),
  itsWrittenObjects()
{
DOTRACE("AsciiStreamWriter::AsciiStreamWriter");
}

AsciiStreamWriter::AsciiStreamWriter(const char* filename) :
  itsOwnedStream(rutz::ogzopen(filename)),
  itsBuf(*itsOwnedStream),
  itsToHandleV(),
  itsWrittenObjects()
{
DOTRACE("AsciiStreamWriter::AsciiStreamWriter(const char*)");
}

AsciiStreamWriter::~AsciiStreamWriter () throw()
{
DOTRACE("AsciiStreamWriter::~AsciiStreamWriter");
}

void AsciiStreamWriter::writeChar(const char* name, char val)
{
DOTRACE("AsciiStreamWriter::writeChar");
  writeBasicType(name, val, "char");
}

void AsciiStreamWriter::writeInt(const char* name, int val)
{
DOTRACE("AsciiStreamWriter::writeInt");
  writeBasicType(name, val, "int");
}

void AsciiStreamWriter::writeBool(const char* name, bool val)
{
DOTRACE("AsciiStreamWriter::writeBool");
  writeBasicType(name, val, "bool");
}

void AsciiStreamWriter::writeDouble(const char* name, double val)
{
DOTRACE("AsciiStreamWriter::writeDouble");
  writeBasicType(name, val, "double");
}

void AsciiStreamWriter::writeCstring(const char* name, const char* val)
{
DOTRACE("AsciiStreamWriter::writeCstring");
  writeStringType(name, val);
}

void AsciiStreamWriter::writeValueObj(const char* name,
                                      const rutz::value& v)
{
DOTRACE("AsciiStreamWriter::writeValueObj");

  writeBasicType<const rutz::value&>(name, v, v.value_typename().c_str());
}

void AsciiStreamWriter::writeObject(const char* name,
                                    SoftRef<const IO::IoObject> obj)
{
DOTRACE("AsciiStreamWriter::writeObject");

  fstring type = "NULL";
  Nub::UID id = 0;

  if (obj.isValid())
    {
      ASSERT(dynamic_cast<const IO::IoObject*>(obj.get()) != 0);

      type = obj->objTypename();
      id = itsIdMap.get(obj->id());

      addObjectToBeHandled(obj);
    }

  itsBuf << type << " "
         << name << " := "
         << id << ATTRIB_ENDER;
}

void AsciiStreamWriter::writeOwnedObject(const char* name,
                                         Ref<const IO::IoObject> obj)
{
DOTRACE("AsciiStreamWriter::writeOwnedObject");

  fstring type = obj->objTypename().c_str();

  itsBuf << type.c_str() << ' ' << name << " := ";

  flattenObject(obj);

  itsBuf << ATTRIB_ENDER;
}

void AsciiStreamWriter::writeBaseClass(const char* baseClassName,
                                       Ref<const IO::IoObject> basePart)
{
DOTRACE("AsciiStreamWriter::writeBaseClass");
  writeOwnedObject(baseClassName, basePart);
}

void AsciiStreamWriter::writeRoot(const IO::IoObject* root)
{
DOTRACE("AsciiStreamWriter::writeRoot");
  itsToHandleV.clear();
  itsWrittenObjects.clear();

  // need the const_cast here because:
  // (1) SoftRef constructor will optionally call Detail::insertItem()
  // (2) insertItem() will put the object in the Nub::ObjDb
  // (3) objects in the Nub::ObjDb are non-const since they can be
  //     retrieved and modified
  itsToHandleV.push_back
    (SoftRef<IO::IoObject>(const_cast<IO::IoObject*>(root)));

  while ( !itsToHandleV.empty() )
    {
      SoftRef<const IO::IoObject> obj = itsToHandleV.back();
      itsToHandleV.pop_back();

      if ( !alreadyWritten(obj) )
        {
          itsBuf << obj->objTypename().c_str() << ' '
                 << itsIdMap.get(obj->id()) << " := ";
          flattenObject(obj);
        }
    }

  itsBuf.flush();
}

void AsciiStreamWriter::flattenObject(SoftRef<const IO::IoObject> obj)
{
DOTRACE("AsciiStreamWriter::flattenObject");

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

shared_ptr<IO::Writer> IO::makeAsciiStreamWriter(STD_IO::ostream& os)
{
  return rutz::make_shared(new AsciiStreamWriter(os));
}

shared_ptr<IO::Writer> IO::makeAsciiStreamWriter(const char* filename)
{
  return rutz::make_shared(new AsciiStreamWriter(filename));
}

static const char vcid_asciistreamwriter_cc[] = "$Id$ $URL$";
#endif // !ASCIISTREAMWRITER_CC_DEFINED
