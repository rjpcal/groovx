///////////////////////////////////////////////////////////////////////
//
// stdiobuf.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Feb 25 13:27:36 2003
// written: Tue Feb 25 14:02:12 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STDIOBUF_H_DEFINED
#define STDIOBUF_H_DEFINED

#include <streambuf>

namespace Util
{
  class stdiobuf : public std::streambuf
  {
  private:
    bool opened;
    int mode;
    FILE* file;

    stdiobuf(const stdiobuf&);
    stdiobuf& operator=(const stdiobuf&);

    static const int bufSize = 4092;
    static const int pbackSize = 4;
    char buffer[bufSize];

    void init(FILE* f, int om, bool throw_exception);

    int flushoutput();

  public:
    stdiobuf(FILE* f, int om, bool throw_exception=false);
    stdiobuf(int fd, int om, bool throw_exception=false);
    ~stdiobuf() { close(); }

    bool is_open() { return opened; }

    void ensure_open();

    void close();

    virtual int underflow();

    virtual int overflow(int c);

    virtual int sync();
  };
}

static const char vcid_stdiobuf_h[] = "$Header$";
#endif // !STDIOBUF_H_DEFINED
