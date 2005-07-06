/** @file io/xmlwriter.cc IO::Writer implementation for writing XML
    files in the GVX format */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Thu Jun 19 18:26:48 2003
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

#ifndef GROOVX_IO_XMLWRITER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_XMLWRITER_CC_UTC20050626084021_DEFINED

#include "io/xmlwriter.h"

#include "io/io.h"
#include "io/writeidmap.h"
#include "io/writer.h"

#include "nub/ref.h"

#include "rutz/arrays.h"
#include "rutz/fstring.h"
#include "rutz/gzstreambuf.h"
#include "rutz/value.h"

#include <ostream>
#include <set>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

using rutz::shared_ptr;

using nub::ref;
using nub::soft_ref;

class XMLWriter : public IO::Writer
{
public:
  XMLWriter(std::ostream& os);

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
                           nub::soft_ref<const IO::IoObject> obj);

  virtual void writeOwnedObject(const char* name,
                                nub::ref<const IO::IoObject> obj);

  virtual void writeBaseClass(const char* baseClassName,
                              nub::ref<const IO::IoObject> basePart);

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

  void flattenObject(soft_ref<const IO::IoObject> obj, const char* name,
                     const char* xmltype);

  bool alreadyWritten(soft_ref<const IO::IoObject> obj) const
  {
    return ( itsWrittenObjects.find(obj.id()) != itsWrittenObjects.end() );
  }

  void markObjectAsWritten(soft_ref<const IO::IoObject> obj)
  {
    itsWrittenObjects.insert(obj.id());
  }

  void indent()
  {
    for (int i = 0; i < itsNestLevel; ++i)
      itsBuf << '\t';
  }

  void writeEscaped(const char* text);

  shared_ptr<std::ostream> itsOwnedStream;
  std::ostream& itsBuf;
  std::set<nub::uid> itsWrittenObjects;
  int itsNestLevel;
  IO::WriteIdMap itsIdMap;
};

///////////////////////////////////////////////////////////////////////
//
// XMLWriter member definitions
//
///////////////////////////////////////////////////////////////////////

XMLWriter::XMLWriter(std::ostream& os) :
  itsOwnedStream(),
  itsBuf(os),
  itsWrittenObjects(),
  itsNestLevel(0)
{
GVX_TRACE("XMLWriter::XMLWriter");
}

XMLWriter::XMLWriter(const char* filename) :
  itsOwnedStream(rutz::ogzopen(filename)),
  itsBuf(*itsOwnedStream),
  itsWrittenObjects(),
  itsNestLevel(0)
{
GVX_TRACE("XMLWriter::XMLWriter(const char*)");
}

XMLWriter::~XMLWriter () throw()
{
GVX_TRACE("XMLWriter::~XMLWriter");
}

void XMLWriter::writeChar(const char* name, char val)
{
GVX_TRACE("XMLWriter::writeChar");
  writeBasicType(name, val, "char");
}

void XMLWriter::writeInt(const char* name, int val)
{
GVX_TRACE("XMLWriter::writeInt");
  writeBasicType(name, val, "int");
}

void XMLWriter::writeBool(const char* name, bool val)
{
GVX_TRACE("XMLWriter::writeBool");
  writeBasicType(name, val, "bool");
}

void XMLWriter::writeDouble(const char* name, double val)
{
GVX_TRACE("XMLWriter::writeDouble");
  writeBasicType(name, val, "double");
}

void XMLWriter::writeValueObj(const char* name, const rutz::value& v)
{
GVX_TRACE("XMLWriter::writeValueObj");
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
GVX_TRACE("XMLWriter::writeRawData");
  defaultWriteRawData(name, data, length);
}

void XMLWriter::writeObject(const char* name,
                            soft_ref<const IO::IoObject> obj)
{
GVX_TRACE("XMLWriter::writeObject");

  if (obj.is_valid())
    {
      GVX_ASSERT(dynamic_cast<const IO::IoObject*>(obj.get()) != 0);

      if (alreadyWritten(obj))
        {
          indent();
          itsBuf << "<objref type=\"" << obj->obj_typename() << "\""
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
                                 ref<const IO::IoObject> obj)
{
GVX_TRACE("XMLWriter::writeOwnedObject");

  flattenObject(obj, name, "ownedobj");
}

void XMLWriter::writeBaseClass(const char* baseClassName,
                               ref<const IO::IoObject> basePart)
{
GVX_TRACE("XMLWriter::writeBaseClass");

  flattenObject(basePart, baseClassName, "baseclass");
}

void XMLWriter::writeRoot(const IO::IoObject* root)
{
GVX_TRACE("XMLWriter::writeRoot");

  itsBuf << "<?xml version=\"1.0\"?>\n"
         << "<!-- GroovX XML 1 -->\n";

  flattenObject(soft_ref<IO::IoObject>(const_cast<IO::IoObject*>(root)),
                "root", "object");

  itsBuf.flush();
}

void XMLWriter::writeCstring(const char* name, const char* val)
{
GVX_TRACE("XMLWriter::writeCstring");

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

void XMLWriter::flattenObject(soft_ref<const IO::IoObject> obj,
                              const char* name, const char* xmltype)
{
GVX_TRACE("XMLWriter::flattenObject");

  indent();
  itsBuf << "<" << xmltype << " type=\"" << obj->obj_typename() << "\""
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
GVX_TRACE("XMLWriter::writeEscaped");

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


shared_ptr<IO::Writer> IO::makeXMLWriter(std::ostream& os)
{
  return rutz::make_shared(new XMLWriter(os));
}

shared_ptr<IO::Writer> IO::makeXMLWriter(const char* filename)
{
  return rutz::make_shared(new XMLWriter(filename));
}

static const char vcid_groovx_io_xmlwriter_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_XMLWRITER_CC_UTC20050626084021_DEFINED
