///////////////////////////////////////////////////////////////////////
//
// gzstreambuf.h
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun 20 09:12:51 2001
// written: Wed Mar 19 17:58:54 2003
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

#ifndef GZSTREAMBUF_H_DEFINED
#define GZSTREAMBUF_H_DEFINED

#include <iostream>
#include <zlib.h>

template <class T> class shared_ptr;
class fstring;

namespace Util
{
  /// A std::streambuf implementation that handles gzip-encoded data.
  class gzstreambuf : public std::streambuf
  {
  private:
    bool opened;
    int mode;
    gzFile file;

    gzstreambuf(const gzstreambuf&);
    gzstreambuf& operator=(const gzstreambuf&);

    static const int bufSize = 4092;
    static const int pbackSize = 4;
    char buffer[bufSize];

    int flushoutput();

  public:
    gzstreambuf(const char* name, int om, bool throw_exception=false);
    ~gzstreambuf() { close(); }

    bool is_open() { return opened; }

    void ensure_open();

    void close();

    virtual int underflow();

    virtual int overflow(int c);

    virtual int sync();
  };

  /** Opens a file for writing. An exception will be thrown if the
      specified file cannot be opened. The output file will be
      gz-compressed if the filename ends with ".gz". */
  shared_ptr<std::ostream> ogzopen(const fstring& filename,
                                   std::ios::openmode flags =
                                   std::ios::openmode(0));

  /// Overload.
  shared_ptr<std::ostream> ogzopen(const char* filename,
                                   std::ios::openmode flags =
                                   std::ios::openmode(0));

  /** Opens a file for reading. An exception will be thrown if the
      specified file cannot be opened. If the file is gz-compressed,
      this will be automagically detected regardless of the filename
      extension. */
  shared_ptr<std::istream> igzopen(const fstring& filename,
                                   std::ios::openmode flags =
                                   std::ios::openmode(0));

  /// Overload.
  shared_ptr<std::istream> igzopen(const char* filename,
                                   std::ios::openmode flags =
                                   std::ios::openmode(0));
}

static const char vcid_gzstreambuf_h[] = "$Header$";
#endif // !GZSTREAMBUF_H_DEFINED
