///////////////////////////////////////////////////////////////////////
//
// asciistreamwriter.cc
//
// Copyright (c) 1999-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Mon Jun  7 13:05:57 1999
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

#ifndef ASCIISTREAMWRITER_CC_DEFINED
#define ASCIISTREAMWRITER_CC_DEFINED

#include "io/asciistreamwriter.h"

#include "io/io.h"
#include "io/ioerror.h"

#include "util/arrays.h"
#include "util/ref.h"
#include "util/strings.h"
#include "util/value.h"

#include <iostream>
#include <fstream>
#include <string>
#include <set>

#include "util/trace.h"
#include "util/debug.h"

#if defined(SHORTEN_SYMBOL_NAMES)
#define AsciiStreamWriter ASW
#endif

namespace
{
  const char* ATTRIB_ENDER = "^\n";

  void addEscapes(std::string& text)
  {
  DOTRACE("AsciiStreamWriter::Impl::addEscapes");
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

class AsciiStreamWriter::Impl
{
private:
  Impl(const Impl&);
  Impl& operator=(const Impl&);

public:
  Impl(AsciiStreamWriter* owner, STD_IO::ostream& os) :
    itsOwner(owner), itsFstream(0), itsBuf(os),
    itsToHandle(), itsWrittenObjects()
  {}

  Impl(AsciiStreamWriter* owner, const char* filename) :
    itsOwner(owner), itsFstream(new STD_IO::ofstream(filename)),
    itsBuf(*itsFstream),
    itsToHandle(), itsWrittenObjects()
  {
    if (itsFstream->fail()) throw IO::FilenameError(filename);
  }

  ~Impl()
  {
    delete itsFstream;
  }

  AsciiStreamWriter* itsOwner;
  STD_IO::ofstream* itsFstream;
  STD_IO::ostream& itsBuf;
  std::set<const IO::IoObject *> itsToHandle;
  std::set<const IO::IoObject *> itsWrittenObjects;

  // Helper functions
private:
  bool haveMoreObjectsToHandle() const
  {
    return !itsToHandle.empty();
  }

  bool alreadyWritten(const IO::IoObject* obj) const
  {
    return ( itsWrittenObjects.find(obj) !=
             itsWrittenObjects.end() );
  }

  const IO::IoObject* getNextObjectToHandle() const
  {
    return *(itsToHandle.begin());
  }

  void markObjectAsWritten(const IO::IoObject* obj)
  {
    itsToHandle.erase(obj);
    itsWrittenObjects.insert(obj);
  }

  void addObjectToBeHandled(const IO::IoObject* obj)
  {
    if ( !alreadyWritten(obj) )
      {
        itsToHandle.insert(obj);
      }
  }

  void flattenObject(const IO::IoObject* obj);

  // Delegands
public:
  void writeValueObj(const char* name, const Value& value);

  void writeObject(const char* name, SoftRef<const IO::IoObject> obj);

  void writeOwnedObject(const char* name, Ref<const IO::IoObject> obj);

  template <class T>
  void writeBasicType(const char* name, T val,
                      const char* string_typename)
  {
    itsBuf << string_typename << " "
           << name << " := "
           << val << ATTRIB_ENDER;
  }

  void writeStringType(const char* name, const char* val,
                       const char* string_typename)
  {
    std::string escaped_val(val);
    int val_length = escaped_val.length();
    addEscapes(escaped_val);

    itsBuf << string_typename << " "
           << name << " := "
           << val_length << " " << escaped_val.c_str() << ATTRIB_ENDER;
  }

  void writeRoot(const IO::IoObject* root);
};

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamWriter::Impl helper functions
//
///////////////////////////////////////////////////////////////////////

void AsciiStreamWriter::Impl::flattenObject(const IO::IoObject* obj)
{
DOTRACE("AsciiStreamWriter::Impl::flattenObject");

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
  obj->writeTo(itsOwner);

  markObjectAsWritten(obj);

  //   ...and finally, close the object's braces.
  itsBuf << '}' << '\n';
}

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamWriter::Impl delegand definitions
//
///////////////////////////////////////////////////////////////////////

void AsciiStreamWriter::Impl::writeRoot(const IO::IoObject* root)
{
DOTRACE("AsciiStreamWriter::Impl::writeRoot");
  itsToHandle.clear();
  itsWrittenObjects.clear();

  itsToHandle.insert(root);

  while ( haveMoreObjectsToHandle() )
    {
      const IO::IoObject* obj = getNextObjectToHandle();

      if ( !alreadyWritten(obj) )
        {
          itsBuf << obj->objTypename().c_str() << ' ' << obj->id() << " := ";
          flattenObject(obj);
        }
    }

  itsBuf.flush();
}

void AsciiStreamWriter::Impl::writeValueObj(const char* attrib_name,
                                            const Value& value)
{
DOTRACE("AsciiStreamWriter::Impl::writeValueObj");

  itsBuf << value.getNativeTypeName() << " "
         << attrib_name << " := "
         << value << ATTRIB_ENDER;
}

void AsciiStreamWriter::Impl::writeObject(const char* name,
                                          SoftRef<const IO::IoObject> obj)
{
DOTRACE("AsciiStreamWriter::Impl::writeObject");

  if (obj.isValid())
    {
      Assert(dynamic_cast<const IO::IoObject*>(obj.get()) != 0);

      itsBuf << obj->objTypename().c_str() << " "
             << name << " := "
             << obj->id() << ATTRIB_ENDER;

      addObjectToBeHandled(obj.get());
    }
  else
    {
      itsBuf << "NULL " << name << " := 0" << ATTRIB_ENDER;
    }
}

void AsciiStreamWriter::Impl::writeOwnedObject(const char* name,
                                               Ref<const IO::IoObject> obj)
{
DOTRACE("AsciiStreamWriter::Impl::writeOwnedObject");

  fstring type = obj->objTypename().c_str();

  itsBuf << type.c_str() << ' ' << name << " := ";

  flattenObject(obj.get());

  itsBuf << ATTRIB_ENDER;
}

///////////////////////////////////////////////////////////////////////
//
// AsciiStreamWriter member definitions
//
///////////////////////////////////////////////////////////////////////

AsciiStreamWriter::AsciiStreamWriter(STD_IO::ostream& os) :
  rep( new Impl(this, os) )
{
DOTRACE("AsciiStreamWriter::AsciiStreamWriter");
}

AsciiStreamWriter::AsciiStreamWriter(const char* filename) :
  rep( new Impl(this, filename) )
{
DOTRACE("AsciiStreamWriter::AsciiStreamWriter(const char*)");
}

AsciiStreamWriter::~AsciiStreamWriter ()
{
DOTRACE("AsciiStreamWriter::~AsciiStreamWriter");
  delete rep;
}

void AsciiStreamWriter::writeChar(const char* name, char val)
{
DOTRACE("AsciiStreamWriter::writeChar");
  rep->writeBasicType(name, val, "char");
}

void AsciiStreamWriter::writeInt(const char* name, int val)
{
DOTRACE("AsciiStreamWriter::writeInt");
  rep->writeBasicType(name, val, "int");
}

void AsciiStreamWriter::writeBool(const char* name, bool val)
{
DOTRACE("AsciiStreamWriter::writeBool");
  rep->writeBasicType(name, val, "bool");
}

void AsciiStreamWriter::writeDouble(const char* name, double val)
{
DOTRACE("AsciiStreamWriter::writeDouble");
  rep->writeBasicType(name, val, "double");
}

void AsciiStreamWriter::writeCstring(const char* name, const char* val)
{
DOTRACE("AsciiStreamWriter::writeCstring");
  rep->writeStringType(name, val, "cstring");
}

void AsciiStreamWriter::writeValueObj(const char* name, const Value& value)
{
  rep->writeValueObj(name, value);
}

void AsciiStreamWriter::writeObject(const char* name,
                                    SoftRef<const IO::IoObject> obj)
{
  rep->writeObject(name, obj);
}

void AsciiStreamWriter::writeOwnedObject(const char* name,
                                         Ref<const IO::IoObject> obj)
{
  rep->writeOwnedObject(name, obj);
}

void AsciiStreamWriter::writeBaseClass(const char* baseClassName,
                                       Ref<const IO::IoObject> basePart)
{
DOTRACE("AsciiStreamWriter::writeBaseClass");
  rep->writeOwnedObject(baseClassName, basePart);
}

void AsciiStreamWriter::writeRoot(const IO::IoObject* root)
{
  rep->writeRoot(root);
}

#if defined(SHORTEN_SYMBOL_NAMES)
#undef AsciiStreamWriter
#endif

static const char vcid_asciistreamwriter_cc[] = "$Header$";
#endif // !ASCIISTREAMWRITER_CC_DEFINED
