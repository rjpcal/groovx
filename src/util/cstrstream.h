///////////////////////////////////////////////////////////////////////
//
// cstrstream.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Fri Mar  7 13:12:33 2003
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

#ifndef CSTRSTREAM_H_DEFINED
#define CSTRSTREAM_H_DEFINED

#include <cstring>

#ifdef HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif

#ifdef HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

namespace rutz
{
  class imembuf;
  class imemstream;
  class icstrstream;
}

/// An input streambuf that reads from memory.
class rutz::imembuf : public STD_IO::streambuf
{
private:
  unsigned int m_len;
  const char* m_buf;
  char* m_owned_mem;

  imembuf(const imembuf&);
  imembuf& operator=(const imembuf&);

public:
  /// Create using the given char array as the input buffer.
  imembuf(const char* s);

  /// Create using the given char array and length as the input buffer.
  imembuf(const char* s, unsigned int len);

  void make_owning();

  /// Destructor.
  virtual ~imembuf();

  /// Underflow operation.
  /** Since there's no "external data source", if we've come to the
      end of our current buffer, then we're just plain out of data. */
  virtual int underflow();
};

/// An input stream class based on imembuf.
class rutz::imemstream : public STD_IO::istream
{
private:
  imembuf m_buf;
public:
  /// Construct using the given char array as the input buffer.
  imemstream(const char* s);

  /// Construct using the given char array and length as the input buffer.
  imemstream(const char* s, unsigned int len);
};

/// An input stream class based on imembuf that makes a private copy.
class rutz::icstrstream : public STD_IO::istream
{
private:
  imembuf m_buf;
public:
  /// Construct using the given char array as the input buffer.
  icstrstream(const char* s);
};

static const char vcid_cstrstream_h[] = "$Id$ $URL$";
#endif // !CSTRSTREAM_H_DEFINED
