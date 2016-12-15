/** @file io/xmlparser.cc xml parsing class used in xmlreader.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Tue May 25 10:29:42 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [https://github.com/rjpcal/groovx]
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

#include "io/xmlparser.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"

#include <cstdio> // for EOF
#include <istream>
#include <limits>

#include "rutz/trace.h"
#include "rutz/debug.h"
GVX_DBG_REGISTER

#ifndef XML_STATUS_OK
#define XML_STATUS_OK    1
#define XML_STATUS_ERROR 0
#endif

io::xml_parser::xml_parser(std::istream& is, int bufsize) :
  m_parser(XML_ParserCreate(/*encoding*/0)),
  m_stream(is),
  m_buf_size(bufsize)
{
GVX_TRACE("io::xml_parser::xml_parser");
  if (m_parser == nullptr)
    {
      throw rutz::error("couldn't allocate memory for XML_Parser",
                        SRC_POS);
    }

  XML_SetUserData(m_parser, this);

  XML_SetElementHandler(m_parser, &c_element_start, &c_element_end);

  XML_SetCharacterDataHandler(m_parser, &c_character_data);
}

io::xml_parser::~xml_parser()
{
GVX_TRACE("io::xml_parser::~xml_parser");
  XML_ParserFree(m_parser);
}

void io::xml_parser::character_data(const char* /*text*/, size_t /*length*/)
{
GVX_TRACE("io::xml_parser::character_data");
}

void io::xml_parser::c_element_start(void* data, const char* el, const char** attr)
{
GVX_TRACE("io::xml_parser::c_element_start");
  io::xml_parser* p = static_cast<io::xml_parser*>(data);
  GVX_ASSERT(p != nullptr);
  p->element_start(el, attr);
}

void io::xml_parser::c_element_end(void* data, const char* el)
{
GVX_TRACE("io::xml_parser::c_element_end");
  io::xml_parser* p = static_cast<io::xml_parser*>(data);
  GVX_ASSERT(p != nullptr);
  p->element_end(el);
}

void io::xml_parser::c_character_data(void* data, const char* text, int length)
{
GVX_TRACE("io::xml_parser::c_character_data");
  io::xml_parser* p = static_cast<io::xml_parser*>(data);
  GVX_ASSERT(p != nullptr);
  GVX_ASSERT(length >= 0);
  p->character_data(text, size_t(length));
}

void io::xml_parser::parse()
{
GVX_TRACE("io::xml_parser::parse");
  while (1)
    {
      void* const buf = XML_GetBuffer(m_parser, m_buf_size);
      if (buf == nullptr)
        {
          throw rutz::error("couldn't get buffer in io::xml_parser::parse()",
                            SRC_POS);
        }

      // very strangely I wasn't able to get things to work using a
      // readsome() approach here...
      m_stream.read(static_cast<char*>(buf), m_buf_size);
      const ssize_t len = m_stream.gcount();
      if (!m_stream.eof() && m_stream.fail())
        {
          throw rutz::error("read error in io::xml_parser::parse()",
                            SRC_POS);
        }

      const int peek = m_stream.peek();

      const int done = (peek == EOF);

      if (GVX_DBG_LEVEL() >= 3)
        {
          dbg_eval(3, buf);
          dbg_eval(3, m_buf_size);
          dbg_eval(3, len);
          dbg_eval(3, peek);
          dbg_eval_nl(3, done);
        }

      GVX_ASSERT(len < std::numeric_limits<int>::max());

      // alternate: use XML_Parse(m_parser, m_buf, len, done) if we have
      // our own memory buffer
      if (XML_ParseBuffer(m_parser, int(len), done) != XML_STATUS_OK)
        {
          throw rutz::error
            (rutz::sfmt("xml parse error at input line %d:\n%s",
                        int(XML_GetCurrentLineNumber(m_parser)),
                        XML_ErrorString(XML_GetErrorCode(m_parser))),
             SRC_POS);
        }

      if (done)
        return;
    }
}

#if 0
// here's a simple subclass of io::xml_parser that prints an outline
// of an XML file just to show that everything is getting parsed
// properly
class Outliner : public io::xml_parser
{
public:
  Outliner(std::istream& is) :
    io::xml_parser(is),
    m_depth(0) {}
  virtual ~Outliner() {}

protected:
  virtual void element_start(const char* el, const char** attr) override;
  virtual void element_end(const char* el) override;

private:
  int m_depth;
};

void Outliner::element_start(const char* el, const char** attr)
{
  for (int i = 0; i < m_depth; i++)
    printf("  ");

  printf("%s", el);

  for (int i = 0; attr[i]; i += 2)
    {
      printf(" %s='%s'", attr[i], attr[i + 1]);
    }

  printf("\n");
  ++m_depth;
}

void Outliner::element_end(const char* el)
{
  --m_depth;
}
#endif
