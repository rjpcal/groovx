///////////////////////////////////////////////////////////////////////
//
// cstrstream.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Mar  7 13:12:33 2003
// written: Fri Mar  7 13:15:41 2003
// $Id$
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

namespace Util
{
  class icstrbuf;
  class icstrstream;
}

class Util::icstrbuf : public STD_IO::streambuf
{
private:
  unsigned int itsLen;
  char* itsBuf;

  icstrbuf(const icstrbuf&);
  icstrbuf& operator=(const icstrbuf&);

public:
  /// Create with a reference to a FILE object.
  icstrbuf(const char* s) :
    itsLen(strlen(s)),
    itsBuf(new char[itsLen+1])
  {
    strcpy(itsBuf, s);
    setg(itsBuf, itsBuf, itsBuf + itsLen);
  }

  ~icstrbuf() { delete [] itsBuf; }

  virtual int underflow()
  {
    if (gptr() < egptr())
      {
        return *gptr();
      }

    // There's no "external data source", so if we've come to the end of
    // the buffer, then we're just out of data.
    return EOF;
  }
};

class Util::icstrstream : public STD_IO::istream
{
private:
  icstrbuf itsBuf;
public:
  icstrstream(const char* s) : STD_IO::istream(&itsBuf), itsBuf(s) {}
};

static const char vcid_cstrstream_h[] = "$Header$";
#endif // !CSTRSTREAM_H_DEFINED
