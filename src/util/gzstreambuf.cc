///////////////////////////////////////////////////////////////////////
//
// gzstreambuf.cc
//
// Copyright (c) 1998-2001 Rob Peters rjpeters@klab.caltech.edu
//
// created: Fri Jul 20 13:13:22 2001
// written: Wed Aug  8 20:16:39 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GZSTREAMBUF_CC_DEFINED
#define GZSTREAMBUF_CC_DEFINED

#include "util/gzstreambuf.h"

#include "util/error.h"
#include "util/pointers.h"
#include "util/strings.h"

#include <fstream>

Util::gzstreambuf::gzstreambuf(const char* name, int om, bool throw_exception) :
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

  if (throw_exception && !opened)
    {
      if (om & STD_IO::ios::in)
        {
          throw Util::Error("couldn't open file '", name, "' for reading");
        }
      else if (om & STD_IO::ios::out)
        {
          throw Util::Error("couldn't open file '", name, "' for writing");
        }
    }
}

void Util::gzstreambuf::close()
{
  if (opened)
    {
      sync();
      opened = false;
      gzclose(file);
    }
}

int Util::gzstreambuf::underflow() // with help from Josuttis, p. 678
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

int Util::gzstreambuf::overflow(int c)
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

int Util::gzstreambuf::sync()
{
  if (flushoutput() == EOF)
    {
      return -1; // ERROR
    }
  return 0;
}

int Util::gzstreambuf::flushoutput()
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

namespace
{
  class gzstream : public STD_IO::iostream
  {
  private:
    Util::gzstreambuf itsBuf;
  public:
    gzstream(const char* filename_cstr, int mode, bool throw_exception) :
      STD_IO::iostream(0),
      itsBuf(filename_cstr, mode, throw_exception)
    {
      rdbuf(&itsBuf);
    }
  };
}

shared_ptr<STD_IO::ostream> Util::ogzopen(const fstring& filename)
{
  static fstring gz_ext(".gz");

  if (filename.ends_with(gz_ext))
    {
      return shared_ptr<STD_IO::ostream>
        (new gzstream(filename.c_str(), STD_IO::ios::out, true));
    }
  else
    {
      return shared_ptr<STD_IO::ostream>
        (new STD_IO::ofstream(filename.c_str()));
    }
}

shared_ptr<STD_IO::ostream> Util::ogzopen(const char*filename)
{
  return ogzopen(fstring(filename));
}

shared_ptr<STD_IO::istream> Util::igzopen(const char* filename)
{
  return shared_ptr<STD_IO::iostream>
    (new gzstream(filename, STD_IO::ios::in, true));
}

shared_ptr<STD_IO::istream> Util::igzopen(const fstring& filename)
{
  return igzopen(filename.c_str());
}

// sample test code

//  #include "src/util/gzstreambuf.h"

//  int main() {
//    {
//      Util::gzstreambuf buf("test.gz", std::ios::out);

//      std::ostream os(&buf);

//      os << "Hello, World!\n";
//    }

//    {
//      Util::gzstreambuf buf2("test.gz", std::ios::in);

//      std::istream is(&buf2);

//      int c;
//      while ( (c=is.get()) != EOF )
//        {
//          std::cout << char(c);
//        }
//    }

//    return 0;
//  }

static const char vcid_gzstreambuf_cc[] = "$Header$";
#endif // !GZSTREAMBUF_CC_DEFINED
