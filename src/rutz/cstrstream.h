/** @file rutz/cstrstream.h like std::strstream, but not deprecated */

///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <https://github.com/rjpcal/>
//
// created: Fri Mar  7 13:12:33 2003
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

#ifndef GROOVX_RUTZ_CSTRSTREAM_H_UTC20050626084019_DEFINED
#define GROOVX_RUTZ_CSTRSTREAM_H_UTC20050626084019_DEFINED

#include <cstring>
#include <istream>
#include <streambuf>

namespace rutz
{
  class imembuf;
  class imemstream;
  class icstrstream;
}

/// An input streambuf that reads from memory.
class rutz::imembuf : public std::streambuf
{
private:
  size_t m_len;
  const char* m_buf;
  char* m_owned_mem;

  imembuf(const imembuf&);
  imembuf& operator=(const imembuf&);

public:
  /// Create using the given char array as the input buffer.
  imembuf(const char* s);

  /// Create using the given char array and length as the input buffer.
  imembuf(const char* s, size_t len);

  void make_owning();

  /// Destructor.
  virtual ~imembuf();

  /// Underflow operation.
  /** Since there's no "external data source", if we've come to the
      end of our current buffer, then we're just plain out of data. */
  virtual int underflow() override;
};

/// An input stream class based on imembuf.
class rutz::imemstream : public std::istream
{
private:
  imembuf m_buf;
public:
  /// Construct using the given char array as the input buffer.
  imemstream(const char* s);

  /// Construct using the given char array and length as the input buffer.
  imemstream(const char* s, size_t len);

  virtual ~imemstream();

  imemstream(const imemstream&) = delete;
  imemstream& operator=(const imemstream&) = delete;
};

/// An input stream class based on imembuf that makes a private copy.
class rutz::icstrstream : public std::istream
{
private:
  imembuf m_buf;
public:
  /// Construct using the given char array as the input buffer.
  icstrstream(const char* s);

  virtual ~icstrstream();

  icstrstream(const icstrstream&) = delete;
  icstrstream& operator=(const icstrstream&) = delete;
};

#endif // !GROOVX_RUTZ_CSTRSTREAM_H_UTC20050626084019_DEFINED
