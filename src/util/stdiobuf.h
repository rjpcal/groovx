///////////////////////////////////////////////////////////////////////
//
// stdiobuf.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Feb 25 13:27:36 2003
// written: Wed Mar 19 17:58:51 2003
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

namespace Util
{
  class stdiobuf;
}

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

  ~stdiobuf() { close(); }

  bool is_open() const { return itsFiledes >= 0; }

  void close();

  virtual int underflow();

  virtual int overflow(int c);

  virtual int sync();
};

static const char vcid_stdiobuf_h[] = "$Header$";
#endif // !STDIOBUF_H_DEFINED
