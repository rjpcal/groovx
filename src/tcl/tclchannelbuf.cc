///////////////////////////////////////////////////////////////////////
//
// tclchannelbuf.cc
//
// Copyright (c) 2004-2004 Rob Peters rjpeters at klab dot caltech dot edu
//
// created: Wed Jun 30 17:01:22 2004
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

#ifndef TCLCHANNELBUF_CC_DEFINED
#define TCLCHANNELBUF_CC_DEFINED

#include "tcl/tclchannelbuf.h"

#include "util/error.h"
#include "util/pointers.h"
#include "util/strings.h"

#include "util/trace.h"

Tcl::ChannelBuf::ChannelBuf(Tcl_Interp* interp,
                            const char* channame, int /*om*/) :
  opened(false), owned(false), mode(0), itsInterp(interp), chan(0)
{
  int origmode = 0;
  chan = Tcl_GetChannel(interp, channame, &origmode);
  if (chan == 0)
    {
      throw Util::Error(fstring("no channel named '", channame, "'"));
    }
  opened = true;
}

void Tcl::ChannelBuf::close()
{
  if (opened && owned)
    {
      opened = false;
      Tcl_Close(itsInterp, chan);
    }
}

int Tcl::ChannelBuf::underflow() // with help from Josuttis, p. 678
{
DOTRACE("Tcl::ChannelBuf::underflow");
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
  int num = Tcl_Read(chan, buffer+pbackSize, bufSize-pbackSize);

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

int Tcl::ChannelBuf::overflow(int c)
{
DOTRACE("Tcl::ChannelBuf::overflow");
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

int Tcl::ChannelBuf::sync()
{
  if (flushoutput() == EOF)
    {
      return -1; // ERROR
    }
  return 0;
}

int Tcl::ChannelBuf::flushoutput()
{
  if (!(mode & std::ios::out) || !opened) return EOF;

  int num = pptr()-pbase();
  if ( Tcl_Write(chan, pbase(), num) != num )
    {
      return EOF;
    }

  pbump(-num);
  return num;
}

namespace
{
  class TclStream : public std::iostream
  {
  private:
    Tcl::ChannelBuf itsBuf;
  public:
    TclStream(Tcl_Interp* interp,
              const char* channame, std::ios::openmode mode) :
      std::iostream(0),
      itsBuf(interp, channame, mode)
    {
      rdbuf(&itsBuf);
    }
  };
}

shared_ptr<std::ostream> Tcl::ochanopen(Tcl_Interp* interp,
                                        const char* channame,
                                        std::ios::openmode flags)
{
  return shared_ptr<std::ostream>
    (new TclStream(interp, channame, std::ios::out|flags));
}

shared_ptr<std::istream> Tcl::ichanopen(Tcl_Interp* interp,
                                        const char* channame,
                                        std::ios::openmode flags)
{
  return shared_ptr<std::iostream>
    (new TclStream(interp, channame, std::ios::in|flags));
}


static const char vcid_tclchannelbuf_cc[] = "$Header$";
#endif // !TCLCHANNELBUF_CC_DEFINED
