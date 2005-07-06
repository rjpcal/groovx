/** @file io/xmlparser.h */

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

class XmlParser
{
public:
  XmlParser(std::istream& is, int bufsize = 8192);

  virtual ~XmlParser();

  void parse();

protected:
  virtual void elementStart(const char* el, const char** attr) = 0;
  virtual void elementEnd(const char* el) = 0;

  /// Default is no-op; override if needed.
  virtual void characterData(const char* text, int length);

private:
  XmlParser(const XmlParser&);
  XmlParser& operator=(const XmlParser&);

  static void elementStartC(void* data, const char* el, const char** attr);
  static void elementEndC(void* data, const char* el);
  static void characterDataC(void* data, const char* text, int length);

  XML_Parser const itsParser;
  std::istream& itsStream;
  const int itsBufSize;
};

static const char vcid_groovx_io_xmlparser_h_utc20050626084021[] = "$Id$ $HeadURL$";
#endif // !GROOVX_IO_XMLPARSER_H_UTC20050626084021_DEFINED
