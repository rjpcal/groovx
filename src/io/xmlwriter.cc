///////////////////////////////////////////////////////////////////////
//
// xmlwriter.cc
//
// Copyright (c) 2003-2004
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Thu Jun 19 18:26:48 2003
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

#ifndef XMLWRITER_CC_DEFINED
#define XMLWRITER_CC_DEFINED

#include "io/xmlwriter.h"

#include "io/io.h"
#include "io/ioerror.h"
#include "io/writeidmap.h"
#include "io/writer.h"

#include "util/arrays.h"
#include "util/fstring.h"
#include "util/gzstreambuf.h"
#include "util/ref.h"
#include "util/value.h"

#include <ostream>
#include <set>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

using rutz::shared_ptr;

using Util::Ref;
using Util::SoftRef;

class XMLWriter : public IO::Writer
{
public:
  XMLWriter(STD_IO::ostream& os);

  XMLWriter(const char* filename);

  virtual ~XMLWriter() throw();

  virtual void writeChar(const char* name, char val);
  virtual void writeInt(const char* name, int val);
  virtual void writeBool(const char* name, bool val);
  virtual void writeDouble(const char* name, double val);
  virtual void writeValueObj(const char* name, const rutz::value& v);

  virtual void writeRawData(const char* name,
                            const unsigned char* data,
                            unsigned int length);

  virtual void writeObject(const char* name,
                           Util::SoftRef<const IO::IoObject> obj);

  virtual void writeOwnedObject(const char* name,
                                Util::Ref<const IO::IoObject> obj);

  virtual void writeBaseClass(const char* baseClassName,
                              Util::Ref<const IO::IoObject> basePart);

  virtual void writeRoot(const IO::IoObject* root);

protected:
  virtual void writeCstring(const char* name, const char* val);

private:
  template <class T>
  void writeBasicType(const char* name, T val,
                      const char* typeName)
  {
    indent();
    itsBuf << "<" << typeName
           << " name=\"" << name << "\""
           << " value=\"" << val << "\"/>\n";
  }

  void flattenObject(SoftRef<const IO::IoObject> obj, const char* name,
                     const char* xmltype);

  bool alreadyWritten(SoftRef<const IO::IoObject> obj) const
  {
    return ( itsWrittenObjects.find(obj.id()) != itsWrittenObjects.end() );
  }

  void markObjectAsWritten(SoftRef<const IO::IoObject> obj)
  {
    itsWrittenObjects.insert(obj.id());
  }

  void indent()
  {
    for (int i = 0; i < itsNestLevel; ++i)
      itsBuf << '\t';
  }

  void writeEscaped(const char* text);

  shared_ptr<STD_IO::ostream> itsOwnedStream;
  STD_IO::ostream& itsBuf;
  std::set<Util::UID> itsWrittenObjects;
  int itsNestLevel;
  IO::WriteIdMap itsIdMap;
};

///////////////////////////////////////////////////////////////////////
//
// XMLWriter member definitions
//
///////////////////////////////////////////////////////////////////////

XMLWriter::XMLWriter(STD_IO::ostream& os) :
  itsOwnedStream(),
  itsBuf(os),
  itsWrittenObjects(),
  itsNestLevel(0)
{
DOTRACE("XMLWriter::XMLWriter");
}

XMLWriter::XMLWriter(const char* filename) :
  itsOwnedStream(rutz::ogzopen(filename)),
  itsBuf(*itsOwnedStream),
  itsWrittenObjects(),
  itsNestLevel(0)
{
DOTRACE("XMLWriter::XMLWriter(const char*)");
}

XMLWriter::~XMLWriter () throw()
{
DOTRACE("XMLWriter::~XMLWriter");
}

void XMLWriter::writeChar(const char* name, char val)
{
DOTRACE("XMLWriter::writeChar");
  writeBasicType(name, val, "char");
}

void XMLWriter::writeInt(const char* name, int val)
{
DOTRACE("XMLWriter::writeInt");
  writeBasicType(name, val, "int");
}

void XMLWriter::writeBool(const char* name, bool val)
{
DOTRACE("XMLWriter::writeBool");
  writeBasicType(name, val, "bool");
}

void XMLWriter::writeDouble(const char* name, double val)
{
DOTRACE("XMLWriter::writeDouble");
  writeBasicType(name, val, "double");
}

