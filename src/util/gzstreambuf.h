///////////////////////////////////////////////////////////////////////
//
// gzstreambuf.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 20 09:12:51 2001
// written: Wed Jun 20 10:08:59 2001
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
#include "zlib.h"

namespace Util
{
  class gzstreambuf : public std::streambuf {
  private:
    bool opened;
    int mode;
    gzFile file;

    static const int bufSize = 512;
    char buffer[bufSize];

  public:
    gzstreambuf(const char* name, int om) :
      opened(false), mode(0), file(0)
    {
      // no append nor read/write mode
      if ( (om & std::ios::ate) || (om & std::ios::app)
           || ((om & std::ios::in) && (om & std::ios::out)) )
        {
          /* do nothing -- opening fails */;
        }
      else
        {
          char fmode[10];
          char* fmodeptr = fmode;

          if (om & std::ios::in)
            {
              *fmodeptr++ = 'r';
            }
          else if (om & std::ios::out)
            {
              *fmodeptr++ = 'w';
              setg(buffer+4, buffer+4, buffer+4);
            }

          *fmodeptr++ = 'b';
          *fmodeptr = '\0';

          file = gzopen(name,fmode);

          if (file != NULL)
            {
              opened = true;
              mode = om;
            }
        }
    }

    ~gzstreambuf() { close(); }

    bool is_open() { return opened; }

    void close()
    {
      if (opened)
        {
          sync();
          opened = false;
          gzclose(file);
        }
    }

    virtual int underflow() // with help from Josuttis, p. 678
    {
      // is read position before end of buffer?
      if (gptr() < egptr())
        return *gptr();

      // process size of putback area
      // -use number of characters read
      // -but at most four
      int numPutback = gptr() - eback();
      if (numPutback > 4)
        numPutback = 4;

      // copy up to four characters previously read into the putback
      // buffer (area of first four characters)
      std::memcpy (buffer+(4-numPutback), gptr()-numPutback,
                   numPutback);

      // read new characters
      int num = gzread(file, buffer+4, bufSize-4);

      if (num <= 0) // error (0) or end-of-file (-1)
        return EOF;

      // reset buffer pointers
      setg (buffer+(4-numPutback),
            buffer+4,
            buffer+4+num);

      // return next character
      return *gptr();
    }

    virtual int overflow(int c)
    {
      if (!(mode & std::ios::out) || !opened) return EOF;

      int written = c;
      if (c != EOF)
        written = gzputc(file, c);

      return written;
    }

    virtual std::streamsize xsputn(const char* s, const std::streamsize n)
    {
      if (!(mode & std::ios::out) || !opened) return 0;

      return gzwrite(file, const_cast<char*>(s), n);
    }

  };

}

static const char vcid_gzstreambuf_h[] = "$Header$";
#endif // !GZSTREAMBUF_H_DEFINED
