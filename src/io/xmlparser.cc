///////////////////////////////////////////////////////////////////////
//
// xmlparser.cc
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue May 25 10:29:42 2004
// commit: $Id$
// $HeadURL$
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

#ifndef GROOVX_IO_XMLPARSER_CC_UTC20050626084021_DEFINED
#define GROOVX_IO_XMLPARSER_CC_UTC20050626084021_DEFINED

#include "io/xmlparser.h"

#include "rutz/error.h"
#include "rutz/fstring.h"

#include <istream>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

#ifndef XML_STATUS_OK
#define XML_STATUS_OK    1
#define XML_STATUS_ERROR 0
#endif

XmlParser::XmlParser(std::istream& is, int bufsize) :
  itsParser(XML_ParserCreate(/*encoding*/0)),
  itsStream(is),
  itsBufSize(bufsize)
{
GVX_TRACE("XmlParser::XmlParser");
  if (itsParser == 0)
    {
      throw rutz::error("couldn't allocate memory for XML_Parser",
                        SRC_POS);
    }

  XML_SetUserData(itsParser, this);

  XML_SetElementHandler(itsParser, &elementStartC, &elementEndC);

  XML_SetCharacterDataHandler(itsParser, &characterDataC);
}

XmlParser::~XmlParser()
{
GVX_TRACE("XmlParser::~XmlParser");
  XML_ParserFree(itsParser);
}

void XmlParser::characterData(const char* /*text*/, int /*length*/)
{
GVX_TRACE("XmlParser::characterData");
}

void XmlParser::elementStartC(void* data, const char* el, const char** attr)
{
GVX_TRACE("XmlParser::elementStartC");
  XmlParser* p = static_cast<XmlParser*>(data);
  GVX_ASSERT(p != 0);
  p->elementStart(el, attr);
}

void XmlParser::elementEndC(void* data, const char* el)
{
GVX_TRACE("XmlParser::elementEndC");
  XmlParser* p = static_cast<XmlParser*>(data);
  GVX_ASSERT(p != 0);
  p->elementEnd(el);
}

void XmlParser::characterDataC(void* data, const char* text, int length)
{
GVX_TRACE("XmlParser::characterDataC");
  XmlParser* p = static_cast<XmlParser*>(data);
  GVX_ASSERT(p != 0);
  p->characterData(text, length);
}

void XmlParser::parse()
{
GVX_TRACE("XmlParser::parse");
  while (1)
    {
      void* const buf = XML_GetBuffer(itsParser, itsBufSize);
      if (buf == 0)
        {
          throw rutz::error("couldn't get buffer in XmlParser::parse()",
                            SRC_POS);
        }

      // very strangely I wasn't able to get things to work using a
      // readsome() approach here...
      itsStream.read(static_cast<char*>(buf), itsBufSize);
      const int len = itsStream.gcount();
      if (!itsStream.eof() && itsStream.fail())
        {
          throw rutz::error("read error in XmlParser::parse()",
                            SRC_POS);
        }

      const int peek = itsStream.peek();

      const int done = (peek == EOF);

      if (GVX_DBG_LEVEL() >= 3)
        {
          dbg_eval(3, buf);
          dbg_eval(3, itsBufSize);
          dbg_eval(3, len);
          dbg_eval(3, peek);
          dbg_eval_nl(3, done);
        }

      // alternate: use XML_Parse(itsParser, itsBuf, len, done) if we have
      // our own memory buffer
      if (XML_ParseBuffer(itsParser, len, done)
          != XML_STATUS_OK)
        {
          throw rutz::error
            (rutz::fstring("xml parse error at input line ",
                           XML_GetCurrentLineNumber(itsParser),
                           ":\n",
                           XML_ErrorString(XML_GetErrorCode(itsParser))),
             SRC_POS);
        }

      if (done)
        return;
    }
}

#if 0
// here's a simple subclass of XmlParser that prints an outline of an XML
// file just to show that everything is getting parsed properly
class Outliner : public XmlParser
{
public:
  Outliner(std::istream& is) :
    XmlParser(is),
    itsDepth(0) {}
  virtual ~Outliner() {}

protected:
  virtual void elementStart(const char* el, const char** attr);
  virtual void elementEnd(const char* el);

private:
  int itsDepth;
};

void Outliner::elementStart(const char* el, const char** attr)
{
  for (int i = 0; i < itsDepth; i++)
    printf("  ");

  printf("%s", el);

  for (int i = 0; attr[i]; i += 2)
    {
      printf(" %s='%s'", attr[i], attr[i + 1]);
    }

  printf("\n");
  ++itsDepth;
}

void Outliner::elementEnd(const char* el)
{
  --itsDepth;
}
#endif

static const char vcid_groovx_io_xmlparser_cc_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_XMLPARSER_CC_UTC20050626084021_DEFINED
