///////////////////////////////////////////////////////////////////////
//
// gzstreambuf.cc
//
// Copyright (c) 2001-2003 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Fri Jul 20 13:13:22 2001
// commit: $Id$
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

#ifndef GZSTREAMBUF_CC_DEFINED
#define GZSTREAMBUF_CC_DEFINED

#include "util/gzstreambuf.h"

#include "util/error.h"
#include "util/pointers.h"
#include "util/strings.h"

#include <fstream>

#include "util/trace.h"

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
      if (om & std::ios::in)
        {
          throw Util::Error(fstring("couldn't open file '",
                                    name, "' for reading"));
        }
      else if (om & std::ios::out)
        {
          throw Util::Error(fstring("couldn't open file '", name,
                                    "' for writing"));
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
DOTRACE("Util::gzstreambuf::underflow");
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

  // return next character Hrmph. We have to cast to unsigned char to avoid
  // problems with eof. Problem is, -1 is a valid char value to
  // return. However, without a cast, char(-1) (0xff) gets converted to
  // int(-1), which is 0xffffffff, which is EOF! What we want is
  // int(0x000000ff), which we have to get by int(unsigned char(-1)).
  return static_cast<unsigned char>(*gptr());
}

int Util::gzstreambuf::overflow(int c)
{
DOTRACE("Util::gzstreambuf::overflow");
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

// If we use an anonymous namespace here, intel's icc 8.0 beta compiler
// complains about "internal error: diag_message: missing symbol
// substitutions"
namespace Util
{
  class gzstream : public std::iostream
  {
  private:
    Util::gzstreambuf itsBuf;
  public:
    gzstream(const char* filename_cstr, std::ios::openmode mode,
             bool throw_exception) :
      std::iostream(0),
      itsBuf(filename_cstr, mode, throw_exception)
    {
      rdbuf(&itsBuf);
    }
  };
}

shared_ptr<std::ostream> Util::ogzopen(const fstring& filename,
                                       std::ios::openmode flags)
{
  static fstring gz_ext(".gz");

  if (filename.ends_with(gz_ext))
    {
      return shared_ptr<std::ostream>
        (new Util::gzstream(filename.c_str(), std::ios::out|flags, true));
    }
  else
    {
      return shared_ptr<std::ostream>
        (new std::ofstream(filename.c_str(), flags));
    }
}

shared_ptr<std::ostream> Util::ogzopen(const char* filename,
                                       std::ios::openmode flags)
{
  return ogzopen(fstring(filename), flags);
}

shared_ptr<std::istream> Util::igzopen(const char* filename,
                                       std::ios::openmode flags)
{
  return shared_ptr<std::iostream>
    (new Util::gzstream(filename, std::ios::in|flags, true));
}

shared_ptr<std::istream> Util::igzopen(const fstring& filename,
                                       std::ios::openmode flags)
{
  return igzopen(filename.c_str(), flags);
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
