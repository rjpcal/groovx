///////////////////////////////////////////////////////////////////////
//
// cstrstream.h
//
// Copyright (c) 2002-2002 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Mar  7 13:12:33 2003
// written: Wed Mar 19 13:04:26 2003
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

/// An input streambuf that reads from c-style strings.
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

  /// Destructor.
  ~icstrbuf() { delete [] itsBuf; }

  /// Underflow operation.
  /** Since there's no "external data source", if we've come to the end of
      our current buffer, then we're just plain out of data. */
  virtual int underflow()
  {
    if (gptr() < egptr())
      {
        return *gptr();
      }

    return EOF;
  }
};

/// An input stream class based on icstrbuf.
class Util::icstrstream : public STD_IO::istream
{
private:
  icstrbuf itsBuf;
public:
  /// Construct using the given char array as the input buffer.
  icstrstream(const char* s) : STD_IO::istream(&itsBuf), itsBuf(s) {}
};

static const char vcid_cstrstream_h[] = "$Header$";
#endif // !CSTRSTREAM_H_DEFINED
