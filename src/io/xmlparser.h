/** @file io/xmlparser.h xml parsing class used in xmlreader.cc */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2005
// Rob Peters <rjpeters at usc dot edu>
//
// created: Tue May 25 10:29:24 2004
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

#ifndef GROOVX_IO_XMLPARSER_H_UTC20050626084021_DEFINED
#define GROOVX_IO_XMLPARSER_H_UTC20050626084021_DEFINED

#include <expat.h>
#include <iosfwd>

namespace io
{
  class xml_parser;
}

class io::xml_parser
{
public:
  xml_parser(std::istream& is, int bufsize = 8192);

  virtual ~xml_parser();

  void parse();

protected:
  virtual void element_start(const char* el, const char** attr) = 0;
  virtual void element_end(const char* el) = 0;

  /// Default is no-op; override if needed.
  virtual void character_data(const char* text, int length);

private:
  xml_parser(const xml_parser&);
  xml_parser& operator=(const xml_parser&);

  static void c_element_start(void* data, const char* el, const char** attr);
  static void c_element_end(void* data, const char* el);
  static void c_character_data(void* data, const char* text, int length);

  XML_Parser     const m_parser;
  std::istream&        m_stream;
  int            const m_buf_size;
};

static const char vcid_groovx_io_xmlparser_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_XMLPARSER_H_UTC20050626084021_DEFINED
