///////////////////////////////////////////////////////////////////////
//
// xmlparser.cc
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue May 25 10:29:42 2004
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

#ifndef XMLPARSER_CC_DEFINED
#define XMLPARSER_CC_DEFINED

#include "io/xmlparser.h"

#include "util/error.h"
#include "util/strings.h"

#include <istream>

#include "util/trace.h"
#include "util/debug.h"
DBG_REGISTER

XmlParser::XmlParser(std::istream& is, int bufsize) :
  itsParser(XML_ParserCreate(/*encoding*/0)),
  itsStream(is),
  itsBufSize(bufsize)
{
  if (itsParser == 0)
    {
      throw Util::Error("couldn't allocate memory for XML_Parser");
    }

  XML_SetUserData(itsParser, this);

  XML_SetElementHandler(itsParser, &elementStartC, &elementEndC);

  XML_SetCharacterDataHandler(itsParser, &characterDataC);
}

XmlParser::~XmlParser()
{
  XML_ParserFree(itsParser);
}

void XmlParser::characterData(const char* /*text*/, int /*length*/) {}

void XmlParser::elementStartC(void* data, const char* el, const char** attr)
{
  XmlParser* p = static_cast<XmlParser*>(data);
  Assert(p != 0);
  p->elementStart(el, attr);
}

void XmlParser::elementEndC(void* data, const char* el)
{
  XmlParser* p = static_cast<XmlParser*>(data);
  Assert(p != 0);
  p->elementEnd(el);
}

void XmlParser::characterDataC(void* data, const char* text, int length)
{
  XmlParser* p = static_cast<XmlParser*>(data);
  Assert(p != 0);
  p->characterData(text, length);
}

void XmlParser::parse()
{
  while (1)
    {
      void* const buf = XML_GetBuffer(itsParser, itsBufSize);
      if (buf == 0)
        {
          throw Util::Error("couldn't get buffer in XmlParser::parse()");
        }

      // very strangely I wasn't able to get things to work using a
      // readsome() approach here...
      itsStream.read(static_cast<char*>(buf), itsBufSize);
      const int len = itsStream.gcount();
      if (!itsStream.eof() && itsStream.fail())
        {
          throw Util::Error("read error in XmlParser::parse()");
        }

      const int peek = itsStream.peek();

      const int done = (peek == EOF);

      if (GET_DBG_LEVEL() >= 3)
        {
          dbgEval(3, buf);
          dbgEval(3, itsBufSize);
          dbgEval(3, len);
          dbgEval(3, peek);
          dbgEvalNL(3, done);
        }

      // alternate: use XML_Parse(itsParser, itsBuf, len, done) if we have
      // our own memory buffer
      if (XML_ParseBuffer(itsParser, len, done)
          != XML_STATUS_OK)
        {
          throw Util::Error
            (fstring("xml parse error at input line ",
                     XML_GetCurrentLineNumber(itsParser),
                     ":\n",
                     XML_ErrorString(XML_GetErrorCode(itsParser))));
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

static const char vcid_xmlparser_cc[] = "$Header$";
#endif // !XMLPARSER_CC_DEFINED
