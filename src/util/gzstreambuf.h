///////////////////////////////////////////////////////////////////////
//
// gzstreambuf.h
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Jun 20 09:12:51 2001
// written: Wed Jun 20 10:53:45 2001
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

    static const int bufSize = 4092;
    static const int pbackSize = 4;
    char buffer[bufSize];

    int flushoutput()
    {
      if (!(mode & std::ios::out) || !opened) return EOF;

      int num = pptr()-pbase();
      if ( gzwrite(file, pbase(), num) != num )
        {
          return EOF;
        }

      pbump(-num);
      return num;
    }

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
              setg(buffer+pbackSize, buffer+pbackSize, buffer+pbackSize);
            }
          else if (om & std::ios::out)
            {
              *fmodeptr++ = 'w';
              setp(buffer, buffer+(bufSize-1));
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

      int numPutback = 0;
      if (pbackSize > 0)
        {
          // process size of putback area
          // -use number of characters read
          // -but at most four
          numPutback = gptr() - eback();
          if (numPutback > 4)
            numPutback = 4;

          // copy up to four characters previously read into the putback
          // buffer (area of first four characters)
          std::memcpy (buffer+(4-numPutback), gptr()-numPutback,
                       numPutback);
        }

      // read new characters
      int num = gzread(file, buffer+pbackSize, bufSize-pbackSize);

      if (num <= 0) // error (0) or end-of-file (-1)
        return EOF;

      // reset buffer pointers
      setg (buffer+(pbackSize-numPutback),
            buffer+pbackSize,
            buffer+pbackSize+num);

      // return next character
      return *gptr();
    }

    virtual int overflow(int c)
    {
      if (!(mode & std::ios::out) || !opened) return EOF;

      if (c != EOF)
        {
          // insert the character into the buffer
          *pptr() = c;
          pbump(1);
        }

      if (flushoutput() == EOF)
        {
          return -1; // ERROR
        }

      return c;
    }

    virtual int sync()
    {
      if (flushoutput() == EOF)
        {
          return -1; // ERROR
        }
      return 0;
    }
  };

}

static const char vcid_gzstreambuf_h[] = "$Header$";
#endif // !GZSTREAMBUF_H_DEFINED
