///////////////////////////////////////////////////////////////////////
//
// stdiobuf.h
//
// Copyright (c) 2002-2005
// Rob Peters <rjpeters at klab dot caltech dot edu>
//
// created: Tue Feb 25 13:27:36 2003
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

#ifndef STDIOBUF_H_DEFINED
#define STDIOBUF_H_DEFINED

#include <cstdio>

#ifdef HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

#ifdef HAVE_ISTREAM
#include <istream>
#else
#include <istream.h>
#endif

namespace rutz
{
  class stdiobuf;
  class stdiostream;
}

/// A C++ streambuf that wraps a standard posix file descriptor.
class rutz::stdiobuf : public STD_IO::streambuf
{
private:
  int m_mode;
  int m_filedes;

  stdiobuf(const stdiobuf&);
  stdiobuf& operator=(const stdiobuf&);

  static const int s_buf_size = 4092;
  static const int s_pback_size = 4;
  char buffer[s_buf_size];

  void init(int fd, int om, bool throw_exception);

  int flushoutput();

public:
  /// Create with a reference to a FILE object.
  /** The stdiobuf will NOT close the FILE on destruction, that is
      left up to the caller (since the caller must also have been the
      one to open the FILE object). */
  stdiobuf(FILE* f, int om, bool throw_exception=false);

  /// Create with a reference to a file descriptor.
  /** The stdiobuf will NOT close the descriptor on destruction, that
      is left up to the caller (since the caller must also have been
      the one to open the file descriptor). */
  stdiobuf(int fd, int om, bool throw_exception=false);

  /// Destructor flushes buffer, but DOES NOT CLOSE the file descriptor.
  ~stdiobuf() { close(); }

  /// Check whether we have an open file descriptor.
  bool is_open() const { return m_filedes >= 0; }

  /// Flushes the buffer and forgets the file descriptor, but DOESN'T CLOSE IT.
  /** It is assumed that the same caller who passed the open file
      descriptor to our constructor will also eventually close that
      file descriptor. */
  void close();

  /// Get more data from the underlying file descriptor.
  /** Called when the streambuf's buffer has run out of data. */
  virtual int underflow();

  /// Send more data to the underlying file descriptor.
  /** Called when the streambuf's buffer has become full. */
  virtual int overflow(int c);

  /// Flush the current buffer contents to the underlying file.
  virtual int sync();
};

class rutz::stdiostream : public STD_IO::iostream
{
private:
  rutz::stdiobuf m_buf;

public:
  /// Create with a reference to a FILE object.
  /** The stdiobuf will NOT close the FILE on destruction, that is
      left up to the caller (since the caller must also have been the
      one to open the FILE object). */
  stdiostream(FILE* f, int om, bool throw_exception=false) :
    STD_IO::iostream(0),
    m_buf(f, om, throw_exception)
  {
    rdbuf(&m_buf);
  }

  /// Create with a reference to a file descriptor.
  /** The stdiobuf will NOT close the descriptor on destruction, that
      is left up to the caller (since the caller must also have been
      the one to open the file descriptor). */
  stdiostream(int fd, int om, bool throw_exception=false) :
    STD_IO::iostream(0),
    m_buf(fd, om, throw_exception)
  {
    rdbuf(&m_buf);
  }

  /// Check whether we have an open file descriptor.
  bool is_open() const { return m_buf.is_open(); }

  /// Flushes the buffer and forgets the file descriptor, but DOESN'T CLOSE IT.
  void close() { m_buf.close(); }
};

static const char vcid_stdiobuf_h[] = "$Id$ $URL$";
#endif // !STDIOBUF_H_DEFINED
