///////////////////////////////////////////////////////////////////////
//
// stdiobuf.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Feb 25 13:27:36 2003
// written: Wed Feb 26 16:37:45 2003
// $Id$
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

  bool is_open() { return itsFiledes >= 0; }

  void ensure_open();

  void close();

  virtual int underflow();

  virtual int overflow(int c);

  virtual int sync();
};

static const char vcid_stdiobuf_h[] = "$Header$";
#endif // !STDIOBUF_H_DEFINED