void XMLWriter::writeValueObj(const char* name, const rutz::value& v)
{
DOTRACE("XMLWriter::writeValueObj");
  indent();
  itsBuf << "<valobj"
         << " type=\"" << v.value_typename() << "\""
         << " name=\"" << name << "\""
         << " value=\"";
  v.print_to(itsBuf);
  itsBuf << "\"/>\n";
}

void XMLWriter::writeRawData(const char* name,
                             const unsigned char* data,
                             unsigned int length)
{
DOTRACE("XMLWriter::writeRawData");
  defaultWriteRawData(name, data, length);
}

void XMLWriter::writeObject(const char* name,
                            SoftRef<const IO::IoObject> obj)
{
DOTRACE("XMLWriter::writeObject");

  if (obj.isValid())
    {
      ASSERT(dynamic_cast<const IO::IoObject*>(obj.get()) != 0);

      if (alreadyWritten(obj))
        {
          indent();
          itsBuf << "<objref type=\"" << obj->objTypename() << "\""
                 << " id=\"" << itsIdMap.get(obj->id()) << "\""
                 << " name=\"" << name << "\"/>\n";
        }
      else
        {
          flattenObject(obj, name, "object");
        }
    }
  else
    {
      indent();
      itsBuf << "<objref type=\"NULL\" id=\"0\" name=\"" << name << "\"/>\n";
    }
}

void XMLWriter::writeOwnedObject(const char* name,
                                 Ref<const IO::IoObject> obj)
{
DOTRACE("XMLWriter::writeOwnedObject");

  flattenObject(obj, name, "ownedobj");
}

void XMLWriter::writeBaseClass(const char* baseClassName,
                               Ref<const IO::IoObject> basePart)
{
DOTRACE("XMLWriter::writeBaseClass");

  flattenObject(basePart, baseClassName, "baseclass");
}

void XMLWriter::writeRoot(const IO::IoObject* root)
{
DOTRACE("XMLWriter::writeRoot");

  itsBuf << "<?xml version=\"1.0\"?>\n"
         << "<!-- GroovX XML 1 -->\n";

  flattenObject(SoftRef<IO::IoObject>(const_cast<IO::IoObject*>(root)),
                "root", "object");

  itsBuf.flush();
}

void XMLWriter::writeCstring(const char* name, const char* val)
{
DOTRACE("XMLWriter::writeCstring");

  indent();
  // special case for empty string:
  if (*val == '\0')
    {
      itsBuf << "<string name=\"" << name << "\"/>\n";
    }
  else
    {
      itsBuf << "<string name=\"" << name << "\">";
      writeEscaped(val);
      itsBuf << "</string>\n";
    }
}

void XMLWriter::flattenObject(SoftRef<const IO::IoObject> obj,
                              const char* name, const char* xmltype)
{
DOTRACE("XMLWriter::flattenObject");

  indent();
  itsBuf << "<" << xmltype << " type=\"" << obj->objTypename() << "\""
         << " id=\"" << itsIdMap.get(obj->id()) << "\""
         << " name=\"" << name << "\""
         << " version=\"" << obj->serialVersionId() << "\">\n";

  ++itsNestLevel;
  obj->writeTo(*this);
  --itsNestLevel;

  markObjectAsWritten(obj);

  indent();
  itsBuf << "</" << xmltype << ">\n";
}

void XMLWriter::writeEscaped(const char* p)
{
DOTRACE("XMLWriter::writeEscaped");

  // Escape any special characters
  for ( ; *p != '\0'; ++p)
    {
      switch (*p)
        {
        case '<':  itsBuf.write("&lt;",   4); break;
        case '>':  itsBuf.write("&gt;",   4); break;
        case '&':  itsBuf.write("&amp;",  5); break;
        case '"':  itsBuf.write("&quot;", 6); break;
        case '\'': itsBuf.write("&apos;", 6); break;
        default:   itsBuf.put(*p);            break;
        }
    }
}


shared_ptr<IO::Writer> IO::makeXMLWriter(STD_IO::ostream& os)
{
  return rutz::make_shared(new XMLWriter(os));
}

shared_ptr<IO::Writer> IO::makeXMLWriter(const char* filename)
{
  return rutz::make_shared(new XMLWriter(filename));
}

static const char vcid_xmlwriter_cc[] = "$Header$";
#endif // !XMLWRITER_CC_DEFINED
