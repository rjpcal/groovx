/** @file tcl/channelbuf.cc bridge class between tcl channels and c++
    streams */
///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2004 California Institute of Technology
// Copyright (c) 2004-2007 University of Southern California
// Rob Peters <rjpeters at usc dot edu>
//
// created: Wed Jun 30 17:01:22 2004
//
// --------------------------------------------------------------------
//
// This file is part of GroovX.
//   [http://ilab.usc.edu/rjpeters/groovx/]
//
// GroovX is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GroovX is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GroovX; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
///////////////////////////////////////////////////////////////////////

#ifndef GROOVX_TCL_CHANNELBUF_CC_UTC20050628162421_DEFINED
#define GROOVX_TCL_CHANNELBUF_CC_UTC20050628162421_DEFINED

#include "tcl/channelbuf.h"

#include "rutz/error.h"
#include "rutz/fstring.h"
#include "rutz/sfmt.h"
#include "rutz/shared_ptr.h"

#include "rutz/trace.h"

tcl::channel_buf::channel_buf(Tcl_Interp* interp,
                            const char* channame, unsigned int /*om*/) :
  opened(false), owned(false), mode(0), m_interp(interp), chan(0)
{
  int origmode = 0;
  chan = Tcl_GetChannel(interp, channame, &origmode);
  if (chan == 0)
    {
      throw rutz::error(rutz::sfmt("no channel named '%s'",
                                   channame), SRC_POS);
    }
  opened = true;
}

void tcl::channel_buf::close()
{
  if (opened && owned)
    {
      opened = false;
      Tcl_Close(m_interp, chan);
    }
}

int tcl::channel_buf::underflow() // with help from Josuttis, p. 678
{
GVX_TRACE("tcl::channel_buf::underflow");
  // is read position before end of buffer?
  if (gptr() < egptr())
    return *gptr();

  off_t num_putback = 0;
  if (s_pback_size > 0)
    {
      // process size of putback area
      // -use number of characters read
      // -but at most four
      num_putback = gptr() - eback();
      if (num_putback > 4)
        num_putback = 4;

      // copy up to four characters previously read into the putback
      // buffer (area of first four characters)
      std::memcpy (m_buffer+(4-num_putback), gptr()-num_putback,
                   size_t(num_putback));
    }

  // read new characters
  const int num = Tcl_Read(chan,
                           m_buffer+s_pback_size,
                           s_buf_size-s_pback_size);

  if (num <= 0) // error (0) or end-of-file (-1)
    return EOF;

  // reset buffer pointers
  setg (m_buffer+(s_pback_size-num_putback),
        m_buffer+s_pback_size,
        m_buffer+s_pback_size+num);

  // return next character Hrmph. We have to cast to unsigned char to avoid
  // problems with eof. Problem is, -1 is a valid char value to
  // return. However, without a cast, char(-1) (0xff) gets converted to
  // int(-1), which is 0xffffffff, which is EOF! What we want is
  // int(0x000000ff), which we have to get by int(unsigned char(-1)).
  return static_cast<unsigned char>(*gptr());
}

int tcl::channel_buf::overflow(int c)
{
GVX_TRACE("tcl::channel_buf::overflow");
  if (!(mode & std::ios::out) || !opened) return EOF;

  if (c != EOF)
    {
      // insert the character into the buffer
      *pptr() = char(c);
      pbump(1);
    }

  if (flushoutput() == EOF)
    {
      return -1; // ERROR
    }

  return c;
}

int tcl::channel_buf::sync()
{
  if (flushoutput() == EOF)
    {
      return -1; // ERROR
    }
  return 0;
}

int tcl::channel_buf::flushoutput()
{
  if (!(mode & std::ios::out) || !opened) return EOF;

  int num = int(pptr() - pbase());
  if ( Tcl_Write(chan, pbase(), num) != num )
    {
      return EOF;
    }

  pbump(-num);
  return num;
}

namespace
{
  class tcl_stream : public std::iostream
  {
  private:
    tcl::channel_buf m_buf;
  public:
    tcl_stream(Tcl_Interp* interp,
               const char* channame, std::ios::openmode mode)
      :
      std::iostream(0),
      m_buf(interp, channame, mode)
    {
      rdbuf(&m_buf);
    }
  };
}

using rutz::shared_ptr;

shared_ptr<std::ostream> tcl::ochanopen(Tcl_Interp* interp,
                                        const char* channame,
                                        std::ios::openmode flags)
{
  return shared_ptr<std::ostream>
    (new tcl_stream(interp, channame, std::ios::out|flags));
}

shared_ptr<std::istream> tcl::ichanopen(Tcl_Interp* interp,
                                        const char* channame,
                                        std::ios::openmode flags)
{
  return shared_ptr<std::iostream>
    (new tcl_stream(interp, channame, std::ios::in|flags));
}


#endif // !GROOVX_TCL_CHANNELBUF_CC_UTC20050628162421_DEFINED
