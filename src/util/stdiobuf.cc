///////////////////////////////////////////////////////////////////////
//
// stdiobuf.cc
//
// Copyright (c) 2002-2002 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Tue Feb 25 13:52:11 2003
// written: Wed Mar 19 17:58:51 2003
// $Id$
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

#ifndef STDIOBUF_CC_DEFINED
#define STDIOBUF_CC_DEFINED

#include "util/stdiobuf.h"

#include "util/error.h"

#ifdef HAVE_IOS
#include <ios>
#endif

#include <unistd.h>

#include "util/debug.h"
#include "util/trace.h"

void Util::stdiobuf::init(int fd, int om, bool throw_exception)
{
DOTRACE("Util::stdiobuf::init");
  itsFiledes = fd;

  if (itsFiledes >= 0)
    {
      itsMode = om;
    }

  if (throw_exception && !is_open())
    {
      throw Util::Error("couldn't open file stdiobuf");
    }
}

Util::stdiobuf::stdiobuf(FILE* f, int om, bool throw_exception) :
  itsMode(0), itsFiledes(-1)
{
DOTRACE("Util::stdiobuf::stdiobuf(FILE*)");
  init(fileno(f), om, throw_exception);
}

Util::stdiobuf::stdiobuf(int fd, int om, bool throw_exception) :
  itsMode(0), itsFiledes(-1)
{
DOTRACE("Util::stdiobuf::stdiobuf(int)");

  init(fd, om, throw_exception);
}

void Util::stdiobuf::close()
{
DOTRACE("Util::stdiobuf::close");
  if (is_open())
    {
      sync();
      // No ::close(itsFiledes) here since we leave that up to whoever
      // instantiated this stdiobuf.
      itsFiledes = -1;
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
  const int num = ::read(itsFiledes, buffer+pbackSize, bufSize-pbackSize);

  if (num <= 0)
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

  const int num = pptr()-pbase();

  if ( ::write(itsFiledes, pbase(), num) == -1 )
    {
      return EOF;
    }

  pbump(-num);
  return num;
}

static const char vcid_stdiobuf_cc[] = "$Header$";
#endif // !STDIOBUF_CC_DEFINED
