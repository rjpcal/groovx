///////////////////////////////////////////////////////////////////////
//
// stdiobuf.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Feb 25 13:27:36 2003
// written: Fri May  2 14:27:52 2003
// $Id$
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

namespace Util
{
  class stdiobuf;
  class stdiostream;
}

/// A C++ streambuf that wraps a standard posix file descriptor.
class Util::stdiobuf : public STD_IO::streambuf
{
private:
  int itsMode;
  int itsFiledes;

  stdiobuf(const stdiobuf&);
  stdiobuf& operator=(const stdiobuf&);

  static const int bufSize = 4092;
  static const int pbackSize = 4;
  char buffer[bufSize];

  void init(int fd, int om, bool throw_exception);

  int flushoutput();

public:
  /// Create with a reference to a FILE object.
  /** The stdiobuf will NOT close the FILE on destruction, that is left
      up to the caller (since the caller must also have been the one to
      open the FILE object). */
  stdiobuf(FILE* f, int om, bool throw_exception=false);

  /// Create with a reference to a file descriptor.
  /** The stdiobuf will NOT close the descriptor on destruction, that is
      left up to the caller (since the caller must also have been the one
      to open the file descriptor). */
  stdiobuf(int fd, int om, bool throw_exception=false);

  /// Destructor flushes buffer, but DOES NOT CLOSE the file descriptor.
  ~stdiobuf() { close(); }

  /// Check whether we have an open file descriptor.
  bool is_open() const { return itsFiledes >= 0; }

  /// Flushes the buffer and forgets the file descriptor, but DOESN'T CLOSE IT.
  void close();

  /// Get more data from the underlying file descriptor.
  /** Called when the streambuf's buffer has run out of data. */
  virtual int underflow();

  /// Send more data to the underlying file descriptor.
  /** Called when the streambuf's buffer has become full. */
  virtual int overflow(int c);

  /// Flush the current buffer contents to the underlying file descriptor.
  virtual int sync();
};

class Util::stdiostream : public STD_IO::iostream
{
private:
  Util::stdiobuf itsBuf;

public:
  /// Create with a reference to a FILE object.
  /** The stdiobuf will NOT close the FILE on destruction, that is left
      up to the caller (since the caller must also have been the one to
      open the FILE object). */
  stdiostream(FILE* f, int om, bool throw_exception=false) :
    STD_IO::iostream(0),
    itsBuf(f, om, throw_exception)
  {
    rdbuf(&itsBuf);
  }

  /// Create with a reference to a file descriptor.
  /** The stdiobuf will NOT close the descriptor on destruction, that is
      left up to the caller (since the caller must also have been the one
      to open the file descriptor). */
  stdiostream(int fd, int om, bool throw_exception=false) :
    STD_IO::iostream(0),
    itsBuf(fd, om, throw_exception)
  {
    rdbuf(&itsBuf);
  }

  /// Check whether we have an open file descriptor.
  bool is_open() const { return itsBuf.is_open(); }

  /// Flushes the buffer and forgets the file descriptor, but DOESN'T CLOSE IT.
  void close() { itsBuf.close(); }
};

static const char vcid_stdiobuf_h[] = "$Header$";
#endif // !STDIOBUF_H_DEFINED
