///////////////////////////////////////////////////////////////////////
//
// gzstreambuf.h
//
// Copyright (c) 1998-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 20 09:12:51 2001
// written: Fri Jan 18 16:06:55 2002
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GZSTREAMBUF_H_DEFINED
#define GZSTREAMBUF_H_DEFINED

#ifndef PRESTANDARD_IOSTREAMS
#include <iostream>
#else
#include <iostream.h>
typedef int streamsize;
#endif
#include <zlib.h>

template <class T> class shared_ptr;
class fstring;

namespace Util
{
  class gzstreambuf : public STD_IO::streambuf
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
  shared_ptr<STD_IO::ostream> ogzopen(const fstring& filename);

  /// Overload.
  shared_ptr<STD_IO::ostream> ogzopen(const char* filename);

  /** Opens a file for reading. An exception will be thrown if the
      specified file cannot be opened. If the file is gz-compressed,
      this will be automagically detected regardless of the filename
      extension. */
  shared_ptr<STD_IO::istream> igzopen(const fstring& filename);

  /// Overload.
  shared_ptr<STD_IO::istream> igzopen(const char* filename);
}

static const char vcid_gzstreambuf_h[] = "$Header$";
#endif // !GZSTREAMBUF_H_DEFINED
