///////////////////////////////////////////////////////////////////////
//
// stdiobuf.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters@klab.caltech.edu
//
// created: Tue Feb 25 13:52:11 2003
// written: Wed Feb 26 08:48:20 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef STDIOBUF_CC_DEFINED
#define STDIOBUF_CC_DEFINED

#include "util/stdiobuf.h"

#include "util/error.h"

#include <ios>

#include "util/trace.h"

void Util::stdiobuf::init(FILE* f, int om, bool throw_exception)
{
DOTRACE("Util::stdiobuf::init");
  itsFile = f;

  if (itsFile != NULL)
    {
      itsMode = om;
    }

  if (throw_exception && !is_open())
    {
      throw Util::Error("couldn't open file stdiobuf");
    }
}

Util::stdiobuf::stdiobuf(FILE* f, int om, bool throw_exception) :
  itsMode(0), itsFile(0)
{
DOTRACE("Util::stdiobuf::stdiobuf(FILE*)");
  init(f, om, throw_exception);
}

Util::stdiobuf::stdiobuf(int fd, int om, bool throw_exception) :
  itsMode(0), itsFile(0)
{
DOTRACE("Util::stdiobuf::stdiobuf(int)");
  FILE* f = 0;

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

      f = fdopen(fd, fmode);
    }

  init(f, om, throw_exception);
}

void Util::stdiobuf::close()
{
DOTRACE("Util::stdiobuf::close");
  if (is_open())
    {
      sync();
      // No fclose(itsFile) here since we leave that up to whoever
      // instantiated this stdiobuf.
      itsFile = 0;
      itsMode = 0;
    }
}

int Util::stdiobuf::underflow() // with help from Josuttis, p. 678
{
DOTRACE("Util::stdiobuf::underflow");
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
  size_t num = fread(buffer+pbackSize, 1, bufSize-pbackSize, itsFile);

  if (num == 0 && (feof(itsFile) || ferror(itsFile)))
    return EOF;

  // reset buffer pointers
  setg (buffer+(pbackSize-numPutback),
        buffer+pbackSize,
        buffer+pbackSize+num);

  // return next character Hrmph. We have to cast to unsigned char to avoid
  // problems with eof. Problem is, -1 is a valid char value to
  // return. However, without a cast, char(-1) (0xff) gets converted to
  // int(-1), which is 0xffffffff, which is EOF! What we want is
  // int(0x000000ff), which we have to get by int(unsigned char(-1)).
  return static_cast<unsigned char>(*gptr());
}

int Util::stdiobuf::overflow(int c)
{
DOTRACE("Util::stdiobuf::overflow");
  if (!(itsMode & std::ios::out) || !is_open()) return EOF;

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

int Util::stdiobuf::sync()
{
  if (flushoutput() == EOF)
    {
      return -1; // ERROR
    }
  return 0;
}

int Util::stdiobuf::flushoutput()
{
  if (!(itsMode & std::ios::out) || !is_open()) return EOF;

  int num = pptr()-pbase();
  if ( fwrite(pbase(), 1, num, itsFile) != num )
    {
      return EOF;
    }

  pbump(-num);
  return num;
}

static const char vcid_stdiobuf_cc[] = "$Header$";
#endif // !STDIOBUF_CC_DEFINED
